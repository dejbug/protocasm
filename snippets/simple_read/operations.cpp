#include "operations.h"

size_t op::read(FILE * file, char * buffer, size_t size)
{
	size_t const good = fread(buffer, 1, size, file);
	if (good < size && !feof(file))
		throw make_error("File::read : error while reading from file at %08X: only %d of %d bytes read", (size_t) file, good, size);
	return good;
}

void op::dump(char const * buffer, size_t size)
{
	for (size_t i=0; i<size; ++i)
	{
		if (i > 0)
		{
			if(i % 16 == 0) printf("\n");
			else if(i % 4 == 0) printf (" ");
		}
		printf("%02X ", (unsigned char) buffer[i]);
	}
	printf("\n");
}

unsigned long op::flip_32(unsigned long value)
{
	// TODO: ws2_32.dll's htonl() does this much more
	// efficiently, using only scratch registers .

	auto x = (unsigned char const *) &value;
	return (unsigned long) (x[0] << 24) | (x[1] << 16) | (x[2] << 8) | x[3];
}

unsigned long long op::flip_64(unsigned long long value)
{
	unsigned long long out = 0;
	auto x = (unsigned char const *) &value;
	auto o = (unsigned char *) &out;
	// return (unsigned long long) (x[0] << 56) | (x[1] << 48) | (x[2] << 40) | (x[3] << 32) | (x[4] << 24) | (x[5] << 16) | (x[6] << 8) | x[7];
	o[7] = x[0];
	o[6] = x[1];
	o[5] = x[2];
	o[4] = x[3];
	o[3] = x[4];
	o[2] = x[5];
	o[1] = x[6];
	o[0] = x[7];
	return out;
}

unsigned long op::read_fixed32(FILE * file)
{
	unsigned long value = 0;
	read(file, (char *) &value, 4);
	return value;
}

unsigned long long op::read_fixed64(FILE * file)
{
	unsigned long long value = 0;
	read(file, (char *) &value, 8);
	return value;
}

char * op::read_string(FILE * file)
{
	return nullptr;
}


void op::Varint::read(FILE * file)
{
	good = op::read_varint(file, data);
}

long op::Varint::to_int32() const
{
	long value = 0;
	for (size_t i=0; i<good; ++i)
		value |= (data[i] & 0x7F) << (i*7);
	return value;
}

// long long to_int64() const;
// signed long to_sint32() const;
// signed long long to_sint64() const;
// unsigned long to_uint32() const;
// unsigned long long to_uint64() const;
// bool to_bool() const;
// int to_enum() const;
