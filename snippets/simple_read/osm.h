#ifndef _OSM_H_
#define _OSM_H_

#include "common.hpp"
#include "pb.h"

namespace osm {

namespace io {

pb::typ::string read_string(FILE * file);

} // namespace io

} // namespace osm

#endif // _OSM_H_
