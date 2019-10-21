/*
 * Move To Front decoding (MTF)
 * see https://en.wikipedia.org/wiki/Move-to-front_transform
 */
#include "mtf.h"

#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

typedef unsigned char u8;

#define NB_ELMT 256

static u8 list[NB_ELMT] = {0};

void *mtf_encode(const void *_data, size_t *len)
{
	assert(_data != NULL);

#ifndef _42_
	u8 *out = calloc(1, *len);
#else
	u8 *out = malloc(*len);
#endif
	if (out == NULL) {
		dprintf(STDERR_FILENO, "Allocation error\n");
		return NULL;
	}
#ifdef _42_
	ft_bzero(out, *len);
#endif

	// Fist initialize list
	for (int i = 0; i < NB_ELMT; i++) {
		list[i] = i;
	};

	const u8 *data = (const u8 *)_data;

	for (size_t i = 0; i < *len; i++) {
		u8 to_find = data[i];
		u8 *location = memchr(list, to_find, NB_ELMT);

		assert(location != NULL);

		u8 offset = location - list;
		out[i] = offset;
		memmove(&list[1], &list[0], offset);
		list[0] = to_find;
	}
	*len = *len;
	return (void *)out;
}

void *mtf_decode(const void *_data, size_t *len)
{
	assert(_data != NULL);

#ifndef _42_
	u8 *out = calloc(1, *len);
#else
	u8 *out = malloc(*len);
#endif
	if (out == NULL) {
		dprintf(STDERR_FILENO, "Allocation error\n");
		return NULL;
	}
#ifdef _42_
	ft_bzero(out, *len);
#endif

	// Fist initialize list
	for (int i = 0; i < NB_ELMT; i++) {
		list[i] = i;
	}

	const u8 *data = (const u8 *)_data;

	for (size_t i = 0; i < *len; i++) {
		u8 offset = data[i];
		u8 elmt = list[offset];
		out[i] = elmt;
		memmove(&list[1], &list[0], offset);
		list[0] = elmt;
	}
	*len = *len;
	return (void *)out;
}
