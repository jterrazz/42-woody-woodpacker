#ifndef __JBE_H__
# define __JBE_H__

/*
 * Thanks to Agus Dwi Suarjaya
 */

#include <string.h>

/*
 * Get a chunk of data and encode it JBE. Set the len as new_len of encoded data.
 */
void *jbe_encode(const void *data, size_t *len);

/*
 * Get a chunk of encoded JBE data and decode it. Set the len as new_len of decoded data.
 */
void *jbe_decode(const void *data, size_t *len);

#endif
