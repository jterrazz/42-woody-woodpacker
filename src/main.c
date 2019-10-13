#include "woody_woodpacker.h"

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/mman.h>

extern u8 payload32;
extern u8 payload64;

extern u64 payload32_size;
extern u64 payload64_size;

void config_payload64(void *payload, size_t payload_size, u64 encrypted_data_start, size_t encrypted_data_len, u64 encryption_key)
{
    u64 *payload_end = payload + payload_size;

    *(payload_end - 3) = encrypted_data_start;
    *(payload_end - 2) = encrypted_data_len;
    *(payload_end - 1) = encryption_key;
}

void encrypt_test(char *start, size_t len)
{
    while (len--) {
        *start += 1;
        start += 1;
    }
}

int main(int argc, char *argv[])
{
	if (argc != 2) {
		dprintf(STDERR_FILENO, "usage: %s executable_file\n", argv[0]);
		return 1;
	}

	printf("size of payload32: %llu\n", payload32_size);
	printf("size of payload64: %llu\n", payload64_size);

	char *tmpDataToEncrypt = strdup("Meuhhhh");
	printf("Original data: %s\n", tmpDataToEncrypt);
    encrypt_test(tmpDataToEncrypt, 7);
    printf("Encrypted data: %s\n", tmpDataToEncrypt);

	char *payload = mmap(0, 4096, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANON, -1, 0);
	if (payload == MAP_FAILED) {
		perror("mmap");
		return 1;
	}
	memcpy(payload, &payload64, (size_t)payload64_size);
    config_payload64(payload, payload64_size, tmpDataToEncrypt, 7, 12);
	((void(*)(void))payload)();
    printf("Decripted data: %s\n", tmpDataToEncrypt);

    return 0;
}
