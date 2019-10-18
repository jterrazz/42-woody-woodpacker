
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/mman.h>
#include <stdlib.h>

typedef unsigned char u8;
typedef unsigned int u32;
typedef int i32;

extern u8 _writer;
extern u32 _writer_size;

extern u8 _string;
extern u8 _string2;
extern u8 _test_fn;
extern i32 _function_return;

static void *symtab[] = {&_string, &_string2, &_test_fn, &_function_return};

/*
 * find a binary pattern into a binary stream.
 * @return value: pointer to location of the first pattern
 * found in the binary stream OR NULL if not found.
 */
static const void *smemchr(const void *data,
                           const void *needle,
                           size_t data_size,
                           size_t needle_size)
{
    if (needle_size > data_size) {
        return NULL;
    }

    for (size_t i = 0; i <= data_size - needle_size; i++) {
        if (memcmp(&((u8 *)data)[i], needle, needle_size) == 0) {
            return &((u8 *)data)[i];
        }
    }
    return NULL;
}

char *get_writer_copy(void)
{
    char *data = mmap(0, 4096, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANON, -1, 0);
    if (data == MAP_FAILED) {
        perror("mmap");
        return NULL;
    }

    printf("absolute address of original fn: %p\n", &_writer);
    printf("allocation mmap at %p\n", data);
    memcpy(data, &_writer, (size_t)_writer_size);
    printf("copied the payload at %p\n", data);
    return data;
}

int test_modification(void)
{
    char *data = get_writer_copy();

    for (size_t i = 0; i < sizeof(symtab) / sizeof(void *); i++) {
	    u32 rel = symtab[i] - (void *)&_writer;
	    u32 new_abs_addr = (u32)(data + rel);
	    printf("new absolute addr: %p rel: %u\n", (void *)new_abs_addr, rel);

	    char *dup_data = data;
	    u32 *s = NULL;
	    while ((s = (u32 *)smemchr(dup_data, &symtab[i], (size_t)_writer_size, sizeof(u32))) != NULL) {
		    printf("founded at %p\n", s);
		    *s = new_abs_addr;
		    dup_data = (char *)(s + sizeof(u32));
	    }
    }

    printf("Before call...\n");
    int r = ((int(*)(void))data)();
    printf("Return value: %i\n", r);

    u32 rel = (void *)&_function_return - (void *)&_writer;
    u32 new_abs_addr = (u32)(data + rel);
    *(i32 *)new_abs_addr = 42;

    r = ((int(*)(void))data)();
    printf("Return value: %i\n", r);

    munmap(data, 4096);
    return 0;
}

void test_original_payload_infos(void)
{
    printf("size of writer: %u\n", _writer_size);
    printf("string address: %p\n", &_string);
    printf("string2 address: %p\n", &_string2);
    printf("size_of symtab: %zu\n", sizeof(symtab));
}

int main(int argc __attribute__((unused)), char *argv[] __attribute__((unused)))
{
	test_original_payload_infos();
	test_modification();
	return 0;
}
