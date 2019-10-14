#include "woody_woodpacker.h"

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/mman.h>

extern u8 _payload32;
extern u8 _payload64;
extern u8 _modification_try;
extern u8 _jump_base;
extern u8 _writer;

extern u64 _payload32_size;
extern u64 _payload64_size;
extern u64 _modification_try_size;
extern u64 _jump_base_size;
extern u64 _writer_size;

#define PATTERN_TO_FIND 0xdeadbeefdeadbeef

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

char *get_payload_copy(void)
{
    char *data = mmap(0, 4096, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANON, -1, 0);
    if (data == MAP_FAILED) {
        perror("mmap");
        return NULL;
    }

    printf("allocation mmap at %p\n", data);
    memcpy(data, &_payload64, (size_t)_payload64_size);
    printf("copied the payload at %p\n", data);
    return data;
}

int config_payload64(void *payload, size_t payload_size, u64 encrypted_data_start, size_t encrypted_data_len, u64 encryption_key)
{
    u64 *payload_end = payload + payload_size;

    if (payload_size < 3 * 8)
        return 1;

    *(payload_end - 3) = encrypted_data_start;
    *(payload_end - 2) = encrypted_data_len;
    *(payload_end - 1) = encryption_key;

    return 0;
}

int test_modification(void)
{
    char *data = get_payload_copy();

    memcpy(data, &_modification_try, (size_t)_modification_try_size);
    u64 v = ((u64(*)(void))data)();
    printf("Received value before code modification: %llx\n", v);

    u64 pattern = PATTERN_TO_FIND;
    u64 *ptr = (u64 *)smemchr(data, &pattern, _modification_try_size, sizeof(pattern));
    printf("Expr found at %p\n", ptr);

    if (ptr != NULL) {
        *ptr = 0x42;
        v = ((u64(*)(void))data)();
        printf("Received value after code modification: %llx\n", v);
    } else {
        dprintf(STDERR_FILENO, "Pattern not found\n");
        return 1;
    }

    memcpy(data, &_jump_base, (size_t)_jump_base_size);
    u32 s = 0xdeadbeef;
    u32 *p = (u32 *)smemchr(data, &s, _jump_base_size, sizeof(s));
    if (p != NULL) {
        // Advance to the next instruction EIP
        u64 rel_start = (u64)p + 4;

        // Allocate a new chunk of code for the writer fm
        char *data2 = mmap(0, 4096, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANON, -1, 0);
        if (data2 == MAP_FAILED) {
            perror("mmap");
            return 1;
        }
        printf("allocation mmap at %p\n", data);
        memcpy(data2, &_writer, (size_t)_writer_size);

        u64 rel_end = (u64)data2;

        // For the test, we assert implicitely that the rel jump offset id positive
        // rel_end > rel_start
        *p = (u32)(rel_end - rel_start);
        printf("rel_start: %p, rel_end: %p\n", (void *)rel_start, (void *)rel_end);
        ((void(*)(void))data)();
    } else {
        dprintf(STDERR_FILENO, "Pattern not found\n");
        return 1;
    }
}

void encrypt_addr(char *start, size_t len)
{
    while (len--) {
        *start += 1;
        start += 1;
    }
}

void test_encryption()
{
    char *addr_to_encrypt = strdup("meuhhh");
    char *payload = get_payload_copy();
    if (!payload)
        return;

    printf("Data to encrypt origin: %s\n", addr_to_encrypt);
    config_payload64(payload, _payload64_size, (u64)addr_to_encrypt, 6, 42424242);
    encrypt_addr(addr_to_encrypt, 6);
    printf("Encrypt data: %s\n", addr_to_encrypt);
    ((void(*)(void))payload)();
    printf("Decripted data:: %s\n", addr_to_encrypt);

}

void test_original_payload_infos(void)
{
    printf("size of payload32: %llu\n", _payload32_size);
    printf("size of payload64: %llu\n", _payload64_size);
}

int main(int argc, char *argv[])
{
//    test_original_payload_infos();
//    test_encryption();
    test_modification();
	return 0;
}
