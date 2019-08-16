#include "comcom.h"

FILE *open_file(const char *filename, const char *mode) {
  FILE *file;
  file = fopen(filename, mode);
  if (file == NULL) {
    fprintf(stderr, "Error Found:%s\n", strerror(errno));
    exit(1);
  }
  return file;
}

struct stat get_file_info(FILE *file) {
  struct stat st;
  int fd = fileno(file);
  if (fstat(fd, &st) != 0) {
    fprintf(stderr, "ファイル情報を取得出来ませんでした｡\n");
    fprintf(stderr, "%s\n", strerror(errno));
    exit(1);
  }
  return st;
}

char *get_contents(const char *filename) {
  FILE *file = open_file(filename, "rb");
  struct stat st = get_file_info(file);
  size_t nmemb = (size_t)(st.st_size);
  char *buf = (char *)malloc(nmemb);
  if (fread(buf, (size_t)(1), nmemb, file) < nmemb) {
    fprintf(stderr, "Error Found:%s\n", strerror(errno));
    exit(1);
  }
  fclose(file);
  buf[nmemb] = '\0';
  return buf;
}

void write_contents(const char *filename, const char *codes) {
  if (codes == NULL) {
    fprintf(stderr, "invalid pointer:%s\n", codes);
    exit(1);
  }
  FILE *file = open_file(filename, "wb");
  size_t nmemb = (size_t)(strlen(codes));
  if (fwrite(codes, (size_t)(1), nmemb, file) < nmemb) {
    fprintf(stderr, "Error Found:%s\n", strerror(errno));
    exit(1);
  }
  fclose(file);
}

int length(int array_size, int type_size) { return array_size / type_size; }
bool startswith(const char *s1, const char *s2) {
  return !strncmp(s1, s2, strlen(s2));
}

Array *new_ary(void) {
  Array *array = malloc(sizeof(Array));
  array->data = malloc(sizeof(void *) * 16);
  array->capacity = 16;
  array->length = 0;
  return array;
};
Array *with_capacity(int len, int capacity);
void *ary_get(Array *array, int idx) {
  assert(idx >= 0 && idx < array->length);
  return array->data[idx];
}
void ary_resize(Array *array) {
  array->capacity *= 2;
  array->data = realloc(array->data, sizeof(void *) * array->capacity);
}
void *ary_remove(Array *array, int idx) {
  void *ret_value = array->data[idx];
  for (int j = idx; j < array->length; j++) array->data[j] = array->data[j + 1];
  array->length--;
  if (array->capacity >= 3 * array->length) ary_resize(array);
  return ret_value;
}
void ary_push(Array *array, void *elem) {
  if (array->length >= array->capacity) ary_resize(array);
  array->data[array->length++] = elem;
}
int ary_check(Array *array, char *val) {
  int ret_idx = -1;
  for (int i = 0; i < array->length; i++) {
    if (!strncmp(val, (char *)array->data[i], strlen(val))) ret_idx = i;
  }
  return ret_idx;
}
int float_check(Array *array, float val) {
  int ret_idx = -1;
  for (int i = 0; i < array->length; i++) {
    if (*(float *)array->data[i] == val) {
      ret_idx = i;
    }
  }
  return ret_idx;
}
void *ary_pop(Array *array) {
  if (array->length == 0) {
    fprintf(stderr, "invalid access\nnot enough length to pop the value \n");

    return NULL;
  }
  return array->data[array->length--];
}
void ary_set(Array *array, int idx, void *elem) {
  if (idx < 0 || idx >= array->length) {
    fprintf(stderr, "invalid access\ncan't indexing out of range\n");
    return;
  }
  array->data[idx] = elem;
}
void ary_add(Array *array, int idx, void *elem) {
  if (array->length >= array->capacity) ary_resize(array);
  memmove(array->data + array->length, array->data + idx,
          array->length * (sizeof(void *)));
  array->data[idx] = elem;
  array->length++;
}
Array *aryget_range(Array *array, int start, int end) {
  Array *dst = new_ary();
  memmove(dst->data, array->data + start, (end - start + 1) * sizeof(void *));
  dst->length = end - start;
  return dst;
}

Map *new_map(void) {
  Map *map = malloc(sizeof(Map));
  map->keys = new_ary();
  map->vals = new_ary();
  return map;
}

void map_put(Map *map, char *key, void *val) {
  ary_push(map->keys, key);
  ary_push(map->vals, val);
}

void map_puti(Map *map, char *key, int val) {
  map_put(map, key, (void *)(intptr_t)val);
}

void *map_get(Map *map, char *key) {
  for (int i = map->keys->length - 1; i >= 0; i--)
    if (!strcmp((char *)map->keys->data[i], key)) return map->vals->data[i];
  return NULL;
}

int map_geti(Map *map, char *key, int default_) {
  for (int i = map->keys->length - 1; i >= 0; i--)
    if (!strcmp((char *)map->keys->data[i], key))
      return (intptr_t)map->vals->data[i];
  return default_;
}

char *format(char *fmt, ...) {
  char buf[2048];
  va_list ap;
  va_start(ap, fmt);
  vsnprintf(buf, sizeof(buf), fmt, ap);
  va_end(ap);
  return strdup(buf);
}

void error(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

void warning(char *message) {
  fprintf(stderr, "%s%s%s%s\n", STRONG, RED, message, CLEAR);
}
void info(char *message) {
  fprintf(stderr, "%s%s%s%s\n", STRONG, GREEN, message, CLEAR);
}
