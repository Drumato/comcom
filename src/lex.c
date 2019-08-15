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
  char *keywords[] = {"return", "if",   "else",   "while",  "for",
                      "int",    "char", "sizeof", "struct", NULL};
  TokenKind tk_kinds[] = {TK_RETURN, TK_IF,   TK_ELSE,   TK_WHILE, TK_FOR,
                          TK_INT,    TK_CHAR, TK_SIZEOF, TK_STRUCT};
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
      p++;
      while (isalpha(*p) || isdigit(*p) || *p == '_') {
        p++;
      }
      cur = new_token(TK_IDENT, cur, start, p - start);
      continue;
    }
    if (*p == '\'') {
      p++;
      cur = new_token(TK_NUM, cur, p, 1);
      cur->val = *p;
      p += 2;
      continue;
    }
    if (strncmp(p, "//", 2) == 0) {
      p += 2;
      while (*p != '\n') {
        p++;
      }
      continue;
    }
    if (strncmp(p, "/*", 2) == 0) {
      char *q = strstr(p + 2, "*/");
      if (!q) {
        warning("unclonsing comment found.");
      }
      p = q + 2;
      continue;
    }
    // " fjkdlsajfkldsa "
    if (*p == '"') {
      p++;
      char *start = p;
      while (*p != '"') {
        if (p == NULL) {
          warning("invalid pointer");
        }
        p++;
      }
      cur = new_token(TK_STR, cur, start, p - start);
      p++;
      continue;
    }
    if (strchr("+-*/[]{}()<>=!;.,&\"", *p) != NULL) {
      cur = new_token(TK_RESERVED, cur, p++, 1);
      continue;
    }

    if (isdigit(*p)) {
      int base = 10;
      if (!strncmp(p, "0b", 2)) {
        base = 2;
        p += 2;
      } else if (!strncmp(p, "0x", 2)) {
        base = 16;
        p += 2;
      } else if (!strncmp(p, "0", 1)) {
        base = 8;
        p++;
      }
      cur = new_token(TK_NUM, cur, p, 0);
      char *start = p;
      cur->val = strtol(p, &p, base);
      cur->len = p - start;
      continue;
    }

    warning("unable to tokenize");
  }

  new_token(TK_EOF, cur, p, 0);
  return head.next;
}
