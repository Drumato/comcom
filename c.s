.intel_syntax noprefix
.global main
main:
  push rbp
  mov rbp, rsp
  sub rsp, 208
  mov rax, rbp
  sub rax, 0
  push rax
  push 30
  pop rdi
  pop rax
  mov [rax],rdi
  push rdi
  mov rax, rbp
  sub rax, 8
  push rax
  push 50
  pop rdi
  pop rax
  mov [rax],rdi
  push rdi
  mov rax, rbp
  sub rax, 16
  lea rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 0
  push rax
  pop rdi
  pop rax
  mov [rax],rdi
  push rdi
  mov rax, rbp
  sub rax, 16
  lea rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 16
  lea rax, [rax]
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 1
  pop rdi
  pop rax
  imul rdi, 8
  sub rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax],rdi
  push rdi
  mov rax, rbp
  sub rax, 16
  lea rax, [rax]
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
