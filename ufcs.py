import tree_sitter_ufcsc as tsufcsc
from tree_sitter import Language, Parser
import argparse, os

parser = argparse.ArgumentParser(
                    prog='UFCS C Preprocessor',
                    description='Converts UFCS C files into C++ files')
parser.add_argument("filename")
parser.add_argument("-o", "--output", required=False)
parser.add_argument("-p", "--print", action='store_true')
args = parser.parse_args()
filedir = os.path.dirname(os.path.abspath(args.filename))
filename = os.path.basename(args.filename)
target = args.output
if target is None and not args.print: target = os.path.join(filedir, filename.replace(".ufcs.c", ".c").replace(".ufcs.h", ".h"))


UFCSC_LANGUAGE = Language(tsufcsc.language())

prototypes = []

parser = Parser(UFCSC_LANGUAGE)

with open(args.filename) as f:
	raw = f.read().encode("utf8")
	tree = parser.parse(raw)


def process(node):

	out = ""
	match node.type:
		case "field_expression":
			out += process_field_expression(node)

		# By default just print whatever parts of this node are unique and recursively process the children
		case other:
			out += process_default_node(node)

	return out

def process_default_node(node):
	out = ""
	start = node.start_byte
	for child in node.children:
		out += raw[start:child.start_byte].decode("utf8")
		out += process(child)
		start = child.end_byte
	out += raw[start:node.end_byte].decode("utf8")
	return out


def process_field_expression(node):
	if node.child_by_field_name("arguments") is None: return process_default_node(node)
	return process_ufcs_call(node)

def process_ufcs_call(node):
	name = process(node.child_by_field_name('field'))
	firstArg = process(node.child_by_field_name('argument'))
	unaryOp = node.child_by_field_name('unary')
	if process(node.child_by_field_name('operator')) == "->": firstArg = f"*({firstArg})"
	if unaryOp is not None: firstArg = f"{process(unaryOp)}({firstArg})"

	args = process(node.child_by_field_name('arguments'))
	args = args[args.find("(") + 1:]
	return f"{name}({firstArg}, {args.lstrip()}".replace(", )", ")")



result = process(tree.root_node)

if target is not None:
	with open(target, "w") as f:
		f.write(result)
if args.print: print(result)
