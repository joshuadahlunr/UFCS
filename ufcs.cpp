#include <iostream>
#include <fstream>
#include <optional>
#include <string>
#include <string_view>
#include <argparse/argparse.hpp>
#include "tree-sitter/lib/include/tree_sitter/api.h"
#include "tree-sitter-ufcsc.h"

std::string replace(std::string text, const std::string_view search, const std::string_view replace) {
	size_t pos = 0;
	while ((pos = text.find(search, pos)) != std::string::npos) {
		text.replace(pos, search.length(), replace);
		pos += replace.length(); // Move past the replacement
	}
	return text;
}

struct UFCSProcessor {
	std::string raw;
	TSParser* parser;
	TSTree* tree;

	UFCSProcessor(std::string content) : raw(content) {
		parser = ts_parser_new();
		ts_parser_set_language(parser, tree_sitter_ufcsc());

		// Parse file content
		tree = ts_parser_parse_string(parser, nullptr, raw.c_str(), raw.length());
	}

	~UFCSProcessor() {
		ts_parser_delete(parser);
	}

	std::string process() {
		return process_node(ts_tree_root_node(tree));
	}

protected:
	TSNode ts_node_child_by_field_name(TSNode node, std::string_view name) {
		return ::ts_node_child_by_field_name(node, name.data(), name.size());
	}

	std::string process_node(TSNode node) {
		std::string out;
		std::string_view nodeType = ts_node_type(node);

		if (nodeType == "field_expression")
			out += process_field_expression(node);
		else
			out += process_default_node(node);

		return out;
	}

	std::string process_default_node(TSNode node) {
		std::string out;
		size_t start = ts_node_start_byte(node);

		for (size_t i = 0; i < ts_node_child_count(node); ++i) {
			auto child = ts_node_child(node, i);
			out += raw.substr(start, ts_node_start_byte(child) - start);
			out += process_node(child);
			start = ts_node_end_byte(child);
		}

		out += raw.substr(start, ts_node_end_byte(node) - start);
		return out;
	}

	std::string process_field_expression(TSNode node) {
		auto argumentsNode = ts_node_named_child(node, 1); // Assuming 'arguments' is the second child
		if (ts_node_is_null(argumentsNode))
			return process_default_node(node);
		return process_ufcs_call(node);
	}

	std::string process_ufcs_call(TSNode node) {
		auto nameNode = ts_node_child_by_field_name(node, "field");
		auto firstArgNode = ts_node_child_by_field_name(node, "argument");
		auto unaryOpNode = ts_node_child_by_field_name(node, "unary");
		auto operatorNode = ts_node_child_by_field_name(node, "operator");
		auto argumentsNode = ts_node_child_by_field_name(node, "arguments");

		auto name = process_node(nameNode);
		auto firstArg = process_node(firstArgNode);
		if (!ts_node_is_null(operatorNode))
			if (process_default_node(operatorNode) == "->")
				firstArg = "*(" + firstArg + ")";
		if (!ts_node_is_null(unaryOpNode))
			firstArg = process_node(unaryOpNode) + "(" + firstArg + ")";

		auto args = process_node(argumentsNode);

		return replace(name + "(" + firstArg + ", " + args.substr(args.find("(") + 1), ", )", ")");
	}
};



struct Args : public argparse::Args {
	std::string& inFile = arg("File to read");
	std::optional<std::string>& outFile = arg("File to write").set_default(std::optional<std::string>{});
	bool& namespace_emulation = kwarg("ne,namespace-emulation", "When enabled :: gets replaced with _ providing an approximation of namespaces.").set_default(false);
};

int main(int argc, char* argv[]) {
	auto args = argparse::parse<Args>(argc, argv);

	std::string content;
	if(args.inFile != ".") {
		std::ifstream fin(args.inFile);
		if(!fin) {
			std::cerr << args.inFile << " not found!" << std::endl;
			return 1;
		}

		fin.seekg(0, std::ios::end);
		size_t size = fin.tellg();
		fin.seekg(0, std::ios::beg);
		content = std::string(size, '\0');
		fin.read(content.data(), size);
	} else {
		std::string line;
		while(std::getline(std::cin, line))
			content += line + "\n";
	}

	std::ofstream fout;
	std::ostream* out;
	if(args.outFile && *args.outFile == ".")
		out = &std::cout;
	else {
		std::string outFile = args.outFile.value_or([inFile = args.inFile] {
			return replace(inFile, ".ufcs", "");
		}());

		fout = std::ofstream(outFile);
		if(!fout) {
			std::cerr << outFile << " not found!" << std::endl;
			return 1;
		}
		out = &fout;
	}

	if(args.namespace_emulation)
		(*out) << replace(UFCSProcessor(content).process(), "::", "_") << std::endl;
	else (*out) << UFCSProcessor(content).process() << std::endl;
	return 0;
}