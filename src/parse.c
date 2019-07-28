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
static bool consume_sizeof(void) {
  if ((token->kind != TK_SIZEOF || strlen("sizeof") != token->len ||
       memcmp(token->str, "sizeof", token->len)))
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
static Token *ptr_to(Token *tok) {
  Token *token = (Token *)calloc(1, sizeof(Token));
  token->str = malloc(tok->len * sizeof(char));
  strncpy(token->str, tok->str, tok->len);
  token->str[tok->len] = '\0';
  token->kind = TK_ADDR;
  token->ptr_to = tok;
  return token;
}
static Token *consume_type(void) {
  if ((token->kind != TK_INT || strlen("int") != token->len ||
       memcmp(token->str, "int", token->len)))
    return NULL;
  Token *tok = token;
  token = token->next;
  return tok;
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

LVar *find_lvar(char *str, int len) {
  for (LVar *var = locals; var; var = var->next) {
    if (var->len == len && !memcmp(str, var->name, var->len)) return var;
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
        ary_push(node->args, (void *)expr());
        for (;;) {
          if (consume(")")) {
            break;
          }
          expect(",");
          ary_push(node->args, (void *)expr());
        }
      }
      node->name = (char *)malloc(tok->len * sizeof(char));
      strncpy(node->name, tok->str, tok->len);
      node->name[tok->len] = '\0';
      return node;
    } else {
      node->kind = ND_LVAR;

      LVar *lvar = find_lvar(tok->str, tok->len);
      if (lvar) {
        node->offset = lvar->offset;
      } else {
        error("undefined variable start with '%c'", tok->str[0]);
      }
      node->name = (char *)malloc(tok->len * sizeof(char));
      strncpy(node->name, tok->str, tok->len);
      node->name[tok->len] = '\0';
      return node;
    }
  }
  if (consume_type() != NULL) {
    tok = consume_ident();
    Node *node = (Node *)calloc(1, sizeof(Node));
    node->kind = ND_LVAR;
    node->name = (char *)malloc(tok->len * sizeof(char));
    strncpy(node->name, tok->str, tok->len);
    node->name[tok->len] = '\0';
    set_lvar(node, tok->str, tok->len);
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
  if (consume("*")) return new_node(ND_DEREF, unary(), new_node_num(0));
  if (consume("&")) return new_node(ND_ADDR, unary(), new_node_num(0));
  if (consume_sizeof()) {
    Node *node = (Node *)calloc(1, sizeof(Node));
    node->kind = ND_SIZEOF;
    node->expr = unary();
    return node;
  }
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
  Token *type;
  if (consume_return()) {  // return-stmt
    node->kind = ND_RETURN;
    node->lhs = expr();
  } else if ((type = consume_type()) != NULL) {
    while (consume("*")) type = ptr_to(type);
    Token *tok = consume_ident();
    LVar *lvar = find_lvar(tok->str, tok->len);
    if (lvar) {
      error("already declared: '%s'", lvar->name);
    }
    Node *ident = (Node *)calloc(1, sizeof(Node));
    ident->kind = ND_LVAR;
    ident->name = malloc(tok->len * sizeof(char));
    strncpy(ident->name, tok->str, tok->len);
    ident->name[tok->len] = '\0';
    set_lvar(ident, tok->str, tok->len);
    if (consume("[")) {
      ident->expr = expr();
      expect("]");
    }
    node = new_node(ND_DEC, new_node_type(type), ident);
    if (ident->expr != NULL) {
      node->lhs->type = new_type(T_ARRAY, node->lhs->type);
      node->lhs->type->ary_size = ident->expr->val;
    }
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
  if (consume_type() == NULL) {
    error("function must be started with type_name: got '%s'", token->str);
  }
  if (!next_lparen() || !(tok = consume_ident())) {
    error("function name must be specified: got '%s'", tok->str);
  }
  node->name = (char *)malloc(tok->len * sizeof(char));
  strncpy(node->name, tok->str, tok->len);
  node->name[tok->len] = '\0';
  node->kind = ND_FUNC;
  expect("(");
  node->args = new_ary();
  Node *arg;
  if (!consume(")")) {
    Token *type = consume_type();
    while (consume("*")) type = ptr_to(type);
    Token *tok = consume_ident();
    arg = (Node *)calloc(1, sizeof(Node));
    arg->kind = ND_LVAR;
    arg->name = (char *)malloc(tok->len * sizeof(char));
    strncpy(arg->name, tok->str, tok->len);
    arg->name[tok->len] = '\0';
    set_lvar(arg, tok->str, tok->len);
    arg->type = inference_type(type);
    ary_push(node->args, (void *)arg);
    for (;;) {
      if (consume(")")) {
        break;
      }
      expect(",");
      Token *type = consume_type();
      while (consume("*")) type = ptr_to(type);
      Token *tok = consume_ident();
      arg = (Node *)calloc(1, sizeof(Node));
      arg->kind = ND_LVAR;
      arg->name = (char *)malloc(tok->len * sizeof(char));
      strncpy(arg->name, tok->str, tok->len);
      arg->name[tok->len] = '\0';
      set_lvar(arg, tok->str, tok->len);
      arg->type = inference_type(type);
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
