#include "comcom.h"

LVar *find_lvar(char *str, int len);
static void set_lvar(Node *node, char *name, int length, int offset);
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
      locals = NULL;
      for (int i = 0; i < node->args->length; i++) {
        Node *arg = (Node *)node->args->data[i];
        set_lvar(arg, arg->name, strlen(arg->name), arg->type->offset);
        LVar *var = find_lvar(arg->name, strlen(arg->name));
        var->type = arg->type;
        walk(arg);
      }
      walk(node->body);
      node->locals = locals;
      break;
    case ND_BLOCK:
      for (int i = 0; i < node->stmts->length; i++) {
        walk((Node *)node->stmts->data[i]);
      }
      break;
    case ND_DEC: {
      Type *type = node->lhs->type;
      node->rhs->type = type;
      if (node->rhs->type->kind == T_ARRAY) {
        set_lvar(node->rhs, node->rhs->name, strlen(node->rhs->name),
                 type->ary_size * type->ptr_to->offset);
      } else {
        set_lvar(node->rhs, node->rhs->name, strlen(node->rhs->name),
                 type->offset);
      }
      LVar *lvar = find_lvar(node->rhs->name, strlen(node->rhs->name));
      lvar->type = type;
      return type;
    } break;
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
      node->offset = lvar->offset;
      node->type = lvar->type;
      return node->type;
    } break;
    case ND_ADDR: {
      Type *content = walk(node->lhs);
      node->type = new_type(T_ADDR, content);
      return node->type;
      break;
    }
    case ND_DEREF: {
      Type *content = walk(node->lhs);
      node->type = content->ptr_to;
      return node->type;
    } break;
    case ND_SIZEOF: {
      Type *content = walk(node->expr);
      node->type = new_type(T_INT, NULL);
      node->kind = ND_NUM;
      switch (content->kind) {
        case T_INT: {
          node->val = 8;
          break;
        }
        case T_ADDR: {
          node->val = 8;
          break;
        }
        case T_ARRAY: {
          node->val = node->expr->type->ary_size * node->expr->type->offset;
          break;
        }
      }
      return node->type;
    } break;
    default:
      node->lhs->type = walk(node->lhs);
      node->rhs->type = walk(node->rhs);
      if (node->lhs->type->kind == T_INT && node->rhs->type->kind == T_INT) {
        node->type = new_type(T_INT, NULL);
      } else if (node->lhs->type->kind == T_ADDR &&
                 node->rhs->type->kind == T_INT) {
        node->type = new_type(T_ADDR, node->lhs->type->ptr_to);
        if (node->kind == ND_ADD) {
          node->kind = ND_SUB;
        } else if (node->kind == ND_SUB) {
          node->kind = ND_ADD;
        }
      } else if (node->lhs->type->kind == T_ARRAY &&
                 node->rhs->type->kind == T_INT) {
        node->type = new_type(T_ARRAY, node->lhs->type->ptr_to);
      }
      return node->type;
      break;
  }
  return NULL;
}

LVar *find_lvar(char *str, int len) {
  for (LVar *var = locals; var; var = var->next) {
    if (var->len == len && !memcmp(str, var->name, var->len)) return var;
  }
  return NULL;
}
static void set_lvar(Node *node, char *name, int length, int offset) {
  node->kind = ND_LVAR;
  LVar *lvar = calloc(1, sizeof(LVar));
  if (locals) lvar->next = locals;
  lvar->name = name;
  lvar->len = length;
  if (locals)
    lvar->offset = locals->offset + offset;
  else {
    lvar->offset = offset;
  }
  node->offset = lvar->offset;
  locals = lvar;
}
