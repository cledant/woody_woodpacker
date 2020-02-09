#include <stdio.h>
#include <elf.h>
#include <string.h>

#include "wood_woodpacker.h"

static uint64_t
checkAvailableSpace(void *startOfPadding, void *endOfFile)
{
    uint64_t freeSpace = 0;

    while (startOfPadding <= endOfFile && *((uint8_t *)startOfPadding) == 0) {
        ++startOfPadding;
        ++freeSpace;
    }
    return (freeSpace);
}

static Elf64_Phdr *
findPtLoadHeaderWithRX(Elf64_Ehdr const *ehdr)
{
    for (uint64_t i = 0; i < ehdr->e_phnum; ++i) {
        Elf64_Phdr *phdr = (Elf64_Phdr *)((uint64_t)ehdr + ehdr->e_phoff +
                                          ehdr->e_phentsize * i);

        if (phdr->p_type == PT_LOAD && (phdr->p_flags & PF_X) &&
            (phdr->p_flags & PF_R)) {
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

    // Copy loader
    memcpy(ptr_to_exec_code + executable_phdr->p_filesz,
           wwp_loader,
           wwp_loader_size);

    // Computing loader jump to real entry point
    loaderData *loader_data = ptr_to_exec_code + executable_phdr->p_filesz +
                              wwp_loader_size - 4 * sizeof(uint64_t) -
                              sizeof(char) * MAX_KEY_SIZE;
    int64_t woody_entrypoint =
      ((uint64_t)ptr_to_exec_code + executable_phdr->p_filesz) -
      (uint64_t)binary;
    loader_data->offset_to_old_entryoint =
      ehdr->e_entry - woody_entrypoint - RIP_OFFSET;
    // Inserting key
    loader_data->key_len = key_size;
    memcpy(loader_data->key, key, key_size);
    // Inserting executable ph_hdr offset and size
    loader_data->exec_pt_load_size = executable_phdr->p_filesz;
    loader_data->offset_to_exec_pt_load =
      executable_phdr->p_offset - ehdr->e_entry;
    // Adding write right
    executable_phdr->p_flags = (PF_X | PF_R | PF_W);

    // Updating entry point to loader location
    ehdr->e_entry = woody_entrypoint;
    executable_phdr->p_filesz += wwp_loader_size;
    executable_phdr->p_memsz += wwp_loader_size;
    return (0);
}
