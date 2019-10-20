#ifndef __RLE_H__
# define __RLE_H__

#include <string.h>

/*
 * Get a chunk of data and encode it RLE. Set the len as new_len of encoded data.
 */
void *rle_encode(const void *data, size_t *len);

/*
 * Get a chunk of encoded RLE data and decode it. Set the len as new_len of decoded data.
 */
void *rle_decode(const void *data, size_t *len);

#endif
