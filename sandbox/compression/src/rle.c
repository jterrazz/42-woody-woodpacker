/*
 * Run Length Encoding (RLE)
 * https://fr.wikipedia.org/wiki/Run-length_encoding
 */
#include "rle.h"

#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

typedef unsigned char u8;
typedef unsigned int u32;

/*
 * the RLE header is 8 bytes len
 */
struct RleHeader {
	u8 magic[4];
	u32 uncompressed_len;
};

/*
 * 0            8                   [...]                 E
 * +------------+-----------------------------------------+
 * + RLE HEADER |                 DATA ARRAY              |
 * +------------+-----------------------------------------+
 */

#define RLE_MAGIC "_RLE"

#ifdef _42_
#define memcmp ft_memcmp
#define memcpy ft_memcpy
#endif

#define MAX_COMPRESSED_SEQ_LEN 257
#define GROUP_SIZE 3

static inline size_t write_group(u8 *out, u8 c, size_t nb_elmts)
{
	size_t len = 0;

	assert(nb_elmts != 0 && nb_elmts != 1);

	// We can encode 257 adjacents elmts
	while (nb_elmts > MAX_COMPRESSED_SEQ_LEN) {
		*out++ = c;
		*out++ = c;
		*out++ = MAX_COMPRESSED_SEQ_LEN - 2;
		nb_elmts -= MAX_COMPRESSED_SEQ_LEN;
		len += GROUP_SIZE;
	}
	if (nb_elmts == 1) {
		*out++ = c;
		return len + 1;
	} else {
		*out++ = c;
		*out++ = c;
		*out++ = nb_elmts - 2;
		return len + GROUP_SIZE;
	}
}

void *rle_encode(const void *_data, size_t *len)
{
	assert(sizeof(struct RleHeader) == 8 && sizeof(RLE_MAGIC) - 1 == 4);

	assert(_data != NULL && *len <= UINT32_MAX);

	const u8 *data = (const u8 *)_data;

	// Max compressed len = len + 50% len + header len
	u8 *out = (u8 *)malloc(*len + (*len >> 1) + sizeof(struct RleHeader));
	if (out == NULL) {
		dprintf(STDERR_FILENO, "Allocation error\n");
		return NULL;
	}

	// Fill RLE header
	struct RleHeader *header = (struct RleHeader *)out;
	memcpy(&header->magic[0], RLE_MAGIC, sizeof(RLE_MAGIC) - 1);
	header->uncompressed_len = *len;

	// Put a pointer at the begin of data_array
	u8 *out_data = out + sizeof(struct RleHeader);

	if (*len == 0) {
		/* wanted dont do anything */
		*len = sizeof(struct RleHeader);
	} else if (*len == 1) {
		out_data[0] = data[0];
		*len = sizeof(struct RleHeader) + 1;
	} else {
		u8 previous_byte = data[0];
		size_t nb_consecutive_elmt = 1;
		size_t out_len = sizeof(struct RleHeader);
		for (size_t i = 1; i < *len; i++) {
			u8 current_byte = data[i];
			if (current_byte == previous_byte) {
				nb_consecutive_elmt += 1;
			} else {
				// dump relative to nb_consecutive_elmt
				if (nb_consecutive_elmt == 1) {
					*out_data++ = previous_byte;
					out_len += 1;
				} else {
					size_t l = write_group(out_data, previous_byte, nb_consecutive_elmt);
					out_data += l;
					out_len += l;
				}
				previous_byte = current_byte;
				nb_consecutive_elmt = 1;
			}
		}
		// dump relative to nb_consecutive_elmt
		if (nb_consecutive_elmt == 1) {
			*out_data++ = previous_byte;
			out_len += 1;
		} else {
			size_t l = write_group(out_data, previous_byte, nb_consecutive_elmt);
			out_data += l;
			out_len += l;
		}
		*len = out_len;
	}
	return (void *)out;
}

void *rle_decode(const void *_data, size_t *len)
{
	assert(sizeof(struct RleHeader) == 8 && sizeof(RLE_MAGIC) - 1 == 4);

	assert(_data != NULL && *len >= sizeof(struct RleHeader));

	const u8 *data = (const u8 *)_data;
	struct RleHeader *header = (struct RleHeader *)data;

	if (memcmp(&header->magic[0], RLE_MAGIC, sizeof(RLE_MAGIC) - 1) != 0) {
		dprintf(STDERR_FILENO, "Bad magic\n");
		return NULL;
	}

	u8 *out = (u8 *)malloc(header->uncompressed_len);
	if (out == NULL) {
		dprintf(STDERR_FILENO, "Allocation error\n");
		return NULL;
	}
	// Put a pointer at the begin of data_array
	const u8 *in_data = data + sizeof(struct RleHeader);

	if (header->uncompressed_len == 0) {
		assert(*len == sizeof(struct RleHeader));
		/* wanted dont do anything */
	} else if (header->uncompressed_len == 1) {
		assert(*len == sizeof(struct RleHeader) + 1);
		out[0] = in_data[0];
	} else if (header->uncompressed_len == 2) {
		assert(*len == sizeof(struct RleHeader) + 2);
		out[0] = in_data[0];
		out[1] = in_data[1];
	} else {
		u8 *out_ptr = out;
		size_t i = 0;
		while (i < (*len - sizeof(struct RleHeader) - 2)) {
			u8 current_data = in_data[i];
			if (current_data == in_data[i + 1]) {
				size_t len = in_data[i + 2] + 2;

				// Check if len is not bullshit
				assert(out_ptr + len <= out + header->uncompressed_len);

				memset(out_ptr, current_data, len);
				out_ptr += len;
				// long jump
				i += 3;
			} else {
				*out_ptr++ = current_data;
				i += 1;
			}
		}
		// Finish sequence in case of non-repeated values at the end
		while (i < *len - sizeof(struct RleHeader)) {
			*out_ptr++ = in_data[i];
			i += 1;
		}
		// Check file coherency
		assert(out_ptr == out + header->uncompressed_len);
	}
	*len = header->uncompressed_len;
	return (void *)out;
}
