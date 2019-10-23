#ifndef __MTF_H__
# define __MTF_H__

#include <string.h>

/*
 * Get a chunk of data and encode it MTF. Set the len as new_len of encoded data.
 */
void *mtf_encode(const void *data, size_t *len);

/*
 * Get a chunk of encoded MTF data and decode it. Set the len as new_len of decoded data.
 */
void *mtf_decode(const void *data, size_t *len);

#endif
