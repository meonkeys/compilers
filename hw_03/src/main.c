/* System headers */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Custom headers */
#include <y.tab.h>
#include <lexer3.h>

int yyparse (void);
#ifdef YYDEBUG
extern int yydebug;
#endif

int
main (int argc, char *argv[])
{
    int parse_rv;
#ifdef YYDEBUG
    yydebug = 1;
#endif

    if (argc > 1) {
        yyin = fopen (argv[1], "r");
        assert (NULL != yyin);
    }

    parse_rv = yyparse ();

    if (0 == parse_rv)
        printf ("Parsing completed. No errors found.\n");
    else
        printf ("Parsing completed with errors.\n");

    if (argc > 1) {
        assert(0 == fclose (yyin));
    }

    /*
     * Not precisely as indicated by the manual, but seems to be the right
     * thing to do based on the C code generated by flex.
     */
    yylex_destroy ();

    /*
     * Successful exit code even if parsing failed. Unit test script depends
     * on this behavior.
     */
    exit(EXIT_SUCCESS);
}


int
yyerror (char const *mesg)
{
    printf ("%s\t%d\t%s\t%s\n", "Error found in Line ", yylineno,
            "next token: ", yytext);
    return 1;
}
