#include "comcom.h"

LVar *find_lvar(char *str, int len);
static LVar *set_lvar(Node *node, char *name, int length, int offset);
static LVar *set_global(Node *node, char *name, int length);
static Type *walk(Node *node);
static int check_size(Type *type);
void semantic(void) {
  globals = new_map();
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
    case ND_GLOBAL: {
      LVar *lvar = find_lvar(node->name, strlen(node->name));
      node->type = lvar->type;
      return node->type;
    } break;
    case ND_FUNC:
      locals = NULL;
      for (int i = 0; i < node->args->length; i++) {
        Node *arg = (Node *)node->args->data[i];
        arg->var =
            set_lvar(arg, arg->name, strlen(arg->name), arg->type->offset);
        arg->var->type = arg->type;
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
    case ND_INIT: {
      node->lhs->type = walk(node->lhs);
      node->rhs->type = walk(node->rhs);
      if (node->lhs->type->kind != node->rhs->type->kind) {
        fprintf(stderr, "assiging %s into %s in initialization\n",
                type_string(node->rhs->type), type_string(node->lhs->type));
      }
    } break;
    case ND_DEC: {
      Type *type = node->lhs->type;
      node->rhs->type = type;
      if (node->rhs->kind == ND_GLOBAL) {
        if (node->rhs->type->kind == T_ARRAY) {
          node->rhs->var =
              set_global(node->rhs, node->rhs->name, strlen(node->rhs->name));
        } else {
          node->rhs->var =
              set_global(node->rhs, node->rhs->name, strlen(node->rhs->name));
        }
        node->rhs->var->is_gvar = true;
      } else {
        if (node->rhs->type->kind == T_ARRAY) {
          node->rhs->var =
              set_lvar(node->rhs, node->rhs->name, strlen(node->rhs->name),
                       check_size(node->rhs->type));
        } else {
          node->rhs->var = set_lvar(node->rhs, node->rhs->name,
                                    strlen(node->rhs->name), type->offset);
        }
      }
      node->rhs->var->type = type;
      return type;
    } break;
    case ND_ASSIGN:
      node->lhs->type = walk(node->lhs);
      node->rhs->type = walk(node->rhs);
      if (node->rhs->type->kind == T_ARRAY &&
          node->rhs->type->ptr_to->kind == T_CHAR) {
        node->lhs->val = ary_check(strings, node->rhs->name);
      }
      return node->rhs->type;
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
    case ND_STR: {
      Type *type = new_type(T_ARRAY, NULL);
      type->ptr_to = new_type(T_CHAR, NULL);
      type->ary_size = strlen(node->name);
      return type;
    } break;
    case ND_NUM:
      return new_type(T_INT, NULL);
      break;
    case ND_LVAR: {
      LVar *lvar;
      lvar = find_lvar(node->name, strlen(node->name));
      if (!lvar) lvar = (LVar *)map_get(globals, node->name);

      node->var = lvar;
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
      node->val = check_size(content);
      return node->type;
    } break;
    default:
      node->lhs->type = walk(node->lhs);
      node->rhs->type = walk(node->rhs);
      if (node->lhs->type->kind == T_CHAR) {
        node->type = new_type(T_CHAR, NULL);
      } else if (node->lhs->type->kind == T_INT &&
                 node->rhs->type->kind == T_INT) {
        node->type = new_type(T_INT, NULL);
      } else if ((node->lhs->type->kind == T_ADDR ||
                  node->lhs->type->kind == T_ARRAY) &&
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
static LVar *set_lvar(Node *node, char *name, int length, int offset) {
  LVar *lvar = calloc(1, sizeof(LVar));
  if (locals) lvar->next = locals;
  lvar->name = name;
  lvar->len = length;
  if (locals)
    lvar->offset = locals->offset + offset;
  else
    lvar->offset = offset;
  lvar->is_gvar = false;
  lvar->type = node->type;
  locals = lvar;
  return lvar;
}
static LVar *set_global(Node *node, char *name, int length) {
  LVar *lvar = calloc(1, sizeof(LVar));
  lvar->name = name;
  lvar->len = length;
  lvar->is_gvar = true;
  lvar->type = node->type;
  map_put(globals, name, (void *)lvar);
  return lvar;
}
static int check_size(Type *type) {
  switch (type->kind) {
    case T_INT: {
      return 8;
      break;
    }
    case T_ADDR: {
      return 8;
      break;
    }
    case T_ARRAY: {
      int offset = check_size(type->ptr_to);
      return type->ary_size * offset;
      break;
    }
    case T_CHAR: {
      return 1;
      break;
    }
    default:
      fprintf(stderr, "unexpected type\n");
      return -42;
      break;
  }
}
