#include "woody_woodpacker.h"

int config_payload64(void *payload, size_t payload_size, u64 encrypted_data_start, size_t encrypted_data_len, u64 encryption_key)
{
    u64 *payload_end = payload + payload_size;

    if (payload_size < 3 * 8)
        return 1

    *(payload_end - 3) = encrypted_data_start;
    *(payload_end - 2) = encrypted_data_len;
    *(payload_end - 1) = encryption_key;

    return 0
}