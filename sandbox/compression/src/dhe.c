#include "dhe.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

typedef unsigned long int u64;
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

/*
 * the DHE header is 16 bytes len
 */
struct DheHeader {
	u8 magic[4];
	u32 uncompressed_len;
	u16 huffman_tree_root_idx;
	u8 padding[6];
};

/*
 * 0           0x10                          0x810
 * +------------+-----------------------------+-------------------
 * + DHE HEADER |          HUFFMAN TREE       | COMPRESSED DATA ..
 * +------------+-----------------------------+-------------------
 */

#define DHE_MAGIC "_DHE"

#define NODE 1
#define LEAF 0
#define NB_SYMB 256

struct node {
	u16 index_right;
	u16 index_left;
};

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
};

/*
 * In a serialized form to decompression, weight isn't important and type can be found only
 * just with position [0..NB_SYMB] => LEAF & [NB_SYMB..NB_SYMB * 2] => NODE.
 * The entire tree can fit in a (2KB for NB_SYMB = 256) container. (NB_SYMB * 2 * (u16, u16))
 */
union serialized_form {
	struct node node;
	struct leaf leaf;
};

static void serialize_huffman_tree(u8 *stream, struct btree *huffman_tree);
static void recursive_search(struct btree *huffman_tree, u16 root_idx, u32 deep, struct regs regs[NB_SYMB]);

static void leaves_bubble_sort(struct btree *huffman_tree);
static void regs_replacement(struct regs regs[NB_SYMB]);

void *dhe_encode(const void *_data, size_t *len)
{
	assert(_data != NULL && len != NULL && *len <= UINT32_MAX);

	u8 *data = (u8 *)_data;

	// Prepare Huffmann tree
	struct btree *huffman_tree = (struct btree *)calloc(NB_SYMB * 2, sizeof(struct btree));
	if (huffman_tree == NULL) {
		dprintf(STDERR_FILENO, "Cannot allocate memory for huffman tree\n");
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

	// Assign weight for all symbols in input
	for (size_t i = 0; i < *len; i++) {
		huffman_tree[data[i]].weight += 1;
	}

	// Sort symbols
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

		// printf("merge between %hu and %hu -> %hu\n", min_index_a, min_index_b, available_node_idx);

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
			assert(huffman_tree[i].weight >= weight);
			weight = huffman_tree[i].weight;
		}
	}

	u16 root_idx = NB_SYMB + nb_nodes - 1;

	// Coherency check
	assert(huffman_tree[root_idx].weight == *len);

	struct regs regs[NB_SYMB] = {0};
	recursive_search(huffman_tree, root_idx, 0, regs);

	for (u32 i = 0; i < NB_SYMB; i++) {
		// printf("%2x: %9d bits: %u, %2x\n", huffman_tree[i].leaf.value, huffman_tree[i].weight, regs[i].bits, regs[i].value);
		assert(huffman_tree[i].leaf.value == regs[i].value || regs[i].bits == 0);
	}

	regs_replacement(regs);

	size_t projected_len = 0;
	for (size_t i = 0; i < *len; i++) {
		if (regs[data[i]].bits == 0) {
			printf("0 bit char -> %hhx\n", data[i]);
		}
		assert(regs[data[i]].bits != 0);
		projected_len += regs[data[i]].bits;
	}
	printf("Projected Len: %zu: in bits %zu\n", projected_len / 8, projected_len);
	free(huffman_tree);


	void *s = malloc(*len);
	return (void *)s;
}

void *dhe_decode(const void *data, size_t *len)
{
	(void)len;
	return (void *)data;
}

static void leaves_bubble_sort(struct btree *huffman_tree)
{
	assert(huffman_tree != NULL && NB_SYMB != 0);

	bool is_done;

	do {
		is_done = true;
		for (size_t i = 0; i < NB_SYMB - 1; i++) {
			if (huffman_tree[i].weight > huffman_tree[i + 1].weight) {
				u32 tmp_weight = huffman_tree[i].weight;
				huffman_tree[i].weight = huffman_tree[i + 1].weight;
				huffman_tree[i + 1].weight = tmp_weight;
				u32 tmp_leaf_value = huffman_tree[i].leaf.value;
				huffman_tree[i].leaf.value = huffman_tree[i + 1].leaf.value;
				huffman_tree[i + 1].leaf.value = tmp_leaf_value;
				is_done = false;
			}
		}
	} while (is_done == false);
}

static void regs_replacement(struct regs regs[NB_SYMB])
{
	for (size_t i = 0; i < NB_SYMB; i++) {
		for (size_t j = 0; j < NB_SYMB; j++) {
			if (i != j && regs[j].value == i && regs[j].bits != 0) {
				u32 tmp_value = regs[j].value;
				regs[j].value = regs[i].value;
				regs[i].value = tmp_value;
				u32 tmp_bits = regs[j].bits;
				regs[j].bits = regs[i].bits;
				regs[i].bits = tmp_bits;
				break;
			}
		}
	}
}

static void serialize_huffman_tree(u8 *stream, struct btree *huffman_tree)
{
	union serialized_form *sf = (union serialized_form *)stream;
	for (u16 idx = 0; idx < NB_SYMB * 2; idx++) {
		sf[idx] = (union serialized_form)huffman_tree[idx].node;
	}
}

static void recursive_search(struct btree *huffman_tree, u16 root_idx, u32 deep, struct regs regs[NB_SYMB])
{
	if (huffman_tree[root_idx].type == LEAF) {
		assert(root_idx < NB_SYMB);
		regs[root_idx].value = huffman_tree[root_idx].leaf.value;
		regs[root_idx].bits = deep;
		return;
	} else {
		recursive_search(huffman_tree, huffman_tree[root_idx].node.index_left, deep + 1, regs);
		recursive_search(huffman_tree, huffman_tree[root_idx].node.index_right, deep + 1, regs);
	}
}
