#include <stdio.h>
#include <string.h>
#include <sys/mman.h>

#include "wood_woodpacker.h"

static void
cleanEnv(woodyEnv *env)
{
    if (env->binary && env->binary != MAP_FAILED) {
        munmap(env->binary, env->binary_size);
    }
}

static void
displayHelp()
{
    printf("Usage: ./woody_woodpacker elf64_binary\n");
}

int
main(int argc, char const **argv)
{
    woodyEnv env = { NULL, 0 };

    if (argc == 1 || argc > 2) {
        displayHelp();
        return (-1);
    } else if (!strcmp("--help", argv[1]) || !strcmp("-h", argv[1])) {
        displayHelp();
        return (-1);
    }
    if (loadBinary(argv[1], &env.binary, &env.binary_size)) {
        printf("woody_woodpacker: Failed to load binary\n");
        return (-1);
    }
    if (checkElf64(env.binary, env.binary_size)) {
        printf("woody_woodpacker: Not a valid elf64 binary\n");
        cleanEnv(&env);
        return (-1);
    }
    updateEntryPoint(env.binary, env.binary_size);
    if (dumpModifiedBinary(MODIFIED_BINARY_NAME, env.binary, env.binary_size)) {
        printf("woody_woodpacker: Can't create file: %s\n",
               MODIFIED_BINARY_NAME);
        cleanEnv(&env);
        return (-1);
    }
    cleanEnv(&env);
    return (0);
}