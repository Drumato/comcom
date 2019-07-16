.intel_syntax noprefix
.global main
main:
  push rbp
  mov rbp, rsp
  sub rsp, 208
  push 3
  push 4
  pop rsi
  pop rdi
  call foo
  pop rax
  mov rsp, rbp
  pop rbp
  ret
