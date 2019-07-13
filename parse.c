#include "comcom.h"

static bool consume(char *op) {
  if ((token->kind != TK_RESERVED || strlen(op) != token->len ||
       memcmp(token->str, op, token->len)))
    return false;
  token = token->next;
  return true;
}
static bool consume_return(void) {
  if ((token->kind != TK_RETURN || strlen("return") != token->len ||
       memcmp(token->str, "return", token->len)))
    return false;
  token = token->next;
  return true;
}
static Token *consume_ident(void) {
  if (token->kind != TK_IDENT) return NULL;
  Token *tok = token;
  token = token->next;
  return tok;
}

static void expect(char *op) {
  if ((token->kind != TK_RESERVED || strlen(op) != token->len ||
       memcmp(token->str, op, token->len)))
    error("'%c' isn't '%c'", token->str[0], op);
  token = token->next;
}

static int expect_number() {
  if (token->kind != TK_NUM) error("not number");
  int val = token->val;
  token = token->next;
  return val;
}

static bool at_eof() { return token->kind == TK_EOF; }

static Token *new_token(TokenKind kind, Token *cur, char *str) {
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
    if (!strncmp(p, "return", 6) && !isalnum(p[6])) {
      cur = new_token(TK_RETURN, cur, p);
      cur->len = 6;
      p += 6;
      continue;
    }
    if (isalpha(*p)) {
      char *start = p;
      while (isalnum(*p)) {
        p++;
      }
      cur = new_token(TK_IDENT, cur, start);
      cur->len = p - start;
      continue;
    }
    if (!strncmp(p, "<=", 2) || !strncmp(p, ">=", 2) || !strncmp(p, "==", 2) ||
        !strncmp(p, "!=", 2)) {
      cur = new_token(TK_RESERVED, cur, p);
      cur->len = 2;
      p += 2;
    }

    if (strchr("+-*/()<>=!;", *p) != NULL) {
      cur = new_token(TK_RESERVED, cur, p++);
      cur->len = 1;
      continue;
    }

    if (isdigit(*p)) {
      cur = new_token(TK_NUM, cur, p);
      char *start = p;
      cur->val = strtol(p, &p, 10);
      cur->len = p - start;
      continue;
    }

    error("can't tokenize");
  }

  new_token(TK_EOF, cur, p);
  return head.next;
}
static LVar *find_lvar(Token *tok) {
  for (LVar *var = locals; var; var = var->next) {
    if (var->len == tok->len && !memcmp(tok->str, var->name, var->len))
      return var;
  }
  return NULL;
}
static Node *expr(void);
static Node *term(void) {
  Token *tok = consume_ident();
  if (tok) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_LVAR;

    LVar *lvar = find_lvar(tok);
    if (lvar) {
      node->offset = lvar->offset;
    } else {
      lvar = calloc(1, sizeof(LVar));
      if (locals) lvar->next = locals;
      lvar->name = tok->str;
      lvar->len = tok->len;
      if (locals) lvar->offset = locals->offset + 8;
      node->offset = lvar->offset;
      locals = lvar;
    }
    return node;
  }
  if (consume("(")) {
    Node *node = expr();
    expect(")");
    return node;
  }
  return new_node_num(expect_number());
}
static Node *unary(void) {
  if (consume("+")) return term();
  if (consume("-")) return new_node(ND_SUB, new_node_num(0), term());
  return term();
}
static Node *mul(void) {
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
static Node *add(void) {
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
static Node *relational(void) {
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
static Node *equality(void) {
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
static Node *assign(void) {
  Node *node = equality();
  if (consume("=")) node = new_node(ND_ASSIGN, node, assign());
  return node;
}
static Node *expr(void) { return assign(); }
static Node *stmt(void) {
  Node *node = calloc(1, sizeof(Node));
  if (consume_return()) {
    node->kind = ND_RETURN;
    node->lhs = expr();
  } else {
    node = expr();
  }
  if (!consume(";")) error("'%s' is not ';'", token->str);
  return node;
}
void program(void) {
  int i = 0;
  while (!at_eof()) code[i++] = stmt();
  code[i] = NULL;
}
char *tk_string(TokenKind tk) {
  switch (tk) {
    case TK_RESERVED:
      return "RESERVED";
    case TK_IDENT:
      return "IDENTIFIER";
    case TK_NUM:
      return "NUMBER";
    case TK_EOF:
      return "EOF";
    default:
      return "";
  }
}
char *nk_string(NodeKind nk) {
  switch (nk) {
    case ND_ADD:  //+
      return "add-node";
    case ND_SUB:  //-
      return "sub-node";
    case ND_MUL:  //
      return "mul-node";
    case ND_DIV:  // /
      return "div-node";
    case ND_EQ:  // ==
      return "eq-node";
    case ND_NTEQ:  // !=
      return "nt-node";
    case ND_GT:  // <
      return "gt-node";
    case ND_GTEQ:  // <=
      return "gteq-node";
    case ND_NUM:  // integer
      return "num-node";
    case ND_RETURN:  // integer
      return "return-stmt";
    default:
      return "";
  }
}
