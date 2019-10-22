/*
 * https://fr.wikipedia.org/wiki/Codage_de_Huffman
 * https://www2.cs.duke.edu/csed/poop/huff/info/#hufftree
 */
#include "dhe.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

/*
 * the DHE header is 16 bytes len
 */
struct DheHeader {
	// DHE magic
	u8 magic[4];
	// Len of the original and uncompressed file
	u32 uncompressed_len;
	// Offset from begin of file when start compressed data seq
	u32 data_offset;
	// Root node of the huffman tree [rel NB_SYMB..NB_SYMB * 2]
	u16 huffman_tree_root_idx;
	u8 padding[2];
};

/*
 * 0           0x10                          0x410 (max)
 * +------------+-----------------------------+-------------------
 * + DHE HEADER |          HUFFMAN TREE       | COMPRESSED DATA ..
 * +------------+-----------------------------+-------------------
 */

#define DHE_MAGIC "_DHE"

// The code is calibrated for 'byte' chunk.
// You need to change some lines to allow other len chunk
#define NB_SYMB 256

struct node {
	u16 index_right;
	u16 index_left;
};

#define NODE 1
#define LEAF 0

struct leaf {
	u32 value;
};

struct btree {
	union {
		struct node node;
		struct leaf leaf;
	};
	u32 weight;
	u32 type;
};

struct regs {
	u32 value;
	u32 bits;
	u32 map;
};

/*
 * In a serialized form to decompression, weight isn't important and
 * type can be found only just with position [NB_SYMB..NB_SYMB * 2]
 * => NODE translated to [0..NB_SYMB]
 * The entire tree can fit in a (max 1KB for NB_SYMB = 256) container.
 * (NB_SYMB * (u16, u16))
 */
static u16 serialize_huffman_tree(struct node *stream,
				  struct btree *huffman_tree);

static void recursive_search(struct btree *huffman_tree,
			     u16 root_idx,
			     u32 deep,
			     u32 map,
			     struct regs regs[NB_SYMB]);

static void leaves_bubble_sort(struct btree *huffman_tree);

static void regs_replacement(struct regs regs[NB_SYMB]);

void *dhe_encode(const void *_data, size_t *len)
{
	assert(_data != NULL && len != NULL && *len <= UINT32_MAX);

	u8 *data = (u8 *)_data;

	// Prepare Huffman tree of max size (can swit NB_SYMB symboles)
	struct btree *huffman_tree =
		(struct btree *)calloc(NB_SYMB * 2, sizeof(struct btree));
	if (huffman_tree == NULL) {
		dprintf(STDERR_FILENO, "Cannot alloc mem for huffman tree\n");
		return NULL;
	}

	// Mark all first symbols as leaves
	for (u32 i = 0; i < NB_SYMB; i++) {
		huffman_tree[i].type = LEAF;
		huffman_tree[i].leaf.value = i;
	}

	// Mark all last symbols as node
	for (u32 i = NB_SYMB; i < NB_SYMB * 2; i++) {
		huffman_tree[i].type = NODE;
	}

	// Assign weight for all symbols in input [0..NB_SYMB]
	for (size_t i = 0; i < *len; i++) {
		huffman_tree[data[i]].weight += 1;
	}

	// Sort symbols [0, 0, ..., 0, 'c', d', NB_SYMB]
	leaves_bubble_sort(huffman_tree);

	// Advance offset to the first present symbol
	u16 leaf_idx = 0;
	while (huffman_tree[leaf_idx].weight == 0) {
		leaf_idx += 1;
	}

	// Set the node index
	u16 node_idx = 0;

	// WARNING: Exeption if *len == 0 OR just one leaf
	u16 nb_nodes = NB_SYMB - leaf_idx - 1;
	u16 available_node_idx = NB_SYMB;
	while (true) {

		u16 min_index_a;
		if (node_idx == 0) {
			min_index_a = leaf_idx;
			leaf_idx += 1;
		} else if (leaf_idx != NB_SYMB
			   && (huffman_tree[leaf_idx].weight <= huffman_tree[node_idx].weight
			       || node_idx >= available_node_idx)) {
			min_index_a = leaf_idx;
			leaf_idx += 1;
		} else if (node_idx != NB_SYMB + nb_nodes) {
			min_index_a = node_idx;
			node_idx += 1;
		} else {
			// Coherency check
			assert(false);
			break;
		}

		u16 min_index_b;
		if (node_idx == 0) {
			min_index_b = leaf_idx;
			leaf_idx += 1;
		} else if (leaf_idx != NB_SYMB
			   && (huffman_tree[leaf_idx].weight <= huffman_tree[node_idx].weight
			       || node_idx >= available_node_idx)) {
			min_index_b = leaf_idx;
			leaf_idx += 1;
		} else if (node_idx != NB_SYMB + nb_nodes) {
			min_index_b = node_idx;
			node_idx += 1;
		} else {
			break;
		}
#ifdef TRACE
		printf("merge between %hu and %hu -> %hu\n",
		       min_index_a,
		       min_index_b,
		       available_node_idx);
#endif
		huffman_tree[available_node_idx].weight = huffman_tree[min_index_a].weight + huffman_tree[min_index_b].weight;
		huffman_tree[available_node_idx].node.index_left = min_index_a;
		huffman_tree[available_node_idx].node.index_right = min_index_b;
		available_node_idx += 1;
		if (node_idx == 0) {
			node_idx = NB_SYMB;
		}

		// Coherency check
		u32 weight = 0;
		for (u16 i = NB_SYMB; i < node_idx; i++) {
			// Coherency check
			assert(huffman_tree[i].weight >= weight);
			weight = huffman_tree[i].weight;
		}
	}

	u16 root_idx = NB_SYMB + nb_nodes - 1;

	// Coherency check
	assert(huffman_tree[root_idx].weight == *len);

	struct regs regs[NB_SYMB] = {0};
	recursive_search(huffman_tree, root_idx, 0, 0, regs);

	for (u32 i = 0; i < NB_SYMB; i++) {
#ifdef TRACE
		printf("%2x: %9d bits: %2u, %2x map: %8x\n",
		       huffman_tree[i].leaf.value,
		       huffman_tree[i].weight,
		       regs[i].bits,
		       regs[i].value,
		       regs[i].map);
#endif
		// Coherency check
		assert(huffman_tree[i].leaf.value == regs[i].value || regs[i].bits == 0);
	}

	regs_replacement(regs);

	size_t projected_len = 0;
	for (size_t i = 0; i < *len; i++) {
		// Coherency check
		assert(regs[data[i]].bits != 0);
		projected_len += regs[data[i]].bits;
	}
#ifdef TRACE
	printf("Projected Len: %zu: in bits %zu\n", projected_len / 8, projected_len);
#endif
	size_t compressed_len = sizeof(struct DheHeader)
		+ (root_idx - NB_SYMB + 1) * sizeof(struct node)
		+ ((projected_len & 0xf) != 0 ? (projected_len >> 3) + 1 : projected_len >> 3);

#ifndef _42_
	u8 *out = (u8 *)calloc(1, compressed_len);
#else
	u8 *out = (u8 *)malloc(compressed_len);
#endif
	if (out == NULL) {
		dprintf(STDERR_FILENO, "Allocation error\n");
		return NULL;
	}
#ifdef _42_
	ft_bzero(out, header->compressed_len);
#endif
	// Fill DHE header
	struct DheHeader *header = (struct DheHeader *)out;
	memcpy(&header->magic[0], DHE_MAGIC, sizeof(DHE_MAGIC) - 1);
	header->uncompressed_len = *len;
	header->huffman_tree_root_idx = root_idx;

	header->data_offset = sizeof(struct DheHeader) + serialize_huffman_tree((struct node *)(out + sizeof(struct DheHeader)), huffman_tree) * sizeof(struct node);
	free(huffman_tree);

	// Put a pointer at the begin of data_array
	u8 *out_data = out + header->data_offset;
	u32 bit_offset = 0;
	for (size_t i = 0; i < *len; i++) {
		u32 map = regs[data[i]].map;
		u32 bits = regs[data[i]].bits;
		for (u32 j = 0; j < bits; j++) {
			*out_data |= (map & 0x1) << bit_offset;
			map >>= 1;
			if (bit_offset == 7) {
				bit_offset = 0;
				out_data += 1;
			} else {
				bit_offset += 1;
			}
		}
	}
	*len = compressed_len;
	return (void *)out;
}

void *dhe_decode(const void *_data, size_t *len)
{
	assert(sizeof(struct DheHeader) == 16 && sizeof(DHE_MAGIC) - 1 == 4);

	assert(_data != NULL && *len >= sizeof(struct DheHeader));

	const u8 *data = (const u8 *)_data;
	struct DheHeader *header = (struct DheHeader *)data;

	if (memcmp(&header->magic[0], DHE_MAGIC, sizeof(DHE_MAGIC) - 1) != 0) {
		dprintf(STDERR_FILENO, "Bad magic\n");
		return NULL;
	}

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

	const u16 root_idx = header->huffman_tree_root_idx;
	// Put a pointer at the begin of the huffman table
	const struct node *huffman_tree = (struct node *)(data + sizeof(struct DheHeader));
	// Put a pointer at the begin of compressed data
	const u8 *map = data + header->data_offset;

	u32 bit_offset = 0;
	for (u32 i = 0; i < header->uncompressed_len; i++) {
		u16 idx = root_idx;
		while (idx >= NB_SYMB) {
			if (((*map >> bit_offset) & 0x1) == 0x1) {
				idx = huffman_tree[idx - NB_SYMB].index_right;
			} else {
				idx = huffman_tree[idx - NB_SYMB].index_left;
			}
			if (bit_offset == 7) {
				bit_offset = 0;
				map += 1;
			} else {
				bit_offset += 1;
			}
		}
		out[i] = (u8)idx;
	}
	*len = header->uncompressed_len;
	return (void *)out;
}

static void leaves_bubble_sort(struct btree *huffman_tree)
{
	inline void u32_swap(u32 *a, u32 *b)
	{
		u32 tmp = *a;
		*a = *b;
		*b = tmp;
	}
	assert(huffman_tree != NULL && NB_SYMB != 0);

	bool is_done;
	do {
		is_done = true;
		for (size_t i = 0; i < NB_SYMB - 1; i++) {
			if (huffman_tree[i].weight > huffman_tree[i + 1].weight) {
				u32_swap(&huffman_tree[i].weight,
					 &huffman_tree[i + 1].weight);
				u32_swap(&huffman_tree[i].leaf.value,
					 &huffman_tree[i + 1].leaf.value);
				is_done = false;
			}
		}
	} while (is_done == false);
}

static void regs_replacement(struct regs regs[NB_SYMB])
{
	inline void u32_swap(u32 *a, u32 *b)
	{
		u32 tmp = *a;
		*a = *b;
		*b = tmp;
	}
	for (size_t i = 0; i < NB_SYMB; i++) {
		for (size_t j = 0; j < NB_SYMB; j++) {
			if (i != j
			    && regs[j].value == i
			    && regs[j].bits != 0) {
				u32_swap(&regs[i].value, &regs[j].value);
				u32_swap(&regs[i].bits, &regs[j].bits);
				u32_swap(&regs[i].map, &regs[j].map);
				break;
			}
		}
	}
}

static u16 serialize_huffman_tree(struct node *stream,
				  struct btree *huffman_tree)
{
	u16 nb_nodes = 0;

	for (u16 idx = NB_SYMB; idx < NB_SYMB * 2; idx++) {
		if (huffman_tree[idx].weight > 0) {

			u16 left_idx = huffman_tree[idx].node.index_left;
			if (left_idx < NB_SYMB) {
				stream[idx - NB_SYMB].index_left =
					huffman_tree[left_idx].leaf.value;
			} else {
				stream[idx - NB_SYMB].index_left = left_idx;
			}

			u16 right_idx = huffman_tree[idx].node.index_right;
			if (right_idx < NB_SYMB) {
				stream[idx - NB_SYMB].index_right =
					huffman_tree[right_idx].leaf.value;
			} else {
				stream[idx - NB_SYMB].index_right = right_idx;
			}
			nb_nodes += 1;
		}
	}
	return nb_nodes;
}

static void recursive_search(struct btree *huffman_tree,
			     u16 root_idx,
			     u32 deep,
			     u32 map,
			     struct regs regs[NB_SYMB])
{
	if (huffman_tree[root_idx].type == LEAF) {

		// Coherency check
		assert(root_idx < NB_SYMB);

		regs[root_idx].value = huffman_tree[root_idx].leaf.value;
		regs[root_idx].bits = deep;
		regs[root_idx].map = map;
		return;
	} else {
		recursive_search(huffman_tree,
				 huffman_tree[root_idx].node.index_left,
				 deep + 1,
				 map,
				 regs);
		recursive_search(huffman_tree,
				 huffman_tree[root_idx].node.index_right,
				 deep + 1,
				 map | (1 << deep),
				 regs);
	}
}
