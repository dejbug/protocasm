#include <assert.h>
#include <stdio.h>
#include <zlib.h>
#include <string>
#include "common.hpp"

namespace idx {

std::string read_file(char const * path);
std::string inflate(std::string const &, size_t);

}

int main(int argc, char ** argv)
{
	char const * path = "OSMData.raw-613641.st-5189.(0)-relations-4006.blob";
	std::string zip = idx::read_file(path);
	size_t raw_size = 613641;
	std::string raw = idx::inflate(zip, raw_size);
	printf("--- zip ---\n");
	common::hexdump(zip.data(), 32);
	printf("--- raw ---\n");
	common::hexdump(raw.data(), 32);
	return 0;
}

std::string idx::read_file(char const * path)
{
	FILE * file = fopen(path, "rb");
	if (!file) throw common::make_error("idx::read_file : no such file \"%s\".", path);

	fseek(file, 0, SEEK_END);
	size_t const size = ftell(file);
	fseek(file, 0, SEEK_SET); // rewind(file);

	std::string data;
	data.resize(size);
	assert(data.size() == size);

	size_t const good = fread((char *) data.data(), sizeof(char), data.size(), file);
	if (good != data.size()) throw common::make_error("idx::read_file : error while reading file \"%s\"; only %d of %d were read.", path, good, size);

	fclose(file);
	return data;
}

std::string idx::inflate(std::string const & data, size_t raw_size)
{
	std::string raw;
	raw.resize(raw_size);

	z_stream strm;
	memset(&strm, 0, sizeof(z_stream));

	strm.next_in = (Bytef *) data.data();
	strm.avail_in = data.size();
	strm.next_out = (Bytef *) raw.data();
	strm.avail_out = raw.capacity();

	int const ok_init = inflateInit(&strm);

	if (Z_VERSION_ERROR == ok_init) throw common::make_error("goo::inflate_zlib : wrong zlib version.");
	else if (Z_STREAM_ERROR == ok_init) throw common::make_error("goo::inflate_zlib : invalid argument to inflateInit.");
	else if (Z_MEM_ERROR == ok_init) throw common::make_error("goo::inflate_zlib : not enough memory to inflate.");

	// loop: int const ok_inflate = inflate(&strm, Z_NO_FLUSH);
	int const ok_inflate = inflate(&strm, Z_FINISH);

	switch (ok_inflate)
	{
		default: case Z_STREAM_END: /* ok */ break;
		// case Z_OK: goto loop; break;
		case Z_OK: throw common::make_error("goo::inflate_zlib : inflate ended prematurely: needed to keep calling until finished.");
		case Z_DATA_ERROR: throw common::make_error("goo::inflate_zlib : data is either not conforming to the zlib format or has incorrect check value '%s': needed to call inflateSync() and keep calling to attempt partial recovery.", strm.msg ? strm.msg : "");
		case Z_STREAM_ERROR: throw common::make_error("goo::inflate_zlib : unexpected error while inflating: was stream accessed by another thread?");
		case Z_MEM_ERROR: throw common::make_error("goo::inflate_zlib : out of memory while inflating");
		case Z_BUF_ERROR: throw common::make_error("goo::inflate_zlib : output buffer was not large enough for inflate: needed to resize output buffer and keep calling until finished.");
	}

	if (Z_OK != inflateEnd(&strm)) throw common::make_error("goo::inflate_zlib : error while concluding inflate: '%s'.", strm.msg ? strm.msg : "");

	return raw;
}
