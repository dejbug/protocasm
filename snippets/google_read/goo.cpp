#include "goo.h"
#include <zlib.h>
#include <string>
#include <sstream>
#include <time.h>



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
	if (good < (size_t) size && ferror(file)) return -1;
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
	printf("- zlibVersion = '%s'\n", zlibVersion());

	file = new infile(path);
	raw_input_adaptor = new goo::CopyingInputStreamAdaptor((goo::CopyingInputStream *) file);
	coded_input = new goo::CodedInputStream(raw_input_adaptor);
}

goo::context::~context()
{
	delete coded_input;
	delete raw_input_adaptor;
	delete file;

	google::protobuf::ShutdownProtobufLibrary();
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
	if (!ctx.coded_input->GetDirectBufferPointer(&ptr, &size)) return;
	if (size < 0) return;
	common::hexdump(ptr, MIN((goo::uint32) size, max_len));
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

OSMPBF::Blob goo::read_bb(goo::context & ctx, goo::int32 datasize)
{
	assert(datasize > 0);

	OSMPBF::Blob bb;
	CodedInputStream::Limit limit = -1;

	limit = ctx.coded_input->PushLimit(datasize);
	bool const ok = bb.ParseFromCodedStream(ctx.coded_input);
	ctx.coded_input->CheckEntireMessageConsumedAndPopLimit(limit);

	if (!ok) throw common::make_error("goo::read_bb : failed to parse Blob (with limit %d)", datasize);

	return bb;
}

std::string goo::describe_bb(OSMPBF::BlobHeader const & bh, OSMPBF::Blob const & bb)
{
	std::ostringstream ss;
	ss << "- [ " << bh.type();
	if (bb.has_raw()) ss << " raw " << bh.datasize();
	else if (bb.has_zlib_data())
	{
		ss << " zlib " << bh.datasize();
		if (bb.has_raw_size()) ss << " (" << bb.raw_size() << ")";
	}
	else ss << " ? " << bh.datasize();
	ss << " ]";

	return ss.str();
}

void goo::inflate_zlib(OSMPBF::Blob & bb)
{
	if (bb.has_raw()) return;

	if (!bb.has_zlib_data()) throw common::make_error("goo::inflate_zlib : could not find zlib compressed data in Blob.");
	if (!bb.has_raw_size()) throw common::make_error("goo::inflate_zlib : could not find uncompressed data size for zlib compressed data in Blob.");

	std::string raw;
	raw.resize(bb.raw_size());

	z_stream strm;
	memset(&strm, 0, sizeof(z_stream));

	strm.next_in = (Bytef *) bb.zlib_data().data();
	strm.avail_in = bb.zlib_data().size();
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

	bb.clear_zlib_data();
	bb.set_raw(raw);
}

OSMPBF::HeaderBlock goo::read_hb(OSMPBF::Blob const & bb)
{
	OSMPBF::HeaderBlock hb;

	if (!bb.has_raw()) throw common::make_error("goo::read_hb : could not find raw Blob data: need to inflate zlib data?");

	if (!hb.ParseFromString(bb.raw())) throw common::make_error("goo::read_hb : failed to parse HeaderBlock from Blob.");

	return hb;
}

OSMPBF::PrimitiveBlock goo::read_pb(OSMPBF::Blob const & bb)
{
	OSMPBF::PrimitiveBlock hb;

	if (!bb.has_raw()) throw common::make_error("goo::read_pb : could not find raw Blob data: need to inflate zlib data?");

	if (!hb.ParseFromString(bb.raw())) throw common::make_error("goo::read_pb : failed to parse PrimitiveBlock from Blob.");

	return hb;
}

std::string goo::describe_pb(OSMPBF::PrimitiveBlock const & pb)
{
	std::ostringstream ss;

	ss << "- [ PrimitiveBlock || pg = " << pb.primitivegroup_size() << " |";

	for (int i = 0; i < pb.primitivegroup_size(); ++i)
	{
		if (i > 0) ss << " | ";

		OSMPBF::PrimitiveGroup const & pg = pb.primitivegroup(i);

		ss << " " << i << ":";
		if (pg.nodes_size() > 0) ss << " nodes = " << pg.nodes_size();
		else if (pg.ways_size() > 0) ss << " ways = " << pg.ways_size();
		else if (pg.relations_size() > 0) ss << " relations = " << pg.relations_size();
		else if (pg.changesets_size() > 0) ss << " changesets = " << pg.changesets_size();
		else if (pg.has_dense() > 0) ss << " has_dense";
		else ss << " -?-";
	}

	ss << " ||";

	if (pb.has_stringtable())
	{
		OSMPBF::StringTable const & st = pb.stringtable();
		ss << " st = " << st.s_size();
	}

	ss << " ]";

	return ss.str();
}

bool goo::file_exists(char const * path)
{
	FILE * file = fopen(path, "rb");
	if (!file) return false;
	fclose(file);
	return true;
}

std::string goo::make_path(char const * dir, char const * name)
{
	if (!dir || !*dir)
	{
		if (!name || !*name) return "";
		else return name;
	}

	if (!name || !*name) return dir;

	size_t const dir_len = strlen(dir);

	if ('\\' != dir[dir_len-1] && '\\' != name[0])
	{
		return std::string(dir) + "\\" + name;
	}

	if ('\\' == dir[dir_len-1] && '\\' == name[0])
	{
		return std::string(dir) + (name + 1);
	}

	return std::string(dir) + name;
}

std::string goo::get_random_hexstring(size_t len, bool upper_case)
{
	srand((unsigned) time(nullptr));

	char const A = upper_case ? 'A' : 'a';
	char const F = upper_case ? 'F' : 'f';

	std::ostringstream ss;

	for (size_t i = 0; i < len; ++i)
		ss << (char const) (RAND(0, 1) ? RAND('0', '9') : RAND(A, F));

	return ss.str();
}

std::string goo::get_unique_filename(char const * dir, size_t len)
{
	size_t const max_tries = 64;

	for (size_t i = 0; i < max_tries; ++i)
	{
		std::string const name = (dir && *dir) ? goo::make_path(dir, goo::get_random_hexstring(len).c_str()) : goo::get_random_hexstring(len);
		if (!goo::file_exists(name.c_str())) return name;
	}

	throw common::make_error("goo::get_unique_filename : max attempts exceeded; tried %d random names, all names were already present in destination folder \"%s\".", max_tries, dir);
}

void goo::write_zlib(OSMPBF::Blob const & bb, char const * path)
{
	if (goo::file_exists(path)) throw common::make_error("goo::write_zlib : a file already exists at \"%s\": must not overwrite", path);
	FILE * file = fopen(path, "wb");
	size_t const good = fwrite(bb.zlib_data().data(), 1, bb.zlib_data().size(), file);
	int const err = ferror(file);
	fclose(file);
	if (good != bb.zlib_data().size())
		throw common::make_error("goo::write_zlib : an error occurred while writing to \"%s\": ferror code %08X (%d)", path, err, err);
}

std::string goo::get_pb_type_str(OSMPBF::PrimitiveBlock const & pb)
{
	std::ostringstream ss;

	if (pb.has_stringtable())
	{
		OSMPBF::StringTable const & st = pb.stringtable();
		ss << "st-" << st.s_size() << ".";
	}

	for (int i = 0; i < pb.primitivegroup_size(); ++i)
	{
		if (i > 0) ss << ".";

		OSMPBF::PrimitiveGroup const & pg = pb.primitivegroup(i);

		ss << "(" << i << ")-";
		if (pg.nodes_size() > 0) ss << "nodes-" << pg.nodes_size();
		else if (pg.ways_size() > 0) ss << "ways-" << pg.ways_size();
		else if (pg.relations_size() > 0) ss << "relations-" << pg.relations_size();
		else if (pg.changesets_size() > 0) ss << "changesets-" << pg.changesets_size();
		else if (pg.has_dense() > 0) ss << "dense";
	}

	return ss.str();
}
