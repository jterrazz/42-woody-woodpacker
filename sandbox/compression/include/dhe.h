#ifndef __DHE_H__
# define __DHE_H__

#include <string.h>

/*
 * Get a chunk of data and encode it DHE. Set the len as new_len of encoded data.
 */
void *dhe_encode(const void *data, size_t *len);

/*
 * Get a chunk of encoded DHE data and decode it. Set the len as new_len of decoded data.
 */
void *dhe_decode(const void *data, size_t *len);

#endif
