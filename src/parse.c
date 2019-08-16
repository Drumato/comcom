#include "comcom.h"
static bool consume(char *op) {
  if ((token->kind != TK_RESERVED || strlen(op) != token->len ||
       memcmp(token->str, op, token->len)))
    return false;
  token = token->next;
  return true;
}
static bool consume_keyword(TokenKind kind, char *p) {
  if ((token->kind != kind || strlen(p) != token->len ||
       memcmp(token->str, p, token->len)))
    return false;
  token = token->next;
  return true;
}
static char *scopy(char *src, char *dst, int length) {
  src = (char *)malloc(length * sizeof(char));
  strncpy(src, dst, length);
  src[length] = '\0';
  assert(length == strlen(src));
  return src;
}
static Token *ptr_to(Token *tok) {
  Token *token = (Token *)calloc(1, sizeof(Token));
  token->str = scopy(token->str, tok->str, tok->len);
  token->kind = TK_ADDR;
  token->ptr_to = tok;
  return token;
}
static Token *consume_type(void) {
  Token *tok = NULL;
  if ((token->kind == TK_INT && strlen("int") == token->len &&
       !memcmp(token->str, "int", token->len))) {
    tok = token;
    token = token->next;
  } else if ((token->kind == TK_CHAR && strlen("char") == token->len &&
              !memcmp(token->str, "char", token->len))) {
    tok = token;
    token = token->next;
  } else if ((token->kind == TK_FLOAT && strlen("float") == token->len &&
              !memcmp(token->str, "float", token->len))) {
    tok = token;
    token = token->next;
  }
  return tok;
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
static Token *consume_str(void) {
  if (token->kind != TK_STR) return NULL;
  Token *tok = token;
  token = token->next;
  return tok;
}
static void expect(char *op) {
  if ((token->kind != TK_RESERVED || strlen(op) != token->len ||
       memcmp(token->str, op, token->len)))
    error("'%s' isn't '%s'", token->str, op);
  token = token->next;
}

static Token *expect_number() {
  Token *tok;
  switch (token->kind) {
    case TK_NUM:
    case TK_FLOAT: {
      tok = token;
      token = token->next;
      return tok;
    } break;
    default:
      error("not number");
      break;
  }
  return NULL;
}
static Node *expr(void);
static Node *declare(Node *node) {
  if (consume("[")) {
    node->expr = expr();
    expect("]");
  }
  return node;
}
static Node *parse_ident(Token *tok) {
  Node *ident = (Node *)calloc(1, sizeof(Node));
  ident->kind = ND_LVAR;
  ident->name = scopy(ident->name, tok->str, tok->len);
  return ident;
}

static bool at_eof() { return token->kind == TK_EOF; }
static Node *member(void) {
  Node *node = (Node *)calloc(1, sizeof(Node));
  Token *type = consume_type();
  while (consume("*")) type = ptr_to(type);
  Token *tok = consume_ident();
  Node *ident = parse_ident(tok);
  ident = declare(ident);
  node = new_node(ND_MEMBER, new_node_type(type), ident);
  if (ident->expr != NULL) {
    node->lhs->type = new_type(T_ARRAY, node->lhs->type);
    node->lhs->type->ary_size = ident->expr->val;
  }
  return node;
}

static Node *term(void) {
  Token *tok = consume_ident();
  if (tok) {
    Node *node = calloc(1, sizeof(Node));
    node->name = scopy(node->name, tok->str, tok->len);
    if (!consume("(")) {
      node->kind = ND_LVAR;
      if (consume("[")) {
        Node *n = (Node *)calloc(1, sizeof(Node));
        n->kind = ND_DEREF;
        Node *add = (Node *)calloc(1, sizeof(Node));
        n->lhs = add;
        add->lhs = node;
        add->rhs = expr();
        expect("]");
        return n;
      }
      if (consume(".")) {
        node = new_node(ND_ACMEMBER, node, term());
        return node;
      }
      return node;
    }
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
    return node;
  }
  if (consume_type() != NULL) {
    tok = consume_ident();
    Node *node = parse_ident(tok);
    return node;
  }
  if ((tok = consume_str()) != NULL) {
    Node *node = (Node *)calloc(1, sizeof(Node));
    node->kind = ND_STR;
    node->name = scopy(node->name, tok->str, tok->len);
    ary_push(strings, (void *)node->name);
    return node;
  }
  if (consume("(")) {
    Node *node = expr();
    expect(")");
    return node;
  }
  tok = expect_number();
  if (tok->is_float) {
    ary_push(floats, (void *)&tok->float_val);
    return new_node_float(tok->float_val);
  } else {
    return new_node_num(tok->val);
  }
}
static Node *unary(void) {
  if (consume("+")) return unary();
  if (consume("-")) return new_node(ND_SUB, new_node_num(0), unary());
  if (consume("*")) return new_node(ND_DEREF, unary(), new_node_num(0));
  if (consume("&")) return new_node(ND_ADDR, unary(), new_node_num(0));
  if (consume_keyword(TK_SIZEOF, "sizeof")) {
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
  Node *node = (Node *)calloc(1, sizeof(Node));
  Token *type;
  if (consume_keyword(TK_RETURN, "return")) {  // return-stmt
    node->kind = ND_RETURN;
    node->lhs = expr();
  } else if ((type = consume_type()) != NULL) {
    while (consume("*")) type = ptr_to(type);
    Token *tok = consume_ident();
    Node *ident = parse_ident(tok);
    ident = declare(ident);
    node = new_node(ND_DEC, new_node_type(type), ident);
    if (ident->expr != NULL) {
      node->lhs->type = new_type(T_ARRAY, node->lhs->type);
      node->lhs->type->ary_size = ident->expr->val;
    }
    if (consume("=")) {
      Node *n = (Node *)calloc(1, sizeof(Node));
      n->kind = ND_INIT;
      node = new_node(ND_INIT, node, new_node(ND_ASSIGN, node->rhs, assign()));
    }
  } else if (consume_keyword(TK_IF, "if")) {  // if-stmt
    node->kind = ND_IF;
    expect("(");
    node->expr = expr();
    expect(")");
    node->body = stmt();
    if (consume_keyword(TK_ELSE, "else")) node->alter = stmt();
    return node;
  } else if (consume_keyword(TK_STRUCT, "struct")) {
    node->kind = ND_STRUCT;
    node->members = new_ary();
    consume("{");
    while (!consume("}")) {
      ary_push(node->members, (void *)member());
      expect(";");
    }
    Token *tok = consume_ident();
    node->name = scopy(node->name, tok->str, tok->len);
  } else if (consume_keyword(TK_WHILE, "while")) {  // while-stmt

    node->kind = ND_WHILE;
    expect("(");
    node->expr = expr();
    expect(")");
    node->body = stmt();
    return node;
  } else if (consume_keyword(TK_FOR, "for")) {  // for-stmt
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
static Node *func(Node *node) {
  node->kind = ND_FUNC;
  node->args = new_ary();
  Node *arg;
  if (!consume(")")) {
    Token *type = consume_type();
    while (consume("*")) type = ptr_to(type);
    Token *tok = consume_ident();
    arg = parse_ident(tok);
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
      arg = parse_ident(tok);
      arg->type = inference_type(type);
      ary_push(node->args, (void *)arg);
    }
  }
  node->body = stmt();
  return node;
}
static Node *global(Node *node, Token *type) {
  node->kind = ND_GLOBAL;
  node = declare(node);
  return new_node(ND_DEC, new_node_type(type), node);
}
static Node *toplevel(void) {
  Node *node = calloc(1, sizeof(Node));
  Token *tok;
  Token *type;
  while (consume("*")) type = ptr_to(type);
  if ((type = consume_type()) == NULL) {
    error("toplevel must be started with type_name: got '%s'", token->str);
  }
  if ((tok = consume_ident()) == NULL) {
    error("toplevel name must be specified: got '%s'", tok->str);
  }
  node->name = scopy(node->name, tok->str, tok->len);
  if (consume("(")) {
    node = func(node);
    if (node->kind != ND_FUNC) {
      error("expected declaration func");
    }
  } else {
    node = global(node, type);
    if (node->rhs->expr != NULL) {
      node->lhs->type = new_type(T_ARRAY, node->lhs->type);
      node->lhs->type->ary_size = node->rhs->expr->val;
    }
    if (node->kind != ND_DEC || node->rhs->kind != ND_GLOBAL) {
      error("expected declaration global-var");
    }
    if (!consume(";")) error("'%s' is not ';'", token->str);
  }
  return node;
}
void program(void) {
  strings = new_ary();
  floats = new_ary();
  int i = 0;
  while (!at_eof()) code[i++] = toplevel();
  code[i] = NULL;
}
