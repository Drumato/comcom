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
static bool consume_if(void) {
  if ((token->kind != TK_IF || strlen("if") != token->len ||
       memcmp(token->str, "if", token->len)))
    return false;
  token = token->next;
  return true;
}
static bool consume_else(void) {
  if ((token->kind != TK_ELSE || strlen("else") != token->len ||
       memcmp(token->str, "else", token->len)))
    return false;
  token = token->next;
  return true;
}
static bool consume_while(void) {
  if ((token->kind != TK_WHILE || strlen("while") != token->len ||
       memcmp(token->str, "while", token->len)))
    return false;
  token = token->next;
  return true;
}
static bool consume_for(void) {
  if ((token->kind != TK_FOR || strlen("for") != token->len ||
       memcmp(token->str, "for", token->len)))
    return false;
  token = token->next;
  return true;
}
static bool consume_block(void) {
  if ((token->kind != TK_RESERVED || strlen("{") != token->len ||
       memcmp(token->str, "{", token->len)))
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
static bool next_lparen(void) {
  if (token->next->kind != TK_RESERVED ||
      strncmp(token->next->str, "(", 1) != 0)
    return false;
  return true;
}

static void expect(char *op) {
  if ((token->kind != TK_RESERVED || strlen(op) != token->len ||
       memcmp(token->str, op, token->len)))
    error("'%s' isn't '%s'", token->str, op);
  token = token->next;
}

static int expect_number() {
  if (token->kind != TK_NUM) error("not number");
  int val = token->val;
  token = token->next;
  return val;
}

static bool at_eof() { return token->kind == TK_EOF; }

static LVar *find_lvar(Token *tok) {
  for (LVar *var = locals; var; var = var->next) {
    if (var->len == tok->len && !memcmp(tok->str, var->name, var->len))
      return var;
  }
  return NULL;
}
static void set_lvar(Node *node, char *name, int length) {
  node->kind = ND_LVAR;
  LVar *lvar = calloc(1, sizeof(LVar));
  if (locals) lvar->next = locals;
  lvar->name = name;
  lvar->len = length;
  if (locals) lvar->offset = locals->offset + 8;
  node->offset = lvar->offset;
  locals = lvar;
}
static Node *expr(void);
static Node *term(void) {
  Token *tok = consume_ident();
  if (tok) {
    Node *node = calloc(1, sizeof(Node));
    if (consume("(")) {
      node->kind = ND_CALL;
      node->args = new_ary();
      if (!consume(")")) {
        ary_push(node->args, (void *)term());
        for (;;) {
          if (consume(")")) {
            break;
          }
          expect(",");
          ary_push(node->args, (void *)term());
        }
      }
      node->name = (char *)malloc(tok->len * sizeof(char));
      strncpy(node->name, tok->str, tok->len);
      node->name[tok->len] = '\0';
      return node;
    } else {
      node->kind = ND_LVAR;

      LVar *lvar = find_lvar(tok);
      if (lvar) {
        node->offset = lvar->offset;
      } else {
        set_lvar(node, tok->str, tok->len);
      }
      node->name = (char *)malloc(tok->len * sizeof(char));
      strncpy(node->name, tok->str, tok->len);
      node->name[tok->len] = '\0';
      return node;
    }
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
  if (consume_return()) {  // return-stmt
    node->kind = ND_RETURN;
    node->lhs = expr();
  } else if (consume_if()) {  // if-stmt
    node->kind = ND_IF;
    expect("(");
    node->expr = expr();
    expect(")");
    node->body = stmt();
    if (consume_else()) node->alter = stmt();
    return node;
  } else if (consume_while()) {  // while-stmt
    node->kind = ND_WHILE;
    expect("(");
    node->expr = expr();
    expect(")");
    node->body = stmt();
    return node;
  } else if (consume_for()) {  // for-stmt
    node->kind = ND_FOR;
    expect("(");
    if (!consume(";")) {
      node->init = expr();
      expect(";");
    }
    if (!consume(";")) {
      node->expr = expr();
      expect(";");
    }
    if (!consume(")")) {
      node->inc = expr();
      expect(")");
    }
    node->body = stmt();
    return node;
  } else if (consume_block()) {  // block-stmt
    node->kind = ND_BLOCK;
    node->stmts = new_ary();
    while (!consume("}")) ary_push(node->stmts, (void *)stmt());
    return node;
  } else {
    node = expr();
  }
  if (!consume(";")) error("'%s' is not ';'", token->str);
  return node;
}
static Node *func(void) {
  Node *node = calloc(1, sizeof(Node));
  Token *tok;
  if (!next_lparen() || !(tok = consume_ident())) {
    error("function must be started with func_name: got '%s'", token->str);
  }
  node->name = (char *)malloc(tok->len * sizeof(char));
  strncpy(node->name, tok->str, tok->len);
  node->name[tok->len] = '\0';
  node->kind = ND_FUNC;
  expect("(");
  node->args = new_ary();
  Node *arg;
  if (!consume(")")) {
    arg = term();
    set_lvar(arg, arg->name, strlen(arg->name));
    ary_push(node->args, (void *)arg);
    for (;;) {
      if (consume(")")) {
        break;
      }
      expect(",");
      arg = term();
      set_lvar(arg, node->name, strlen(node->name));
      ary_push(node->args, (void *)arg);
    }
  }
  node->body = stmt();
  return node;
}
void program(void) {
  int i = 0;
  while (!at_eof()) code[i++] = func();
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
    case ND_IF:  // integer
      return "if-stmt";
    default:
      return "";
  }
}
