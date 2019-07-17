.intel_syntax noprefix
.global main
foo:
  push rbp
  mov rbp, rsp
  sub rsp, 208
  push 3
  pop rax
  mov rsp,rbp
  pop rbp
  ret
main:
  push rbp
  mov rbp, rsp
  sub rsp, 208
  call foo
  push 10
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  mov rsp, rbp
  pop rbp
  ret
