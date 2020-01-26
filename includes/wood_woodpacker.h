#ifndef WOODY_WOODPACKER_H
#define WOODY_WOODPACKER_H

#include <stdint.h>

#define MODIFIED_BINARY_NAME "woody"

typedef struct woodyEnv
{
    void *binary;
    size_t binary_size;
} woodyEnv;

// loader.asm
extern void wwp_loader();
extern uint64_t wwp_loader_size;

// file_utility.c
uint8_t loadBinary(char const *filename, void **filedata, size_t *filesize);
uint8_t checkElf64(void const *binary, size_t filesize);
uint8_t dumpModifiedBinary(char const *binary_name,
                           void const *binary,
                           size_t binary_size);
void updateEntryPoint(void *binary, uint64_t binary_size);

#endif