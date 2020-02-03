#ifndef WOODY_WOODPACKER_H
#define WOODY_WOODPACKER_H

#include <string.h>
#include <stdint.h>

#define MIN_KEY_SIZE 16
#define MAX_KEY_SIZE 32
#define MODIFIED_BINARY_NAME "woody"

// RIP_OFFSET should be check when loader is updated
#define RIP_OFFSET (wwp_rip_offset + 0)

typedef struct woodyEnv
{
    void *binary;
    uint64_t binary_size;
    char key[MAX_KEY_SIZE + 1];
    uint64_t current_key_size;
} woodyEnv;

typedef struct loaderData
{
    uint64_t offset_to_entryoint;
    char key[MAX_KEY_SIZE];
    uint8_t key_len;
} loaderData;

// loader.asm
extern void wwp_loader();
extern uint64_t wwp_loader_size;
extern uint64_t wwp_rip_offset;

// file_utility.c
uint8_t loadBinary(char const *filename, void **filedata, uint64_t *filesize);
uint8_t checkElf64(void const *binary, uint64_t filesize);
uint8_t dumpModifiedBinary(char const *binary_name,
                           void const *binary,
                           uint64_t binary_size);

// elf64.c
uint8_t injectAndEncrypt(void *binary,
                         uint64_t binary_size,
                         char const *key,
                         uint64_t key_size);

// encrypt.c
uint8_t generateKey(char *key, uint64_t *key_size);
void encryptData(char const *key,
                 uint8_t *start,
                 uint64_t size,
                 uint64_t key_size);

#endif