cmake_minimum_required(VERSION 3.10)
project(woody_woodpacker)

enable_language(ASM_NASM)
add_executable(woody_woodpacker
		srcs/woody_woodpacker.c
		srcs/file_utility.c
		srcs/loader.asm
		srcs/elf64.c
		srcs/encrypt.c)
target_include_directories(woody_woodpacker PRIVATE
		includes)
target_compile_options(woody_woodpacker PRIVATE
		$<$<COMPILE_LANGUAGE:C>:-Wall -Wextra -Werror -std=c11>)