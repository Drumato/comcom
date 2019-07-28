#include "comcom.h"
Node *new_node(NodeKind kind, Node *lhs, Node *rhs) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

Node *new_node_num(int val) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_NUM;
  node->val = val;
  return node;
}

Node *new_node_type(Token *tok) {
  Node *node = calloc(1, sizeof(Node));
  Type *type = NULL;
  if (!strncmp("int", tok->str, 3)) {
    type = new_type(T_INT, NULL);
  }
  while (tok->ptr_to != NULL) {
    type = new_type(T_ADDR, type);
    tok = tok->ptr_to;
  }
  node->type = type;
  return node;
}

Type *new_type(TypeKind kind, Type *ptr_to) {
  int offset;
  switch (kind) {
    case T_INT: {
      offset = 4;
      break;
    }
    case T_ADDR:
      offset = 8;
      break;
    default: {
      offset = 8;
      break;
    }
  }
  Type *type = (Type *)calloc(1, sizeof(Type));
  type->kind = kind;
  type->offset = offset;
  if (ptr_to != NULL) {
    type->ptr_to = ptr_to;
  }
  return type;
}

Type *inference_type(Token *tok) {
  Type *type = new_type(T_INT, NULL);
  if ((tok->kind != TK_INT || strlen("int") != tok->len ||
       memcmp(tok->str, "int", tok->len))) {
    while (tok->ptr_to != NULL) {
      type = new_type(T_ADDR, type);
      tok = tok->ptr_to;
    }
  }
  return type;
}

char *type_string(Type *type) {
  if (type == NULL) {
    return "";
  }
  switch (type->kind) {
    case T_INT:
      return "Integer";
    case T_ADDR:
      return format("address_of -> %s", type_string(type->ptr_to));
    case T_ARRAY:
      return format("array of '%s'", type_string(type->ptr_to));
    default:
      return "";
  }
}
