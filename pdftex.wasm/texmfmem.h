#ifndef MIN_PDFTEX_TEXMFMEM
#define MIN_PDFTEX_TEXMFMEM
 typedef union 
 {
        struct { int32_t LH, RH; } v;
        struct { short B1, B0; } u;
 } twohalves;
 
 typedef struct {
        struct { uint16_t B3, B2, B1, B0; } u;
 } fourquarters;
 
typedef union 
{
	glueratio gr;
    twohalves hh;
    struct 
    {
            int32_t junk;
            int32_t CINT;
    } u;
	struct 
	{
	    fourquarters QQQQ;
	} v;
} memoryword;

#define cint u.CINT
#define qqqq v.QQQQ
#define	b0 u.B0
#define	b1 u.B1
#define	b2 u.B2
#define	b3 u.B3
#define rh v.RH
#define lhfield	v.LH

typedef union
{
  struct
  {
    integer CINT;
  } u;
  struct
  {
    fourquarters QQQQ;
  } v;
} fmemoryword;

#endif
