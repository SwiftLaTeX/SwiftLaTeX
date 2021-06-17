#ifndef XMEM_HEADER
#define XMEM_HEADER
extern void *xrealloc(void *, size_t newsize);
extern void *xmalloc(size_t sizeofitem);
extern void *xcalloc(size_t num, size_t size);
extern void xfree(void *ptr);
extern char *xstrdup(const char *s);
#define libcfree free
#define xmallocarray(type, size) ((type *)xmalloc((size + 1) * sizeof(type)))
#define xreallocarray(ptr, type, size)                                         \
  ((type *)xrealloc(ptr, (size + 1) * sizeof(type)))
#define xtalloc(n, t) ((t *)xmalloc((n) * sizeof(t)))
#define xretalloc(addr, n, t) ((addr) = (t *)xrealloc(addr, (n) * sizeof(t)))
#endif