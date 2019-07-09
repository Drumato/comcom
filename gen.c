#include "comcom.h"
static char *reg64[] = {"rax", "rbx", "rcx", "rdx", "rsi", "rdi", "rbp", "rsp"};
static char *xreg64[] = {"r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15"};
static FILE *out;
static int regnum;

static void gen_func(Function *func);
static void gen_stmt(Node *node);
static void gen_expr(Node *node);
void generate(Rootnode *rootnode) {
  regnum = 0;
  out = stdout;
  fprintf(out, ".intel_syntax noprefix\n");
  fprintf(out, ".globl main\n");
  for (int i = 0; i < rootnode->functions->length; i++) {
    Function *func = (Function *)rootnode->functions->data[i];
    fprintf(out, "%s:\n", func->name);
    gen_func(func);
  }
}

static void gen_func(Function *func) {
  for (int i = 0; i < func->nodes->length; i++) {
    gen_stmt((Node *)func->nodes->data[i]);
  }
}

static void gen_stmt(Node *node) {
  switch (node->nty) {
    case N_RETURN:
      gen_expr(node->expr);
      fprintf(out, "    pop %s\n", reg64[++regnum]);
      fprintf(out, "    mov rax, %s\n", reg64[regnum]);
      fprintf(out, "    ret\n");
      break;
  }
}
static void gen_expr(Node *node) {
  switch (node->nty) {
    case N_INTEGER:
      fprintf(out, "    push 0x%x\n", node->intval);
      break;
    default:
      gen_expr(node->lch);
      gen_expr(node->rch);
      char *lop = reg64[regnum++];
      char *rop = reg64[regnum--];
      fprintf(out, "    pop %s\n", lop);
      fprintf(out, "    pop %s\n", rop);
      switch (node->nty) {
        case N_ADD:
          fprintf(out, "    add %s, %s\n", lop, rop);
          fprintf(out, "    push %s\n", lop);
          break;
        case N_SUB:
          fprintf(out, "    sub %s, %s\n", lop, rop);
          fprintf(out, "    push %s\n", lop);
          break;
        case N_MUL:
          fprintf(out, "    add %s, %s\n", lop, rop);
          fprintf(out, "    push %s\n", lop);
          break;
        case N_DIV:
          fprintf(out, "    add %s, %s\n", lop, rop);
          fprintf(out, "    push %s\n", lop);
          break;
      }
  }
}
