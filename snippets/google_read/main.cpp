#include <assert.h>
#include <stdio.h>

#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>

#include <google/protobuf/io/zero_copy_stream_impl.h>

#include "common.hpp"
#include "fileformat.pb.h"
#include "osmformat.pb.h"

google::protobuf::uint32 flip(google::protobuf::uint32 value);
google::protobuf::uint64 flip(google::protobuf::uint64 value);

struct context
{
	int fd = -1;
	google::protobuf::io::ZeroCopyInputStream * raw_input = nullptr;
	google::protobuf::io::CodedInputStream * coded_input = nullptr;

	context(char const * path);
	virtual ~context();
};

int test_1(char const *);
int test_2(char const *);

int main()
{
	char const * path = "..\\..\\data\\Darmstadt.osm.pbf";

	return test_2(path);
	// return test_1(path);
	return 0;
}

int test_2(char const * path)
{
	context ctx(path);

	google::protobuf::uint32 bh_len = 0;
	ctx.coded_input->ReadLittleEndian32(&bh_len);
	bh_len = flip(bh_len);
	printf("- bh_len = %d\n", bh_len);

	OSMPBF::BlobHeader bh;

	if (!bh.ParseFromCodedStream(ctx.coded_input)) {
		fprintf(stderr, "Failed to parse BlobHeader.\n");
		return -1;
	}

	if (bh.has_datasize())
		printf("- bh.datasize = %d\n", bh.datasize());

	return 0;
}

int test_1(char const * path)
{
	context ctx(path);

	google::protobuf::uint32 bh_len = 0;
	ctx.coded_input->ReadLittleEndian32(&bh_len);
	bh_len = flip(bh_len);
	fprintf(stderr, "- bh_len = %d\n", bh_len);

	assert(14 == bh_len);

	google::protobuf::uint32 key_1;
	ctx.coded_input->ReadVarint32(&key_1);
	fprintf(stderr, "- key_1 = %d\n", key_1);

	assert(0x0A == key_1);

	return 0;
}

google::protobuf::uint32 flip(google::protobuf::uint32 value)
{
	auto buffer = (char * const) &value;

	return
		(static_cast<google::protobuf::uint32>(buffer[3])      ) |
		(static_cast<google::protobuf::uint32>(buffer[2]) <<  8) |
		(static_cast<google::protobuf::uint32>(buffer[1]) << 16) |
		(static_cast<google::protobuf::uint32>(buffer[0]) << 24);
}

google::protobuf::uint64 flip(google::protobuf::uint64 value)
{
	auto buffer = (char * const) &value;

	google::protobuf::uint32 const part0 =
		(static_cast<google::protobuf::uint32>(buffer[7])      ) |
		(static_cast<google::protobuf::uint32>(buffer[6]) <<  8) |
		(static_cast<google::protobuf::uint32>(buffer[5]) << 16) |
		(static_cast<google::protobuf::uint32>(buffer[4]) << 24);

	google::protobuf::uint32 const part1 =
		(static_cast<google::protobuf::uint32>(buffer[3])      ) |
		(static_cast<google::protobuf::uint32>(buffer[2]) <<  8) |
		(static_cast<google::protobuf::uint32>(buffer[1]) << 16) |
		(static_cast<google::protobuf::uint32>(buffer[0]) << 24);

	return
		static_cast<google::protobuf::uint64>(part0) |
		(static_cast<google::protobuf::uint64>(part1) << 32);
}

context::context(char const * path)
{
	GOOGLE_PROTOBUF_VERIFY_VERSION;

	fd = _open(path, _O_RDONLY | _O_SEQUENTIAL, _S_IREAD);
	if (-1 == fd) throw common::make_error("context::context : input file not found at '%s'\n", path);

	raw_input = new google::protobuf::io::FileInputStream(fd);
	coded_input = new google::protobuf::io::CodedInputStream(raw_input);
}

context::~context()
{
	delete coded_input;
	delete raw_input;
	_close(fd);

}
