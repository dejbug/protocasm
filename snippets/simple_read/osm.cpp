#include "osm.h"
#include <assert.h>

osm::typ::bh osm::io::read_bh(input::iinput & iin)
{
	if (!iin.more()) throw common::make_error<osm::err::end>("end of data");

	osm::typ::bh x;
	x.size = pb::trans::flip(pb::io::read_i4(iin));

	input::heapinput hin(x.size);
	iin.read(hin.data, hin.data_size);

	bool more = true;
	bool seen_type = false;
	bool seen_datasize = false;

	for (size_t i=0; more; ++i)
	{
		if (!hin.more()) break;

		pb::typ::key const key = pb::io::read_key(hin);
		// ECHO2(08llx, lld, key.id);
		// ECHO2(08llx, lld, key.wt);

		switch (key.id)
		{
			default: more = false; break;

			case 1:
			{
				assert(2 == key.wt);
				seen_type = true;
				x.type = pb::io::read_string(hin);
				// ECHO1(s, x.type.c_str());
				break;
			}

			case 2:
			{
				assert(2 == key.wt);
				x.indexdata = pb::io::read_string(hin);
				// common::hexdump(x.indexdata.c_str(), MIN(48, x.indexdata.size()));
				break;
			}

			case 3:
			{
				assert(0 == key.wt);
				seen_datasize = true;
				x.datasize = pb::io::read_v8(hin);
				// ECHO2(08lx, lu, x.datasize);
				break;
			}
		}
	}

	// ECHO2(08lx, lu, ftell(file));

	// assert(ftell(file) - mark == x.size);
	assert(seen_type);
	assert(seen_datasize);

	return x;
}

osm::typ::bb osm::io::read_bb(input::iinput & iin, pb::typ::u4 datasize)
{
	// pb::io::read_bytes(file, datasize);
	iin.skip(datasize);
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
