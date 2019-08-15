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
int ary_check(Array *array, char *val);

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
  TK_INT,
  TK_CHAR,
  TK_STR,
  TK_IF,
  TK_ELSE,
  TK_WHILE,
  TK_FOR,
  TK_SIZEOF,
  TK_ADDR,
  TK_EOF,
} TokenKind;

typedef struct Token Token;

struct Token {
  TokenKind kind;
  Token *next;
  int val;
  char *str;
  int len;
  Token *ptr_to;
};
Token *token;
/* parse.c */
Token *tokenize(char *p);
void program(void);
char *tk_string(TokenKind tk);
typedef struct LVar LVar;

typedef struct Type Type;
// ローカル変数の型
typedef struct LVar {
  LVar *next;  // 次の変数かNULL
  char *name;  // 変数の名前
  int len;     // 名前の長さ
  int offset;  // RBPからのオフセット
  Type *type;  // the type of local-var
  bool is_gvar;
} LVar;
LVar *locals;
Map *globals;
LVar *find_lvar(char *str, int len);

/* node.c */
typedef enum {
  ND_ADD,     //+
  ND_SUB,     //-
  ND_MUL,     //
  ND_DIV,     // /
  ND_ASSIGN,  // =
  ND_DEC,     // int x;
  ND_INIT,    // int x = 0;
  ND_CALL,    // call-expression
  ND_IF,      // if-stmt
  ND_WHILE,   // while-stmt
  ND_FOR,     // for-stmt
  ND_RETURN,  // return-stmt
  ND_FUNC,    // function
  ND_EQ,      // ==
  ND_NTEQ,    // !=
  ND_GT,      // <
  ND_GTEQ,    // <=
  ND_NUM,     // integer
  ND_STR,     // string
  ND_LVAR,    // local variables
  ND_GLOBAL,  // global variables
  ND_BLOCK,   // { stmt* }
  ND_ADDR,    // &x
  ND_DEREF,   // *x
  ND_SIZEOF,  // sizeof
} NodeKind;

typedef enum {
  T_INT,
  T_CHAR,
  T_ADDR,
  T_ARRAY,
} TypeKind;

typedef struct Type Type;
typedef struct Type {
  TypeKind kind;
  int offset;
  Type *ptr_to;
  size_t ary_size;
} Type;

Type *new_type(TypeKind kind, Type *ptr_to);
char *type_string(Type *type);

typedef struct Node Node;
struct Node {
  NodeKind kind;
  Node *expr;    // expression and condition
  Node *body;    // body with if-stmt
  Node *alter;   // body with else-stmt
  Array *stmts;  // statements in ND_BLOCK
  Array *args;   // arguments in ND_CALL
  Node *lhs;     // left-child
  Node *rhs;     // right-child
  Node *init;    // for(init)
  Node *inc;     // for(incdec)
  Type *type;    // indicates node_type
  char *name;    // function names
  int val;       // integer-value for integer
  LVar *var;     // local-variables
  LVar *locals;
};
char *nk_string(NodeKind nk);
Node *new_node(NodeKind kind, Node *lhs, Node *rhs);
Node *new_node_num(int val);
Node *new_node_ident(int offset);
Node *new_node_type(Token *tok);
Type *inference_type(Token *tok);
Array *strings;

Node *code[100];

/* sema.c */

void semantic(void);

/* genx86.c */
void gen(Node *node);
void gen_global(void);
void gen_strings(void);

