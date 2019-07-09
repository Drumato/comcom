#include "comcom.h"

Node *new_binop(int nty, Node *lch, Node *rch) {
  Node *node = (Node *)calloc(1, sizeof(Node));
  node->nty = nty;
  node->lch = lch;
  node->rch = rch;
  return node;
}
Node *new_number(const char *input) {
  Node *node = (Node *)calloc(1, sizeof(Node));
  node->nty = N_INTEGER;
  if (startswith(input, "0x")) {
    node->intval = strtol(input, NULL, 16);
  } else if (startswith(input, "0")) {
    node->intval = strtol(input, NULL, 8);
  } else {
    node->intval = strtol(input, NULL, 10);
  }
  return node;
}
Node *new_string(const char *input) {
  Node *node = (Node *)calloc(1, sizeof(Node));
  node->nty = N_STRING;
  node->strval = (char *)malloc(sizeof(char) * strlen(input));
  strncpy(node->strval, input, strlen(input));
  node->strval[strlen(input)] = '\0';
  return node;
}
void dump_node(Node *node) {
  switch (node->nty) {
    case N_INTEGER:
      printf("%s:%d\n", nt_string(node->nty), node->intval);
      break;
    case N_STRING:
      printf("%s:%s\n", nt_string(node->nty), node->strval);
      break;
    case N_IDENT:
      printf("%s:%s\n", nt_string(node->nty), node->name);
      break;
    case N_RETURN:
      printf("%s%s%s:expr->", GREEN, nt_string(node->nty), CLEAR);
      dump_node(node->expr);
      break;
    case N_EXPRSTMT:
      printf("%sEXPRSTMT%s:expr->", GREEN, CLEAR);
      dump_node(node->expr);
      break;
    default:
      printf("%s\nlchild:", nt_string(node->nty));
      dump_node(node->lch);
      printf("rchild:");
      dump_node(node->rch);
      // printf("%s:lchild->%s\trchild->%s", nt_string(node->nty),
      //       nt_string(node->lch->nty), nt_string(node->rch->nty));
      break;
  }
}
char *nt_string(int nty) {
  switch (nty) {
    case N_INTEGER:
      return "INTEGER";
      break;
    case N_STRING:
      return "STRING";
      break;
    case N_IDENT:
      return "IDENTIFIER";
      break;
    case N_ADD:
      return "ADD";
      break;
    case N_SUB:
      return "SUB";
      break;
    case N_MUL:
      return "MUL";
      break;
    case N_DIV:
      return "DIV";
      break;
    case N_EXPRSTMT:
      return "EXPRSTMT";
      break;
    case N_RETURN:
      return "RETURN";
      break;
    case N_INVALID:
      return "INVALID";
      break;
  }
  return "INVALID";
}
