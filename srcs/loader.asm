[BITS 64]

%define STDOUT  1
%define WRITE   1
%define EXIT    60

%define KEY_LEN 32

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
    lea r11, [rel $]    ; Loading rip + loading old entry point location
    add r11, [rel offset_to_old_entrypoint]

decrypt:
    mov rsi, r11    ; Loading code location
    add rsi, [rel offset_to_exec_pt_load]
    mov r8, [rel key_len]
    mov r9, [rel exec_pt_load_size]
    xor rcx, rcx    ; Counter for total pt_load size

loop:
    mov rax, rcx
    div r8
    lea rdi, [rel key]
    add rdi, rdx
    mov rax, [rsi]
    mov r10, [rdi]
    xor al, r10b
    mov byte [rsi], al
    inc rsi
    inc rcx
    cmp rcx, r9
    jne loop

running_real_program:
    mov rax, r11
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

data:
    offset_to_exec_pt_load dq 0x0
    exec_pt_load_size dq 0x0
    offset_to_old_entrypoint dq 0x0
    key times KEY_LEN db 0x0
    key_len dq 0x0

end: