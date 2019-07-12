#include "comcom.h"
bool consume(char *op) {
  if (token->kind != TK_RESERVED || strlen(op) != token->len ||
      memcmp(token->str, op, token->len))
    return false;
  token = token->next;
  return true;
}

void expect(char op) {
  if (token->kind != TK_RESERVED || token->str[0] != op)
    error("'%c' isn't '%c'", token->str[0], op);
  token = token->next;
}

int expect_number() {
  if (token->kind != TK_NUM) error("not number");
  int val = token->val;
  token = token->next;
  return val;
}

bool at_eof() { return token->kind == TK_EOF; }

Token *new_token(TokenKind kind, Token *cur, char *str) {
  Token *tok = calloc(1, sizeof(Token));
  tok->kind = kind;
  tok->str = str;
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

    if (strchr("+-*/()<>=!", *p) != NULL) {
      cur = new_token(TK_RESERVED, cur, p++);
      continue;
    }

    if (isdigit(*p)) {
      cur = new_token(TK_NUM, cur, p);
      cur->val = strtol(p, &p, 10);
      continue;
    }

    error("can't tokenize");
  }

  new_token(TK_EOF, cur, p);
  return head.next;
}
Node *expr(void);
Node *term(void) {
  if (consume("(")) {
    Node *node = expr();
    expect(')');
    return node;
  }
  return new_node_num(expect_number());
}
Node *unary(void) {
  if (consume("+")) return term();
  if (consume("-")) return new_node(ND_SUB, new_node_num(0), term());
  return term();
}
Node *mul(void) {
  Node *node = unary();
  for (;;) {
    if (consume("*"))
      node = new_node(ND_MUL, node, unary());
    else if (consume("/"))
      node = new_node(ND_DIV, node, unary());
    else
      return node;
  }
}
Node *add(void) {
  Node *node = mul();
  for (;;) {
    if (consume("+"))
      node = new_node(ND_ADD, node, mul());
    else if (consume("-"))
      node = new_node(ND_SUB, node, mul());
    else
      return node;
  }
}
Node *relational(void) {
  Node *node = add();

  for (;;) {
    if (consume("<=")) {
      node = new_node(ND_GTEQ, node, add());
    } else if (consume(">=")) {
      node = new_node(ND_GTEQ, add(), node);
    } else if (consume("<")) {
      node = new_node(ND_GT, node, add());
    } else if (consume(">")) {
      node = new_node(ND_GT, add(), node);
    } else
      return node;
  }
}
Node *equality(void) {
  Node *node = relational();

  for (;;) {
    if (consume("==")) {
      node = new_node(ND_EQ, node, relational());
    } else if (consume("!=")) {
      node = new_node(ND_NTEQ, node, relational());
    } else
      return node;
  }
}
Node *expr(void) {
  Node *node = equality();
  return equality();
}
