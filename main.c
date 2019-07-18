#include "comcom.h"
int main(int argc, char **argv) {
  if (argc != 2) {
    error("%s%sinvalid argument%s", STRONG, RED, CLEAR);
    return 1;
  }

  token = tokenize(argv[1]);
  program();

  printf(".intel_syntax noprefix\n");
  printf(".global main\n");

  for (int i = 0; code[i]; i++) {
    gen(code[i]);
  }
  return 0;
}
