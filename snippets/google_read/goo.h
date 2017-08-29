#ifndef _GOO_H_
#define _GOO_H_

#include <stdio.h>
#include <memory>

#include <google/protobuf/io/zero_copy_stream_impl_lite.h>
#include <google/protobuf/io/coded_stream.h>

#include "fileformat.pb.h"
#include "osmformat.pb.h"

#include "common.hpp"


namespace goo {

using uint8 = google::protobuf::uint8;
using uint32 = google::protobuf::uint32;
using uint64 = google::protobuf::uint64;

using int8 = google::protobuf::int8;
using int32 = google::protobuf::int32;
using int64 = google::protobuf::int64;

using CopyingInputStream = google::protobuf::io::CopyingInputStream;
using CopyingInputStreamAdaptor = google::protobuf::io::CopyingInputStreamAdaptor;
using CodedInputStream = google::protobuf::io::CodedInputStream;

struct infile : public CopyingInputStream
{
	FILE * file = nullptr;

	infile(char const * path);
	virtual ~infile();

	virtual int Read(void * buffer, int size);
	virtual int Skip(int count);
};

struct context
{
	infile * file = nullptr;
	CopyingInputStreamAdaptor * raw_input_adaptor = nullptr;
	CodedInputStream * coded_input = nullptr;

	context(char const * path);
	virtual ~context();
};

uint32 flip(uint32 value);
uint64 flip(uint64 value);

bool more(context & ctx);
std::string read_raw(context & ctx, uint32 size);
std::unique_ptr<uint8> read_mem(goo::context & ctx, goo::uint32 size);
void dump_ahead(context & ctx, uint32 size);
uint32 read_bh_len(context & ctx);
OSMPBF::BlobHeader read_bh(context & ctx, std::string const & buf);
OSMPBF::BlobHeader read_bh(context & ctx, uint32 bh_len = 0);
void skip_bb(context & ctx, int32 datasize);

}

#endif // _GOO_H_
