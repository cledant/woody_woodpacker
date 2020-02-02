[BITS 64]

%define STDOUT  1
%define WRITE   1
%define EXIT    60

global wwp_loader
global wwp_loader_size
global wwp_rip_offset

wwp_loader_size dq  end - wwp_loader
wwp_rip_offset dq  real_program - wwp_loader

section .text
wwp_loader:
    pushfq  ; Saving flags and register
    push rdi
    push rsi
    push rdx
    push rcx
    push r8
    push r9
    push r10
    push r11
    lea rsi, [rel woody.string] ; Printing woody
    mov rdi, STDOUT
    mov rdx, woody.len
    mov rax, WRITE
    syscall

real_program:
    lea rax, [rel $]    ; Loading rip + loading old entry point location
    add rax, [rel offset_to_old_entrypoint]
    pop r11  ; Restoring registers + flags before going to old entry point
    pop r10
    pop r9
    pop r8
    pop rcx
    pop rdx
    pop rsi
    pop rdi
    popfq
    jmp rax

woody:
    .string db "....WOODY....", 10
    .len equ $ - woody.string

offset_to_old_entrypoint dq 0x0

end: