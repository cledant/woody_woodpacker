#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <elf.h>
#include <stdlib.h>
#include <string.h>

uint8_t
loadBinary(char const *filename, void **filedata, uint64_t *filesize)
{
    if (!filesize || !filedata) {
        return (1);
    }

    int fd = -1;
    if ((fd = open(filename, O_RDONLY)) <= 2) {
        return (1);
    }
    *filesize = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);
    if ((*filedata =
           mmap(NULL, *filesize, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0)) ==
        MAP_FAILED) {
        close(fd);
        return (1);
    }
    close(fd);
    return (0);
}

uint8_t
checkElf64(void const *binary, uint64_t filesize)
{
    if (!binary || filesize < sizeof(Elf64_Ehdr)) {
        return (1);
    }

    Elf64_Ehdr const *ehdr = binary;
    if (ehdr->e_ident[EI_MAG0] != ELFMAG0 ||
        ehdr->e_ident[EI_MAG1] != ELFMAG1 ||
        ehdr->e_ident[EI_MAG2] != ELFMAG2 ||
        ehdr->e_ident[EI_MAG3] != ELFMAG3) {
        return (1);
    }
    if (ehdr->e_machine != EM_X86_64 || ehdr->e_ident[EI_CLASS] != ELFCLASS64 ||
        (ehdr->e_type != ET_EXEC && ehdr->e_type != ET_DYN)) {
        return (1);
    }
    return (0);
}

uint8_t
copyBinary(void const *binary, void **cpy, uint64_t size)
{
    if (!binary || !cpy) {
        return (1);
    }

    if (!(*cpy = (void *)malloc(size))) {
        return (1);
    }
    memcpy(*cpy, binary, size);
    return (0);
}

uint8_t
dumpModifiedBinary(char const *binary_name,
                   void const *binary,
                   uint64_t binary_size)
{
    if (!binary_name || !binary) {
        return (1);
    }

    int fd = -1;
    if ((fd = open(
           binary_name, O_CREAT | O_WRONLY | O_TRUNC | O_APPEND, 00744)) <= 2) {
        return (1);
    }
    if (write(fd, binary, binary_size) <= 0) {
        close(fd);
        return (1);
    }
    close(fd);
    return (0);
}