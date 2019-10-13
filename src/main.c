#include "woody_woodpacker.h"

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/mman.h>

extern u8 _payload32;
extern u8 _payload64;

extern u64 _payload32_size;
extern u64 _payload64_size;

int main(int argc, char *argv[])
{
	if (argc != 2) {
		dprintf(STDERR_FILENO, "usage: %s executable_file\n", argv[0]);
		return 1;
	}

	printf("size of payload32: %llu\n", _payload32_size);
	printf("size of payload64: %llu\n", _payload64_size);

	char *data = mmap(0, 4096, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANON, -1, 0);
	if (data == MAP_FAILED) {
		perror("mmap");
		return 1;
	}
	memcpy(data, &_payload64, (size_t)_payload64_size);
	((void(*)(void))data)();
	return 0;
}
