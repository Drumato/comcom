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
  node->name = malloc(tok->len * sizeof(char));
  strncpy(node->name, tok->str, tok->len);
  node->name[tok->len] = '\0';
  return node;
}

Type *new_type(TypeKind kind, Type *pointer_of) {
  int offset;
  switch (kind) {
    case T_INT: {
      offset = 32;
      break;
    }
    case T_ADDR:
      offset = 8;
      break;
    default: {
      offset = 32;
      break;
    }
  }
  Type *type = (Type *)calloc(1, sizeof(Type));
  type->kind = kind;
  type->offset = offset;
  if (pointer_of != NULL) {
    type->pointer_of = pointer_of;
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
      return format("address_of -> %s", type_string(type->pointer_of));
  }
}
