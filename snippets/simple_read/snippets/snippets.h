#ifndef _SNIPPETS_H_
#define _SNIPPETS_H_

#include <stdio.h>
#include <string.h>
#include "common.hpp"

namespace snippets {

template<size_t N=1024>
struct Buffer
{
	unsigned char * data = new unsigned char[N + 1];
	size_t size = N;
	size_t good = 0;

	Buffer()
	{
		data[N] = 0;
	}

	virtual ~Buffer()
	{
		delete data;
	}

	void read(FILE * file, size_t size=0)
	{
		if (!size) size = this->size;
		if (size > this->size)
			throw common::make_error("Buffer::read : too many bytes: %d bytes requested, capacity is %d", size, this->size);
		good = fread(data, 1, size, file);
		if (good < size && !feof(file))
			throw common::make_error("Buffer::read : error while reading from file at%08X: only %d of %d bytes read", (size_t) file, good, size);
	}

	void dump()
	{
		for (size_t i=0; i<good; ++i)
		{
			if (i > 0)
			{
				if(i % 16 == 0) printf("\n");
				else if(i % 4 == 0) printf (" ");
			}
			printf("%02X ", data[i]);
		}
		printf("\n");
	}

	unsigned int to_int() const
	{
		if (good < 4)
			throw common::make_error("Buffer::to_int : not enough bytes: only %d available, required 4", good);
		return *(unsigned int *) data;
	}

	unsigned int to_int_be() const
	{
		if (good < 4)
			throw common::make_error("Buffer::to_int : not enough bytes: only %d available, required 4", good);
		unsigned int value = op::flip_32(*(unsigned int *) data);
		return value;
	}

	void for_each(bool (* callback)(unsigned char), size_t offset=0)
	{
		for (size_t i=offset; i<good; ++i)
			callback(data[i]);
	}
};


template<class T>
struct Varint
{
	T value = 0;
	size_t done = 0;

	template<size_t N>
	T read(Buffer<N> & buffer, size_t offset=0, size_t max_loop=64)
	{
		value = 0;

		bool need_more = true;
		done = 0;

		for (size_t i=offset; i<buffer.good && need_more; ++i, ++done)
		{
			if (i >= max_loop)
				throw common::make_error("Field::read_varint : max_loop (%d) reached", max_loop);
			unsigned char const c = buffer.data[i];
			need_more = 0x80 & c;
			value |= (0x7F & c) << (7 * done);
		}

		if (need_more)
			throw common::make_error("Field::read : premature end of buffer while reading varint: could read only %d so far (preliminary value is %llu", done, value);

		return value;
	}

	void dump()
	{
		__mingw_printf("%llu", value);
	}

	size_t get_key_wire_type() const
	{
		return value & 0x3;
	}

	size_t get_key_id() const
	{
		return value >> 3;
	}
};


struct Bytes
{
	unsigned char * data = nullptr;
	size_t size = 0;

	virtual ~Bytes()
	{
		if (data) delete data;
	}

	template<size_t N>
	void read(Buffer<N> & buffer, size_t offset=0)
	{
		this->size = 0;
		if (data) delete data;
		data = nullptr;

		Varint<unsigned long long> size;
		size.read(buffer, offset);

		this->size = size.value;
		data = new unsigned char[this->size + 1];
		data[this->size] = 0;

		memcpy(data, buffer.data + offset + size.done, this->size);
		printf("|%s|\n", data);
	}
};


struct Field
{
	void read(FILE * file)
	{
	}

	template<size_t N>
	void read(Buffer<N> & buffer, size_t offset=0)
	{
		Varint<unsigned long long> key;
		key.read(buffer, offset);
		__mingw_printf("key : %llu (wire type %d, id %d)\n", key.value, key.get_key_wire_type(), key.get_key_id());

		Bytes bytes;
		bytes.read(buffer, key.done);
	}
};


bool test(char const * path)
{
	FILE * file = fopen(path, "rb");
	if (!file) return false;

	Buffer<64> buffer;
	buffer.read(file, 4);
	printf("\n");
	buffer.dump();
	// printf("%d\n", buffer.to_int_be());

	buffer.read(file, buffer.to_int_be());
	printf("\n");
	buffer.dump();

	Field field;
	field.read(buffer);

	return true;
}


namespace osm {

struct item
{
	long mark = 0;
	lib::typ::i4 bh_len_le = 0;
	lib::typ::bytes b1;

	item(FILE * file)
	{
		lib::typ::i4 bh_len_be = 0;
		lib::io::read(file, bh_len_be);
		bh_len_le = lib::trans::flip(bh_len_be);
		ECHO2(08lx, ld, bh_len_le);

		mark = ftell(file);

		b1.read(file);
		lib::io::dump(b1);
	}

	virtual ~item()
	{
	}

	void dump()
	{
		printf(" --- osm::item --- :\n");
		ECHO2(08lx, ld, bh_len_le);
		printf(" . ---\n");
	}
};

} // namespace osm


} // namespace snippets

#endif // _SNIPPETS_H_
