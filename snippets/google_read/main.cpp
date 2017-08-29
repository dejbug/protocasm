#include <assert.h>
#include <stdio.h>
#include <memory>

#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>

#include <google/protobuf/io/zero_copy_stream_impl_lite.h>

#include "common.hpp"
#include "fileformat.pb.h"
#include "osmformat.pb.h"

google::protobuf::uint32 flip(google::protobuf::uint32 value);
google::protobuf::uint64 flip(google::protobuf::uint64 value);

struct infile : public google::protobuf::io::CopyingInputStream
{
	FILE * file = nullptr;

	infile(char const * path)
	{
		file = fopen(path, "rb");
	}

	virtual ~infile()
	{
		fclose(file);
	}

	virtual int Read(void * buffer, int size)
	{
		if (feof(file)) return 0;
		size_t const good = fread((char *) buffer, sizeof(char), size, file);
		if (good < size && ferror(file)) return -1;
		return good;
	}

	virtual int Skip(int count)
	{
		if (feof(file)) return 0;
		long const mark = ftell(file);
		if (0 != fseek(file, count, SEEK_CUR)) return -1;
		return (int) (ftell(file) - mark);
	}
};

struct context
{
	int fd = -1;
	google::protobuf::io::CopyingInputStream * raw_input = nullptr;
	google::protobuf::io::CopyingInputStreamAdaptor * raw_input_adaptor = nullptr;
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

google::protobuf::uint32 read_bh_len(context & ctx)
{
	google::protobuf::uint32 bh_len = 0;
	ctx.coded_input->ReadLittleEndian32(&bh_len);
	return flip(bh_len);
}

std::unique_ptr<google::protobuf::uint8> read_bh_mem(context & ctx, google::protobuf::uint32 bh_len)
{
	std::unique_ptr<google::protobuf::uint8> buffer(new google::protobuf::uint8[bh_len]);
	ctx.coded_input->ReadRaw(buffer.get(), bh_len);
	return buffer;
}

OSMPBF::BlobHeader read_bh(context & ctx, google::protobuf::uint32 bh_len)
{
	OSMPBF::BlobHeader bh;

	auto buffer = read_bh_mem(ctx, bh_len);
	// common::hexdump(buffer.get(), bh_len);

	if (!bh.ParseFromArray(buffer.get(), bh_len))
		throw common::make_error("failed to parse BlobHeader from memory");

	assert(bh.has_type());
	assert(bh.has_datasize());

	return bh;
}

OSMPBF::BlobHeader read_bh(context & ctx)
{
	OSMPBF::BlobHeader bh;

	if (!bh.ParseFromCodedStream(ctx.coded_input))
		throw common::make_error("failed to parse BlobHeader");

	assert(bh.has_type());
	assert(bh.has_datasize());

	return bh;
}

int test_2(char const * path)
{
	context ctx(path);

	auto bh_len = read_bh_len(ctx);
	printf("- bh_len = %d\n", bh_len);

	// OSMPBF::BlobHeader bh = read_bh(ctx, bh_len);
	OSMPBF::BlobHeader bh = read_bh(ctx);

	printf("- bh.type = '%s'\n", bh.type().c_str());
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

	raw_input = new infile(path);
	raw_input_adaptor = new google::protobuf::io::CopyingInputStreamAdaptor(raw_input);
	coded_input = new google::protobuf::io::CodedInputStream(raw_input_adaptor);
}

context::~context()
{
	delete coded_input;
	delete raw_input_adaptor;
	delete raw_input;
	_close(fd);

}
