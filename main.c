#include "comcom.h"
bool consume(char op);
void expect(char op);
int expect_number(void);
bool at_eof(void);
Token *new_token(TokenKind kind, Token *cur, char *str);
Token *tokenize(char *p);
Token *token;
int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "%s%sinvalid arguments\n%s", STRONG, RED, CLEAR);
    return 1;
  }
  char *ptr = argv[1];

  printf(".intel_syntax noprefix\n");
  printf(".globl main\n");
  printf("main:\n");
  printf("  mov rax, %ld\n", strtol(ptr, &ptr, 10));
  while (*ptr) {
    if (*ptr == '+') {
      ptr++;
      printf("  add rax, %ld\n", strtol(ptr, &ptr, 10));
      continue;
    }
    if (*ptr == '-') {
      ptr++;
      printf("  sub rax, %ld\n", strtol(ptr, &ptr, 10));
      continue;
    }
    fprintf(stderr, "invalid char: '%c'\n", *ptr);
    return 1;
  }
  printf("  ret\n");
  return 0;
}
bool consume(char op) {
  if (token->kind != TK_RESERVED || token->str[0] != op) return false;
  token = token->next;
  return true;
}
void expect(char op) {
  if (token->kind != TK_RESERVED || token->str[0] != op) error("not '%c'", op);
  token = token->next;
}

int expect_number(void) {
  if (token->kind != TK_NUM) error("not number");
  int val = token->val;
  token = token->next;
  return val;
}

bool at_eof(void) { return token->kind == TK_EOF; }
Token *new_token(TokenKind kind, Token *cur, char *str) {
  Token *tok = (Token *)calloc(1, sizeof(Token));
  tok->kind = kind;
  tok->str = str;
  cur->next = tok;
  return tok;
}

Token *tokenize(char *p) {
  Token head;
  head.next = NULL;
  Token *cur = &head;
}
