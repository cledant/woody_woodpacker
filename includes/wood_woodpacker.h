#ifndef WOODY_WOODPACKER_H
#define WOODY_WOODPACKER_H

#include <string.h>
#include <stdint.h>

#define MODIFIED_BINARY_NAME "woody"
#define RIP_OFFSET (wwp_rip_offset + 1)

typedef struct woodyEnv
{
    void *binary;
    uint64_t binary_size;
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
uint8_t checkDestination(uint64_t binary_start,
                         uint64_t binary_size,
                         uint64_t destination);

// elf64.c
uint8_t saveAndUpdateEntrypoint(void *binary, uint64_t binary_size);

#endif