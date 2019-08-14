  .intel_syntax noprefix
  .data
  .text
  .global main
main:
  push rbp
  mov rbp, rsp
  sub rsp, 8
  mov rax, rbp
  sub rax, 8
  push rax
  push 97
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
