#include "comcom.h"
int main(int argc, char **argv) {
  if (argc != 2) {
    error("%s%sinvalid argument%s", STRONG, RED, CLEAR);
    return 1;
  }

  token = tokenize(argv[1]);
  Node *node = expr();

  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");

  gen(node);
  printf("  pop rax\n");
  printf("  ret\n");
  return 0;
}
