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
	int fd = -1;
	CopyingInputStream * raw_input = nullptr;
	CopyingInputStreamAdaptor * raw_input_adaptor = nullptr;
	CodedInputStream * coded_input = nullptr;

	context(char const * path);
	virtual ~context();
};

uint32 flip(uint32 value);
uint64 flip(uint64 value);

uint32 read_bh_len(context & ctx);
std::unique_ptr<uint8> read_bh_mem(context & ctx, uint32 bh_len);
OSMPBF::BlobHeader read_bh(context & ctx, uint32 bh_len);
OSMPBF::BlobHeader read_bh(context & ctx);

}

#endif // _GOO_H_
