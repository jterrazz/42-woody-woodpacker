/*
 * Jbit encoding (JBE)
 * see https://arxiv.org/pdf/1209.1045.pdf
 */
#include "jbe.h"

#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

typedef unsigned char u8;
typedef unsigned int u32;

/*
 * the JBE header is 16 bytes len
 */
struct JbeHeader {
	u8 magic[4];
	u32 uncompressed_len;
	u32 data_array_len;
	u8 padding[4];
	// right array len is relative to uncompressed_len
};

/*
 * 0            16           [...]       map_array_off    E
 * +------------+-----------------------------+-----------+
 * + JBE HEADER |          DATA ARRAY         | MAP ARRAY |
 * +------------+-----------------------------+-----------+
 */

#define JBE_MAGIC "_JBE"

#ifdef _42_
#define memcmp ft_memcmp
#define memcpy ft_memcpy
#endif

void *jbe_encode(const void *_data, size_t *len)
{
	assert(sizeof(struct JbeHeader) == 16 && sizeof(JBE_MAGIC) - 1 == 4);

	assert(_data != NULL && *len <= UINT32_MAX);

	const u8 *data = (const u8 *)_data;

	// Count the number of no-zero bytes to determine the len of the compressed output
	size_t data_array_len = 0;
	for (size_t i = 0; i < *len; i++) {
		if (data[i] != 0x00) {
			data_array_len += 1;
		}
	}
	size_t compressed_len = sizeof(struct JbeHeader) + data_array_len;
	compressed_len += (*len & 0x7) == 0 ? *len >> 3: (*len >> 3) + 1;
	// Important: Init all at 0
#ifndef _42_
	void *out = calloc(1, compressed_len);
#else
	void *out = malloc(compressed_len);
#endif
	if (out == NULL) {
		dprintf(STDERR_FILENO, "Allocation error\n");
		return NULL;
	}
#ifdef _42_
	ft_bzero(out, header->compressed_len);
#endif
	// Fill JBE header
	struct JbeHeader *header = (struct JbeHeader *)out;
	memcpy(&header->magic[0], JBE_MAGIC, sizeof(JBE_MAGIC) - 1);
	header->uncompressed_len = *len;
	header->data_array_len = data_array_len;

	// Put a pointer at the begin of data_array
	u8 *out_data = out + sizeof(struct JbeHeader);
	// Put a pointer at the begin of map_array
	u8 *out_map = out_data + data_array_len;
	for (size_t i = 0; i < *len; i++) {
		u8 d = data[i];
		if (d != 0) {
			*out_data++ = d;
			out_map[i >> 3] |= 1 << (i & 0x7);
		}
	}

	// Return compressed len and data
	*len = compressed_len;
	return out;
}

void *jbe_decode(const void *_data, size_t *len)
{
	assert(sizeof(struct JbeHeader) == 16 && sizeof(JBE_MAGIC) - 1 == 4);

	assert(_data != NULL && *len >= sizeof(struct JbeHeader));

	const u8 *data = (const u8 *)_data;
	struct JbeHeader *header = (struct JbeHeader *)data;

	if (memcmp(&header->magic[0], JBE_MAGIC, sizeof(JBE_MAGIC) - 1) != 0) {
		dprintf(STDERR_FILENO, "Bad magic\n");
		return NULL;
	}
	// Important: Init all at 0
#ifndef _42_
	u8 *out = (u8 *)calloc(1, header->uncompressed_len);
#else
	u8 *out = (u8 *)malloc(header->uncompressed_len);
#endif
	if (out == NULL) {
		dprintf(STDERR_FILENO, "Allocation error\n");
		return NULL;
	}
#ifdef _42_
	ft_bzero(out, header->uncompressed_len);
#endif
	// Put a pointer at the begin of data_array
	const u8 *in_data = data + sizeof(struct JbeHeader);
	// Put a pointer at the begin of map_array
	const u8 *in_map = in_data + header->data_array_len;
	for (size_t i = 0; i < header->uncompressed_len; i++) {
		if ((in_map[i >> 3] & (1 << (i & 0x7))) != 0) {
			out[i] = *in_data++;
		}
	}
	*len = header->uncompressed_len;
	return (void *)out;
}
