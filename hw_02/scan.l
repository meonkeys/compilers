%option noyywrap
%top{
int num_lines = 0, num_chars = 0;
}

%%

\n      ++num_lines; ++num_chars;
.       ++num_chars;

%%

int
main(int argc, char *argv[])
{
    yylex();
    printf( "# of lines = %d, # of chars = %d\n",
            num_lines, num_chars );
    return 0;
}
