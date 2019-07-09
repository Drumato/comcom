#include "comcom.h"

enum {
  F_DUMPSOURCE = 1,
  F_DUMPIR = 2,
  F_DUMPTOKEN = 4,
  F_DUMPAST = 8,
};

static void help() {
  printf(
      "Options:\n"
      "\t--dump-ir\tDump IRs.\n"
      "\t--dump-token\tDump all tokens with lexer.\n"
      "\t--help\tshow all option that implemented in omo\n"
      "\t--version\tprint ccomo version\n");
}

static void version() { printf("ccomo version 0.1.0\n"); }

int main(int argc, char *argv[]) {
  int do_flag = 0;
  if (argc < 2) {
    fprintf(stderr,
            "ccomo: %s%sfatal error%s: no input files\n"
            "compilation terminated.\n",
            STRONG, RED, CLEAR);
    exit(1);
  }
  FILE *fp = fopen(argv[argc - 1], "r");
  if (fp == NULL) {
    fprintf(stderr,
            "ccomo: %s%sfatal error%s: invalid file '%s'\n"
            "compilation terminated.\n",
            STRONG, RED, CLEAR, argv[argc - 1]);
    exit(1);
  }

  fclose(fp);
  int opt;
  struct option longopts[] = {
      {"dump-ir", no_argument, NULL, 'i'},
      {"help", no_argument, NULL, 'h'},
      {"version", no_argument, NULL, 'v'},
      {"dump-source", no_argument, NULL, 's'},
      {"dump-token", no_argument, NULL, 't'},
      {"dump-ast", no_argument, NULL, 'a'},
      {0, 0, 0, 0},
  };
  while ((opt = getopt_long(argc, argv, "ihvsta:", longopts, NULL)) != -1) {
    switch (opt) {
      case 'h':
        help();
        break;
      case 'v':
        version();
        break;
      case 's':
        do_flag |= F_DUMPSOURCE;
        break;
      case 't':
        do_flag |= F_DUMPTOKEN;
        break;
      case 'a':
        do_flag |= F_DUMPAST;
        break;
      default:
        break;
    }
  }
  char *codes = get_contents(argv[optind]);
  if (do_flag & F_DUMPSOURCE) {
    printf("%s%s------------dump source------------%s\n", STRONG, BLUE, CLEAR);
    printf("%s", codes);
  }
  Array *tokens = lexing(argv[optind], codes);
  if (do_flag & F_DUMPTOKEN) {
    printf("%s%s------------dump tokens------------%s\n", STRONG, BLUE, CLEAR);
    for (int i = 0; i < tokens->length; i++)
      dump_token((Token *)tokens->data[i]);
  }
  Rootnode *rootnode = parse(tokens);
  if (do_flag & F_DUMPAST) {
    printf("%s%s------------dump ast------------%s\n", STRONG, BLUE, CLEAR);
    for (int i = 0; i < rootnode->functions->length; i++) {
      Function *func = (Function *)rootnode->functions->data[i];
      printf("%s%s%s's stmts%s\n", STRONG, GREEN, func->name, CLEAR);
      for (int j = 0; j < func->nodes->length; j++) {
        Node *node = (Node *)malloc(sizeof(Node));
        memmove(node, (Node *)func->nodes->data[j], sizeof(Node));
        dump_node(node);
        free(node);
      }
    }
  }
  generate(rootnode);
  return 0;
}
