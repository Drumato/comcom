#include "comcom.h"
static Token *new_token(TokenKind kind, Token *cur, char *str, int length) {
  Token *tok = calloc(1, sizeof(Token));
  tok->kind = kind;
  tok->str = str;
  tok->len = length;
  cur->next = tok;
  return tok;
}
static Token *multilength(char *p, Token *cur) {
  char *keywords[] = {"return", "if",  "else",   "while",
                      "for",    "int", "sizeof", NULL};
  TokenKind tk_kinds[] = {TK_RETURN, TK_IF,  TK_ELSE,  TK_WHILE,
                          TK_FOR,    TK_INT, TK_SIZEOF};
  for (int i = 0; keywords[i] != NULL; i++) {
    if (!strncmp(p, keywords[i], strlen(keywords[i])) &&
        !isalnum(p[strlen(keywords[i])]))
      return new_token(tk_kinds[i], cur, p, strlen(keywords[i]));
  }
  char *marks[] = {"<=", ">=", "==", "!=", NULL};
  for (int i = 0; marks[i] != NULL; i++) {
    if (!strncmp(p, marks[i], strlen(marks[i])))
      return new_token(TK_RESERVED, cur, p, 2);
  }
  return NULL;
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
    Token *t;
    if ((t = multilength(p, cur)) != NULL) {
      cur = t;
      p += t->len;
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

    if (strchr("+-*/[]{}()<>=!;,&", *p) != NULL) {
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
