#include <stdio.h>
#include <string.h>

#include "lib.h"

void test_1(lib::io::file & file)
{
	lib::typ::bytes bytes(64);
	bytes.read(file, 64);
	lib::io::dump(bytes);
	rewind(file);
}

void test_2(lib::io::file & file)
{
	auto bytes_2 = lib::io::read(file, 64);
	lib::io::dump(bytes_2);
	rewind(file);
}

void test_3(lib::io::file & file)
{
	lib::typ::i4 bh_len_be = 0;
	lib::io::read(file, bh_len_be);
	ECHO2(08lx, ld, bh_len_be);
	lib::typ::i4 bh_len_le = lib::trans::flip(bh_len_be);
	ECHO2(08lx, ld, bh_len_le);
}

int main()
{
	char const * path = "..\\..\\data\\Darmstadt.osm.pbf";

	lib::io::file file(path);

	// test_1(file);
	test_2(file);
	test_3(file);



	return 0;

	// /// OPEN "..."
	// raii::ifile file(path);

	// return 0;

	// raii::buffer buf(64);
	// buf.read(file, 64);
	// common::dump(buf);

	// auto x = 0x1234000000000000LL;
	// ECHO2(016llx, lld, x);
	// auto y = io::flip64(x);
	// ECHO2(016llx, lld, y);

	// fseek(file, 0, SEEK_SET);

	// {
	// 	long const bh_len_be = io::read32(file);
	// 	ECHO2(08lx, ld, bh_len_be);
	// 	long const bh_len_le = io::flip32(bh_len_be);
	// 	ECHO2(08lx, ld, bh_len_le);
	// }

	// return 0;


	// /// blobheader_size = READ FIXED32 AS BigEndian
	// unsigned int const blobheader_size = op::flip_32(op::read_fixed32(file));
	// printf("blobheader_size = %d\n", blobheader_size);

	// /// We must only read the next {blobheader_size} bytes, so we
	// /// set a mark at the current file pos and check against it after
	// /// every read operation to see whether we have exceeded the
	// /// blobheader's advertised size . If so, this would indicate an
	// /// unexpected error .

	// /// mark = MARK
	// op::Mark mark(file);
	// mark.set();

	// while (mark < blobheader_size)
	// {
	// 	/// varint = READ VARINT
	// 	op::Varint varint;
	// 	varint.read(file);

	// 	/// key = varint AS KEY
	// 	op::Key key = varint.to_key();
	// 	/// DUMP key
	// 	printf("key id %d wire type %d\n", key.id, key.wt);

	// 	switch (key.wt)
	// 	{
	// 		case 3:
	// 		{
	// 			std::string s = op::read_string(file);
	// 			printf("string |%s|\n", s.c_str());
	// 			break;
	// 		}
	// 	}
	// }

	// /// FAIL IF MARK - mark >= blobheader_size
	// if (mark >= blobheader_size)
	// 	throw common::make_error("read past hard limit: either our varint-reading logic has a bug, or the input file's generating application is at fault");

	return 0;
}
