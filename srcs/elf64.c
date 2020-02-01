#include "wood_woodpacker.h"

#include <stdio.h>
#include <elf.h>

static uint8_t
checkPhdrTableValidity(Elf64_Ehdr const *ehdr, uint64_t binary_size)
{
    // TODO check addition overflow
    Elf64_Phdr const *first_phdr = (Elf64_Phdr *)ehdr + ehdr->e_phoff;
    if (checkDestination((uint64_t)ehdr, binary_size, (uint64_t)first_phdr)) {
        printf("woody_woodpacker: Phdr offset wrong\n");
        return (1);
    }
    // TODO check multiplication overflow
    uint64_t total_phdr_size = ehdr->e_phnum * ehdr->e_phentsize;
    // TODO check addition overflow
    void const *phdr_end = first_phdr + total_phdr_size;
    if (checkDestination((uint64_t)ehdr, binary_size, (uint64_t)phdr_end)) {
        printf("woody_woodpacker: Phdr invalid\n");
        return (1);
    }
    return (0);
}

static Elf64_Phdr *
findExecPtLoad(Elf64_Ehdr const *ehdr)
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
saveAndUpdateEntrypoint(void *binary, uint64_t binary_size)
{
    Elf64_Ehdr *ehdr = binary;
    Elf64_Phdr *executable_phdr = NULL;

    if (checkPhdrTableValidity(ehdr, binary_size)) {
        return (1);
    }
    if (!(executable_phdr = findExecPtLoad(ehdr))) {
        printf("woody_woodpacker: Can't find executable Phdr\n");
        return (1);
    }
    // Checking if there is enough space for loader
    if ((ehdr->e_phentsize - executable_phdr->p_filesz) < wwp_loader_size) {
        printf("woody_woodpacker: Can't insert loader\n");
        return (1);
    }
    // TODO check addition overflow
    // TODO Check available size
    void *ptr_to_exec_code =
      (void *)((uint64_t)ehdr + executable_phdr->p_offset);
    if (checkDestination(
          (uint64_t)ehdr, binary_size, (uint64_t)ptr_to_exec_code)) {
        printf("woody_woodpacker: RX Phdr offset to data wrong\n");
        return (1);
    }
    // TODO Encrypt data should go here
    memcpy(ptr_to_exec_code + executable_phdr->p_filesz,
           wwp_loader,
           wwp_loader_size);

    int64_t *old_entrypoint_offset = ptr_to_exec_code +
                                     executable_phdr->p_filesz +
                                     wwp_loader_size - sizeof(int64_t);

    int64_t woody_entrypoint =
      ((uint64_t)ptr_to_exec_code + executable_phdr->p_filesz) -
      (uint64_t)binary;

    *old_entrypoint_offset = ehdr->e_entry - woody_entrypoint - RIP_OFFSET;

    ehdr->e_entry = woody_entrypoint;
    executable_phdr->p_filesz += wwp_loader_size;
    executable_phdr->p_memsz += wwp_loader_size;
    return (0);
}
