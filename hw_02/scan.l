%option warn nodefault
%option noinput nounput noyywrap yylineno
/* %option debug */

%top{
/* For fileno(3). See feature_test_macros(7) manpage. */
#define _GNU_SOURCE
}

%{
#include <assert.h>

/* could use %option yylineno in __FILE__ instead of num_lines */
int num_tokens = 0, num_comments = 0;
/* FIXME, FIXME, FIXME: use growable structure(s)! */
long comment_start_pos[] = {0,0,0}, comment_end_pos[] = {0,0,0};
long num_chars = 0;

void start_comment();
void end_comment();
void display_comments();

/*
 * from
 * http://flex.sf.net/manual/How-do-I-track-the-byte-offset-for-lseek_0028_0029_003f.html
 */
#define YY_USER_ACTION num_chars += yyleng;
%}

/* reserved words */
RESERVED        return|typedef|if|else|int|float|for|struct|union|void|while
ID              [a-z][a-z0-9_]*
DIGIT           [0-9]
INT             {DIGIT}+
FLOAT           \.{DIGIT}+|{DIGIT}+\.|{DIGIT}+\.{DIGIT}+
STRING          \"[^"]*\"
ARITHOP         "+"|"-"|"*"|"/"
RELOP           "<"|">"|">="|"<="|"!="|"=="
LOGOP           "||"|"&&"|"!"
ASSIGNOP        "="
WHITESPACE      [ \t]+
SEPARATOR       "{"|"}"|"["|"]"|"("|")"|";"|","|"."
/* we still need to define string literals */

%x comment

%%

{RESERVED}      num_tokens++;
{ID}            num_tokens++;
{INT}           num_tokens++;
{FLOAT}         num_tokens++;
{STRING}        num_tokens++;
{ARITHOP}       num_tokens++;
{RELOP}         num_tokens++;
{LOGOP}         num_tokens++;
{SEPARATOR}     num_tokens++;
{ASSIGNOP}      num_tokens++;

"/*"                    { BEGIN(comment); start_comment();}
<comment>[^*\n]*        /* eat anything that's not a '*' */
<comment>"*"+[^*/\n]*   /* eat up '*'s not followed by '/'s */
<comment>\n             /* blank lines in comments are ok */
<comment>"*"+"/"        {
                                end_comment();
                                num_comments++;
                                num_tokens++;
                                BEGIN(INITIAL);
                        }


{WHITESPACE}    {}
\n              {}
 .              { printf("Unrecognized character: %s on line: %d\n",
                         yytext, yylineno); }

%%

int main(int argc, char *argv[])
{
    yylex();
    printf("There are %d tokens /* comments are counted as tokens */\n", num_tokens);
    printf("There are %d lines\n", yylineno);
    printf("There are %d comments:\n", num_comments);

    display_comments();
    printf("\n");

    /* print out the frequency of identifiers here */
    printf("Frequency of identifiers:\n");

    return EXIT_SUCCESS;
}

/**
 * Record start of current comment within input stream (yyin). Note that the
 * scanner will already have read past the / and the *, so we have to account
 * for the difference.
 */
void start_comment()
{
     comment_start_pos[num_comments] = num_chars - 2;
}

/*
 * End of comment is exactly num_chars when this function is called. This
 * includes the * and the /.
 */
void end_comment()
{
     comment_end_pos[num_comments] = num_chars;
}

void display_comments()
{
    int i = 0;
    for (i = 0; i < num_comments; i++)
    {
        long left = comment_end_pos[i] - comment_start_pos[i];
        int current;
        assert(0 == fseek(yyin, comment_start_pos[i], SEEK_SET));
        do {
            current = getc(yyin);
            assert(EOF != current);
            assert(EOF != putchar(current));
            left -= 1;
        } while (left > 0);
        assert(EOF != putchar('\n'));
    }
}
