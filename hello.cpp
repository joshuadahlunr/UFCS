#include <iostream>

#ifdef _MSC_VER 
#define YY_NO_UNISTD_H
#include <io.h>
#endif

#include "gen/parser.h"
#include "gen/scanner.h"

void flexTest(std::string string) {
	yy_scan_string(string.c_str());
	int	token;

	while((token=yylex()) != 0){
		switch(token){
			break; case PLUS: std::cout << "+" << std::endl;
			break; case MINUS: std::cout << "-" << std::endl;
			break; case MULT: std::cout << "*" << std::endl;
			break; case DIV: std::cout << "/" << std::endl;
			break; case SEMI: std::cout << ";" << std::endl;
			break; case OPEN: std::cout << "(" << std::endl;
			break; case CLOSE: std::cout << ")" << std::endl;
			break; case INTEGER: std::cout << yylval << std::endl;
			break; default: std::cout << "!ERROR!" << std::endl;
		}
	}
}

void bisonTest(std::string string) {
	yy_scan_string(string.c_str());
	yyparse();
}

int main() {
	flexTest("2 + 2; 4 / 2; 5 - 6;");
	bisonTest("2 + 2; 4 / 2; 5 - 6;");

	// std::cout << "Hello World" << std::endl;
}

void yyerror(const char* s) {
	std::cerr << s << std::endl;
}