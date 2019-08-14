#include "comcom.h"
int main(int argc, char **argv) {
  if (argc != 2) {
    error("%s%sinvalid argument%s", STRONG, RED, CLEAR);
    return 1;
  }

  char *content;
  if (fopen(argv[1], "r") == NULL)
    content = argv[1];
  else
    content = get_contents(argv[1]);

  token = tokenize(content);
  program();
  semantic();

  printf("  .intel_syntax noprefix\n");
  printf("  .data\n");
  gen_global();
  printf("  .text\n");
  printf("  .global main\n");

  for (int i = 0; code[i]; i++) {
    gen(code[i]);
  }
  return 0;
}
