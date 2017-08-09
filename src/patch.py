import argparse
import base64
import contextlib
import os
import re
import sys
import traceback


def create_parser(argv=sys.argv[1:]):
	parser = argparse.ArgumentParser(description="", epilog="")
	parser.add_argument("file", help="path to input file")
	parser.add_argument("seek", help="pattern to seek for")
	parser.add_argument("repl", help="pattern to replace 'seek' with")
	parser.add_argument("-b", dest="base64", action="store_true",
		help="arguments are given in base64")
	parser.add_argument("-r", dest="regex", action="store_true",
		help="arguments are regex patterns")
	parser.add_argument("-o", dest="outfile", metavar="PATH", help="print output to file instead of STDOUT")
	parser.add_argument("-f", dest="force", action="store_true", help="force overwrite of existing output file")

	args = parser.parse_args(argv)

	if not os.path.isfile(args.file):
		parser.error('file not found "%s"' % args.file)

	if not args.force and args.outfile and os.path.exists(args.outfile):
		parser.error('output file already exists "%s"' % args.outfile)

	if args.base64:

		try: args.seek = base64.b64decode(args.seek)
		except TypeError:
			traceback.print_exc()
			parser.error("argument 'seek' is not in valid base64 form")

		try: args.repl = base64.b64decode(args.repl)
		except TypeError:
			traceback.print_exc()
			parser.error("argument 'repl' is not in valid base64 form")

	# print args

	return parser, args


def subst_str(line, args):
	offset = line.find(args.seek)
	if offset >= 0:
		return line[:offset] + args.repl + line[len(args.seek)+offset:]
	return line


def subst_rex(line, args):
	return re.sub(args.seek, args.repl, line)


@contextlib.contextmanager
def open_output_file(object):

	if args.outfile:
		file = open(args.outfile, "wb")
	else:
		file = sys.stdout

	yield file

	if args.outfile:
		file.close()


if "__main__" == __name__:
	parser, args = create_parser()
	subst = subst_rex if args.regex else subst_str

	with open_output_file(args) as outfile:
		for line in open(args.file, "rb"):
			outfile.write(subst(line, args))

