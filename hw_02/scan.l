%option warn nodefault
%option noinput nounput noyywrap
/* %option debug */

%top{
/* For fileno(3). See feature_test_macros(7) manpage. */
#define _GNU_SOURCE
}

%{
/* could use %option yylineno in __FILE__ instead of num_lines */
int num_lines = 1, num_tokens = 0, num_comments = 0;

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
RESERVED        int|float|struct
ID              [a-z][a-z0-9]*
NUMBER          [0-9]+
STRING          \"[^"]*\"
OP              "+"|"-"|"*"|"/"|=
WHITESPACE      [ \t]+

%x comment

%%

{RESERVED}      num_tokens++;
{ID}            num_tokens++;
{NUMBER}        num_tokens++;
{STRING}        num_tokens++;
{OP}            num_tokens++;

"/*"                    { BEGIN(comment); start_comment(); }
<comment>[^*\n]*        /* eat anything that's not a '*' */
<comment>"*"+[^*/\n]*   /* eat up '*'s not followed by '/'s */
<comment>\n             num_lines++; /* blank lines in comments are ok */
<comment>"*"+"/"        {
                                end_comment();
                                num_comments++;
                                num_tokens++;
                                BEGIN(INITIAL);
                        }


{WHITESPACE}    num_tokens++;
\n              num_lines++;
 .              { printf("Unrecognized character: %s\n", yytext); }

%%

int main(int argc, char *argv[])
{
    yylex();
    printf("There are %d tokens /* comments are counted as tokens */\n", num_tokens);
    printf("There are %d lines\n", num_lines);
    printf("There are %d comments:\n", num_comments);
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
