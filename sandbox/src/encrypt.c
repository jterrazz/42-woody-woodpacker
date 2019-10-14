#include "woody_woodpacker.h"

void encrypt_addr(char *start, size_t len)
{
    while (len--) {
        *start += 1;
        start += 1;
    }
}