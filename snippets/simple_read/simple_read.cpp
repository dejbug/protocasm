#include <stdio.h>
#include <assert.h>

#include "common.hpp"
#include "pb.h"
#include "osm.h"

int main()
{
	char const * path = "..\\..\\data\\Darmstadt.osm.pbf";
	pb::raii::file file(path);

	auto bh = osm::io::read_bh(file);
	osm::dbg::dump(bh);

	auto bb = osm::io::read_bb(file, bh.datasize);
	osm::dbg::dump(bb);

	osm::dbg::dump(osm::io::read_bh(file));

	return 0;
}
