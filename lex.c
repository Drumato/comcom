#include "comcom.h"
static char *source_code;
static Lexer *new_lexer(const char *filepath);
Array *lexing(const char *filepath, char *code) {
  Array *tokens = new_ary();
  source_code = code;
  Lexer *lexer = new_lexer(filepath);
  Token *token;
  while ((token = next_token(lexer))->ty != TK_EOF) ary_push(tokens, token);
  ary_push(tokens, token);
  free(lexer);
  return tokens;
}
static int lookup_identifier(const char *input);
static Lexer *new_lexer(const char *filepath) {
  Lexer *lexer;
  if ((lexer = (Lexer *)calloc(1, sizeof(Lexer))) == NULL) {
    fprintf(stderr, "can't allocate memory for Lexer\n");
  }
  lexer->pos = 0;
  lexer->filename = filepath;
  lexer->ch = source_code[lexer->pos];
  return lexer;
}

static void _read_char(Lexer *lexer) {
  char c;
  if ((c = source_code[lexer->pos + 1]) == '\0') {
    lexer->ch = 0;
  } else {
    lexer->ch = source_code[lexer->pos + 1];
  }
  lexer->pos = lexer->pos + 1;
}

static char peak_char(Lexer *lexer) {
  char c;
  if ((c = source_code[lexer->pos + 1]) == '\0') {
    return '\0';
  } else {
    return source_code[lexer->pos + 1];
  }
}

static bool is_whitespace(Lexer *lexer) {
  return (lexer->ch == '\t' || lexer->ch == '\n' || lexer->ch == ' ');
}
static void skip_whitespace(Lexer *lexer) {
  while (is_whitespace(lexer)) _read_char(lexer);
}

static bool is_decimal(Lexer *lexer) {
  return (lexer->ch >= '0' && lexer->ch <= '9');
}

static bool is_hexdecimal(Lexer *lexer) {
  return ((lexer->ch >= '0' && lexer->ch <= '9') ||
          (lexer->ch >= 'A' && lexer->ch <= 'F') ||
          (lexer->ch >= 'a' && lexer->ch <= 'f'));
}

static bool is_octal(Lexer *lexer) {
  return (lexer->ch >= '0' && lexer->ch <= '7');
}

static bool is_ascii_text(Lexer *lexer) {
  return ((lexer->ch >= 'a' && lexer->ch <= 'z') ||
          (lexer->ch >= 'A' && lexer->ch <= 'Z') || (lexer->ch == '_'));
}

static char *read_number(Lexer *lexer) {
  char *start = &source_code[lexer->pos];
  size_t size = 1;
  if (lexer->ch == '0') {
    _read_char(lexer);
    size++;
    switch (lexer->ch) {
      case 'x':
      case 'X': {
        _read_char(lexer);
        size++;
        while (is_hexdecimal(lexer)) {
          _read_char(lexer);
          size++;
        }
      } break;
      default: {
        while (is_octal(lexer)) {
          _read_char(lexer);
          size++;
        }
      } break;
    }
  } else {
    while (is_decimal(lexer)) {
      _read_char(lexer);
      size++;
    }
  }
  assert(size > 0);
  char *buf = (char *)malloc(sizeof(char) * size);
  if (memccpy(buf, start, '\0', size - 1) == NULL) {
    buf[size - 1] = '\0';
  }
  return buf;
}

static char *read_string(Lexer *lexer) {
  _read_char(lexer);  // ignore "
  char *start = &source_code[lexer->pos];
  size_t size = 1;
  while (lexer->ch != '"') {
    _read_char(lexer);
    size++;
  }
  _read_char(lexer);  // ignore "
  assert(size > 0);
  char *buf = (char *)malloc(sizeof(char) * size);
  if (memccpy(buf, start, '\0', size - 1) == NULL) {
    buf[size - 1] = '\0';
  }
  return buf;
}

static char *read_char(Lexer *lexer) {
  _read_char(lexer);  // ignore '
  char *ret = &source_code[lexer->pos];
  if (lexer->ch == '\'') {
    error_found(PARSE, "ch modify comment's format -> '/* ~~~ */'");
  }
  size_t size = 1;
  while (lexer->ch != '\'') {
    _read_char(lexer);
    size++;
  }
  char *buf = (char *)malloc(sizeof(char) * size);
  if (memccpy(buf, ret, '\0', size - 1) == NULL) {
    buf[size - 1] = '\0';
  }
  return buf;
}

static Token *read_identifier(Lexer *lexer) {
  char *start = &source_code[lexer->pos];
  size_t size = 1;
  while (is_ascii_text(lexer) || is_decimal(lexer)) {
    _read_char(lexer);
    size++;
  };
  assert(size > 0);
  char *buf = (char *)malloc(sizeof(char) * size);
  if (memccpy(buf, start, '\0', size - 1) == NULL) {
    buf[size - 1] = '\0';
  }
  return new_token(lookup_identifier(buf), buf);
}

Token *next_token(Lexer *lexer) {
  skip_whitespace(lexer);
  Token *t;
  switch (lexer->ch) {
    case '#': {
      t = new_token(TK_HASH, "#");
      break;
    } break;
    case '+': {
      switch (peak_char(lexer)) {
        case '+': {
          _read_char(lexer);
          t = new_token(TK_INC, "++");
          break;
        }
        case '=': {
          _read_char(lexer);
          t = new_token(TK_ADD_EQ, "+=");
          break;
        }
        default: {
          t = new_token(TK_ADD, "+");
          break;
        }
      }
    } break;
    case '-': {
      switch (peak_char(lexer)) {
        case '>': {
          _read_char(lexer);
          t = new_token(TK_ARROW, "->");
          break;
        }
        case '-': {
          _read_char(lexer);
          t = new_token(TK_DEC, "--");
          break;
        }
        case '=': {
          _read_char(lexer);
          t = new_token(TK_SUB_EQ, "-=");
          break;
        }
        default: {
          t = new_token(TK_SUB, "-");
          break;
        }
      }
    } break;
    case '*': {
      switch (peak_char(lexer)) {
        case '=': {
          _read_char(lexer);
          t = new_token(TK_MUL_EQ, "*=");
          break;
        }
        default: {
          t = new_token(TK_MUL, "*");
          break;
        }
      }
    } break;
    case '%': {
      switch (peak_char(lexer)) {
        case '=': {
          _read_char(lexer);
          t = new_token(TK_MOD_EQ, "%=");
          break;
        }
        default: {
          t = new_token(TK_MOD, "%");
          break;
        }
      }
    } break;
    case '=': {
      switch (peak_char(lexer)) {
        case '=': {
          _read_char(lexer);
          t = new_token(TK_EQ, "==");
          break;
        }
        default:
          t = new_token(TK_ASSIGN, "=");
          break;
      }
    } break;
    case '!': {
      switch (peak_char(lexer)) {
        case '=': {
          _read_char(lexer);
          t = new_token(TK_NOT_EQ, "!=");
          break;
        }
        default:
          t = new_token(TK_EXCLAMATION, "!");
          break;
      }
    } break;
    case '&': {
      switch (peak_char(lexer)) {
        case '&': {
          _read_char(lexer);
          t = new_token(TK_LOGAND, "&&");
          break;
        }
        case '=': {
          _read_char(lexer);
          t = new_token(TK_AND_EQ, "&=");
          break;
        }
        default: {
          t = new_token(TK_AMPERSAND, "&");
          break;
        }
      }
    } break;
    case '(': {
      t = new_token(TK_LPAREN, "(");
      break;
    } break;
    case ')': {
      t = new_token(TK_RPAREN, ")");
      break;
    } break;
    case '[': {
      t = new_token(TK_LBRACKET, "[");
      break;
    } break;
    case ']': {
      t = new_token(TK_RBRACKET, "]");
      break;
    } break;
    case '{': {
      t = new_token(TK_LBRACE, "{");
      break;
    } break;
    case '}': {
      t = new_token(TK_RBRACE, "}");
      break;
    } break;
    case ':': {
      t = new_token(TK_COLON, ":");
      break;
    } break;
    case ';': {
      t = new_token(TK_SEMICOLON, ";");
      break;
    } break;
    case '.': {
      t = new_token(TK_DOT, ".");
      break;
    } break;
    case ',': {
      t = new_token(TK_COMMA, ",");
      break;
    } break;
    case '^': {
      switch (peak_char(lexer)) {
        case '=': {
          _read_char(lexer);
          t = new_token(TK_XOR_EQ, "^=");
          break;
        }
        default:
          t = new_token(TK_XOR, "^");
          break;
      }
    } break;
    case '/': {
      switch (peak_char(lexer)) {
        case '/':
          while (peak_char(lexer) != '\n') {
            _read_char(lexer);
          }
          t = new_token(TK_ILLEGAL, "");
          break;
        case '*': {
          while (peak_char(lexer) != '*') {
            _read_char(lexer);
          }
          _read_char(lexer);
          if (peak_char(lexer) != '/')
            error_found(PARSE, "please modify comment's format -> '/* ~~~ */'");
          break;
        }
        case '=': {
          _read_char(lexer);
          t = new_token(TK_DIV_EQ, "/=");
          break;
        }
        default: {
          t = new_token(TK_DIV, "/");
          break;
        }
      }
    } break;
    case '\0': {
      t = new_token(TK_EOF, "\0");
      break;
    } break;
    case '<': {
      switch (peak_char(lexer)) {
        case '<': {
          switch (peak_char(lexer)) {
            case '=': {
              _read_char(lexer);
              t = new_token(TK_SHL_EQ, "<<=");
              break;
            }
            default: {
              _read_char(lexer);
              t = new_token(TK_SHL, "<<");
              break;
            }
          }
        }
        case '=': {
          _read_char(lexer);
          t = new_token(TK_LE, "<=");
          break;
        }
        default: {
          t = new_token(TK_LT, "<");
          break;
        }
      }
    } break;
    case '>': {
      switch (peak_char(lexer)) {
        case '>':
          switch (peak_char(lexer)) {
            case '=':
              _read_char(lexer);
              t = new_token(TK_SHR_EQ, ">>=");
              break;
            default:
              _read_char(lexer);
              t = new_token(TK_SHR, ">>");
              break;
          }
        case '=':
          _read_char(lexer);
          t = new_token(TK_GE, ">=");
          break;
        default:
          t = new_token(TK_GT, ">");
          break;
      }
    } break;
    case '|': {
      switch (peak_char(lexer)) {
        case '|':
          _read_char(lexer);
          t = new_token(TK_LOGOR, "||");
          break;
        case '=':
          _read_char(lexer);
          t = new_token(TK_OR_EQ, "|=");
          break;
        default:
          t = new_token(TK_OR, "|");
          break;
      }
    } break;
    case '"': {
      t = new_token(TK_STR, read_string(lexer));
      return t;
      break;
    }
    case '\'': {
      t = new_token(TK_CH, read_char(lexer));
      break;
    }
    default:
      if (is_decimal(lexer)) {
        char *lit = read_number(lexer);
        t = new_token(TK_NUM, lit);
        return t;
      } else if (is_ascii_text(lexer)) {
        t = read_identifier(lexer);
        return t;
      }
  }
  _read_char(lexer);
  return t;
}

static int lookup_identifier(const char *input) {
  if (strncmp(input, "auto", 4) == 0 && strlen(input) == strlen("auto"))
    return TK_AUTO;
  if (strncmp(input, "break", 5) == 0 && strlen(input) == strlen("break"))
    return TK_BREAK;
  if (strncmp(input, "case", 4) == 0 && strlen(input) == strlen("case"))
    return TK_CASE;
  if (strncmp(input, "char", 4) == 0 && strlen(input) == strlen("char"))
    return TK_CHAR;
  if (strncmp(input, "const", 5) == 0 && strlen(input) == strlen("const"))
    return TK_CONST;
  if (strncmp(input, "continue", 7) == 0 && strlen(input) == strlen("continue"))
    return TK_CONTINUE;
  if (strncmp(input, "default", 7) == 0 && strlen(input) == strlen("default"))
    return TK_DEFAULT;
  if (strncmp(input, "do", 2) == 0 && strlen(input) == strlen("do"))
    return TK_DO;
  if (strncmp(input, "double", 6) == 0 && strlen(input) == strlen("double"))
    return TK_DOUBLE;
  if (strncmp(input, "else", 4) == 0 && strlen(input) == strlen("else"))
    return TK_ELSE;
  if (strncmp(input, "enum", 4) == 0 && strlen(input) == strlen("enum"))
    return TK_ENUM;
  if (strncmp(input, "extern", 6) == 0 && strlen(input) == strlen("extern"))
    return TK_EXTERN;
  if (strncmp(input, "float", 5) == 0 && strlen(input) == strlen("float"))
    return TK_FLOAT;
  if (strncmp(input, "for", 3) == 0 && strlen(input) == strlen("for"))
    return TK_FOR;
  if (strncmp(input, "goto", 4) == 0 && strlen(input) == strlen("goto"))
    return TK_GOTO;
  if (strncmp(input, "if", 2) == 0 && strlen(input) == strlen("if"))
    return TK_IF;
  if (strncmp(input, "inline", 6) == 0 && strlen(input) == strlen("inline"))
    return TK_INLINE;
  if (strncmp(input, "int", 3) == 0 && strlen(input) == strlen("int"))
    return TK_INT;
  if (strncmp(input, "long", 4) == 0 && strlen(input) == strlen("long"))
    return TK_LONG;
  if (strncmp(input, "register", 8) == 0 && strlen(input) == strlen("register"))
    return TK_REGISTER;
  if (strncmp(input, "restrict", 8) == 0 && strlen(input) == strlen("restrict"))
    return TK_RESTRICT;
  if (strncmp(input, "return", 6) == 0 && strlen(input) == strlen("return"))
    return TK_RETURN;
  if (strncmp(input, "short", 5) == 0 && strlen(input) == strlen("short"))
    return TK_SHORT;
  if (strncmp(input, "signed", 6) == 0 && strlen(input) == strlen("signed"))
    return TK_SIGNED;
  if (strncmp(input, "sizeof", 6) == 0 && strlen(input) == strlen("sizeof"))
    return TK_SIZEOF;
  if (strncmp(input, "static", 6) == 0 && strlen(input) == strlen("static"))
    return TK_STATIC;
  if (strncmp(input, "struct", 6) == 0 && strlen(input) == strlen("struct"))
    return TK_STRUCT;
  if (strncmp(input, "switch", 6) == 0 && strlen(input) == strlen("switch"))
    return TK_SWITCH;
  if (strncmp(input, "typedef", 7) == 0 && strlen(input) == strlen("typedef"))
    return TK_TYPEDEF;
  if (strncmp(input, "union", 5) == 0 && strlen(input) == strlen("union"))
    return TK_UNION;
  if (strncmp(input, "unsigned", 8) == 0 && strlen(input) == strlen("unsigned"))
    return TK_UNSIGNED;
  if (strncmp(input, "void", 4) == 0 && strlen(input) == strlen("void"))
    return TK_VOID;
  if (strncmp(input, "volatile", 8) == 0 && strlen(input) == strlen("volatile"))
    return TK_VOLATILE;
  if (strncmp(input, "while", 5) == 0 && strlen(input) == strlen("while"))
    return TK_WHILE;
  if (strncmp(input, "_Alignas", 8) == 0 && strlen(input) == strlen("_Alignas"))
    return TK_ALIGNAS;
  if (strncmp(input, "_Alignof", 8) == 0 && strlen(input) == strlen("_Alignof"))
    return TK_ALIGNOF;
  if (strncmp(input, "_Atomic", 7) == 0 && strlen(input) == strlen("Atomic"))
    return TK_ATOMIC;
  if (strncmp(input, "_Bool", 5) == 0 && strlen(input) == strlen("_Bool"))
    return TK_BOOL;
  if (strncmp(input, "_Complex", 8) == 0 && strlen(input) == strlen("_Complex"))
    return TK_COMPLEX;
  if (strncmp(input, "_Generic", 8) == 0 && strlen(input) == strlen("_Generic"))
    return TK_GENERIC;
  if (strncmp(input, "_Imaginary", 10) == 0 &&
      strlen(input) == strlen("_Imaginary"))
    return TK_IMAGINARY;
  if (strncmp(input, "_Noreturn", 9) == 0 &&
      strlen(input) == strlen("_Noreturn"))
    return TK_NORETURN;
  if (strncmp(input, "_Static_assert", 14) == 0 &&
      strlen(input) == strlen("_Static_assert"))
    return TK_STATICASSERT;
  if (strncmp(input, "_Thread_local", 13) == 0 &&
      strlen(input) == strlen("_Thread_local"))
    return TK_THREADLOCAL;
  return TK_IDENT;
}
