#define _POSIX_SOURCE
#define _GNU_SOURCE
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <getopt.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define STRONG "\x1b[1m"
#define CLEAR "\x1b[0m"
#define RED "\x1b[31m"
#define GREEN "\x1b[32m"
#define BLUE "\x1b[34m"
#define SKYBLUE "\x1b[36m"
/* util.c*/
typedef struct {
  int length;    // elements of number
  int capacity;  // size of array(allocated)
  void **data;
} Array;

Array *new_ary(void);
Array *with_capacity(int len, int capacity);
void *ary_get(Array *array, int idx);
void ary_push(Array *array, void *elem);
void ary_resize(Array *array);
void *ary_remove(Array *array, int idx);
void *ary_pop(Array *array);
void ary_set(Array *array, int idx, void *elem);
void ary_add(Array *array, int idx, void *elem);
Array *aryget_range(Array *array, int start, int end);

typedef struct {
  Array *keys;
  Array *vals;
} Map;

Map *new_map(void);
void map_put(Map *map, char *key, void *val);
void map_puti(Map *map, char *key, int val);
void *map_get(Map *map, char *key);
int map_geti(Map *map, char *key, int default_);
bool map_exists(Map *map, char *key);

FILE *open_file(const char *filename, const char *mode);
struct stat get_file_info(FILE *file);
char *get_contents(const char *filepath);
void write_contents(const char *filepath, const char *codes);
int length(int array_size, int type_size);
bool startswith(const char *s1, const char *s2);

char *format(char *fmt, ...) __attribute__((format(printf, 1, 2)));
void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);
/* token.c */
typedef enum {
  TK_RESERVED,
  TK_IDENT,
  TK_NUM,
  TK_RETURN,
  TK_EOF,
} TokenKind;

typedef struct Token Token;

struct Token {
  TokenKind kind;
  Token *next;
  int val;
  char *str;
  int len;
};
Token *token;
/* parse.c */
Token *tokenize(char *p);
void program(void);
char *tk_string(TokenKind tk);
typedef struct LVar LVar;

// ローカル変数の型
typedef struct LVar {
  LVar *next;  // 次の変数かNULL
  char *name;  // 変数の名前
  int len;     // 名前の長さ
  int offset;  // RBPからのオフセット
} LVar;
LVar *locals;
/* node.c */
typedef enum {
  ND_ADD,     //+
  ND_SUB,     //-
  ND_MUL,     //
  ND_DIV,     // /
  ND_ASSIGN,  //=
  ND_RETURN,  // return-stmt
  ND_EQ,      // ==
  ND_NTEQ,    // !=
  ND_GT,      // <
  ND_GTEQ,    // <=
  ND_NUM,     // integer
  ND_LVAR,    // local variables
} NodeKind;

typedef struct Node Node;
struct Node {
  NodeKind kind;
  Node *lhs;   // left-child
  Node *rhs;   // right-child
  int val;     // integer-value for integer
  int offset;  // stack-offset for local variables
};
char *nk_string(NodeKind nk);
Node *new_node(NodeKind kind, Node *lhs, Node *rhs);
Node *new_node_num(int val);
Node *new_node_ident(int offset);

Node *code[100];
/* genx86.c */
void gen(Node *node);
