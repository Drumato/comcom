#include "comcom.h"
Token *new_token(int ty, const char *input) {
  Token *token;
  token = (Token *)calloc(1, sizeof(Token));
  token->ty = ty;
  token->input = input;
  return token;
}

void dump_token(Token *token) {
  printf("ty->%s%s%s%s\tinput->%s%s%s%s\n", STRONG, GREEN, tt_string(token->ty),
         CLEAR, STRONG, GREEN, token->input, CLEAR);
}

char *tt_string(int ty) {
  switch (ty) {
    case TK_NUM:  // Number literal
      return "NUMBER";
    case TK_STR:  // String literal
      return "STRING";
    case TK_CH:  // Char literal
      return "CHAR";
    case TK_IDENT:  // Identifier
      return "IDENTIFIER";
    case TK_PARAM:  // Function-like macro parameter
      return "PARAM";
    case TK_EOF:  // End marker
      return "EOF";

    /* type keywords*/
    case TK_AUTO:  // "auto"
      return "AUTO";
    case TK_BREAK:  // "break"
      return "BREAK";
    case TK_CASE:  // "case"
      return "CASE";
    case TK_CHAR:  // "char"
      return "CHAR";
    case TK_CONST:  // "const"
      return "CONST";
    case TK_CONTINUE:  // "continue"
      return "CONTINUE";
    case TK_DEFAULT:  // "default"
      return "DEFAULT";
    case TK_DO:  // "do"
      return "DO";
    case TK_DOUBLE:  // "double"
      return "DOUBLE";
    case TK_ELSE:  // "else"
      return "ELSE";
    case TK_ENUM:  // "enum"
      return "ENUM";
    case TK_EXTERN:  // "extern"
      return "EXTERN";
    case TK_FLOAT:  // "float"
      return "FLOAT";
    case TK_FOR:  // "for"
      return "FOR";
    case TK_GOTO:  // "goto"
      return "GOTO";
    case TK_IF:  // "if"
      return "IF";
    case TK_INLINE:  // "inline"
      return "INLINE";
    case TK_INT:  // "int"
      return "INT";
    case TK_LONG:  // "long"
      return "LONG";
    case TK_REGISTER:  // "register"
      return "REGISTER";
    case TK_RESTRICT:  // "restrict"
      return "RESTRICT";
    case TK_RETURN:  // "return"
      return "RETURN";
    case TK_SHORT:  // "short"
      return "SHORT";
    case TK_SIGNED:  // "signed"
      return "SIGNED";
    case TK_SIZEOF:  // "sizeof"
      return "SIZEOF";
    case TK_STATIC:  // "static"
      return "STATIC";
    case TK_STRUCT:  // "struct"
      return "STRUCT";
    case TK_SWITCH:  // "switch"
      return "SWITCH";
    case TK_TYPEDEF:  // "typedef"
      return "TYPEDEF";
    case TK_UNION:  // "union"
      return "UNION";
    case TK_UNSIGNED:  // "unsigned"
      return "UNSIGNED";
    case TK_VOID:  // "void"
      return "VOID";
    case TK_VOLATILE:  // "volatile"
      return "VOLATILE";
    case TK_WHILE:  // "while"
      return "WHILE";
    case TK_ALIGNAS:  // "_Alignas"
      return "ALIGNAS";
    case TK_ALIGNOF:  // "_Alignof"
      return "ALIGNOF";
    case TK_ATOMIC:  // "_Atomic"
      return "ATOMIC";
    case TK_BOOL:  // "_Bool"
      return "BOOL";
    case TK_COMPLEX:  // "_Complex"
      return "COMPLEX";
    case TK_GENERIC:  // "_Generic"
      return "GENERIC";
    case TK_IMAGINARY:  // "_Imaginary"
      return "IMAGINARY";
    case TK_NORETURN:  // "_Noreturn"
      return "NORETURN";
    case TK_STATICASSERT:  // "_Static_assert"
      return "STATICASSERT";
    case TK_THREADLOCAL:  // "_Thread_local"
      return "THREADLOCAL";

    /* symbols */
    case TK_AMPERSAND:  // &
      return "AMPERSAND";
    case TK_COLON:  // :
      return "COLON";
    case TK_SEMICOLON:  // ;
      return "SEMICOLON";
    case TK_DOT:  // .
      return "DOT";
    case TK_COMMA:  //
      return "COMMA";
    case TK_LPAREN:  // (
      return "LPAREN";
    case TK_RPAREN:  // )
      return "RPAREN";
    case TK_LBRACE:  // {
      return "LBRACE";
    case TK_RBRACE:  // }
      return "RBRACE";
    case TK_LBRACKET:  // [
      return "LBRACKET";
    case TK_RBRACKET:  // ]
      return "RBRACKET";
    case TK_ARROW:  // ->
      return "ARROW";
    case TK_ADD:  // +
      return "ADD";
    case TK_SUB:  // -
      return "SUB";
    case TK_MUL:  // *
      return "MUL";
    case TK_DIV:  // '/'
      return "DIV";
    case TK_MOD:  // %
      return "MOD";
    case TK_ASSIGN:  // =
      return "ASSIGN";
    case TK_EQ:  // ==
      return "EQUAL";
    case TK_NE:  // !=
      return "NOT_EQ";
    case TK_LT:  // <
      return "LT";
    case TK_GT:  // >
      return "GT";
    case TK_LE:  // <=
      return "LT_EQUAL";
    case TK_GE:  // >=
      return "GT_EQUAL";
    case TK_LOGOR:  // ||
      return "LOGOR";
    case TK_LOGAND:  // &&
      return "LOGAND";
    case TK_XOR:  // ^
      return "XOR";
    case TK_OR:  // |
      return "OR";
    case TK_SHL:  // <<
      return "LSHIFT";
    case TK_SHR:  // >>
      return "RSHIFT";
    case TK_INC:  // ++
      return "INCREMENT";
    case TK_DEC:  // --
      return "DECREMENT";
    case TK_MUL_EQ:  // *=
      return "MUL_EQ";
    case TK_DIV_EQ:  // /=
      return "DIV_EQ";
    case TK_MOD_EQ:  // %=
      return "MOD_EQ";
    case TK_ADD_EQ:  // +=
      return "ADD_EQ";
    case TK_SUB_EQ:  // -=
      return "SUB_EQ";
    case TK_SHL_EQ:  // <<=
      return "LSHIFT_EQ";
    case TK_SHR_EQ:  // >>=
      return "RSHIFT_EQ";
    case TK_AND_EQ:  // &=
      return "AND_EQ";
    case TK_XOR_EQ:  // ^=
      return "XOR_EQ";
    case TK_OR_EQ:  // |=
      return "OR_EQ";
    case TK_SLASH:  // '\'
      return "SLASH";
    case TK_EXCLAMATION:  // !
      return "EXCLAMATION";
    case TK_QUESTION:  // ?
      return "QUESTION";
    case TK_HASH:  // #
      return "HASH";
    default:
      return "ILlEGAL";
  }
}

bool is_typename(int type) {
  switch (type) {
    case TK_CHAR:
    case TK_VOID:
    case TK_SHORT:
    case TK_LONG:
    case TK_DOUBLE:
    case TK_FLOAT:
    case TK_INT:
      return true;
      break;
    default:
      return false;
      break;
  };
}
