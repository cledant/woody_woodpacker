#include <stdio.h>
#include <elf.h>

#include "wood_woodpacker.h"

static uint64_t
checkAvailableSpace(void *startOfPadding, void *endOfFile)
{
    uint64_t freeSpace = 0;

    while (startOfPadding <= endOfFile && *((uint8_t *)startOfPadding) == 0) {
        ++startOfPadding;
        ++freeSpace;
    }
    printf("---> Free space in PT_LOAD with RX = %lu\n", freeSpace);
    printf("---> Required space = %lu\n", wwp_loader_size);
    return (freeSpace);
}

static Elf64_Phdr *
findPtLoadHeaderWithRX(Elf64_Ehdr const *ehdr)
{
    for (uint64_t i = 0; i < ehdr->e_phnum; ++i) {
        Elf64_Phdr *phdr = (Elf64_Phdr *)((uint64_t)ehdr + ehdr->e_phoff +
                                          ehdr->e_phentsize * i);

        if (phdr->p_type == PT_LOAD && phdr->p_flags == (PF_X | PF_R)) {
            return (phdr);
        }
    }
    return (NULL);
}

uint8_t
injectAndEncrypt(void *binary,
                 uint64_t binary_size,
                 char const *key,
                 uint64_t key_size)
{
    Elf64_Ehdr *ehdr = binary;
    Elf64_Phdr *executable_phdr = NULL;

    // Finding location of RX code
    if (!(executable_phdr = findPtLoadHeaderWithRX(ehdr))) {
        printf("woody_woodpacker: Can't find Phdr with RX rights\n");
        return (1);
    }
    void *ptr_to_exec_code =
      (void *)((uint64_t)ehdr + executable_phdr->p_offset);

    // Checking if there is enough space to copy loader
    if (checkAvailableSpace(ptr_to_exec_code + executable_phdr->p_filesz,
                            binary + binary_size) < wwp_loader_size) {
        printf("woody_woodpacker: Can't insert loader\n");
        return (1);
    }

    // Encrypting data
    encryptData(key, ptr_to_exec_code, executable_phdr->p_filesz, key_size);
    // TODO REMOVE WHEN DECRYPT IN ASM
    encryptData(key, ptr_to_exec_code, executable_phdr->p_filesz, key_size);

    // Copy loader
    memcpy(ptr_to_exec_code + executable_phdr->p_filesz,
           wwp_loader,
           wwp_loader_size);

    // Computing loader jump to real entry point
    // updating entry point to loader location + setting key
    loaderData *loader_data = ptr_to_exec_code + executable_phdr->p_filesz +
                              wwp_loader_size - sizeof(uint64_t) -
                              sizeof(char) * MAX_KEY_SIZE - sizeof(char);
    int64_t woody_entrypoint =
      ((uint64_t)ptr_to_exec_code + executable_phdr->p_filesz) -
      (uint64_t)binary;
    loader_data->offset_to_entryoint =
      ehdr->e_entry - woody_entrypoint - RIP_OFFSET;
    loader_data->key_len = key_size;
    memcpy(loader_data->key, key, key_size);

    ehdr->e_entry = woody_entrypoint;
    executable_phdr->p_filesz += wwp_loader_size;
    executable_phdr->p_memsz += wwp_loader_size;
    return (0);
}
