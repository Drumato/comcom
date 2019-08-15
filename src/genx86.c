#include "comcom.h"

char *caller_regs64[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9", NULL};
char *caller_regs32[] = {"edi", "esi", "edx", "ecx", "r8d", "r9d", NULL};
char *caller_regs16[] = {"di", "si", "dx", "cx", "r8w", "r9w", NULL};
char *caller_regs8[] = {"dil", "sil", "dl", "cl", "r8b", "r9b", NULL};
int label = 1;
static void push_reg(char *reg) { printf("  push %s\n", reg); }
static void push_const(int val) { printf("  push %d\n", val); }
static void push_string(int val) { printf("  push offset .LS%d\n", val); }
static void pop_reg(char *reg) { printf("  pop %s\n", reg); }
static void mov_reg_to_reg(char *dst, char *src) {
  printf("  mov %s, %s\n", dst, src);
}
static void compare(char *inst) {
  printf("  cmp rax,rdi\n");
  printf("  %s al\n", inst);
  printf("  movzb rax, al\n");
}
static void lea_reg_to_mem(char *dst, char *src) {
  printf("  lea %s, [%s]\n", dst, src);
}
static void gen_lval(Node *node) {
  if (node->var && node->var->is_gvar) {
    printf("  push offset .%s\n", node->name);
    return;
  }
  switch (node->kind) {
    case ND_ACMEMBER:
      gen(node->lhs);
      pop_reg("rax");
      printf("  add rax, %d\n", node->rhs->type->position);
      break;
    case ND_LVAR:
      mov_reg_to_reg("rax", "rbp");
      printf("  sub rax, %d\n", node->var->offset);
      if (node->type->kind == T_ADDR) {
        lea_reg_to_mem("rax", "rax");
      }
      push_reg("rax");
      break;
    case ND_DEREF:
      gen(node->lhs);
      break;
    default: {
      fprintf(stderr, "unexpected node\n");
    } break;
  }
  if (node->var && node->var->type->kind == T_ADDR &&
      node->var->type->ptr_to->kind == T_CHAR) {
    printf("  lea rax, .LS%d\n", node->val);
    push_reg("rax");
  }
  return;
}
void gen(Node *node) {
  switch (node->kind) {
    case ND_IF:
      gen(node->expr);  // condition
      pop_reg("rax");
      printf("  cmp rax, 0\n");
      if (node->alter) {
        printf("  je .Lelse%d\n", label);
        gen(node->body);
        printf("  jmp .Lend%d\n", label);
        printf(".Lelse%d:\n", label);
        gen(node->alter);
        printf(".Lend%d:\n", label++);
      } else {
        printf("  je .Lend%d\n", label);
        gen(node->body);
        printf(".Lend%d:\n", label++);
      }
      return;
    case ND_WHILE:
      printf(".Lbegin%d:\n", label);
      gen(node->expr);  // condition
      pop_reg("rax");
      printf("  cmp rax, 0\n");
      printf("  je .Lend%d\n", label);
      gen(node->body);
      printf("  jmp .Lbegin%d\n", label);
      printf(".Lend%d:\n", label++);
      return;
    case ND_CALL:
      for (int i = 0; i < node->args->length; i++)
        gen((Node *)node->args->data[i]);
      for (int i = 0; i < node->args->length; i++) {
        char *reg = caller_regs64[i];
        if (reg == NULL) error("exhausted register");
        pop_reg(reg);
      }
      printf("  call %s\n", node->name);
      push_reg("rax");
      return;
    case ND_FOR:
      if (node->init) gen(node->init);  // initialization
      printf(".Lbegin%d:\n", label);
      if (node->expr) gen(node->expr);  // condition
      pop_reg("rax");
      printf("  cmp rax, 0\n");
      printf("  je .Lend%d\n", label);
      gen(node->body);
      if (node->inc) gen(node->inc);  // increment/decrement
      printf("  jmp .Lbegin%d\n", label);
      printf(".Lend%d:\n", label++);
      return;
    case ND_RETURN:
      gen(node->lhs);
      pop_reg("rax");
      mov_reg_to_reg("rsp", "rbp");
      printf("  pop rbp\n");
      printf("  ret\n");
      return;
    case ND_NUM:
      push_const(node->val);
      return;
    case ND_STR:
      push_string(ary_check(strings, node->name));
      return;
    case ND_LVAR:
      gen_lval(node);
      if (node->type->kind != T_ARRAY) {
        pop_reg("rax");
        if (node->type->kind == T_CHAR) {
          printf("  movsx rax, BYTE PTR [rax]\n");
        } else {
          printf("  mov rax, QWORD PTR [rax]\n");
        }
        push_reg("rax");
      }
      return;
    case ND_ASSIGN:
      gen_lval(node->lhs);
      gen(node->rhs);
      pop_reg("rdi");
      pop_reg("rax");
      if (node->lhs->type->kind == T_CHAR) {
        printf("  mov [rax], dil\n");
      } else {
        printf("  mov [rax],rdi\n");
      }
      push_reg("rdi");
      return;
    case ND_BLOCK:
      for (int i = 0; i < node->stmts->length; i++) {
        gen((Node *)node->stmts->data[i]);
      }
      return;
    case ND_FUNC:
      printf("%s:\n", node->name);
      push_reg("rbp");
      mov_reg_to_reg("rbp", "rsp");
      if (node->locals && node->locals->offset != 0) {
        node->locals->offset += 7;
        node->locals->offset &= ~7;
        printf("  sub rsp, %d\n", node->locals->offset);
      }
      for (int i = 0; i < node->args->length; i++) {
        char *reg = caller_regs64[i];
        if (reg == NULL) error("exhausted register");
        printf("  mov -%d[rbp], %s\n",
               ((Node *)node->args->data[i])->var->offset, reg);
        push_reg(reg);
      }
      gen(node->body);
      return;
    case ND_ADDR:
      gen_lval(node->lhs);
      return;
    case ND_DEREF:
      gen(node->lhs);

      printf("  pop rax\n");
      if (node->lhs->type->kind == T_CHAR) {
        printf("  movzx rax, BYTE PTR [rax]\n");
      } else {
        printf("  mov rax, QWORD PTR [rax]\n");
      }
      push_reg("rax");
      return;
    case ND_DEC:
      return;
    case ND_ACMEMBER:
      // gen_lval(node->lhs);
      return;
    case ND_STRUCT:
      printf("  lea rax, -%d[rbp]\n", node->var->offset);
      push_reg("rax");
      return;
    default:
      break;
  }
  gen(node->lhs);
  gen(node->rhs);
  pop_reg("rdi");
  pop_reg("rax");
  switch (node->kind) {
    case ND_ADD:
      if (node->lhs->type->kind == T_ADDR || node->lhs->type->kind == T_ARRAY) {
        // printf("  imul rdi, %d\n", node->lhs->type->offset);
        printf("  imul rdi, %d\n", node->type->offset);
      }
      printf("  add rax, rdi\n");
      break;
    case ND_SUB:
      if (node->lhs->type->kind == T_ADDR || node->lhs->type->kind == T_ARRAY) {
        printf("  imul rdi, %d\n", node->lhs->type->offset);
      }
      printf("  sub rax, rdi\n");
      break;
    case ND_MUL:
      printf("  imul rax, rdi\n");
      break;
    case ND_DIV:
      printf("  cqo\n");
      printf("  idiv rdi\n");
      break;
    case ND_EQ:
      compare("sete");
      break;
    case ND_NTEQ:
      compare("setne");
      break;
    case ND_GT:
      compare("setl");
      break;
    case ND_GTEQ:
      compare("setle");
      break;
    default:
      break;
  }
  push_reg("rax");
}
void gen_global(void) {
  for (int i = 0; i < globals->keys->length; i++) {
    LVar *grob = (LVar *)globals->vals->data[i];
    printf(".%s:\n", grob->name);
    printf("  .zero %d\n", grob->type->offset);
  }
}
void gen_strings(void) {
  for (int i = 0; i < strings->length; i++) {
    char *strlit = (char *)strings->data[i];
    printf(".LS%d:\n", i);
    printf("  .string \"%s\"\n", strlit);
  }
}
