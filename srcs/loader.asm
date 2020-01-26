[BITS 64]

%define STDOUT  1
%define WRITE   1
%define EXIT    60

global wwp_loader
global wwp_loader_size

wwp_loader_size dq  end - wwp_loader

section .text
wwp_loader:
    lea rsi, [rel woody.string]
    mov rdi, STDOUT
    mov rdx, woody.len
    mov rax, WRITE
    syscall
    mov rax, EXIT
    syscall

woody:
    .string db "....WOODY....", 10
    .len equ $ - woody.string

end: