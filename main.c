#include "comcom.h"

int main(int argc, char **argv) {
  if (argc != 2) {
    error("引数の個数が正しくありません");
    return 1;
  }

  Token *token = tokenize(argv[1]);

  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");

  printf("  mov rax, %d\n", expect_number(token));

  while (!at_eof(token)) {
    if (consume(token, '+')) {
      printf("  add rax, %d\n", expect_number(token));
      continue;
    }

    expect(token, '-');
    printf("  sub rax, %d\n", expect_number(token));
  }

  printf("  ret\n");
  return 0;
}
