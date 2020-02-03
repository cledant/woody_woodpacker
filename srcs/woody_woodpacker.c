#include <stdio.h>
#include <string.h>
#include <sys/mman.h>

#include "wood_woodpacker.h"

static void
displayHelp()
{
    printf("Usage: ./woody_woodpacker elf64_binary [encryption key]\n");
    printf("\tEncryption key size needs to be between %u and %u\n",
           MIN_KEY_SIZE,
           MAX_KEY_SIZE);
}

static uint8_t
checkArgs(int argc, char const **argv, char *key, uint64_t *key_size)
{
    if (argc == 1 || argc > 3) {
        displayHelp();
        return (1);
    }
    for (int i = 0; i < argc; ++i) {
        if (!strcmp("--help", argv[i]) || !strcmp("-h", argv[i])) {
            displayHelp();
            return (1);
        }
    }
    if (argc == 3) {
        *key_size = strlen(argv[2]);
        if (*key_size < MIN_KEY_SIZE || *key_size > MAX_KEY_SIZE) {
            displayHelp();
            return (1);
        }
        memcpy(key, argv[2], *key_size);
    }
    return (0);
}

static void
cleanEnv(woodyEnv *env)
{
    if (env->binary && env->binary != MAP_FAILED) {
        munmap(env->binary, env->binary_size);
    }
}

int
main(int argc, char const **argv)
{
    woodyEnv env = { NULL, 0, { 0 }, 0 };

    if (checkArgs(argc, argv, env.key, &env.current_key_size)) {
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
    if (!env.current_key_size && generateKey(env.key, &env.current_key_size)) {
        printf("woody_woodpacker: Failed to generate key\n");
        cleanEnv(&env);
        return (-1);
    }
    printf("Encryption key = %s\n", env.key);
    if (injectAndEncrypt(
          env.binary, env.binary_size, env.key, env.current_key_size)) {
        cleanEnv(&env);
        return (-1);
    }
    if (dumpModifiedBinary(MODIFIED_BINARY_NAME, env.binary, env.binary_size)) {
        printf("woody_woodpacker: Can't create file: %s\n",
               MODIFIED_BINARY_NAME);
        cleanEnv(&env);
        return (-1);
    }
    cleanEnv(&env);
    return (0);
}