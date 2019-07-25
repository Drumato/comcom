.intel_syntax noprefix
.global main
add:
  push rbp
  mov rbp, rsp
  push rsi
  mov rax, rbp
  sub rax, 8
  lea rax, [rax]
  push rax
  pop rax
  pop rsi
  push rsi
  mov rax, rbp
  sub rax, 8
  lea rax, [rax]
  push rax
  pop rax
  mov rax, [rax]
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
main:
  push rbp
  mov rbp, rsp
  sub rsp, 208
  mov rax, rbp
  sub rax, 8
  push rax
  push 30
  pop rdi
  pop rax
  mov [rax],rdi
  push rdi
  mov rax, rbp
  sub rax, 8
  push rax
  pop rsi
  call add
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
