#ifndef _OSM_H_
#define _OSM_H_

#include "common.hpp"
#include "pb.h"

namespace osm {

namespace err {

DEFERR(end);

} // namespace err

namespace typ {

// message BlobHeader {
// 	required string type = 1;
// 	optional bytes indexdata = 2;
// 	required int32 datasize = 3; }
struct bh
{
	pb::typ::u4 size;
	pb::typ::string type;
	pb::typ::string indexdata;
	pb::typ::u4 datasize;
};

// message Blob {
// 	optional bytes raw = 1;
// 	optional int32 raw_size = 2;
// 	optional bytes zlib_data = 3;
// 	optional bytes lzma_data = 4;
// 	optional bytes OBSOLETE_bzip2_data = 5 [deprecated=true]; }
struct bb
{
	pb::typ::string raw;
	pb::typ::u4 raw_size;
	pb::typ::string zlib_data;
	pb::typ::string lzma_data;	// Support is NOT REQUIRED.
	pb::typ::string bzip2_data;	// DEPRECATED since 2010.
};

} // namespace typ

namespace io {

osm::typ::bh read_bh(input::iinput &);
osm::typ::bb read_bb(input::iinput &, pb::typ::u4 datasize);

} // namespace io

namespace dbg {

void dump(typ::bh const &);
void dump(typ::bb const &);

} // namespace dbg

} // namespace osm

#endif // _OSM_H_
