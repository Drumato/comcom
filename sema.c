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
  if (node == NULL) return NULL;
  switch (node->kind) {
    case ND_FUNC:
      for (int i = 0; i < node->args->length; i++) {
        Type *type = ((Node *)node->args->data[i])->type;
        LVar *lvar = find_lvar(((Node *)node->args->data[i])->name,
                               strlen(((Node *)node->args->data[i])->name));
        lvar->type = type;
        ((Node *)node->args->data[i])->type = type;
        ((Node *)node->args->data[i])->offset = (i + 1) * 8;
        walk(((Node *)node->args->data[i]));
      }
      walk(node->body);
      break;
    case ND_BLOCK:
      for (int i = 0; i < node->stmts->length; i++) {
        walk((Node *)node->stmts->data[i]);
      }
      break;
    case ND_DEC: {
      Type *type = node->lhs->type;
      LVar *lvar = find_lvar(node->rhs->name, strlen(node->rhs->name));
      lvar->type = type;
      node->rhs->type = type;
      return type;
    } break;
    case ND_ADD:
    case ND_SUB:
    case ND_MUL:
    case ND_DIV:
    case ND_EQ:
    case ND_NTEQ:
    case ND_GT:
    case ND_GTEQ:
      node->lhs->type = walk(node->lhs);
      node->rhs->type = walk(node->rhs);
      break;
    case ND_ASSIGN:
      node->lhs->type = walk(node->lhs);
      node->rhs->type = walk(node->rhs);
      break;  // check type in future
    case ND_CALL:
      for (int i = 0; i < node->args->length; i++) {
        walk((Node *)node->args->data[i]);
      }
      break;
    case ND_IF:
      walk(node->expr);
      walk(node->body);
      break;
    case ND_WHILE:
      walk(node->expr);
      walk(node->body);
      break;
    case ND_FOR:
      walk(node->init);
      walk(node->expr);
      walk(node->inc);
      walk(node->body);
      break;
    case ND_RETURN:
      node->lhs->type = walk(node->lhs);
      return node->lhs->type;
      break;  // check type in future
    case ND_NUM:
      return new_type(T_INT, NULL);
      break;
    case ND_LVAR: {
      LVar *lvar = find_lvar(node->name, strlen(node->name));
      node->type = lvar->type;
      return node->type;
    } break;
    case ND_ADDR: {
      Type *old = walk(node->lhs);
      node->type = new_type(T_ADDR, old);
      return node->type;
      break;
    }
    case ND_DEREF:
      node->type = node->type->ptr_to;
      return node->type;
      break;
    default:
      break;
  }
  return NULL;
}