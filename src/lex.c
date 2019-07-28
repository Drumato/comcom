#include "comcom.h"
static Token *new_token(TokenKind kind, Token *cur, char *str, int length) {
  Token *tok = calloc(1, sizeof(Token));
  tok->kind = kind;
  tok->str = str;
  tok->len = length;
  cur->next = tok;
  return tok;
}

Token *tokenize(char *p) {
  Token head;
  head.next = NULL;
  Token *cur = &head;

  while (*p) {
    if (isspace(*p)) {
      p++;
      continue;
    }
    if (!strncmp(p, "return", 6) && !isalnum(p[6])) {
      cur = new_token(TK_RETURN, cur, p, 6);
      p += 6;
      continue;
    }
    if (!strncmp(p, "if", 2) && !isalnum(p[2])) {
      cur = new_token(TK_IF, cur, p, 2);
      p += 2;
      continue;
    }
    if (!strncmp(p, "else", 4) && !isalnum(p[4])) {
      cur = new_token(TK_ELSE, cur, p, 4);
      p += 4;
      continue;
    }
    if (!strncmp(p, "while", 5) && !isalnum(p[5])) {
      cur = new_token(TK_WHILE, cur, p, 5);
      p += 5;
      continue;
    }
    if (!strncmp(p, "for", 3) && !isalnum(p[3])) {
      cur = new_token(TK_FOR, cur, p, 3);
      p += 3;
      continue;
    }
    if (!strncmp(p, "int", 3) && !isalnum(p[3])) {
      cur = new_token(TK_INT, cur, p, 3);
      p += 3;
      continue;
    }
    if (!strncmp(p, "sizeof", 6) && !isalnum(p[6])) {
      cur = new_token(TK_SIZEOF, cur, p, 6);
      p += 6;
      continue;
    }
    if (isalpha(*p)) {
      char *start = p;
      while (isalpha(*p) || *p == '_') {
        p++;
      }
      cur = new_token(TK_IDENT, cur, start, p - start);
      continue;
    }
    if (!strncmp(p, "<=", 2) || !strncmp(p, ">=", 2) || !strncmp(p, "==", 2) ||
        !strncmp(p, "!=", 2)) {
      cur = new_token(TK_RESERVED, cur, p, 2);
      p += 2;
    }

    if (strchr("+-*/{}()<>=!;,&", *p) != NULL) {
      cur = new_token(TK_RESERVED, cur, p++, 1);
      continue;
    }

    if (isdigit(*p)) {
      cur = new_token(TK_NUM, cur, p, 0);
      char *start = p;
      cur->val = strtol(p, &p, 10);
      cur->len = p - start;
      continue;
    }

    error("can't tokenize");
  }

  new_token(TK_EOF, cur, p, 0);
  return head.next;
}
