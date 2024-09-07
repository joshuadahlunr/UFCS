/* Definitions */

%{
std::string yyoutput;
std::string substitue_ufcs(std::string input);
%}

%option noyywrap

ws [ \t\n\r]

%% /* Actions */

(([^.;=\n]*?\.)+)([^(!\n]*?)\s*(\([^)]*\)) {
	yyoutput += substitue_ufcs(yytext);
	return 1;
}
. { yyoutput += yytext[0]; return 1; } /* Non function tokens get passed through as normal */
{ws} { yyoutput += yytext[0]; return 1; }

%% /* code */