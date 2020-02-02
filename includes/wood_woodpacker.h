#ifndef WOODY_WOODPACKER_H
#define WOODY_WOODPACKER_H

#include <string.h>
#include <stdint.h>

#define MODIFIED_BINARY_NAME "woody"

// RIP_OFFSET should be check when loader is updated
#define RIP_OFFSET (wwp_rip_offset + 0)

typedef struct woodyEnv
{
    void *binary;
    uint64_t binary_size;
    uint64_t key;
} woodyEnv;

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
uint8_t injectAndEncrypt(void *binary, uint64_t binary_size, uint64_t key);

// encrypt.c
uint64_t generateKey();
void encryptData(uint64_t key, void *start, uint64_t size);

#endif