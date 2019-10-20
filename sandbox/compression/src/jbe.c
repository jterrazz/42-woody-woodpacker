#include "jbe.h"

typedef unsigned char u8;
typedef unsigned int u32;

/*
 * the JBE header is 16 bytes len
 */
struct JbeHeader {
	u8 magic[4];
	u32 uncompressed_len;
	u32 data_array_len;
	u32 map_array_off;
	// right array len is relative to uncompressed_len
};

/*
 * 0            16           [...]       map_array_off    E
 * +------------+-----------------------------+-----------+
 * + JBE HEADER |          DATA ARRAY         | MAP ARRAY |
 * +------------+-----------------------------+-----------+
 */

#define JBE_MAGIC "_JBE"

void *jbe_encode(const void *data, size_t *len)
{
	(void)len;
	return (void *)data;
}

void *jbe_decode(const void *data, size_t *len)
{
	(void)len;
	return (void *)data;
}
