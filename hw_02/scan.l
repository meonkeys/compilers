%option warn nodefault
%option noinput nounput noyywrap yylineno
/* %option debug */

%top{
/* For fileno(3). See feature_test_macros(7) manpage. */
#define _GNU_SOURCE
}

%{
/* could use %option yylineno in __FILE__ instead of num_lines */
int num_tokens = 0, num_comments = 0;

void start_comment();
void end_comment();
long num_chars = 0;

/*
 * from
 * http://flex.sf.net/manual/How-do-I-track-the-byte-offset-for-lseek_0028_0029_003f.html
 */
#define YY_USER_ACTION num_chars += yyleng;
%}

/* reserved words */
RESERVED        return|typedef|if|else|int|float|for|struct|union|void|while
ID              [a-z][a-z0-9\_]*
INT				[0-9]+
FLOAT			.[0-9]+|[0-9]+.|[0-9]+.[0-9]+
STRING          \"[^"]*\"
ARITHOP         "+"|"-"|"*"|"/"
RELOP			"<"|">"|">="|"<="|"!="|"=="
LOGOP			"||"|"&&"|"!"
ASSIGNOP		"="
WHITESPACE      [ \t]+
SEPERATOR		"{"|"}"|"["|"]"|"("|")"|";"|","|"."
/* we still need to define string literals */

%x comment

%%

{RESERVED}      num_tokens++;
{ID}            num_tokens++;
{INT}			num_tokens++;
{FLOAT}			num_tokens++;
{STRING}        num_tokens++;
{ARITHOP}       num_tokens++;
{RELOP}			num_tokens++;
{LOGOP}			num_tokens++;
{SEPERATOR}		num_tokens++;
{ASSIGNOP}      num_tokens++;

"/*"                    { BEGIN(comment); }  /* start_comment();} */
<comment>[^*\n]*        /* eat anything that's not a '*' */
<comment>"*"+[^*/\n]*   /* eat up '*'s not followed by '/'s */
<comment>\n             /* blank lines in comments are ok */
<comment>"*"+"/"        {
                                /*end_comment();*/
                                num_comments++;
                                num_tokens++;
                                BEGIN(INITIAL);
                        }


{WHITESPACE}    {}
\n				{}
 .              { printf("Unrecognized character: %s on line: %d\n", yytext, yylineno); }

%%

int main(int argc, char *argv[])
{
    yylex();
    printf("There are %d tokens /* comments are counted as tokens */\n", num_tokens);
    printf("There are %d lines\n", yylineno);
    printf("There are %d comments:\n", num_comments);
	
	/* printfs for comments here */

	/* print out the frequency of identifiers here */
	printf("Frequency of identifiers:\n");

    return EXIT_SUCCESS;
}

void start_comment()
{
    /* subtract 2 for the '/' and the '*' characters starting the comment. */
    printf("comment %d started at %ld\n", num_comments, num_chars - 2);
}

void end_comment()
{
    printf("comment %d ended at %ld\n", num_comments, num_chars);
}
