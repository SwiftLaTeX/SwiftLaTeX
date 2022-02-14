#include "texmfmp.h"
// char *backup_pool = 0;
// unsigned int backupptr = 0;
// void*  dlmalloc(size_t);
// void*  dlrealloc(void*, size_t);
// void   dlfree(void*);
void *xmalloc(size_t newsize) {

  void *ptr = malloc(newsize);
  if (!ptr) {
    fprintf(stderr, "Malloc Failed");
    abort();
  }
  memset(ptr, 0, newsize);
  // printf("malloc %p size %ld\n", ptr, newsize);
  return ptr;
}

void *xrealloc(void *oriptr, size_t newsize) {
  void *ptr = realloc(oriptr, newsize);
  if (!ptr) {
    fprintf(stderr, "Realloc Failed");
    abort();
  }
  // fprintf(stderr, "realloc %p size %ld\n", ptr, newsize);
  return ptr;
}

void *xcalloc(size_t num, size_t size) {
  void *ptr = calloc(num, size);
  if (!ptr) {
    fprintf(stderr, "Realloc Failed");
    abort();
  }
  // fprintf(stderr, "realloc %p size %ld\n", ptr, newsize);
  return ptr;
}

void xfree(void *ptr) { free(ptr); }

string xstrdup(const_string s) {
  string new_string = (string)malloc(strlen(s) + 1);
  return strcpy(new_string, s);
}

// void xpreparedatadump()
// {
// 	if(backup_pool == 0)
// 	{
// 		backup_pool = malloc(320 * 1024);
// 		if(backup_pool == 0) abort();
// 	}

// 	backupptr = 0;
// }

// void xprepredataundump()
// {
// 	backupptr = 0;
// }