#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>

#include "wood_woodpacker.h"

uint8_t
generateKey(char *key, uint64_t *key_size)
{
    if (!key || !key_size) {
        return (1);
    }

    int fd_urandom = -1;
    if ((fd_urandom = open("/dev/urandom", O_RDONLY)) < 3) {
        return (1);
    }
    while (*key_size < MAX_KEY_SIZE) {
        char buff = 0;

        while (!isalnum(buff) || !isascii(buff)) {
            read(fd_urandom, &buff, 1);
        }
        key[*key_size] = buff;
        ++(*key_size);
    }
    key[MAX_KEY_SIZE] = 0;
    return (0);
}

void
encryptData(char const *key,
            uint8_t *data,
            uint64_t data_size,
            uint64_t key_size)
{
    for (uint64_t i = 0; i < data_size; ++i) {
        data[i] = data[i] ^ key[i % key_size];
    }
}
