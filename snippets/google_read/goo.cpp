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

	file = new infile(path);
	raw_input_adaptor = new goo::CopyingInputStreamAdaptor((goo::CopyingInputStream *) file);
	coded_input = new goo::CodedInputStream(raw_input_adaptor);
}

goo::context::~context()
{
	delete coded_input;
	delete raw_input_adaptor;
	delete file;
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

bool goo::more(goo::context & ctx)
{
	void const * ptr = nullptr;
	int size = 0;
	if (!ctx.coded_input->GetDirectBufferPointer(&ptr, &size)) return false;
	return size > 0;
}

std::string goo::read_raw(goo::context & ctx, goo::uint32 size)
{
	std::string bytes;
	bytes.resize(size);
	ctx.coded_input->ReadRaw((goo::uint8 *) bytes.data(), size);
	return bytes;
}

std::unique_ptr<goo::uint8> goo::read_mem(goo::context & ctx, goo::uint32 size)
{
	std::unique_ptr<goo::uint8> bytes(new goo::uint8[size]);
	ctx.coded_input->ReadRaw(bytes.get(), size);
	return bytes;
}

void goo::dump_ahead(goo::context & ctx, goo::uint32 max_len)
{
	void const * ptr = nullptr;
	int size = 0;
	printf("%08X : \n", ctx.coded_input->CurrentPosition());
	if (ctx.coded_input->GetDirectBufferPointer(&ptr, &size))
		common::hexdump(ptr, MIN(size, max_len));
}

goo::uint32 goo::read_bh_len(goo::context & ctx)
{
	goo::uint32 bh_len = 0;
	ctx.coded_input->ReadLittleEndian32(&bh_len);
	return goo::flip(bh_len);
}

OSMPBF::BlobHeader goo::read_bh(goo::context & ctx, std::string const & buf)
{
	OSMPBF::BlobHeader bh;

	if (!bh.ParseFromArray(buf.data(), buf.size()))
		throw common::make_error("goo::read_bh : failed to parse BlobHeader from memory");

	assert(bh.has_type());
	assert(bh.has_datasize());

	return bh;
}

OSMPBF::BlobHeader goo::read_bh(goo::context & ctx, goo::uint32 bh_len)
{
	OSMPBF::BlobHeader bh;
	CodedInputStream::Limit limit = -1;

	if (bh_len > 0) limit = ctx.coded_input->PushLimit(bh_len);
	bool const ok = bh.ParseFromCodedStream(ctx.coded_input);
	if (bh_len > 0) ctx.coded_input->CheckEntireMessageConsumedAndPopLimit(limit);

	if (!ok)
	{
		if (bh_len > 0) throw common::make_error("goo::read_bh : failed to parse BlobHeader (with limit %d)", bh_len);
		else throw common::make_error("goo::read_bh : failed to parse BlobHeader (without limit)");
	}

	assert(bh.has_type());
	assert(bh.has_datasize());

	return bh;
}

void goo::skip_bb(goo::context & ctx, goo::int32 datasize)
{
	ctx.coded_input->Skip(datasize);
}
