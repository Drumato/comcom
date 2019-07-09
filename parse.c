#include "comcom.h"

static Parser *new_parser(Array *tokens) {
  Parser *parser = (Parser *)malloc(sizeof(Parser));
  parser->tokens = tokens;
  parser->pos = 2;
  parser->cur = (Token *)parser->tokens->data[0];
  parser->next = (Token *)parser->tokens->data[1];
  return parser;
}

static Node *stmt(Parser *parser);
static Node *expr(Parser *parser);
static void next(Parser *parser) {
  parser->cur = parser->next;
  parser->next = (Token *)parser->tokens->data[parser->pos];
  parser->pos++;
}

static bool consume(Parser *parser, int ty) {
  if (parser->cur->ty == ty) {
    next(parser);
    return true;
  }
  error_found(PARSE, format("expected %s but got %s", tt_string(ty),
                            tt_string(parser->cur->ty)));
  return false;
}

static void expect(Parser *parser, int ty) {
  if (parser->next->ty == ty) {
    next(parser);
    return;
  }
  error_found(PARSE, format("expected %s but got %s", tt_string(ty),
                            tt_string(parser->next->ty)));
  return;
}
static Node *term(Parser *parser) {
  Token *token = parser->cur;
  switch (token->ty) {
    case TK_NUM: {
      Node *number = new_number(token->input);
      next(parser);
      return number;
    }
    default: {
      Node *node = (Node *)calloc(1, sizeof(Node));
      node->nty = N_INVALID;
      return node;
    }
  }
}
static Node *muldiv(Parser *parser) {
  Node *lchild = term(parser);
  for (;;) {
    Token *token = parser->cur;
    if (token->ty != TK_MUL && token->ty != TK_DIV) {
      break;
    }
    next(parser);
    if (token->ty == TK_MUL)
      lchild = new_binop(N_MUL, lchild, term(parser));
    else
      lchild = new_binop(N_DIV, lchild, term(parser));
  }
  return lchild;
}
static Node *adsub(Parser *parser) {
  Node *lchild = muldiv(parser);
  for (;;) {
    Token *token = parser->cur;
    if (token->ty != TK_ADD && token->ty != TK_SUB) {
      break;
    }
    next(parser);
    if (token->ty == TK_ADD)
      lchild = new_binop(N_ADD, lchild, muldiv(parser));
    else
      lchild = new_binop(N_SUB, lchild, muldiv(parser));
  }
  return lchild;
}
static Node *expr(Parser *parser) { return adsub(parser); }
static Node *return_stmt(Parser *parser) {
  Node *node = (Node *)calloc(1, sizeof(Node));
  node->nty = N_RETURN;
  consume(parser, TK_RETURN);
  node->expr = (Node *)calloc(1, sizeof(Node));
  node->expr = expr(parser);
  consume(parser, TK_SEMICOLON);
  return node;
}
static Array *compound_stmt(Parser *parser) {
  Array *stmts = new_ary();
  consume(parser, TK_LBRACE);
  while (parser->cur->ty != TK_RBRACE) {
    ary_push(stmts, stmt(parser));
  }
  consume(parser, TK_RBRACE);
  return stmts;
}
static Node *expr_stmt(Parser *parser) {
  Node *expr_st = (Node *)calloc(1, sizeof(Node));
  expr_st->nty = N_EXPRSTMT;
  expr_st->expr = expr(parser);
  consume(parser, TK_SEMICOLON);
  return expr_st;
}
static Node *stmt(Parser *parser) {
  switch (parser->cur->ty) {
    case TK_RETURN:
      return return_stmt(parser);
      break;
    case TK_IDENT:
    case TK_NUM:
      return expr_stmt(parser);
      break;
    default: {
      Node *node = (Node *)calloc(1, sizeof(Node));
      node->nty = N_INVALID;
      return node;
      break;
    }
  }
}

Rootnode *parse(Array *tokens) {
  Array *funcs = new_ary();
  Parser *parser = new_parser(tokens);
  while (is_typename(parser->cur->ty)) {  // parsing top-level
    Function *function = (Function *)calloc(1, sizeof(Function));
    next(parser);

    strncpy(function->name, parser->cur->input, strlen(parser->cur->input));
    function->name[strlen(parser->cur->input)] = '\0';
    consume(parser, TK_IDENT);
    consume(parser, TK_LPAREN);
    if (!is_typename(parser->cur->ty)) {
      error_found(PARSE, format("expected typename but got %s",
                                tt_string(parser->cur->ty)));
    }
    expect(parser, TK_RPAREN);
    expect(parser, TK_LBRACE);
    function->nodes = compound_stmt(parser);
    ary_push(funcs, (void *)function);
  }
  Rootnode *rootnode = (Rootnode *)calloc(1, sizeof(Rootnode));
  rootnode->functions = funcs;
  free(parser);
  return rootnode;
}
