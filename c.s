.intel_syntax noprefix
.globl main
main:
    push 0xa
    push 0x14
    pop rax
    pop rbx
    add rax, rbx
    push rax
    push 0x1e
    pop rax
    pop rbx
    add rax, rbx
    push rax
    push 0x28
    pop rax
    pop rbx
    add rax, rbx
    push rax
    pop rbx
    mov rax, rbx
    ret
