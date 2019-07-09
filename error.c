#include "comcom.h"
char* dump_error(int ty) {
  switch (ty) {
    case PARSE:
      return "ParseError";
    case TYPE:
      return "TypeError";
    default:
      return "IllegalError";
  }
}
void error_found(int ty, const char* msg) {
  printf("%s%s%s%s:\t%s%s%s\n", STRONG, RED, dump_error(ty), CLEAR, RED, msg,
         CLEAR);
}
