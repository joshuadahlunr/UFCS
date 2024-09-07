#include <iostream>

#include <fstream>
#include <iostream>

#ifdef _MSC_VER 
#define YY_NO_UNISTD_H
#include <io.h>
#endif

#include "gen/scanner.h"

#include <ctre.hpp>
#include <argparse/argparse.hpp>

struct Args : public argparse::Args {
    std::string& inFile = arg("File to read");
    std::optional<std::string>& outFile = arg("File to write").set_default(std::optional<std::string>{});
};

std::string substitue_ufcs(std::string content) {
    auto match = ctre::match<R"_(((?:[^.;=\n]*?\.)+)([^(!\n]*?)\s*(\([^)]*\)|\[[^\]]*\]))_">(content);
    if(!match.data()) return content;

    auto arg1 = match.get<1>().to_string();
    size_t arg1Start = arg1.find_first_not_of(" \t\n\r");
    auto spacing = arg1.substr(0, arg1Start);
    arg1 = substitue_ufcs(arg1.substr(arg1Start, arg1.size() - arg1Start - 1));
    auto function = match.get<2>().to_string();
    auto arguments = match.get<3>().to_string();
    arguments = arguments.substr(arguments.find("(") + 1);
    
    std::string result = spacing + function + "(";
    if(arguments[arguments.find_first_not_of(" \t\n\r")] != ')')
        return result + arg1 + ", " + arguments;
    else return result + arg1 + ")";
}

std::string process_input(std::string content) {
	yyoutput.clear();
	yy_scan_string(content.c_str());
	while(yylex());
	return yyoutput;
}

std::string replace_dotbang(std::string content) {
	size_t pos = 0;
    while ((pos = content.find(".!", pos)) != std::string::npos) {
        content.replace(pos, 2, ".");
        pos += 2; // Move past the replaced part
    }
	return content;
}

int main(int argc, char* argv[]) {
    auto args = argparse::parse<Args>(argc, argv);

    std::ifstream fin(args.inFile);
    if(!fin) {
        std::cerr << args.inFile << " not found!" << std::endl;
        return 1;
    }
    std::ofstream fout(args.outFile.value_or([if_ = args.inFile] {
		auto inFile = if_;
        size_t lastIndex = inFile.find_last_of(".ufcs"); 
        inFile.erase(lastIndex - 5, 5); 
		return inFile;
    }()));

    fin.seekg(0, std::ios::end);
    size_t size = fin.tellg();
    fin.seekg(0, std::ios::beg);
    std::string content(size, '\0');
    fin.read(content.data(), size);

    fout << replace_dotbang(process_input(content));
	return 0;
}