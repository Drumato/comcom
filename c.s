.intel_syntax noprefix
.global main
main:
  push rbp
  mov rbp, rsp
  sub rsp, 208
  mov rax, rbp
  sub rax, 192
  push rax
  push 3
  pop rdi
  pop rax
  mov [rax],rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 192
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 3
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
