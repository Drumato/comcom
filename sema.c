#include "comcom.h"

static Type *walk(Node *node);
void semantic(void) {
  for (int i = 0; i < 100; i++) {
    Node *node = code[i];
    if (node == NULL) {
      break;
    }
    walk(node);
  }
}
static Type *walk(Node *node) {
  switch (node->kind) {
    case ND_FUNC:
      walk(node->body);
      break;
    case ND_BLOCK:
      for (int i = 0; i < node->stmts->length; i++) {
        walk((Node *)node->stmts->data[i]);
      }
      break;
    case ND_DEC:
      if (!strncmp("int", node->lhs->name, strlen(node->lhs->name))) {
        node->rhs->type = new_type(T_INT, NULL);
      }
      break;
    case ND_ADD:
    case ND_SUB:
    case ND_MUL:
    case ND_DIV:
      walk(node->lhs);
      walk(node->rhs);
      break;
    case ND_ASSIGN:
      walk(node->lhs);
      walk(node->rhs);
      break;  // check type in future
    case ND_CALL:
      break;
    case ND_IF:
      break;
    case ND_WHILE:
      break;
    case ND_FOR:
      break;
    case ND_RETURN:
      break;  // check type in future
    case ND_EQ:
      break;  // check type in future
    case ND_NTEQ:
      break;  // check type in future
    case ND_GT:
      break;  // check type in future
    case ND_GTEQ:
      break;  // check type in future
    case ND_NUM:
      break;
    case ND_LVAR:
      fprintf(stderr, "%p", node->type);
      return node->type;
      break;
    case ND_ADDR: {
      Type *old = walk(node->lhs);
      node->type = new_type(T_ADDR, old);
      break;
    }
    case ND_DEREF:
      node->type = node->type->pointer_of;
      break;
    default:
      break;
  }
  return NULL;
}
