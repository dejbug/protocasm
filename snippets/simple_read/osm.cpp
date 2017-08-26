#include "osm.h"
#include <assert.h>

osm::typ::bh osm::io::read_bh(FILE * file)
{
	if ((size_t) ftell(file) >= common::filesize(file)) throw common::make_error<osm::err::end>("end of data");

	osm::typ::bh x;
	x.size = pb::trans::flip(pb::io::read_i4(file));

	bool more = true;
	bool seen_type = false;
	bool seen_datasize = false;
	unsigned long const mark = ftell(file);

	for (size_t i=0; more; ++i)
	{
		if (ftell(file) - mark >= x.size) break;

		pb::typ::key const key = pb::io::read_key(file);
		// ECHO2(08llx, lld, key.id);
		// ECHO2(08llx, lld, key.wt);

		switch (key.id)
		{
			default: more = false; break;

			case 1:
			{
				assert(2 == key.wt);
				seen_type = true;
				x.type = pb::io::read_string(file);
				// ECHO1(s, x.type.c_str());
				break;
			}

			case 2:
			{
				assert(2 == key.wt);
				x.indexdata = pb::io::read_string(file);
				// common::hexdump(x.indexdata.c_str(), MIN(48, x.indexdata.size()));
				break;
			}

			case 3:
			{
				assert(0 == key.wt);
				seen_datasize = true;
				x.datasize = pb::io::read_v8(file);
				// ECHO2(08lx, lu, x.datasize);
				break;
			}
		}
	}

	// ECHO2(08lx, lu, ftell(file));

	assert(ftell(file) - mark == x.size);
	assert(seen_type);
	assert(seen_datasize);

	return x;
}

osm::typ::bb osm::io::read_bb(FILE * file, pb::typ::u4 datasize)
{
	// pb::io::read_bytes(file, datasize);
	fseek(file, datasize, SEEK_CUR);
	return {};
}

void osm::dbg::dump(osm::typ::bh const & x)
{
	printf("> osm::typ::bh(size=%lu, type='%s', indexdata=%s, datasize=%lu)\n", x.size, x.type.c_str(), x.indexdata.empty() ? "null" : "'...'", x.datasize);
}

void osm::dbg::dump(osm::typ::bb const & x)
{
	printf("> osm::typ::bb(...)\n");
}
