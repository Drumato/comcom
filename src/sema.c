#include "comcom.h"

LVar *find_lvar(char *str, int len);
static LVar *set_lvar(Node *node, char *name, int length, int offset);
static LVar *set_global(Node *node, char *name, int length);
static Type *checktype_declare(Node *node);
static Type *checktype_binop(Node *lhs, Node *rhs);
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
        arg->type = walk(arg);
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
      return node->lhs->type;
    } break;
    case ND_DEC: {
      Type *type = node->lhs->type;
      node->rhs->type = type;
      return checktype_declare(node->rhs);
    } break;
    case ND_ASSIGN:
      node->lhs->type = walk(node->lhs);
      node->rhs->type = walk(node->rhs);
      if (node->rhs->type->kind == T_ARRAY &&
          node->rhs->type->ptr_to->kind == T_CHAR) {
        node->lhs->val = ary_check(strings, node->rhs->name);
      }
      if (node->lhs->type->kind == T_INT && node->rhs->type->kind == T_FLOAT) {
        info(format("implicit type conversion %s when assigning %s into '%s'",
                    type_string(node->lhs->type), type_string(node->rhs->type),
                    node->lhs->name));
        node->lhs->type = node->rhs->type;
        if (node->rhs->is_float) node->rhs->is_float = false;
        node->rhs->val = (int)node->rhs->float_val;
        node->did_cast = true;
      } else if (node->lhs->type->kind == T_FLOAT &&
                 node->rhs->type->kind == T_INT) {
        info(format("implicit type conversion %s when assigning %s into '%s'",
                    type_string(node->lhs->type), type_string(node->rhs->type),
                    node->lhs->name));
        node->rhs->type = node->lhs->type;
        if (!node->rhs->is_float) node->rhs->is_float = true;
        node->rhs->float_val = (float)node->rhs->val;
        node->did_cast = true;
      }
      if (node->lhs->var) {
        node->lhs->var->type = node->lhs->type;
      }
      return node->rhs->type;
      break;  // check type in future
    case ND_CALL:
      for (int i = 0; i < node->args->length; i++) {
        Node *arg = (Node *)node->args->data[i];
        arg->type = walk(arg);
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
      type->ary_size = strlen(node->name) + 1;
      return type;
    } break;
    case ND_NUM:
      if (node->is_float) {
        return new_type(T_FLOAT, NULL);
      } else {
        return new_type(T_INT, NULL);
      }
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
    case ND_MEMBER: {
      Type *type = node->lhs->type;
      node->rhs->type = type;
      return checktype_declare(node->rhs);
    } break;
    case ND_STRUCT: {
      node->type = new_type(T_STRUCT, NULL);
      node->type->members = new_map();
      node->type = checktype_declare(node);
      int offset = 0;
      for (int i = 0; i < node->members->length; i++) {
        Node *member = (Node *)node->members->data[i];
        member->type = walk(member);
        member->type->position = offset;
        offset += member->type->offset;
        map_put(node->type->members, (char *)member->rhs->name,
                (Type *)member->rhs->type);
      }
      node->type = checktype_declare(node);
      return node->type;
    } break;
    case ND_ACMEMBER: {
      node->lhs->type = walk(node->lhs);
      for (int i = 0; i < node->lhs->type->members->keys->length; i++) {
        if (!strncmp(node->rhs->name,
                     (char *)node->lhs->type->members->keys->data[i],
                     strlen(node->rhs->name))) {
          node->rhs->type = (Type *)node->lhs->type->members->vals->data[i];
        }
      }
      if (node->rhs->type == NULL) {
        fprintf(stderr, "not found such a member '%s'\n", node->rhs->name);
      }
      node->type = node->rhs->type;
      return node->rhs->type;
    } break;
    default:
      node->lhs->type = walk(node->lhs);
      node->rhs->type = walk(node->rhs);
      node->type = checktype_binop(node->lhs, node->rhs);
      if (node->type->kind == T_ADDR || node->type->kind == T_ARRAY) {
        if (node->kind == ND_ADD) {
          node->kind = ND_SUB;
        } else if (node->kind == ND_SUB) {
          node->kind = ND_ADD;
        }
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
    case T_FLOAT: {
      return 4;
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
    case T_STRUCT: {
      int offset = 0;
      for (int i = 0; i < type->members->keys->length; i++) {
        Type *member = (Type *)map_get(type->members,
                                       (char *)type->members->keys->data[i]);
        offset += check_size(member);
      }
      return offset;
    } break;
    default:
      fprintf(stderr, "unexpected type\n");
      return -42;
      break;
  }
}

static Type *checktype_declare(Node *node) {
  if (node->kind == ND_GLOBAL) {
    if (node->type->kind == T_ARRAY || node->type->kind == T_STRUCT) {
      node->var = set_global(node, node->name, strlen(node->name));
    } else {
      node->var = set_global(node, node->name, strlen(node->name));
    }
    node->var->is_gvar = true;
  } else {
    if (node->type->kind == T_ARRAY || node->type->kind == T_STRUCT) {
      node->var = set_lvar(node, node->name, strlen(node->name),
                           check_size(node->type));
    } else {
      node->var =
          set_lvar(node, node->name, strlen(node->name), node->type->offset);
    }
  }
  node->var->type = node->type;
  return node->type;
}

static Type *checktype_binop(Node *lhs, Node *rhs) {
  switch (lhs->type->kind) {
    case T_CHAR:
      if (rhs->type->kind == T_INT) {
        return new_type(T_INT, NULL);
      }
      return new_type(T_CHAR, NULL);
      break;
    case T_INT:
      if (rhs->type->kind != T_FLOAT) {
        return new_type(T_INT, NULL);
      }
      info(format("implicit type conversion %s of left value to %s",
                  type_string(lhs->type), type_string(rhs->type)));
      lhs->type = new_type(T_INT, NULL);
      if (!lhs->is_float) lhs->is_float = true;
      lhs->float_val = (float)lhs->val;
      return new_type(T_FLOAT, NULL);
      break;
    case T_FLOAT:
      if (rhs->type->kind != T_INT) {
        return new_type(T_FLOAT, NULL);
      }
      info(format("implicit type conversion %s of right value to %s",
                  type_string(rhs->type), type_string(lhs->type)));
      rhs->type = new_type(T_INT, NULL);
      if (!rhs->is_float) rhs->is_float = true;
      rhs->float_val = (float)rhs->val;
      return new_type(T_FLOAT, NULL);
      break;
    case T_ADDR:
    case T_ARRAY:
      if (rhs->type->kind == T_INT) {
        return lhs->type;
      }
      break;
    default:
      break;
  }
  return lhs->type;
}
