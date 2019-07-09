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
/* token.c */

typedef struct {
  int ty;
  int intval;
  float floatval;
  char *str;
  const char *input;
} Token;

enum {
  /* types*/
  TK_NUM = 256,  // Number literal
  TK_STR,        // String literal
  TK_CH,         // Char literal
  TK_IDENT,      // Identifier
  TK_PARAM,      // Function-like macro parameter
  TK_EOF,        // End marker

  /* type keywords*/
  TK_AUTO,          // "auto"
  TK_BREAK,         // "break"
  TK_CASE,          // "case"
  TK_CHAR,          // "char"
  TK_CONST,         // "const"
  TK_CONTINUE,      // "continue"
  TK_DEFAULT,       // "default"
  TK_DO,            // "do"
  TK_DOUBLE,        // "double"
  TK_ELSE,          // "else"
  TK_ENUM,          // "enum"
  TK_EXTERN,        // "extern"
  TK_FLOAT,         // "float"
  TK_FOR,           // "for"
  TK_GOTO,          // "goto"
  TK_IF,            // "if"
  TK_INLINE,        // "inline"
  TK_INT,           // "int"
  TK_LONG,          // "long"
  TK_REGISTER,      // "register"
  TK_RESTRICT,      // "restrict"
  TK_RETURN,        // "return"
  TK_SHORT,         // "short"
  TK_SIGNED,        // "signed"
  TK_SIZEOF,        // "sizeof"
  TK_STATIC,        // "static"
  TK_STRUCT,        // "struct"
  TK_SWITCH,        // "switch"
  TK_TYPEDEF,       // "typedef"
  TK_UNION,         // "union"
  TK_UNSIGNED,      // "unsigned"
  TK_VOID,          // "void"
  TK_VOLATILE,      // "volatile"
  TK_WHILE,         // "while"
  TK_ALIGNAS,       // "_Alignas"
  TK_ALIGNOF,       // "_Alignof"
  TK_ATOMIC,        // "_Atomic"
  TK_BOOL,          // "_Bool"
  TK_COMPLEX,       // "_Complex"
  TK_GENERIC,       // "_Generic"
  TK_IMAGINARY,     // "_Imaginary"
  TK_NORETURN,      // "_Noreturn"
  TK_STATICASSERT,  // "_Static_assert"
  TK_THREADLOCAL,   // "_Thread_local"

  /* symbols */
  TK_AMPERSAND,    // &
  TK_COLON,        // :
  TK_SEMICOLON,    // ;
  TK_DOT,          // .
  TK_COMMA,        // ,
  TK_LPAREN,       // (
  TK_RPAREN,       // )
  TK_LBRACE,       // {
  TK_RBRACE,       // }
  TK_LBRACKET,     // [
  TK_RBRACKET,     // ]
  TK_ARROW,        // ->
  TK_ADD,          // +
  TK_SUB,          // -
  TK_MUL,          // *
  TK_DIV,          // '/'
  TK_MOD,          // %
  TK_ASSIGN,       // =
  TK_EQ,           // ==
  TK_NE,           // !=
  TK_LT,           // <
  TK_GT,           // >
  TK_LE,           // <=
  TK_GE,           // >=
  TK_LOGOR,        // ||
  TK_LOGAND,       // &&
  TK_XOR,          // ^
  TK_OR,           // |
  TK_SHL,          // <<
  TK_SHR,          // >>
  TK_INC,          // ++
  TK_DEC,          // --
  TK_MUL_EQ,       // *=
  TK_DIV_EQ,       // /=
  TK_MOD_EQ,       // %=
  TK_ADD_EQ,       // +=
  TK_SUB_EQ,       // -=
  TK_SHL_EQ,       // <<=
  TK_SHR_EQ,       // >>=
  TK_AND_EQ,       // &=
  TK_XOR_EQ,       // ^=
  TK_NOT_EQ,       // !=
  TK_OR_EQ,        // |=
  TK_QUOTE,        // '
  TK_DOUBLEQUOTE,  // "
  TK_SLASH,        // '/'
  TK_EXCLAMATION,  // !
  TK_QUESTION,     // ?
  TK_HASH,         // #

  TK_ILLEGAL,
};

Token *new_token(int ty, const char *input);
void dump_token(Token *token);
char *tt_string(int ty);
bool is_typename(int type);

/* lex.c */

typedef struct {
  char ch;
  int pos;
  const char *filename;
} Lexer;

Token *next_token(Lexer *lexer);
Array *lexing(const char *filepath, char *code);

/* logger.c*/

void log_print(char log_txt[256], ...);

/* error.c*/
enum {
  PARSE,
  TYPE,
};
char *dump_error(int ty);
void error_found(int ty, const char *msg);

enum {
  N_INTEGER,   // 30
  N_STRING,    // "30"
  N_IDENT,     // x
  N_ADD,       // 30 + 50
  N_SUB,       // 30 - 50
  N_MUL,       // 30 * 50
  N_DIV,       // 30 / 50
  N_RETURN,    // return <expr>
  N_EXPRSTMT,  // printf();
  N_INVALID,
};
enum {
  C_INTEGER,
  C_CHAR,
  C_PTR,
};

enum {
  V_AUTO,
  V_GLOBAL,
};
typedef struct {
  int ty;
  int stacksize;
  int vty;
} Type;
typedef struct Node Node;
typedef struct Node {
  int nty;  // nodetype
  int intval;
  char charval;
  char *strval;
  float floatval;
  char name[31];
  Type *cty;
  Node *lch;  // left-child
  Node *rch;  // right-child
  Node *cond;
  Node *alter;
  Node *init;
  Node *incdec;
  Array *stmts;
  Node *expr;
  Node *index;
  Array *args;
} Node;
typedef struct {
  char name[31];
  Array *nodes;
} Function;

/* parse.c */
typedef struct {
  Array *functions;
} Rootnode;
typedef struct {
  int pos;
  Token *cur;
  Token *next;
  Array *tokens;
} Parser;

/* node.c */

Type *new_int(void);
Type *new_char(void);
Type *new_ptr(void);
void dump_node(Node *node);
char *ct_string(int cty);
char *nt_string(int nty);
Rootnode *parse(Array *tokens);
Node *new_binop(int nty, Node *lch, Node *rch);
Node *new_number(const char *input);
Node *new_string(const char *input);

/*gen.c*/
void generate(Rootnode *rootnode);
