#include "goo.h"


goo::infile::infile(char const * path)
{
	file = fopen(path, "rb");
}

goo::infile::~infile()
{
	fclose(file);
}

int goo::infile::Read(void * buffer, int size)
{
	if (feof(file)) return 0;
	size_t const good = fread((char *) buffer, sizeof(char), size, file);
	if (good < size && ferror(file)) return -1;
	return good;
}

int goo::infile::Skip(int count)
{
	if (feof(file)) return 0;
	long const mark = ftell(file);
	if (0 != fseek(file, count, SEEK_CUR)) return -1;
	return (int) (ftell(file) - mark);
}


goo::context::context(char const * path)
{
	GOOGLE_PROTOBUF_VERIFY_VERSION;

	raw_input = new infile(path);
	raw_input_adaptor = new goo::CopyingInputStreamAdaptor(raw_input);
	coded_input = new goo::CodedInputStream(raw_input_adaptor);
}

goo::context::~context()
{
	delete coded_input;
	delete raw_input_adaptor;
	delete raw_input;
}


goo::uint32 goo::flip(goo::uint32 value)
{
	auto buffer = (char * const) &value;

	return
		(static_cast<goo::uint32>(buffer[3])      ) |
		(static_cast<goo::uint32>(buffer[2]) <<  8) |
		(static_cast<goo::uint32>(buffer[1]) << 16) |
		(static_cast<goo::uint32>(buffer[0]) << 24);
}

goo::uint64 goo::flip(goo::uint64 value)
{
	auto buffer = (char * const) &value;

	goo::uint32 const part0 =
		(static_cast<goo::uint32>(buffer[7])      ) |
		(static_cast<goo::uint32>(buffer[6]) <<  8) |
		(static_cast<goo::uint32>(buffer[5]) << 16) |
		(static_cast<goo::uint32>(buffer[4]) << 24);

	goo::uint32 const part1 =
		(static_cast<goo::uint32>(buffer[3])      ) |
		(static_cast<goo::uint32>(buffer[2]) <<  8) |
		(static_cast<goo::uint32>(buffer[1]) << 16) |
		(static_cast<goo::uint32>(buffer[0]) << 24);

	return
		static_cast<goo::uint64>(part0) |
		(static_cast<goo::uint64>(part1) << 32);
}

goo::uint32 goo::read_bh_len(goo::context & ctx)
{
	goo::uint32 bh_len = 0;
	ctx.coded_input->ReadLittleEndian32(&bh_len);
	return goo::flip(bh_len);
}

std::unique_ptr<goo::uint8> goo::read_bh_mem(goo::context & ctx, goo::uint32 bh_len)
{
	std::unique_ptr<goo::uint8> buffer(new goo::uint8[bh_len]);
	ctx.coded_input->ReadRaw(buffer.get(), bh_len);
	return buffer;
}

OSMPBF::BlobHeader goo::read_bh(goo::context & ctx, goo::uint32 bh_len)
{
	OSMPBF::BlobHeader bh;

	auto buffer = goo::read_bh_mem(ctx, bh_len);
	// common::hexdump(buffer.get(), bh_len);

	if (!bh.ParseFromArray(buffer.get(), bh_len))
		throw common::make_error("failed to parse BlobHeader from memory");

	assert(bh.has_type());
	assert(bh.has_datasize());

	return bh;
}

OSMPBF::BlobHeader goo::read_bh(goo::context & ctx)
{
	OSMPBF::BlobHeader bh;

	if (!bh.ParseFromCodedStream(ctx.coded_input))
		throw common::make_error("failed to parse BlobHeader");

	assert(bh.has_type());
	assert(bh.has_datasize());

	return bh;
}
