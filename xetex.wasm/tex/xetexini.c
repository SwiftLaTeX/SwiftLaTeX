#define EXTERN extern
#include "xetexd.h"

void 
initialize ( void ) 
{
  initialize_regmem 
  integer i  ;
  integer k  ;
  hyphpointer z  ;
  doingspecial = false ;
  nativetextsize = 128 ;
  nativetext = xmalloc ( nativetextsize * sizeof ( UTF16code ) ) ;
  if ( interactionoption == 4 ) 
  interaction = 3 ;
  else interaction = interactionoption ;
  deletionsallowed = true ;
  setboxallowed = true ;
  errorcount = 0 ;
  helpptr = 0 ;
  useerrhelp = false ;
  interrupt = 0 ;
  OKtointerrupt = true ;
  twotothe [0 ]= 1 ;
  {register integer for_end; k = 1 ;for_end = 30 ; if ( k <= for_end) do 
    twotothe [k ]= 2 * twotothe [k - 1 ];
  while ( k++ < for_end ) ;} 
  speclog [1 ]= 93032640L ;
  speclog [2 ]= 38612034L ;
  speclog [3 ]= 17922280L ;
  speclog [4 ]= 8662214L ;
  speclog [5 ]= 4261238L ;
  speclog [6 ]= 2113709L ;
  speclog [7 ]= 1052693L ;
  speclog [8 ]= 525315L ;
  speclog [9 ]= 262400L ;
  speclog [10 ]= 131136L ;
  speclog [11 ]= 65552L ;
  speclog [12 ]= 32772L ;
  speclog [13 ]= 16385 ;
  {register integer for_end; k = 14 ;for_end = 27 ; if ( k <= for_end) do 
    speclog [k ]= twotothe [27 - k ];
  while ( k++ < for_end ) ;} 
  speclog [28 ]= 1 ;
	;
#ifdef TEXMF_DEBUG
  wasmemend = memmin ;
  waslomax = memmin ;
  washimin = memmax ;
  panicking = false ;
#endif /* TEXMF_DEBUG */
  nestptr = 0 ;
  maxneststack = 0 ;
  curlist .modefield = 1 ;
  curlist .headfield = memtop - 1 ;
  curlist .tailfield = memtop - 1 ;
  curlist .eTeXauxfield = -268435455L ;
  curlist .auxfield .cint = -65536000L ;
  curlist .mlfield = 0 ;
  curlist .pgfield = 0 ;
  shownmode = 0 ;
  pagecontents = 0 ;
  pagetail = memtop - 2 ;
  lastglue = 1073741823L ;
  lastpenalty = 0 ;
  lastkern = 0 ;
  lastnodetype = -1 ;
  pagesofar [7 ]= 0 ;
  pagemaxdepth = 0 ;
  {register integer for_end; k = 8939240L ;for_end = 10053969L ; if ( k <= 
  for_end) do 
    xeqlevel [k ]= 1 ;
  while ( k++ < for_end ) ;} 
  nonewcontrolsequence = true ;
  prim [0 ].v.LH = 0 ;
  prim [0 ].v.RH = 0 ;
  {register integer for_end; k = 1 ;for_end = 500 ; if ( k <= for_end) do 
    prim [k ]= prim [0 ];
  while ( k++ < for_end ) ;} 
  saveptr = 0 ;
  curlevel = 1 ;
  curgroup = 0 ;
  curboundary = 0 ;
  maxsavestack = 0 ;
  magset = 0 ;
  expanddepthcount = 0 ;
  isincsname = false ;
  curmark [0 ]= -268435455L ;
  curmark [1 ]= -268435455L ;
  curmark [2 ]= -268435455L ;
  curmark [3 ]= -268435455L ;
  curmark [4 ]= -268435455L ;
  curval = 0 ;
  curvallevel = 0 ;
  radix = 0 ;
  curorder = 0 ;
  {register integer for_end; k = 0 ;for_end = 16 ; if ( k <= for_end) do 
    readopen [k ]= 2 ;
  while ( k++ < for_end ) ;} 
  condptr = -268435455L ;
  iflimit = 0 ;
  curif = 0 ;
  ifline = 0 ;
  nullcharacter .b0 = 0 ;
  nullcharacter .b1 = 0 ;
  nullcharacter .b2 = 0 ;
  nullcharacter .b3 = 0 ;
  totalpages = 0 ;
  maxv = 0 ;
  maxh = 0 ;
  maxpush = 0 ;
  lastbop = -1 ;
  doingleaders = false ;
  deadcycles = 0 ;
  curs = -1 ;
  halfbuf = dvibufsize / 2 ;
  dvilimit = dvibufsize ;
  dviptr = 0 ;
  dvioffset = 0 ;
  dvigone = 0 ;
  downptr = -268435455L ;
  rightptr = -268435455L ;
  adjusttail = -268435455L ;
  lastbadness = 0 ;
  preadjusttail = -268435455L ;
  packbeginline = 0 ;
  emptyfield .v.RH = 0 ;
  emptyfield .v.LH = -268435455L ;
  nulldelimiter .b0 = 0 ;
  nulldelimiter .b1 = 0 ;
  nulldelimiter .b2 = 0 ;
  nulldelimiter .b3 = 0 ;
  alignptr = -268435455L ;
  curalign = -268435455L ;
  curspan = -268435455L ;
  curloop = -268435455L ;
  curhead = -268435455L ;
  curtail = -268435455L ;
  curprehead = -268435455L ;
  curpretail = -268435455L ;
  maxhyphchar = 256 ;
  {register integer for_end; z = 0 ;for_end = hyphsize ; if ( z <= for_end) 
  do 
    {
      hyphword [z ]= 0 ;
      hyphlist [z ]= -268435455L ;
      hyphlink [z ]= 0 ;
    } 
  while ( z++ < for_end ) ;} 
  hyphcount = 0 ;
  hyphnext = 608 ;
  if ( hyphnext > hyphsize ) 
  hyphnext = 607 ;
  outputactive = false ;
  insertpenalties = 0 ;
  ligaturepresent = false ;
  cancelboundary = false ;
  lfthit = false ;
  rthit = false ;
  insdisc = false ;
  aftertoken = 0 ;
  longhelpseen = false ;
  formatident = 0 ;
  {register integer for_end; k = 0 ;for_end = 17 ; if ( k <= for_end) do 
    writeopen [k ]= false ;
  while ( k++ < for_end ) ;} 
  secondsandmicros ( epochseconds , microseconds ) ;
  initstarttime () ;
  LRptr = -268435455L ;
  LRproblems = 0 ;
  curdir = 0 ;
  pseudofiles = -268435455L ;
  saroot [7 ]= -268435455L ;
  sanull .hh .v.LH = -268435455L ;
  sanull .hh .v.RH = -268435455L ;
  sachain = -268435455L ;
  salevel = 0 ;
  discptr [2 ]= -268435455L ;
  discptr [3 ]= -268435455L ;
  editnamestart = 0 ;
  stopatspace = true ;
  mltexenabledp = false ;
	;
#ifdef INITEX
  if ( iniversion ) 
  {
    {register integer for_end; k = membot + 1 ;for_end = membot + 19 ; if ( 
    k <= for_end) do 
      mem [k ].cint = 0 ;
    while ( k++ < for_end ) ;} 
    k = membot ;
    while ( k <= membot + 19 ) {
	
      mem [k ].hh .v.RH = -268435454L ;
      mem [k ].hh.b0 = 0 ;
      mem [k ].hh.b1 = 0 ;
      k = k + 4 ;
    } 
    mem [membot + 6 ].cint = 65536L ;
    mem [membot + 4 ].hh.b0 = 1 ;
    mem [membot + 10 ].cint = 65536L ;
    mem [membot + 8 ].hh.b0 = 2 ;
    mem [membot + 14 ].cint = 65536L ;
    mem [membot + 12 ].hh.b0 = 1 ;
    mem [membot + 15 ].cint = 65536L ;
    mem [membot + 12 ].hh.b1 = 1 ;
    mem [membot + 18 ].cint = -65536L ;
    mem [membot + 16 ].hh.b0 = 1 ;
    rover = membot + 20 ;
    mem [rover ].hh .v.RH = 1073741823L ;
    mem [rover ].hh .v.LH = 1000 ;
    mem [rover + 1 ].hh .v.LH = rover ;
    mem [rover + 1 ].hh .v.RH = rover ;
    lomemmax = rover + 1000 ;
    mem [lomemmax ].hh .v.RH = -268435455L ;
    mem [lomemmax ].hh .v.LH = -268435455L ;
    {register integer for_end; k = memtop - 14 ;for_end = memtop ; if ( k <= 
    for_end) do 
      mem [k ]= mem [lomemmax ];
    while ( k++ < for_end ) ;} 
    mem [memtop - 10 ].hh .v.LH = 35797662L ;
    mem [memtop - 9 ].hh .v.RH = 65536L ;
    mem [memtop - 9 ].hh .v.LH = -268435455L ;
    mem [memtop - 7 ].hh.b0 = 1 ;
    mem [memtop - 6 ].hh .v.LH = 1073741823L ;
    mem [memtop - 7 ].hh.b1 = 0 ;
    mem [memtop ].hh.b1 = 255 ;
    mem [memtop ].hh.b0 = 1 ;
    mem [memtop ].hh .v.RH = memtop ;
    mem [memtop - 2 ].hh.b0 = 10 ;
    mem [memtop - 2 ].hh.b1 = 0 ;
    avail = -268435455L ;
    memend = memtop ;
    himemmin = memtop - 14 ;
    varused = membot + 20 - membot ;
    dynused = 15 ;
    eqtb [2252739L ].hh.b0 = 103 ;
    eqtb [2252739L ].hh .v.RH = -268435455L ;
    eqtb [2252739L ].hh.b1 = 0 ;
    {register integer for_end; k = 1 ;for_end = eqtbtop ; if ( k <= for_end) 
    do 
      eqtb [k ]= eqtb [2252739L ];
    while ( k++ < for_end ) ;} 
    eqtb [2252740L ].hh .v.RH = membot ;
    eqtb [2252740L ].hh.b1 = 1 ;
    eqtb [2252740L ].hh.b0 = 119 ;
    {register integer for_end; k = 2252741L ;for_end = 2253270L ; if ( k <= 
    for_end) do 
      eqtb [k ]= eqtb [2252740L ];
    while ( k++ < for_end ) ;} 
    mem [membot ].hh .v.RH = mem [membot ].hh .v.RH + 531 ;
    eqtb [2253271L ].hh .v.RH = -268435455L ;
    eqtb [2253271L ].hh.b0 = 120 ;
    eqtb [2253271L ].hh.b1 = 1 ;
    {register integer for_end; k = 2253539L ;for_end = 2253542L ; if ( k <= 
    for_end) do 
      eqtb [k ]= eqtb [2253271L ];
    while ( k++ < for_end ) ;} 
    {register integer for_end; k = 2253272L ;for_end = 2253538L ; if ( k <= 
    for_end) do 
      eqtb [k ]= eqtb [2252739L ];
    while ( k++ < for_end ) ;} 
    eqtb [2253543L ].hh .v.RH = -268435455L ;
    eqtb [2253543L ].hh.b0 = 121 ;
    eqtb [2253543L ].hh.b1 = 1 ;
    {register integer for_end; k = 2253544L ;for_end = 2253798L ; if ( k <= 
    for_end) do 
      eqtb [k ]= eqtb [2253543L ];
    while ( k++ < for_end ) ;} 
    eqtb [2253799L ].hh .v.RH = 0 ;
    eqtb [2253799L ].hh.b0 = 122 ;
    eqtb [2253799L ].hh.b1 = 1 ;
    {register integer for_end; k = 2253800L ;for_end = 2254567L ; if ( k <= 
    for_end) do 
      eqtb [k ]= eqtb [2253799L ];
    while ( k++ < for_end ) ;} 
    eqtb [2254568L ].hh .v.RH = 0 ;
    eqtb [2254568L ].hh.b0 = 122 ;
    eqtb [2254568L ].hh.b1 = 1 ;
    {register integer for_end; k = 2254569L ;for_end = 8939239L ; if ( k <= 
    for_end) do 
      eqtb [k ]= eqtb [2254568L ];
    while ( k++ < for_end ) ;} 
    {register integer for_end; k = 0 ;for_end = 1114111L ; if ( k <= 
    for_end) do 
      {
	eqtb [2254568L + k ].hh .v.RH = 12 ;
	eqtb [6711016L + k ].hh .v.RH = k ;
	eqtb [5596904L + k ].hh .v.RH = 1000 ;
      } 
    while ( k++ < for_end ) ;} 
    eqtb [2254581L ].hh .v.RH = 5 ;
    eqtb [2254600L ].hh .v.RH = 10 ;
    eqtb [2254660L ].hh .v.RH = 0 ;
    eqtb [2254605L ].hh .v.RH = 14 ;
    eqtb [2254695L ].hh .v.RH = 15 ;
    eqtb [2254568L ].hh .v.RH = 9 ;
    {register integer for_end; k = 48 ;for_end = 57 ; if ( k <= for_end) do 
      eqtb [6711016L + k ].hh .v.RH = k + setclassfield ( 7 ) ;
    while ( k++ < for_end ) ;} 
    {register integer for_end; k = 65 ;for_end = 90 ; if ( k <= for_end) do 
      {
	eqtb [2254568L + k ].hh .v.RH = 11 ;
	eqtb [2254568L + k + 32 ].hh .v.RH = 11 ;
	eqtb [6711016L + k ].hh .v.RH = k + setfamilyfield ( 1 ) + 
	setclassfield ( 7 ) ;
	eqtb [6711016L + k + 32 ].hh .v.RH = k + 32 + setfamilyfield ( 1 ) + 
	setclassfield ( 7 ) ;
	eqtb [3368680L + k ].hh .v.RH = k + 32 ;
	eqtb [3368680L + k + 32 ].hh .v.RH = k + 32 ;
	eqtb [4482792L + k ].hh .v.RH = k ;
	eqtb [4482792L + k + 32 ].hh .v.RH = k ;
	eqtb [5596904L + k ].hh .v.RH = 999 ;
      } 
    while ( k++ < for_end ) ;} 
    {register integer for_end; k = 8939240L ;for_end = 8939578L ; if ( k <= 
    for_end) do 
      eqtb [k ].cint = 0 ;
    while ( k++ < for_end ) ;} 
    eqtb [8939295L ].cint = 256 ;
    eqtb [8939296L ].cint = -1 ;
    eqtb [8939257L ].cint = 1000 ;
    eqtb [8939241L ].cint = 10000 ;
    eqtb [8939281L ].cint = 1 ;
    eqtb [8939280L ].cint = 25 ;
    eqtb [8939285L ].cint = 92 ;
    eqtb [8939288L ].cint = 13 ;
    {register integer for_end; k = 0 ;for_end = 1114111L ; if ( k <= 
    for_end) do 
      eqtb [8939579L + k ].cint = -1 ;
    while ( k++ < for_end ) ;} 
    eqtb [8939625L ].cint = 0 ;
    {register integer for_end; k = 10053691L ;for_end = 10053969L ; if ( k 
    <= for_end) do 
      eqtb [k ].cint = 0 ;
    while ( k++ < for_end ) ;} 
    primused = 500 ;
    hashused = 2243226L ;
    hashhigh = 0 ;
    cscount = 0 ;
    eqtb [2243235L ].hh.b0 = 118 ;
    hash [2243235L ].v.RH = 65809L ;
    eqtb [2243237L ].hh.b0 = 39 ;
    eqtb [2243237L ].hh .v.RH = 1 ;
    eqtb [2243237L ].hh.b1 = 1 ;
    hash [2243237L ].v.RH = 65810L ;
    {register integer for_end; k = - (integer) trieopsize ;for_end = 
    trieopsize ; if ( k <= for_end) do 
      trieophash [k ]= 0 ;
    while ( k++ < for_end ) ;} 
    {register integer for_end; k = 0 ;for_end = 255 ; if ( k <= for_end) do 
      trieused [k ]= mintrieop ;
    while ( k++ < for_end ) ;} 
    maxopused = mintrieop ;
    trieopptr = 0 ;
    trienotready = true ;
    hash [2243226L ].v.RH = 66621L ;
    if ( iniversion ) 
    formatident = 66703L ;
    hash [2243234L ].v.RH = 66761L ;
    eqtb [2243234L ].hh.b1 = 1 ;
    eqtb [2243234L ].hh.b0 = 115 ;
    eqtb [2243234L ].hh .v.RH = -268435455L ;
    eTeXmode = 0 ;
    maxregnum = 255 ;
    maxreghelpline = 66953L ;
    {register integer for_end; i = 0 ;for_end = 6 ; if ( i <= for_end) do 
      saroot [i ]= -268435455L ;
    while ( i++ < for_end ) ;} 
    eqtb [8939322L ].cint = 63 ;
  } 
#endif /* INITEX */
} 
#ifdef INITEX
boolean 
getstringsstarted ( void ) 
{
  /* 30 10 */ register boolean Result; getstringsstarted_regmem 
  strnumber g  ;
  poolptr = 0 ;
  strptr = 0 ;
  strstart [0 ]= 0 ;
  {
    strptr = 65536L ;
    strstart [( strptr ) - 65536L ]= poolptr ;
  } 
  g = loadpoolstrings ( ( poolsize - stringvacancies ) ) ;
  if ( g == 0 ) 
  {
    ;
    fprintf ( stdout , "%s\n",  "! You have to increase POOLSIZE." ) ;
    Result = false ;
    return Result ;
  } 
  Result = true ;
  return Result ;
} 
#endif /* INITEX */
#ifdef INITEX
void 
sortavail ( void ) 
{
  sortavail_regmem 
  halfword p, q, r  ;
  halfword oldrover  ;
  p = getnode ( 1073741824L ) ;
  p = mem [rover + 1 ].hh .v.RH ;
  mem [rover + 1 ].hh .v.RH = 1073741823L ;
  oldrover = rover ;
  while ( p != oldrover ) if ( p < rover ) 
  {
    q = p ;
    p = mem [q + 1 ].hh .v.RH ;
    mem [q + 1 ].hh .v.RH = rover ;
    rover = q ;
  } 
  else {
      
    q = rover ;
    while ( mem [q + 1 ].hh .v.RH < p ) q = mem [q + 1 ].hh .v.RH ;
    r = mem [p + 1 ].hh .v.RH ;
    mem [p + 1 ].hh .v.RH = mem [q + 1 ].hh .v.RH ;
    mem [q + 1 ].hh .v.RH = p ;
    p = r ;
  } 
  p = rover ;
  while ( mem [p + 1 ].hh .v.RH != 1073741823L ) {
      
    mem [mem [p + 1 ].hh .v.RH + 1 ].hh .v.LH = p ;
    p = mem [p + 1 ].hh .v.RH ;
  } 
  mem [p + 1 ].hh .v.RH = rover ;
  mem [rover + 1 ].hh .v.LH = p ;
} 
#endif /* INITEX */
#ifdef INITEX
void 
zprimitive ( strnumber s , quarterword c , halfword o ) 
{
  primitive_regmem 
  poolpointer k  ;
  integer j  ;
  smallnumber l  ;
  integer primval  ;
  if ( s < 256 ) 
  {
    curval = s + 1114113L ;
    primval = primlookup ( s ) ;
  } 
  else {
      
    k = strstart [( s ) - 65536L ];
    l = strstart [( s + 1 ) - 65536L ]- k ;
    if ( first + l > bufsize + 1 ) 
    overflow ( 65538L , bufsize ) ;
    {register integer for_end; j = 0 ;for_end = l - 1 ; if ( j <= for_end) 
    do 
      buffer [first + j ]= strpool [k + j ];
    while ( j++ < for_end ) ;} 
    curval = idlookup ( first , l ) ;
    {
      decr ( strptr ) ;
      poolptr = strstart [( strptr ) - 65536L ];
    } 
    hash [curval ].v.RH = s ;
    primval = primlookup ( s ) ;
  } 
  eqtb [curval ].hh.b1 = 1 ;
  eqtb [curval ].hh.b0 = c ;
  eqtb [curval ].hh .v.RH = o ;
  eqtb [2243238L + primval ].hh.b1 = 1 ;
  eqtb [2243238L + primval ].hh.b0 = c ;
  eqtb [2243238L + primval ].hh .v.RH = o ;
} 
#endif /* INITEX */
#ifdef INITEX
trieopcode 
znewtrieop ( smallnumber d , smallnumber n , trieopcode v ) 
{
  /* 10 */ register trieopcode Result; newtrieop_regmem 
  integer h  ;
  trieopcode u  ;
  integer l  ;
  h = abs ( n + 313 * d + 361 * v + 1009 * curlang ) % ( trieopsize - 
  negtrieopsize ) + negtrieopsize ;
  while ( true ) {
      
    l = trieophash [h ];
    if ( l == 0 ) 
    {
      if ( trieopptr == trieopsize ) 
      overflow ( 66374L , trieopsize ) ;
      u = trieused [curlang ];
      if ( u == maxtrieop ) 
      overflow ( 66375L , maxtrieop - mintrieop ) ;
      incr ( trieopptr ) ;
      incr ( u ) ;
      trieused [curlang ]= u ;
      if ( u > maxopused ) 
      maxopused = u ;
      hyfdistance [trieopptr ]= d ;
      hyfnum [trieopptr ]= n ;
      hyfnext [trieopptr ]= v ;
      trieoplang [trieopptr ]= curlang ;
      trieophash [h ]= trieopptr ;
      trieopval [trieopptr ]= u ;
      Result = u ;
      return Result ;
    } 
    if ( ( hyfdistance [l ]== d ) && ( hyfnum [l ]== n ) && ( hyfnext [l 
    ]== v ) && ( trieoplang [l ]== curlang ) ) 
    {
      Result = trieopval [l ];
      return Result ;
    } 
    if ( h > - (integer) trieopsize ) 
    decr ( h ) ;
    else h = trieopsize ;
  } 
  return Result ;
} 
triepointer 
ztrienode ( triepointer p ) 
{
  /* 10 */ register triepointer Result; trienode_regmem 
  triepointer h  ;
  triepointer q  ;
  h = abs ( triec [p ]+ 1009 * trieo [p ]+ 2718 * triel [p ]+ 3142 * 
  trier [p ]) % triesize ;
  while ( true ) {
      
    q = triehash [h ];
    if ( q == 0 ) 
    {
      triehash [h ]= p ;
      Result = p ;
      return Result ;
    } 
    if ( ( triec [q ]== triec [p ]) && ( trieo [q ]== trieo [p ]) && ( 
    triel [q ]== triel [p ]) && ( trier [q ]== trier [p ]) ) 
    {
      Result = q ;
      return Result ;
    } 
    if ( h > 0 ) 
    decr ( h ) ;
    else h = triesize ;
  } 
  return Result ;
} 
triepointer 
zcompresstrie ( triepointer p ) 
{
  register triepointer Result; compresstrie_regmem 
  if ( p == 0 ) 
  Result = 0 ;
  else {
      
    triel [p ]= compresstrie ( triel [p ]) ;
    trier [p ]= compresstrie ( trier [p ]) ;
    Result = trienode ( p ) ;
  } 
  return Result ;
} 
void 
zfirstfit ( triepointer p ) 
{
  /* 45 40 */ firstfit_regmem 
  triepointer h  ;
  triepointer z  ;
  triepointer q  ;
  UTF16code c  ;
  triepointer l, r  ;
  integer ll  ;
  c = triec [p ];
  z = triemin [c ];
  while ( true ) {
      
    h = z - c ;
    if ( triemax < h + maxhyphchar ) 
    {
      if ( triesize <= h + maxhyphchar ) 
      overflow ( 66376L , triesize ) ;
      do {
	  incr ( triemax ) ;
	trietaken [triemax ]= false ;
	trietrl [triemax ]= triemax + 1 ;
	trietro [triemax ]= triemax - 1 ;
      } while ( ! ( triemax == h + maxhyphchar ) ) ;
    } 
    if ( trietaken [h ]) 
    goto lab45 ;
    q = trier [p ];
    while ( q > 0 ) {
	
      if ( trietrl [h + triec [q ]]== 0 ) 
      goto lab45 ;
      q = trier [q ];
    } 
    goto lab40 ;
    lab45: z = trietrl [z ];
  } 
  lab40: trietaken [h ]= true ;
  triehash [p ]= h ;
  q = p ;
  do {
      z = h + triec [q ];
    l = trietro [z ];
    r = trietrl [z ];
    trietro [r ]= l ;
    trietrl [l ]= r ;
    trietrl [z ]= 0 ;
    if ( l < maxhyphchar ) 
    {
      if ( z < maxhyphchar ) 
      ll = z ;
      else ll = maxhyphchar ;
      do {
	  triemin [l ]= r ;
	incr ( l ) ;
      } while ( ! ( l == ll ) ) ;
    } 
    q = trier [q ];
  } while ( ! ( q == 0 ) ) ;
} 
void 
ztriepack ( triepointer p ) 
{
  triepack_regmem 
  triepointer q  ;
  do {
      q = triel [p ];
    if ( ( q > 0 ) && ( triehash [q ]== 0 ) ) 
    {
      firstfit ( q ) ;
      triepack ( q ) ;
    } 
    p = trier [p ];
  } while ( ! ( p == 0 ) ) ;
} 
void 
ztriefix ( triepointer p ) 
{
  triefix_regmem 
  triepointer q  ;
  UTF16code c  ;
  triepointer z  ;
  z = triehash [p ];
  do {
      q = triel [p ];
    c = triec [p ];
    trietrl [z + c ]= triehash [q ];
    trietrc [z + c ]= c ;
    trietro [z + c ]= trieo [p ];
    if ( q > 0 ) 
    triefix ( q ) ;
    p = trier [p ];
  } while ( ! ( p == 0 ) ) ;
} 
void 
newpatterns ( void ) 
{
  /* 30 31 */ newpatterns_regmem 
  short k, l  ;
  boolean digitsensed  ;
  trieopcode v  ;
  triepointer p, q  ;
  boolean firstchild  ;
  UTF16code c  ;
  if ( trienotready ) 
  {
    if ( eqtb [8939290L ].cint <= 0 ) 
    curlang = 0 ;
    else if ( eqtb [8939290L ].cint > 255 ) 
    curlang = 0 ;
    else curlang = eqtb [8939290L ].cint ;
    scanleftbrace () ;
    k = 0 ;
    hyf [0 ]= 0 ;
    digitsensed = false ;
    while ( true ) {
	
      getxtoken () ;
      switch ( curcmd ) 
      {case 11 : 
      case 12 : 
	if ( digitsensed || ( curchr < 48 ) || ( curchr > 57 ) ) 
	{
	  if ( curchr == 46 ) 
	  curchr = 0 ;
	  else {
	      
	    curchr = eqtb [3368680L + curchr ].hh .v.RH ;
	    if ( curchr == 0 ) 
	    {
	      {
		if ( interaction == 3 ) 
		;
		if ( filelineerrorstylep ) 
		printfileline () ;
		else printnl ( 65544L ) ;
		print ( 66382L ) ;
	      } 
	      {
		helpptr = 1 ;
		helpline [0 ]= 66381L ;
	      } 
	      error () ;
	    } 
	  } 
	  if ( curchr > maxhyphchar ) 
	  maxhyphchar = curchr ;
	  if ( k < maxhyphenatablelength () ) 
	  {
	    incr ( k ) ;
	    hc [k ]= curchr ;
	    hyf [k ]= 0 ;
	    digitsensed = false ;
	  } 
	} 
	else if ( k < maxhyphenatablelength () ) 
	{
	  hyf [k ]= curchr - 48 ;
	  digitsensed = true ;
	} 
	break ;
      case 10 : 
      case 2 : 
	{
	  if ( k > 0 ) 
	  {
	    if ( hc [1 ]== 0 ) 
	    hyf [0 ]= 0 ;
	    if ( hc [k ]== 0 ) 
	    hyf [k ]= 0 ;
	    l = k ;
	    v = mintrieop ;
	    while ( true ) {
		
	      if ( hyf [l ]!= 0 ) 
	      v = newtrieop ( k - l , hyf [l ], v ) ;
	      if ( l > 0 ) 
	      decr ( l ) ;
	      else goto lab31 ;
	    } 
	    lab31: ;
	    q = 0 ;
	    hc [0 ]= curlang ;
	    while ( l <= k ) {
		
	      c = hc [l ];
	      incr ( l ) ;
	      p = triel [q ];
	      firstchild = true ;
	      while ( ( p > 0 ) && ( c > triec [p ]) ) {
		  
		q = p ;
		p = trier [q ];
		firstchild = false ;
	      } 
	      if ( ( p == 0 ) || ( c < triec [p ]) ) 
	      {
		if ( trieptr == triesize ) 
		overflow ( 66376L , triesize ) ;
		incr ( trieptr ) ;
		trier [trieptr ]= p ;
		p = trieptr ;
		triel [p ]= 0 ;
		if ( firstchild ) 
		triel [q ]= p ;
		else trier [q ]= p ;
		triec [p ]= c ;
		trieo [p ]= mintrieop ;
	      } 
	      q = p ;
	    } 
	    if ( trieo [q ]!= mintrieop ) 
	    {
	      {
		if ( interaction == 3 ) 
		;
		if ( filelineerrorstylep ) 
		printfileline () ;
		else printnl ( 65544L ) ;
		print ( 66383L ) ;
	      } 
	      {
		helpptr = 1 ;
		helpline [0 ]= 66381L ;
	      } 
	      error () ;
	    } 
	    trieo [q ]= v ;
	  } 
	  if ( curcmd == 2 ) 
	  goto lab30 ;
	  k = 0 ;
	  hyf [0 ]= 0 ;
	  digitsensed = false ;
	} 
	break ;
	default: 
	{
	  {
	    if ( interaction == 3 ) 
	    ;
	    if ( filelineerrorstylep ) 
	    printfileline () ;
	    else printnl ( 65544L ) ;
	    print ( 66380L ) ;
	  } 
	  printesc ( 66378L ) ;
	  {
	    helpptr = 1 ;
	    helpline [0 ]= 66381L ;
	  } 
	  error () ;
	} 
	break ;
      } 
    } 
    lab30: ;
    if ( eqtb [8939306L ].cint > 0 ) 
    {
      c = curlang ;
      firstchild = false ;
      p = 0 ;
      do {
	  q = p ;
	p = trier [q ];
      } while ( ! ( ( p == 0 ) || ( c <= triec [p ]) ) ) ;
      if ( ( p == 0 ) || ( c < triec [p ]) ) 
      {
	if ( trieptr == triesize ) 
	overflow ( 66376L , triesize ) ;
	incr ( trieptr ) ;
	trier [trieptr ]= p ;
	p = trieptr ;
	triel [p ]= 0 ;
	if ( firstchild ) 
	triel [q ]= p ;
	else trier [q ]= p ;
	triec [p ]= c ;
	trieo [p ]= mintrieop ;
      } 
      q = p ;
      p = triel [q ];
      firstchild = true ;
      {register integer for_end; c = 0 ;for_end = 255 ; if ( c <= for_end) 
      do 
	if ( ( eqtb [3368680L + c ].hh .v.RH > 0 ) || ( ( c == 255 ) && 
	firstchild ) ) 
	{
	  if ( p == 0 ) 
	  {
	    if ( trieptr == triesize ) 
	    overflow ( 66376L , triesize ) ;
	    incr ( trieptr ) ;
	    trier [trieptr ]= p ;
	    p = trieptr ;
	    triel [p ]= 0 ;
	    if ( firstchild ) 
	    triel [q ]= p ;
	    else trier [q ]= p ;
	    triec [p ]= c ;
	    trieo [p ]= mintrieop ;
	  } 
	  else triec [p ]= c ;
	  trieo [p ]= eqtb [3368680L + c ].hh .v.RH ;
	  q = p ;
	  p = trier [q ];
	  firstchild = false ;
	} 
      while ( c++ < for_end ) ;} 
      if ( firstchild ) 
      triel [q ]= 0 ;
      else trier [q ]= 0 ;
    } 
  } 
  else {
      
    {
      if ( interaction == 3 ) 
      ;
      if ( filelineerrorstylep ) 
      printfileline () ;
      else printnl ( 65544L ) ;
      print ( 66377L ) ;
    } 
    printesc ( 66378L ) ;
    {
      helpptr = 1 ;
      helpline [0 ]= 66379L ;
    } 
    error () ;
    mem [memtop - 12 ].hh .v.RH = scantoks ( false , false ) ;
    flushlist ( defref ) ;
  } 
} 
void 
inittrie ( void ) 
{
  inittrie_regmem 
  triepointer p  ;
  integer j, k, t  ;
  triepointer r, s  ;
  incr ( maxhyphchar ) ;
  opstart [0 ]= - (integer) mintrieop ;
  {register integer for_end; j = 1 ;for_end = 255 ; if ( j <= for_end) do 
    opstart [j ]= opstart [j - 1 ]+ trieused [j - 1 ];
  while ( j++ < for_end ) ;} 
  {register integer for_end; j = 1 ;for_end = trieopptr ; if ( j <= for_end) 
  do 
    trieophash [j ]= opstart [trieoplang [j ]]+ trieopval [j ];
  while ( j++ < for_end ) ;} 
  {register integer for_end; j = 1 ;for_end = trieopptr ; if ( j <= for_end) 
  do 
    while ( trieophash [j ]> j ) {
	
      k = trieophash [j ];
      t = hyfdistance [k ];
      hyfdistance [k ]= hyfdistance [j ];
      hyfdistance [j ]= t ;
      t = hyfnum [k ];
      hyfnum [k ]= hyfnum [j ];
      hyfnum [j ]= t ;
      t = hyfnext [k ];
      hyfnext [k ]= hyfnext [j ];
      hyfnext [j ]= t ;
      trieophash [j ]= trieophash [k ];
      trieophash [k ]= k ;
    } 
  while ( j++ < for_end ) ;} 
  {register integer for_end; p = 0 ;for_end = triesize ; if ( p <= for_end) 
  do 
    triehash [p ]= 0 ;
  while ( p++ < for_end ) ;} 
  trier [0 ]= compresstrie ( trier [0 ]) ;
  triel [0 ]= compresstrie ( triel [0 ]) ;
  {register integer for_end; p = 0 ;for_end = trieptr ; if ( p <= for_end) 
  do 
    triehash [p ]= 0 ;
  while ( p++ < for_end ) ;} 
  {register integer for_end; p = 0 ;for_end = 65535L ; if ( p <= for_end) do 
    triemin [p ]= p + 1 ;
  while ( p++ < for_end ) ;} 
  trietrl [0 ]= 1 ;
  triemax = 0 ;
  if ( triel [0 ]!= 0 ) 
  {
    firstfit ( triel [0 ]) ;
    triepack ( triel [0 ]) ;
  } 
  if ( trier [0 ]!= 0 ) 
  {
    if ( triel [0 ]== 0 ) 
    {register integer for_end; p = 0 ;for_end = 255 ; if ( p <= for_end) do 
      triemin [p ]= p + 2 ;
    while ( p++ < for_end ) ;} 
    firstfit ( trier [0 ]) ;
    triepack ( trier [0 ]) ;
    hyphstart = triehash [trier [0 ]];
  } 
  if ( triemax == 0 ) 
  {
    {register integer for_end; r = 0 ;for_end = maxhyphchar ; if ( r <= 
    for_end) do 
      {
	trietrl [r ]= 0 ;
	trietro [r ]= mintrieop ;
	trietrc [r ]= 0 ;
      } 
    while ( r++ < for_end ) ;} 
    triemax = maxhyphchar ;
  } 
  else {
      
    if ( trier [0 ]> 0 ) 
    triefix ( trier [0 ]) ;
    if ( triel [0 ]> 0 ) 
    triefix ( triel [0 ]) ;
    r = 0 ;
    do {
	s = trietrl [r ];
      {
	trietrl [r ]= 0 ;
	trietro [r ]= mintrieop ;
	trietrc [r ]= 0 ;
      } 
      r = s ;
    } while ( ! ( r > triemax ) ) ;
  } 
  trietrc [0 ]= 63 ;
  trienotready = false ;
} 
#endif /* INITEX */
void 
zlinebreak ( boolean d ) 
{
  /* 30 31 32 33 34 35 36 22 20 */ linebreak_regmem 
  boolean autobreaking  ;
  halfword prevp  ;
  halfword q, r, s, prevs  ;
  internalfontnumber f  ;
  smallnumber j  ;
  UnicodeScalar c  ;
  integer l  ;
  integer i  ;
  packbeginline = curlist .mlfield ;
  mem [memtop - 3 ].hh .v.RH = mem [curlist .headfield ].hh .v.RH ;
  if ( ( curlist .tailfield >= himemmin ) ) 
  {
    mem [curlist .tailfield ].hh .v.RH = newpenalty ( 10000 ) ;
    curlist .tailfield = mem [curlist .tailfield ].hh .v.RH ;
  } 
  else if ( mem [curlist .tailfield ].hh.b0 != 10 ) 
  {
    mem [curlist .tailfield ].hh .v.RH = newpenalty ( 10000 ) ;
    curlist .tailfield = mem [curlist .tailfield ].hh .v.RH ;
  } 
  else {
      
    mem [curlist .tailfield ].hh.b0 = 12 ;
    deleteglueref ( mem [curlist .tailfield + 1 ].hh .v.LH ) ;
    flushnodelist ( mem [curlist .tailfield + 1 ].hh .v.RH ) ;
    mem [curlist .tailfield + 1 ].cint = 10000 ;
  } 
  mem [curlist .tailfield ].hh .v.RH = newparamglue ( 14 ) ;
  lastlinefill = mem [curlist .tailfield ].hh .v.RH ;
  initcurlang = curlist .pgfield % 65536L ;
  initlhyf = curlist .pgfield / 4194304L ;
  initrhyf = ( curlist .pgfield / 65536L ) % 64 ;
  popnest () ;
  noshrinkerroryet = true ;
  if ( ( mem [eqtb [2252747L ].hh .v.RH ].hh.b1 != 0 ) && ( mem [eqtb [
  2252747L ].hh .v.RH + 3 ].cint != 0 ) ) 
  {
    eqtb [2252747L ].hh .v.RH = finiteshrink ( eqtb [2252747L ].hh .v.RH ) 
    ;
  } 
  if ( ( mem [eqtb [2252748L ].hh .v.RH ].hh.b1 != 0 ) && ( mem [eqtb [
  2252748L ].hh .v.RH + 3 ].cint != 0 ) ) 
  {
    eqtb [2252748L ].hh .v.RH = finiteshrink ( eqtb [2252748L ].hh .v.RH ) 
    ;
  } 
  q = eqtb [2252747L ].hh .v.RH ;
  r = eqtb [2252748L ].hh .v.RH ;
  background [1 ]= mem [q + 1 ].cint + mem [r + 1 ].cint ;
  background [2 ]= 0 ;
  background [3 ]= 0 ;
  background [4 ]= 0 ;
  background [5 ]= 0 ;
  background [2 + mem [q ].hh.b0 ]= mem [q + 2 ].cint ;
  background [2 + mem [r ].hh.b0 ]= background [2 + mem [r ].hh.b0 ]+ 
  mem [r + 2 ].cint ;
  background [6 ]= mem [q + 3 ].cint + mem [r + 3 ].cint ;
  dolastlinefit = false ;
  activenodesize = 3 ;
  if ( eqtb [8939304L ].cint > 0 ) 
  {
    q = mem [lastlinefill + 1 ].hh .v.LH ;
    if ( ( mem [q + 2 ].cint > 0 ) && ( mem [q ].hh.b0 > 0 ) ) {
	
      if ( ( background [3 ]== 0 ) && ( background [4 ]== 0 ) && ( 
      background [5 ]== 0 ) ) 
      {
	dolastlinefit = true ;
	activenodesize = 5 ;
	fillwidth [0 ]= 0 ;
	fillwidth [1 ]= 0 ;
	fillwidth [2 ]= 0 ;
	fillwidth [mem [q ].hh.b0 - 1 ]= mem [q + 2 ].cint ;
      } 
    } 
  } 
  minimumdemerits = 1073741823L ;
  minimaldemerits [3 ]= 1073741823L ;
  minimaldemerits [2 ]= 1073741823L ;
  minimaldemerits [1 ]= 1073741823L ;
  minimaldemerits [0 ]= 1073741823L ;
  if ( eqtb [2253271L ].hh .v.RH == -268435455L ) {
      
    if ( eqtb [10053708L ].cint == 0 ) 
    {
      lastspecialline = 0 ;
      secondwidth = eqtb [10053694L ].cint ;
      secondindent = 0 ;
    } 
    else {
	
      lastspecialline = abs ( eqtb [8939281L ].cint ) ;
      if ( eqtb [8939281L ].cint < 0 ) 
      {
	firstwidth = eqtb [10053694L ].cint - abs ( eqtb [10053708L ].cint 
	) ;
	if ( eqtb [10053708L ].cint >= 0 ) 
	firstindent = eqtb [10053708L ].cint ;
	else firstindent = 0 ;
	secondwidth = eqtb [10053694L ].cint ;
	secondindent = 0 ;
      } 
      else {
	  
	firstwidth = eqtb [10053694L ].cint ;
	firstindent = 0 ;
	secondwidth = eqtb [10053694L ].cint - abs ( eqtb [10053708L ]
	.cint ) ;
	if ( eqtb [10053708L ].cint >= 0 ) 
	secondindent = eqtb [10053708L ].cint ;
	else secondindent = 0 ;
      } 
    } 
  } 
  else {
      
    lastspecialline = mem [eqtb [2253271L ].hh .v.RH ].hh .v.LH - 1 ;
    secondwidth = mem [eqtb [2253271L ].hh .v.RH + 2 * ( lastspecialline + 
    1 ) ].cint ;
    secondindent = mem [eqtb [2253271L ].hh .v.RH + 2 * lastspecialline + 1 
    ].cint ;
  } 
  if ( eqtb [8939259L ].cint == 0 ) 
  easyline = lastspecialline ;
  else easyline = 1073741823L ;
  threshold = eqtb [8939240L ].cint ;
  if ( threshold >= 0 ) 
  {
	;
#ifdef STAT
    if ( eqtb [8939272L ].cint > 0 ) 
    {
      begindiagnostic () ;
      printnl ( 66356L ) ;
    } 
#endif /* STAT */
    secondpass = false ;
    finalpass = false ;
  } 
  else {
      
    threshold = eqtb [8939241L ].cint ;
    secondpass = true ;
    finalpass = ( eqtb [10053711L ].cint <= 0 ) ;
	;
#ifdef STAT
    if ( eqtb [8939272L ].cint > 0 ) 
    begindiagnostic () ;
#endif /* STAT */
  } 
  while ( true ) {
      
    if ( threshold > 10000 ) 
    threshold = 10000 ;
    if ( secondpass ) 
    {
	;
#ifdef INITEX
      if ( trienotready ) 
      inittrie () ;
#endif /* INITEX */
      curlang = initcurlang ;
      lhyf = initlhyf ;
      rhyf = initrhyf ;
      if ( trietrc [hyphstart + curlang ]!= curlang ) 
      hyphindex = 0 ;
      else hyphindex = trietrl [hyphstart + curlang ];
    } 
    q = getnode ( activenodesize ) ;
    mem [q ].hh.b0 = 0 ;
    mem [q ].hh.b1 = 2 ;
    mem [q ].hh .v.RH = memtop - 7 ;
    mem [q + 1 ].hh .v.RH = -268435455L ;
    mem [q + 1 ].hh .v.LH = curlist .pgfield + 1 ;
    mem [q + 2 ].cint = 0 ;
    mem [memtop - 7 ].hh .v.RH = q ;
    if ( dolastlinefit ) 
    {
      mem [q + 3 ].cint = 0 ;
      mem [q + 4 ].cint = 0 ;
    } 
    activewidth [1 ]= background [1 ];
    activewidth [2 ]= background [2 ];
    activewidth [3 ]= background [3 ];
    activewidth [4 ]= background [4 ];
    activewidth [5 ]= background [5 ];
    activewidth [6 ]= background [6 ];
    passive = -268435455L ;
    printednode = memtop - 3 ;
    passnumber = 0 ;
    fontinshortdisplay = 0 ;
    curp = mem [memtop - 3 ].hh .v.RH ;
    autobreaking = true ;
    {
      prevp = curp ;
      globalprevp = curp ;
    } 
    firstp = curp ;
    while ( ( curp != -268435455L ) && ( mem [memtop - 7 ].hh .v.RH != 
    memtop - 7 ) ) {
	
      if ( ( curp >= himemmin ) ) 
      {
	{
	  prevp = curp ;
	  globalprevp = curp ;
	} 
	do {
	    f = mem [curp ].hh.b0 ;
	  activewidth [1 ]= activewidth [1 ]+ fontinfo [widthbase [f ]+ 
	  fontinfo [charbase [f ]+ effectivechar ( true , f , mem [curp ]
	  .hh.b1 ) ].qqqq .b0 ].cint ;
	  curp = mem [curp ].hh .v.RH ;
	} while ( ! ( ! ( curp >= himemmin ) ) ) ;
      } 
      switch ( mem [curp ].hh.b0 ) 
      {case 0 : 
      case 1 : 
      case 2 : 
	activewidth [1 ]= activewidth [1 ]+ mem [curp + 1 ].cint ;
	break ;
      case 8 : 
	if ( mem [curp ].hh.b1 == 4 ) 
	{
	  curlang = mem [curp + 1 ].hh .v.RH ;
	  lhyf = mem [curp + 1 ].hh.b0 ;
	  rhyf = mem [curp + 1 ].hh.b1 ;
	  if ( trietrc [hyphstart + curlang ]!= curlang ) 
	  hyphindex = 0 ;
	  else hyphindex = trietrl [hyphstart + curlang ];
	} 
	else if ( ( ( ( mem [curp ].hh.b1 >= 40 ) && ( mem [curp ].hh.b1 
	<= 41 ) ) ) || ( mem [curp ].hh.b1 == 42 ) || ( mem [curp ].hh.b1 
	== 43 ) || ( mem [curp ].hh.b1 == 44 ) ) 
	{
	  activewidth [1 ]= activewidth [1 ]+ mem [curp + 1 ].cint ;
	} 
	break ;
      case 10 : 
	{
	  if ( autobreaking ) 
	  {
	    if ( ( prevp >= himemmin ) ) 
	    trybreak ( 0 , 0 ) ;
	    else if ( ( mem [prevp ].hh.b0 < 9 ) ) 
	    trybreak ( 0 , 0 ) ;
	    else if ( ( mem [prevp ].hh.b0 == 11 ) && ( mem [prevp ].hh.b1 
	    != 1 ) ) 
	    trybreak ( 0 , 0 ) ;
	  } 
	  if ( ( mem [mem [curp + 1 ].hh .v.LH ].hh.b1 != 0 ) && ( mem [
	  mem [curp + 1 ].hh .v.LH + 3 ].cint != 0 ) ) 
	  {
	    mem [curp + 1 ].hh .v.LH = finiteshrink ( mem [curp + 1 ].hh 
	    .v.LH ) ;
	  } 
	  q = mem [curp + 1 ].hh .v.LH ;
	  activewidth [1 ]= activewidth [1 ]+ mem [q + 1 ].cint ;
	  activewidth [2 + mem [q ].hh.b0 ]= activewidth [2 + mem [q ]
	  .hh.b0 ]+ mem [q + 2 ].cint ;
	  activewidth [6 ]= activewidth [6 ]+ mem [q + 3 ].cint ;
	  if ( secondpass && autobreaking ) 
	  {
	    prevs = curp ;
	    s = mem [prevs ].hh .v.RH ;
	    if ( s != -268435455L ) 
	    {
	      while ( true ) {
		  
		if ( ( s >= himemmin ) ) 
		{
		  c = mem [s ].hh.b1 ;
		  hf = mem [s ].hh.b0 ;
		} 
		else if ( mem [s ].hh.b0 == 6 ) {
		    
		  if ( mem [s + 1 ].hh .v.RH == -268435455L ) 
		  goto lab22 ;
		  else {
		      
		    q = mem [s + 1 ].hh .v.RH ;
		    c = mem [q ].hh.b1 ;
		    hf = mem [q ].hh.b0 ;
		  } 
		} 
		else if ( ( mem [s ].hh.b0 == 11 ) && ( mem [s ].hh.b1 == 
		0 ) ) 
		goto lab22 ;
		else if ( ( mem [s ].hh.b0 == 9 ) && ( mem [s ].hh.b1 >= 4 
		) ) 
		goto lab22 ;
		else if ( mem [s ].hh.b0 == 8 ) 
		{
		  if ( ( ( ( mem [s ].hh.b1 >= 40 ) && ( mem [s ].hh.b1 <= 
		  41 ) ) ) ) 
		  {
		    {register integer for_end; l = 0 ;for_end = mem [s + 4 
		    ].qqqq .b2 - 1 ; if ( l <= for_end) do 
		      {
			c = getnativeusv ( s , l ) ;
			if ( eqtb [3368680L + c ].hh .v.RH != 0 ) 
			{
			  hf = mem [s + 4 ].qqqq .b1 ;
			  prevs = s ;
			  if ( ( eqtb [3368680L + c ].hh .v.RH == c ) || ( 
			  eqtb [8939278L ].cint > 0 ) ) 
			  goto lab32 ;
			  else goto lab31 ;
			} 
			if ( c >= 65536L ) 
			incr ( l ) ;
		      } 
		    while ( l++ < for_end ) ;} 
		  } 
		  if ( mem [s ].hh.b1 == 4 ) 
		  {
		    curlang = mem [s + 1 ].hh .v.RH ;
		    lhyf = mem [s + 1 ].hh.b0 ;
		    rhyf = mem [s + 1 ].hh.b1 ;
		    if ( trietrc [hyphstart + curlang ]!= curlang ) 
		    hyphindex = 0 ;
		    else hyphindex = trietrl [hyphstart + curlang ];
		  } 
		  goto lab22 ;
		} 
		else goto lab31 ;
		if ( ( hyphindex == 0 ) || ( ( c ) > 255 ) ) 
		hc [0 ]= eqtb [3368680L + c ].hh .v.RH ;
		else if ( trietrc [hyphindex + c ]!= c ) 
		hc [0 ]= 0 ;
		else hc [0 ]= trietro [hyphindex + c ];
		if ( hc [0 ]!= 0 ) {
		    
		  if ( ( hc [0 ]== c ) || ( eqtb [8939278L ].cint > 0 ) ) 
		  goto lab32 ;
		  else goto lab31 ;
		} 
		lab22: prevs = s ;
		s = mem [prevs ].hh .v.RH ;
	      } 
	      lab32: hyfchar = hyphenchar [hf ];
	      if ( hyfchar < 0 ) 
	      goto lab31 ;
	      if ( hyfchar > 65535L ) 
	      goto lab31 ;
	      ha = prevs ;
	      if ( lhyf + rhyf > maxhyphenatablelength () ) 
	      goto lab31 ;
	      if ( ( ( ( ha ) != -268435455L ) && ( ! ( ha >= himemmin ) ) && 
	      ( mem [ha ].hh.b0 == 8 ) && ( ( ( mem [ha ].hh.b1 >= 40 ) && 
	      ( mem [ha ].hh.b1 <= 41 ) ) ) ) ) 
	      {
		s = mem [ha ].hh .v.RH ;
		while ( true ) {
		    
		  if ( ! ( ( s >= himemmin ) ) ) 
		  switch ( mem [s ].hh.b0 ) 
		  {case 6 : 
		    ;
		    break ;
		  case 11 : 
		    if ( mem [s ].hh.b1 != 0 ) 
		    goto lab36 ;
		    break ;
		  case 8 : 
		  case 10 : 
		  case 12 : 
		  case 3 : 
		  case 5 : 
		  case 4 : 
		    goto lab36 ;
		    break ;
		    default: 
		    goto lab31 ;
		    break ;
		  } 
		  s = mem [s ].hh .v.RH ;
		} 
		lab36: ;
		hn = 0 ;
		lab20: {
		    register integer for_end; l = 0 ;for_end = mem [ha 
		+ 4 ].qqqq .b2 - 1 ; if ( l <= for_end) do 
		  {
		    c = getnativeusv ( ha , l ) ;
		    if ( ( hyphindex == 0 ) || ( ( c ) > 255 ) ) 
		    hc [0 ]= eqtb [3368680L + c ].hh .v.RH ;
		    else if ( trietrc [hyphindex + c ]!= c ) 
		    hc [0 ]= 0 ;
		    else hc [0 ]= trietro [hyphindex + c ];
		    if ( ( hc [0 ]== 0 ) ) 
		    {
		      if ( ( hn > 0 ) ) 
		      {
			q = newnativewordnode ( hf , mem [ha + 4 ].qqqq .b2 
			- l ) ;
			mem [q ].hh.b1 = mem [ha ].hh.b1 ;
			{register integer for_end; i = l ;for_end = mem [ha 
			+ 4 ].qqqq .b2 - 1 ; if ( i <= for_end) do 
			  setnativechar ( q , i - l , getnativechar ( ha , i ) 
			  ) ;
			while ( i++ < for_end ) ;} 
			setnativemetrics ( q , ( eqtb [8939314L ].cint > 0 ) 
			) ;
			mem [q ].hh .v.RH = mem [ha ].hh .v.RH ;
			mem [ha ].hh .v.RH = q ;
			mem [ha + 4 ].qqqq .b2 = l ;
			setnativemetrics ( ha , ( eqtb [8939314L ].cint > 0 
			) ) ;
			goto lab33 ;
		      } 
		    } 
		    else if ( ( hn == 0 ) && ( l > 0 ) ) 
		    {
		      q = newnativewordnode ( hf , mem [ha + 4 ].qqqq .b2 - 
		      l ) ;
		      mem [q ].hh.b1 = mem [ha ].hh.b1 ;
		      {register integer for_end; i = l ;for_end = mem [ha + 
		      4 ].qqqq .b2 - 1 ; if ( i <= for_end) do 
			setnativechar ( q , i - l , getnativechar ( ha , i ) ) 
			;
		      while ( i++ < for_end ) ;} 
		      setnativemetrics ( q , ( eqtb [8939314L ].cint > 0 ) ) 
		      ;
		      mem [q ].hh .v.RH = mem [ha ].hh .v.RH ;
		      mem [ha ].hh .v.RH = q ;
		      mem [ha + 4 ].qqqq .b2 = l ;
		      setnativemetrics ( ha , ( eqtb [8939314L ].cint > 0 ) 
		      ) ;
		      ha = mem [ha ].hh .v.RH ;
		      goto lab20 ;
		    } 
		    else if ( ( hn == maxhyphenatablelength () ) ) 
		    goto lab33 ;
		    else {
			
		      incr ( hn ) ;
		      if ( c < 65536L ) 
		      {
			hu [hn ]= c ;
			hc [hn ]= hc [0 ];
		      } 
		      else {
			  
			hu [hn ]= ( c - 65536L ) / 1024 + 55296L ;
			hc [hn ]= ( hc [0 ]- 65536L ) / 1024 + 55296L ;
			incr ( hn ) ;
			hu [hn ]= c % 1024 + 56320L ;
			hc [hn ]= hc [0 ]% 1024 + 56320L ;
			incr ( l ) ;
		      } 
		      hyfbchar = 65536L ;
		    } 
		  } 
		while ( l++ < for_end ) ;} 
	      } 
	      else {
		  
		hn = 0 ;
		while ( true ) {
		    
		  if ( ( s >= himemmin ) ) 
		  {
		    if ( mem [s ].hh.b0 != hf ) 
		    goto lab33 ;
		    hyfbchar = mem [s ].hh.b1 ;
		    c = hyfbchar ;
		    if ( ( hyphindex == 0 ) || ( ( c ) > 255 ) ) 
		    hc [0 ]= eqtb [3368680L + c ].hh .v.RH ;
		    else if ( trietrc [hyphindex + c ]!= c ) 
		    hc [0 ]= 0 ;
		    else hc [0 ]= trietro [hyphindex + c ];
		    if ( hc [0 ]== 0 ) 
		    goto lab33 ;
		    if ( hc [0 ]> maxhyphchar ) 
		    goto lab33 ;
		    if ( hn == maxhyphenatablelength () ) 
		    goto lab33 ;
		    hb = s ;
		    incr ( hn ) ;
		    hu [hn ]= c ;
		    hc [hn ]= hc [0 ];
		    hyfbchar = 65536L ;
		  } 
		  else if ( mem [s ].hh.b0 == 6 ) 
		  {
		    if ( mem [s + 1 ].hh.b0 != hf ) 
		    goto lab33 ;
		    j = hn ;
		    q = mem [s + 1 ].hh .v.RH ;
		    if ( q > -268435455L ) 
		    hyfbchar = mem [q ].hh.b1 ;
		    while ( q > -268435455L ) {
			
		      c = mem [q ].hh.b1 ;
		      if ( ( hyphindex == 0 ) || ( ( c ) > 255 ) ) 
		      hc [0 ]= eqtb [3368680L + c ].hh .v.RH ;
		      else if ( trietrc [hyphindex + c ]!= c ) 
		      hc [0 ]= 0 ;
		      else hc [0 ]= trietro [hyphindex + c ];
		      if ( hc [0 ]== 0 ) 
		      goto lab33 ;
		      if ( hc [0 ]> maxhyphchar ) 
		      goto lab33 ;
		      if ( j == maxhyphenatablelength () ) 
		      goto lab33 ;
		      incr ( j ) ;
		      hu [j ]= c ;
		      hc [j ]= hc [0 ];
		      q = mem [q ].hh .v.RH ;
		    } 
		    hb = s ;
		    hn = j ;
		    if ( odd ( mem [s ].hh.b1 ) ) 
		    hyfbchar = fontbchar [hf ];
		    else hyfbchar = 65536L ;
		  } 
		  else if ( ( mem [s ].hh.b0 == 11 ) && ( mem [s ].hh.b1 
		  == 0 ) ) 
		  {
		    hb = s ;
		    hyfbchar = fontbchar [hf ];
		  } 
		  else goto lab33 ;
		  s = mem [s ].hh .v.RH ;
		} 
		lab33: ;
	      } 
	      if ( hn < lhyf + rhyf ) 
	      goto lab31 ;
	      while ( true ) {
		  
		if ( ! ( ( s >= himemmin ) ) ) 
		switch ( mem [s ].hh.b0 ) 
		{case 6 : 
		  ;
		  break ;
		case 11 : 
		  if ( mem [s ].hh.b1 != 0 ) 
		  goto lab34 ;
		  break ;
		case 8 : 
		case 10 : 
		case 12 : 
		case 3 : 
		case 5 : 
		case 4 : 
		  goto lab34 ;
		  break ;
		case 9 : 
		  if ( mem [s ].hh.b1 >= 4 ) 
		  goto lab34 ;
		  else goto lab31 ;
		  break ;
		  default: 
		  goto lab31 ;
		  break ;
		} 
		s = mem [s ].hh .v.RH ;
	      } 
	      lab34: ;
	      hyphenate () ;
	    } 
	    lab31: ;
	  } 
	} 
	break ;
      case 11 : 
	if ( mem [curp ].hh.b1 == 1 ) 
	{
	  if ( ! ( mem [curp ].hh .v.RH >= himemmin ) && autobreaking ) {
	      
	    if ( mem [mem [curp ].hh .v.RH ].hh.b0 == 10 ) 
	    trybreak ( 0 , 0 ) ;
	  } 
	  activewidth [1 ]= activewidth [1 ]+ mem [curp + 1 ].cint ;
	} 
	else activewidth [1 ]= activewidth [1 ]+ mem [curp + 1 ].cint ;
	break ;
      case 6 : 
	{
	  f = mem [curp + 1 ].hh.b0 ;
	  xtxligaturepresent = true ;
	  activewidth [1 ]= activewidth [1 ]+ fontinfo [widthbase [f ]+ 
	  fontinfo [charbase [f ]+ effectivechar ( true , f , mem [curp + 
	  1 ].hh.b1 ) ].qqqq .b0 ].cint ;
	} 
	break ;
      case 7 : 
	{
	  s = mem [curp + 1 ].hh .v.LH ;
	  discwidth = 0 ;
	  if ( s == -268435455L ) 
	  trybreak ( eqtb [8939244L ].cint , 1 ) ;
	  else {
	      
	    do {
		if ( ( s >= himemmin ) ) 
	      {
		f = mem [s ].hh.b0 ;
		discwidth = discwidth + fontinfo [widthbase [f ]+ fontinfo 
		[charbase [f ]+ effectivechar ( true , f , mem [s ].hh.b1 
		) ].qqqq .b0 ].cint ;
	      } 
	      else switch ( mem [s ].hh.b0 ) 
	      {case 6 : 
		{
		  f = mem [s + 1 ].hh.b0 ;
		  xtxligaturepresent = true ;
		  discwidth = discwidth + fontinfo [widthbase [f ]+ 
		  fontinfo [charbase [f ]+ effectivechar ( true , f , mem [
		  s + 1 ].hh.b1 ) ].qqqq .b0 ].cint ;
		} 
		break ;
	      case 0 : 
	      case 1 : 
	      case 2 : 
	      case 11 : 
		discwidth = discwidth + mem [s + 1 ].cint ;
		break ;
	      case 8 : 
		if ( ( ( ( mem [s ].hh.b1 >= 40 ) && ( mem [s ].hh.b1 <= 
		41 ) ) ) || ( mem [s ].hh.b1 == 42 ) || ( mem [s ].hh.b1 
		== 43 ) || ( mem [s ].hh.b1 == 44 ) ) 
		discwidth = discwidth + mem [s + 1 ].cint ;
		else confusion ( 66360L ) ;
		break ;
		default: 
		confusion ( 66361L ) ;
		break ;
	      } 
	      s = mem [s ].hh .v.RH ;
	    } while ( ! ( s == -268435455L ) ) ;
	    activewidth [1 ]= activewidth [1 ]+ discwidth ;
	    trybreak ( eqtb [8939243L ].cint , 1 ) ;
	    activewidth [1 ]= activewidth [1 ]- discwidth ;
	  } 
	  r = mem [curp ].hh.b1 ;
	  s = mem [curp ].hh .v.RH ;
	  while ( r > 0 ) {
	      
	    if ( ( s >= himemmin ) ) 
	    {
	      f = mem [s ].hh.b0 ;
	      activewidth [1 ]= activewidth [1 ]+ fontinfo [widthbase [f 
	      ]+ fontinfo [charbase [f ]+ effectivechar ( true , f , mem [
	      s ].hh.b1 ) ].qqqq .b0 ].cint ;
	    } 
	    else switch ( mem [s ].hh.b0 ) 
	    {case 6 : 
	      {
		f = mem [s + 1 ].hh.b0 ;
		xtxligaturepresent = true ;
		activewidth [1 ]= activewidth [1 ]+ fontinfo [widthbase [
		f ]+ fontinfo [charbase [f ]+ effectivechar ( true , f , 
		mem [s + 1 ].hh.b1 ) ].qqqq .b0 ].cint ;
	      } 
	      break ;
	    case 0 : 
	    case 1 : 
	    case 2 : 
	    case 11 : 
	      activewidth [1 ]= activewidth [1 ]+ mem [s + 1 ].cint ;
	      break ;
	    case 8 : 
	      if ( ( ( ( mem [s ].hh.b1 >= 40 ) && ( mem [s ].hh.b1 <= 41 
	      ) ) ) || ( mem [s ].hh.b1 == 42 ) || ( mem [s ].hh.b1 == 43 
	      ) || ( mem [s ].hh.b1 == 44 ) ) 
	      activewidth [1 ]= activewidth [1 ]+ mem [s + 1 ].cint ;
	      else confusion ( 66362L ) ;
	      break ;
	      default: 
	      confusion ( 66363L ) ;
	      break ;
	    } 
	    decr ( r ) ;
	    s = mem [s ].hh .v.RH ;
	  } 
	  {
	    prevp = curp ;
	    globalprevp = curp ;
	  } 
	  curp = s ;
	  goto lab35 ;
	} 
	break ;
      case 9 : 
	{
	  if ( mem [curp ].hh.b1 < 4 ) 
	  autobreaking = odd ( mem [curp ].hh.b1 ) ;
	  {
	    if ( ! ( mem [curp ].hh .v.RH >= himemmin ) && autobreaking ) {
		
	      if ( mem [mem [curp ].hh .v.RH ].hh.b0 == 10 ) 
	      trybreak ( 0 , 0 ) ;
	    } 
	    activewidth [1 ]= activewidth [1 ]+ mem [curp + 1 ].cint ;
	  } 
	} 
	break ;
      case 12 : 
	trybreak ( mem [curp + 1 ].cint , 0 ) ;
	break ;
      case 4 : 
      case 3 : 
      case 5 : 
	;
	break ;
	default: 
	confusion ( 66359L ) ;
	break ;
      } 
      {
	prevp = curp ;
	globalprevp = curp ;
      } 
      curp = mem [curp ].hh .v.RH ;
      lab35: ;
    } 
    if ( curp == -268435455L ) 
    {
      trybreak ( -10000 , 1 ) ;
      if ( mem [memtop - 7 ].hh .v.RH != memtop - 7 ) 
      {
	r = mem [memtop - 7 ].hh .v.RH ;
	fewestdemerits = 1073741823L ;
	do {
	    if ( mem [r ].hh.b0 != 2 ) { 
	    if ( mem [r + 2 ].cint < fewestdemerits ) 
	    {
	      fewestdemerits = mem [r + 2 ].cint ;
	      bestbet = r ;
	    } 
	  } 
	  r = mem [r ].hh .v.RH ;
	} while ( ! ( r == memtop - 7 ) ) ;
	bestline = mem [bestbet + 1 ].hh .v.LH ;
	if ( eqtb [8939259L ].cint == 0 ) 
	goto lab30 ;
	{
	  r = mem [memtop - 7 ].hh .v.RH ;
	  actuallooseness = 0 ;
	  do {
	      if ( mem [r ].hh.b0 != 2 ) 
	    {
	      linediff = mem [r + 1 ].hh .v.LH - bestline ;
	      if ( ( ( linediff < actuallooseness ) && ( eqtb [8939259L ]
	      .cint <= linediff ) ) || ( ( linediff > actuallooseness ) && ( 
	      eqtb [8939259L ].cint >= linediff ) ) ) 
	      {
		bestbet = r ;
		actuallooseness = linediff ;
		fewestdemerits = mem [r + 2 ].cint ;
	      } 
	      else if ( ( linediff == actuallooseness ) && ( mem [r + 2 ]
	      .cint < fewestdemerits ) ) 
	      {
		bestbet = r ;
		fewestdemerits = mem [r + 2 ].cint ;
	      } 
	    } 
	    r = mem [r ].hh .v.RH ;
	  } while ( ! ( r == memtop - 7 ) ) ;
	  bestline = mem [bestbet + 1 ].hh .v.LH ;
	} 
	if ( ( actuallooseness == eqtb [8939259L ].cint ) || finalpass ) 
	goto lab30 ;
      } 
    } 
    q = mem [memtop - 7 ].hh .v.RH ;
    while ( q != memtop - 7 ) {
	
      curp = mem [q ].hh .v.RH ;
      if ( mem [q ].hh.b0 == 2 ) 
      freenode ( q , 7 ) ;
      else freenode ( q , activenodesize ) ;
      q = curp ;
    } 
    q = passive ;
    while ( q != -268435455L ) {
	
      curp = mem [q ].hh .v.RH ;
      freenode ( q , 2 ) ;
      q = curp ;
    } 
    if ( ! secondpass ) 
    {
	;
#ifdef STAT
      if ( eqtb [8939272L ].cint > 0 ) 
      printnl ( 66357L ) ;
#endif /* STAT */
      threshold = eqtb [8939241L ].cint ;
      secondpass = true ;
      finalpass = ( eqtb [10053711L ].cint <= 0 ) ;
    } 
    else {
	
	;
#ifdef STAT
      if ( eqtb [8939272L ].cint > 0 ) 
      printnl ( 66358L ) ;
#endif /* STAT */
      background [2 ]= background [2 ]+ eqtb [10053711L ].cint ;
      finalpass = true ;
    } 
  } 
  lab30: 
	;
#ifdef STAT
  if ( eqtb [8939272L ].cint > 0 ) 
  {
    enddiagnostic ( true ) ;
    normalizeselector () ;
  } 
#endif /* STAT */
  if ( dolastlinefit ) {
      
    if ( mem [bestbet + 3 ].cint == 0 ) 
    dolastlinefit = false ;
    else {
	
      q = newspec ( mem [lastlinefill + 1 ].hh .v.LH ) ;
      deleteglueref ( mem [lastlinefill + 1 ].hh .v.LH ) ;
      mem [q + 1 ].cint = mem [q + 1 ].cint + mem [bestbet + 3 ].cint - 
      mem [bestbet + 4 ].cint ;
      mem [q + 2 ].cint = 0 ;
      mem [lastlinefill + 1 ].hh .v.LH = q ;
    } 
  } 
  postlinebreak ( d ) ;
  q = mem [memtop - 7 ].hh .v.RH ;
  while ( q != memtop - 7 ) {
      
    curp = mem [q ].hh .v.RH ;
    if ( mem [q ].hh.b0 == 2 ) 
    freenode ( q , 7 ) ;
    else freenode ( q , activenodesize ) ;
    q = curp ;
  } 
  q = passive ;
  while ( q != -268435455L ) {
      
    curp = mem [q ].hh .v.RH ;
    freenode ( q , 2 ) ;
    q = curp ;
  } 
  packbeginline = 0 ;
} 
void 
newhyphexceptions ( void ) 
{
  /* 21 10 40 45 46 */ newhyphexceptions_regmem 
  short n  ;
  short j  ;
  hyphpointer h  ;
  strnumber k  ;
  halfword p  ;
  halfword q  ;
  strnumber s  ;
  poolpointer u, v  ;
  scanleftbrace () ;
  if ( eqtb [8939290L ].cint <= 0 ) 
  curlang = 0 ;
  else if ( eqtb [8939290L ].cint > 255 ) 
  curlang = 0 ;
  else curlang = eqtb [8939290L ].cint ;
	;
#ifdef INITEX
  if ( trienotready ) 
  {
    hyphindex = 0 ;
    goto lab46 ;
  } 
#endif /* INITEX */
  if ( trietrc [hyphstart + curlang ]!= curlang ) 
  hyphindex = 0 ;
  else hyphindex = trietrl [hyphstart + curlang ];
  lab46: n = 0 ;
  p = -268435455L ;
  while ( true ) {
      
    getxtoken () ;
    lab21: switch ( curcmd ) 
    {case 11 : 
    case 12 : 
    case 68 : 
      if ( curchr == 45 ) 
      {
	if ( n < maxhyphenatablelength () ) 
	{
	  q = getavail () ;
	  mem [q ].hh .v.RH = p ;
	  mem [q ].hh .v.LH = n ;
	  p = q ;
	} 
      } 
      else {
	  
	if ( ( hyphindex == 0 ) || ( ( curchr ) > 255 ) ) 
	hc [0 ]= eqtb [3368680L + curchr ].hh .v.RH ;
	else if ( trietrc [hyphindex + curchr ]!= curchr ) 
	hc [0 ]= 0 ;
	else hc [0 ]= trietro [hyphindex + curchr ];
	if ( hc [0 ]== 0 ) 
	{
	  {
	    if ( interaction == 3 ) 
	    ;
	    if ( filelineerrorstylep ) 
	    printfileline () ;
	    else printnl ( 65544L ) ;
	    print ( 66370L ) ;
	  } 
	  {
	    helpptr = 2 ;
	    helpline [1 ]= 66371L ;
	    helpline [0 ]= 66372L ;
	  } 
	  error () ;
	} 
	else if ( n < maxhyphenatablelength () ) 
	{
	  incr ( n ) ;
	  if ( hc [0 ]< 65536L ) 
	  hc [n ]= hc [0 ];
	  else {
	      
	    hc [n ]= ( hc [0 ]- 65536L ) / 1024 + 55296L ;
	    incr ( n ) ;
	    hc [n ]= hc [0 ]% 1024 + 56320L ;
	  } 
	} 
      } 
      break ;
    case 16 : 
      {
	scancharnum () ;
	curchr = curval ;
	curcmd = 68 ;
	goto lab21 ;
      } 
      break ;
    case 10 : 
    case 2 : 
      {
	if ( n > 1 ) 
	{
	  incr ( n ) ;
	  hc [n ]= curlang ;
	  {
	    if ( poolptr + n > poolsize ) 
	    overflow ( 65539L , poolsize - initpoolptr ) ;
	  } 
	  h = 0 ;
	  {register integer for_end; j = 1 ;for_end = n ; if ( j <= for_end) 
	  do 
	    {
	      h = ( h + h + hc [j ]) % 607 ;
	      {
		strpool [poolptr ]= hc [j ];
		incr ( poolptr ) ;
	      } 
	    } 
	  while ( j++ < for_end ) ;} 
	  s = makestring () ;
	  if ( hyphnext <= 607 ) 
	  while ( ( hyphnext > 0 ) && ( hyphword [hyphnext - 1 ]> 0 ) ) decr 
	  ( hyphnext ) ;
	  if ( ( hyphcount == hyphsize ) || ( hyphnext == 0 ) ) 
	  overflow ( 66373L , hyphsize ) ;
	  incr ( hyphcount ) ;
	  while ( hyphword [h ]!= 0 ) {
	      
	    k = hyphword [h ];
	    if ( length ( k ) != length ( s ) ) 
	    goto lab45 ;
	    u = strstart [( k ) - 65536L ];
	    v = strstart [( s ) - 65536L ];
	    do {
		if ( strpool [u ]!= strpool [v ]) 
	      goto lab45 ;
	      incr ( u ) ;
	      incr ( v ) ;
	    } while ( ! ( u == strstart [( k + 1 ) - 65536L ]) ) ;
	    {
	      decr ( strptr ) ;
	      poolptr = strstart [( strptr ) - 65536L ];
	    } 
	    s = hyphword [h ];
	    decr ( hyphcount ) ;
	    goto lab40 ;
	    lab45: ;
	    if ( hyphlink [h ]== 0 ) 
	    {
	      hyphlink [h ]= hyphnext ;
	      if ( hyphnext >= hyphsize ) 
	      hyphnext = 607 ;
	      if ( hyphnext > 607 ) 
	      incr ( hyphnext ) ;
	    } 
	    h = hyphlink [h ]- 1 ;
	  } 
	  lab40: hyphword [h ]= s ;
	  hyphlist [h ]= p ;
	} 
	if ( curcmd == 2 ) 
	return ;
	n = 0 ;
	p = -268435455L ;
      } 
      break ;
      default: 
      {
	{
	  if ( interaction == 3 ) 
	  ;
	  if ( filelineerrorstylep ) 
	  printfileline () ;
	  else printnl ( 65544L ) ;
	  print ( 66023L ) ;
	} 
	printesc ( 66366L ) ;
	print ( 66367L ) ;
	{
	  helpptr = 2 ;
	  helpline [1 ]= 66368L ;
	  helpline [0 ]= 66369L ;
	} 
	error () ;
      } 
      break ;
    } 
  } 
} 
boolean 
zdomarks ( smallnumber a , smallnumber l , halfword q ) 
{
  register boolean Result; domarks_regmem 
  smallnumber i  ;
  if ( l < 4 ) 
  {
    {register integer for_end; i = 0 ;for_end = 15 ; if ( i <= for_end) do 
      {
	if ( odd ( i ) ) 
	curptr = mem [q + ( i / 2 ) + 1 ].hh .v.RH ;
	else curptr = mem [q + ( i / 2 ) + 1 ].hh .v.LH ;
	if ( curptr != -268435455L ) {
	    
	  if ( domarks ( a , l + 1 , curptr ) ) 
	  {
	    if ( odd ( i ) ) 
	    mem [q + ( i / 2 ) + 1 ].hh .v.RH = -268435455L ;
	    else mem [q + ( i / 2 ) + 1 ].hh .v.LH = -268435455L ;
	    decr ( mem [q ].hh.b1 ) ;
	  } 
	} 
      } 
    while ( i++ < for_end ) ;} 
    if ( mem [q ].hh.b1 == 0 ) 
    {
      freenode ( q , 33 ) ;
      q = -268435455L ;
    } 
  } 
  else {
      
    switch ( a ) 
    {case 0 : 
      if ( mem [q + 2 ].hh .v.RH != -268435455L ) 
      {
	deletetokenref ( mem [q + 2 ].hh .v.RH ) ;
	mem [q + 2 ].hh .v.RH = -268435455L ;
	deletetokenref ( mem [q + 3 ].hh .v.LH ) ;
	mem [q + 3 ].hh .v.LH = -268435455L ;
      } 
      break ;
    case 1 : 
      if ( mem [q + 2 ].hh .v.LH != -268435455L ) 
      {
	if ( mem [q + 1 ].hh .v.LH != -268435455L ) 
	deletetokenref ( mem [q + 1 ].hh .v.LH ) ;
	deletetokenref ( mem [q + 1 ].hh .v.RH ) ;
	mem [q + 1 ].hh .v.RH = -268435455L ;
	if ( mem [mem [q + 2 ].hh .v.LH ].hh .v.RH == -268435455L ) 
	{
	  deletetokenref ( mem [q + 2 ].hh .v.LH ) ;
	  mem [q + 2 ].hh .v.LH = -268435455L ;
	} 
	else incr ( mem [mem [q + 2 ].hh .v.LH ].hh .v.LH ) ;
	mem [q + 1 ].hh .v.LH = mem [q + 2 ].hh .v.LH ;
      } 
      break ;
    case 2 : 
      if ( ( mem [q + 1 ].hh .v.LH != -268435455L ) && ( mem [q + 1 ].hh 
      .v.RH == -268435455L ) ) 
      {
	mem [q + 1 ].hh .v.RH = mem [q + 1 ].hh .v.LH ;
	incr ( mem [mem [q + 1 ].hh .v.LH ].hh .v.LH ) ;
      } 
      break ;
	;
#ifdef INITEX
    case 3 : 
      {register integer for_end; i = 0 ;for_end = 4 ; if ( i <= for_end) do 
	{
	  if ( odd ( i ) ) 
	  curptr = mem [q + ( i / 2 ) + 1 ].hh .v.RH ;
	  else curptr = mem [q + ( i / 2 ) + 1 ].hh .v.LH ;
	  if ( curptr != -268435455L ) 
	  {
	    deletetokenref ( curptr ) ;
	    if ( odd ( i ) ) 
	    mem [q + ( i / 2 ) + 1 ].hh .v.RH = -268435455L ;
	    else mem [q + ( i / 2 ) + 1 ].hh .v.LH = -268435455L ;
	  } 
	} 
      while ( i++ < for_end ) ;} 
      break ;
#endif /* INITEX */
    } 
    if ( mem [q + 2 ].hh .v.LH == -268435455L ) {
	
      if ( mem [q + 3 ].hh .v.LH == -268435455L ) 
      {
	freenode ( q , 4 ) ;
	q = -268435455L ;
      } 
    } 
  } 
  Result = ( q == -268435455L ) ;
  return Result ;
} 
void 
prefixedcommand ( void ) 
{
  /* 30 10 */ prefixedcommand_regmem 
  smallnumber a  ;
  internalfontnumber f  ;
  halfword j  ;
  fontindex k  ;
  halfword p, q  ;
  integer n  ;
  boolean e  ;
  a = 0 ;
  while ( curcmd == 95 ) {
      
    if ( ! odd ( a / curchr ) ) 
    a = a + curchr ;
    do {
	getxtoken () ;
    } while ( ! ( ( curcmd != 10 ) && ( curcmd != 0 ) ) ) ;
    if ( curcmd <= 71 ) 
    {
      {
	if ( interaction == 3 ) 
	;
	if ( filelineerrorstylep ) 
	printfileline () ;
	else printnl ( 65544L ) ;
	print ( 66607L ) ;
      } 
      printcmdchr ( curcmd , curchr ) ;
      printchar ( 39 ) ;
      {
	helpptr = 1 ;
	helpline [0 ]= 66608L ;
      } 
      if ( ( eTeXmode == 1 ) ) 
      helpline [0 ]= 66609L ;
      backerror () ;
      return ;
    } 
    if ( eqtb [8939276L ].cint > 2 ) {
	
      if ( ( eTeXmode == 1 ) ) 
      showcurcmdchr () ;
    } 
  } 
  if ( a >= 8 ) 
  {
    j = 29360129L ;
    a = a - 8 ;
  } 
  else j = 0 ;
  if ( ( curcmd != 99 ) && ( ( a % 4 != 0 ) || ( j != 0 ) ) ) 
  {
    {
      if ( interaction == 3 ) 
      ;
      if ( filelineerrorstylep ) 
      printfileline () ;
      else printnl ( 65544L ) ;
      print ( 66028L ) ;
    } 
    printesc ( 66599L ) ;
    print ( 66610L ) ;
    printesc ( 66600L ) ;
    {
      helpptr = 1 ;
      helpline [0 ]= 66611L ;
    } 
    if ( ( eTeXmode == 1 ) ) 
    {
      helpline [0 ]= 66612L ;
      print ( 66610L ) ;
      printesc ( 66613L ) ;
    } 
    print ( 66614L ) ;
    printcmdchr ( curcmd , curchr ) ;
    printchar ( 39 ) ;
    error () ;
  } 
  if ( eqtb [8939283L ].cint != 0 ) {
      
    if ( eqtb [8939283L ].cint < 0 ) 
    {
      if ( ( a >= 4 ) ) 
      a = a - 4 ;
    } 
    else {
	
      if ( ! ( a >= 4 ) ) 
      a = a + 4 ;
    } 
  } 
  switch ( curcmd ) 
  {case 89 : 
    if ( ( a >= 4 ) ) 
    geqdefine ( 2253799L , 122 , curchr ) ;
    else eqdefine ( 2253799L , 122 , curchr ) ;
    break ;
  case 99 : 
    {
      if ( odd ( curchr ) && ! ( a >= 4 ) && ( eqtb [8939283L ].cint >= 0 ) 
      ) 
      a = a + 4 ;
      e = ( curchr >= 2 ) ;
      getrtoken () ;
      p = curcs ;
      q = scantoks ( true , e ) ;
      if ( j != 0 ) 
      {
	q = getavail () ;
	mem [q ].hh .v.LH = j ;
	mem [q ].hh .v.RH = mem [defref ].hh .v.RH ;
	mem [defref ].hh .v.RH = q ;
      } 
      if ( ( a >= 4 ) ) 
      geqdefine ( p , 113 + ( a % 4 ) , defref ) ;
      else eqdefine ( p , 113 + ( a % 4 ) , defref ) ;
    } 
    break ;
  case 96 : 
    {
      n = curchr ;
      getrtoken () ;
      p = curcs ;
      if ( n == 0 ) 
      {
	do {
	    gettoken () ;
	} while ( ! ( curcmd != 10 ) ) ;
	if ( curtok == 25165885L ) 
	{
	  gettoken () ;
	  if ( curcmd == 10 ) 
	  gettoken () ;
	} 
      } 
      else {
	  
	gettoken () ;
	q = curtok ;
	gettoken () ;
	backinput () ;
	curtok = q ;
	backinput () ;
      } 
      if ( curcmd >= 113 ) 
      incr ( mem [curchr ].hh .v.LH ) ;
      else if ( ( curcmd == 91 ) || ( curcmd == 72 ) ) {
	  
	if ( ( curchr < membot ) || ( curchr > membot + 19 ) ) 
	incr ( mem [curchr + 1 ].hh .v.LH ) ;
      } 
      if ( ( a >= 4 ) ) 
      geqdefine ( p , curcmd , curchr ) ;
      else eqdefine ( p , curcmd , curchr ) ;
    } 
    break ;
  case 97 : 
    if ( curchr == 7 ) 
    {
      scancharnum () ;
      p = 7825128L + curval ;
      scanoptionalequals () ;
      scancharnum () ;
      n = curval ;
      scancharnum () ;
      if ( ( eqtb [8939297L ].cint > 0 ) ) 
      {
	begindiagnostic () ;
	printnl ( 66636L ) ;
	print ( p - 7825128L ) ;
	print ( 66637L ) ;
	print ( n ) ;
	printchar ( 32 ) ;
	print ( curval ) ;
	enddiagnostic ( false ) ;
      } 
      n = n * 256 + curval ;
      if ( ( a >= 4 ) ) 
      geqdefine ( p , 122 , n ) ;
      else eqdefine ( p , 122 , n ) ;
      if ( ( p - 7825128L ) < eqtb [8939295L ].cint ) {
	  
	if ( ( a >= 4 ) ) 
	geqworddefine ( 8939295L , p - 7825128L ) ;
	else eqworddefine ( 8939295L , p - 7825128L ) ;
      } 
      if ( ( p - 7825128L ) > eqtb [8939296L ].cint ) {
	  
	if ( ( a >= 4 ) ) 
	geqworddefine ( 8939296L , p - 7825128L ) ;
	else eqworddefine ( 8939296L , p - 7825128L ) ;
      } 
    } 
    else {
	
      n = curchr ;
      getrtoken () ;
      p = curcs ;
      if ( ( a >= 4 ) ) 
      geqdefine ( p , 0 , 1114112L ) ;
      else eqdefine ( p , 0 , 1114112L ) ;
      scanoptionalequals () ;
      switch ( n ) 
      {case 0 : 
	{
	  scanusvnum () ;
	  if ( ( a >= 4 ) ) 
	  geqdefine ( p , 68 , curval ) ;
	  else eqdefine ( p , 68 , curval ) ;
	} 
	break ;
      case 1 : 
	{
	  scanfifteenbitint () ;
	  if ( ( a >= 4 ) ) 
	  geqdefine ( p , 69 , curval ) ;
	  else eqdefine ( p , 69 , curval ) ;
	} 
	break ;
      case 8 : 
	{
	  scanxetexmathcharint () ;
	  if ( ( a >= 4 ) ) 
	  geqdefine ( p , 70 , curval ) ;
	  else eqdefine ( p , 70 , curval ) ;
	} 
	break ;
      case 9 : 
	{
	  scanmathclassint () ;
	  n = setclassfield ( curval ) ;
	  scanmathfamint () ;
	  n = n + setfamilyfield ( curval ) ;
	  scanusvnum () ;
	  n = n + curval ;
	  if ( ( a >= 4 ) ) 
	  geqdefine ( p , 70 , n ) ;
	  else eqdefine ( p , 70 , n ) ;
	} 
	break ;
	default: 
	{
	  scanregisternum () ;
	  if ( curval > 255 ) 
	  {
	    j = n - 2 ;
	    if ( j > 3 ) 
	    j = 5 ;
	    findsaelement ( j , curval , true ) ;
	    incr ( mem [curptr + 1 ].hh .v.LH ) ;
	    if ( j == 5 ) 
	    j = 72 ;
	    else j = 91 ;
	    if ( ( a >= 4 ) ) 
	    geqdefine ( p , j , curptr ) ;
	    else eqdefine ( p , j , curptr ) ;
	  } 
	  else switch ( n ) 
	  {case 2 : 
	    if ( ( a >= 4 ) ) 
	    geqdefine ( p , 74 , 8939323L + curval ) ;
	    else eqdefine ( p , 74 , 8939323L + curval ) ;
	    break ;
	  case 3 : 
	    if ( ( a >= 4 ) ) 
	    geqdefine ( p , 75 , 10053714L + curval ) ;
	    else eqdefine ( p , 75 , 10053714L + curval ) ;
	    break ;
	  case 4 : 
	    if ( ( a >= 4 ) ) 
	    geqdefine ( p , 76 , 2252759L + curval ) ;
	    else eqdefine ( p , 76 , 2252759L + curval ) ;
	    break ;
	  case 5 : 
	    if ( ( a >= 4 ) ) 
	    geqdefine ( p , 77 , 2253015L + curval ) ;
	    else eqdefine ( p , 77 , 2253015L + curval ) ;
	    break ;
	  case 6 : 
	    if ( ( a >= 4 ) ) 
	    geqdefine ( p , 73 , 2253283L + curval ) ;
	    else eqdefine ( p , 73 , 2253283L + curval ) ;
	    break ;
	  } 
	} 
	break ;
      } 
    } 
    break ;
  case 98 : 
    {
      j = curchr ;
      scanint () ;
      n = curval ;
      if ( ! scankeyword ( 66240L ) ) 
      {
	{
	  if ( interaction == 3 ) 
	  ;
	  if ( filelineerrorstylep ) 
	  printfileline () ;
	  else printnl ( 65544L ) ;
	  print ( 66497L ) ;
	} 
	{
	  helpptr = 2 ;
	  helpline [1 ]= 66638L ;
	  helpline [0 ]= 66639L ;
	} 
	error () ;
      } 
      getrtoken () ;
      p = curcs ;
      readtoks ( n , p , j ) ;
      if ( ( a >= 4 ) ) 
      geqdefine ( p , 113 , curval ) ;
      else eqdefine ( p , 113 , curval ) ;
    } 
    break ;
  case 72 : 
  case 73 : 
    {
      q = curcs ;
      e = false ;
      if ( curcmd == 72 ) {
	  
	if ( curchr == membot ) 
	{
	  scanregisternum () ;
	  if ( curval > 255 ) 
	  {
	    findsaelement ( 5 , curval , true ) ;
	    curchr = curptr ;
	    e = true ;
	  } 
	  else curchr = 2253283L + curval ;
	} 
	else e = true ;
      } 
      else if ( curchr == 2253282L ) 
      {
	scancharclassnotignored () ;
	curptr = curval ;
	scancharclassnotignored () ;
	findsaelement ( 6 , curptr * 4096 + curval , true ) ;
	curchr = curptr ;
	e = true ;
      } 
      p = curchr ;
      scanoptionalequals () ;
      do {
	  getxtoken () ;
      } while ( ! ( ( curcmd != 10 ) && ( curcmd != 0 ) ) ) ;
      if ( curcmd != 1 ) {
	  
	if ( ( curcmd == 72 ) || ( curcmd == 73 ) ) 
	{
	  if ( curcmd == 72 ) {
	      
	    if ( curchr == membot ) 
	    {
	      scanregisternum () ;
	      if ( curval < 256 ) 
	      q = eqtb [2253283L + curval ].hh .v.RH ;
	      else {
		  
		findsaelement ( 5 , curval , false ) ;
		if ( curptr == -268435455L ) 
		q = -268435455L ;
		else q = mem [curptr + 1 ].hh .v.RH ;
	      } 
	    } 
	    else q = mem [curchr + 1 ].hh .v.RH ;
	  } 
	  else if ( curchr == 2253282L ) 
	  {
	    scancharclassnotignored () ;
	    curptr = curval ;
	    scancharclassnotignored () ;
	    findsaelement ( 6 , curptr * 4096 + curval , false ) ;
	    if ( curptr == -268435455L ) 
	    q = -268435455L ;
	    else q = mem [curptr + 1 ].hh .v.RH ;
	  } 
	  else q = eqtb [curchr ].hh .v.RH ;
	  if ( q == -268435455L ) {
	      
	    if ( e ) {
		
	      if ( ( a >= 4 ) ) 
	      gsadef ( p , -268435455L ) ;
	      else sadef ( p , -268435455L ) ;
	    } 
	    else if ( ( a >= 4 ) ) 
	    geqdefine ( p , 103 , -268435455L ) ;
	    else eqdefine ( p , 103 , -268435455L ) ;
	  } 
	  else {
	      
	    incr ( mem [q ].hh .v.LH ) ;
	    if ( e ) {
		
	      if ( ( a >= 4 ) ) 
	      gsadef ( p , q ) ;
	      else sadef ( p , q ) ;
	    } 
	    else if ( ( a >= 4 ) ) 
	    geqdefine ( p , 113 , q ) ;
	    else eqdefine ( p , 113 , q ) ;
	  } 
	  goto lab30 ;
	} 
      } 
      backinput () ;
      curcs = q ;
      q = scantoks ( false , false ) ;
      if ( mem [defref ].hh .v.RH == -268435455L ) 
      {
	if ( e ) {
	    
	  if ( ( a >= 4 ) ) 
	  gsadef ( p , -268435455L ) ;
	  else sadef ( p , -268435455L ) ;
	} 
	else if ( ( a >= 4 ) ) 
	geqdefine ( p , 103 , -268435455L ) ;
	else eqdefine ( p , 103 , -268435455L ) ;
	{
	  mem [defref ].hh .v.RH = avail ;
	  avail = defref ;
	;
#ifdef STAT
	  decr ( dynused ) ;
#endif /* STAT */
	} 
      } 
      else {
	  
	if ( ( p == 2253272L ) && ! e ) 
	{
	  mem [q ].hh .v.RH = getavail () ;
	  q = mem [q ].hh .v.RH ;
	  mem [q ].hh .v.LH = 4194429L ;
	  q = getavail () ;
	  mem [q ].hh .v.LH = 2097275L ;
	  mem [q ].hh .v.RH = mem [defref ].hh .v.RH ;
	  mem [defref ].hh .v.RH = q ;
	} 
	if ( e ) {
	    
	  if ( ( a >= 4 ) ) 
	  gsadef ( p , defref ) ;
	  else sadef ( p , defref ) ;
	} 
	else if ( ( a >= 4 ) ) 
	geqdefine ( p , 113 , defref ) ;
	else eqdefine ( p , 113 , defref ) ;
      } 
    } 
    break ;
  case 74 : 
    {
      p = curchr ;
      scanoptionalequals () ;
      scanint () ;
      if ( ( a >= 4 ) ) 
      geqworddefine ( p , curval ) ;
      else eqworddefine ( p , curval ) ;
    } 
    break ;
  case 75 : 
    {
      p = curchr ;
      scanoptionalequals () ;
      scandimen ( false , false , false ) ;
      if ( ( a >= 4 ) ) 
      geqworddefine ( p , curval ) ;
      else eqworddefine ( p , curval ) ;
    } 
    break ;
  case 76 : 
  case 77 : 
    {
      p = curchr ;
      n = curcmd ;
      scanoptionalequals () ;
      if ( n == 77 ) 
      scanglue ( 3 ) ;
      else scanglue ( 2 ) ;
      trapzeroglue () ;
      if ( ( a >= 4 ) ) 
      geqdefine ( p , 119 , curval ) ;
      else eqdefine ( p , 119 , curval ) ;
    } 
    break ;
  case 87 : 
    {
      if ( curchr == 5596904L ) 
      {
	p = curchr ;
	scanusvnum () ;
	p = p + curval ;
	n = eqtb [5596904L + curval ].hh .v.RH % 65536L ;
	scanoptionalequals () ;
	scancharclass () ;
	if ( ( a >= 4 ) ) 
	geqdefine ( p , 122 , curval * 65536L + n ) ;
	else eqdefine ( p , 122 , curval * 65536L + n ) ;
      } 
      else if ( curchr == 6711016L ) 
      {
	p = curchr ;
	scanusvnum () ;
	p = p + curval ;
	scanoptionalequals () ;
	scanxetexmathcharint () ;
	if ( ( a >= 4 ) ) 
	geqdefine ( p , 122 , curval ) ;
	else eqdefine ( p , 122 , curval ) ;
      } 
      else if ( curchr == 6711017L ) 
      {
	p = curchr - 1 ;
	scanusvnum () ;
	p = p + curval ;
	scanoptionalequals () ;
	scanmathclassint () ;
	n = setclassfield ( curval ) ;
	scanmathfamint () ;
	n = n + setfamilyfield ( curval ) ;
	scanusvnum () ;
	n = n + curval ;
	if ( ( a >= 4 ) ) 
	geqdefine ( p , 122 , n ) ;
	else eqdefine ( p , 122 , n ) ;
      } 
      else if ( curchr == 8939579L ) 
      {
	p = curchr ;
	scanusvnum () ;
	p = p + curval ;
	scanoptionalequals () ;
	scanint () ;
	if ( ( a >= 4 ) ) 
	geqworddefine ( p , curval ) ;
	else eqworddefine ( p , curval ) ;
      } 
      else {
	  
	p = curchr - 1 ;
	scanusvnum () ;
	p = p + curval ;
	scanoptionalequals () ;
	n = 1073741824L ;
	scanmathfamint () ;
	n = n + curval * 2097152L ;
	scanusvnum () ;
	n = n + curval ;
	if ( ( a >= 4 ) ) 
	geqworddefine ( p , n ) ;
	else eqworddefine ( p , n ) ;
      } 
    } 
    break ;
  case 86 : 
    {
      if ( curchr == 2254568L ) 
      n = 15 ;
      else if ( curchr == 6711016L ) 
      n = 32768L ;
      else if ( curchr == 5596904L ) 
      n = 32767 ;
      else if ( curchr == 8939579L ) 
      n = 16777215L ;
      else n = 1114111L ;
      p = curchr ;
      scanusvnum () ;
      p = p + curval ;
      scanoptionalequals () ;
      scanint () ;
      if ( ( ( curval < 0 ) && ( p < 8939579L ) ) || ( curval > n ) ) 
      {
	{
	  if ( interaction == 3 ) 
	  ;
	  if ( filelineerrorstylep ) 
	  printfileline () ;
	  else printnl ( 65544L ) ;
	  print ( 66121L ) ;
	} 
	printint ( curval ) ;
	if ( p < 8939579L ) 
	print ( 66649L ) ;
	else print ( 66650L ) ;
	printint ( n ) ;
	{
	  helpptr = 1 ;
	  helpline [0 ]= 66651L ;
	} 
	error () ;
	curval = 0 ;
      } 
      if ( p < 6711016L ) 
      {
	if ( p >= 5596904L ) 
	{
	  n = eqtb [p ].hh .v.RH / 65536L ;
	  if ( ( a >= 4 ) ) 
	  geqdefine ( p , 122 , n * 65536L + curval ) ;
	  else eqdefine ( p , 122 , n * 65536L + curval ) ;
	} 
	else if ( ( a >= 4 ) ) 
	geqdefine ( p , 122 , curval ) ;
	else eqdefine ( p , 122 , curval ) ;
      } 
      else if ( p < 8939579L ) 
      {
	if ( curval == 32768L ) 
	curval = 2097151L ;
	else curval = setclassfield ( curval / 4096 ) + setfamilyfield ( ( 
	curval % 4096 ) / 256 ) + ( curval % 256 ) ;
	if ( ( a >= 4 ) ) 
	geqdefine ( p , 122 , curval ) ;
	else eqdefine ( p , 122 , curval ) ;
      } 
      else if ( ( a >= 4 ) ) 
      geqworddefine ( p , curval ) ;
      else eqworddefine ( p , curval ) ;
    } 
    break ;
  case 88 : 
    {
      p = curchr ;
      scanmathfamint () ;
      p = p + curval ;
      scanoptionalequals () ;
      scanfontident () ;
      if ( ( a >= 4 ) ) 
      geqdefine ( p , 122 , curval ) ;
      else eqdefine ( p , 122 , curval ) ;
    } 
    break ;
  case 91 : 
  case 92 : 
  case 93 : 
  case 94 : 
    doregistercommand ( a ) ;
    break ;
  case 100 : 
    {
      scanregisternum () ;
      if ( ( a >= 4 ) ) 
      n = 1073774592L + curval ;
      else n = 1073741824L + curval ;
      scanoptionalequals () ;
      if ( setboxallowed ) 
      scanbox ( n ) ;
      else {
	  
	{
	  if ( interaction == 3 ) 
	  ;
	  if ( filelineerrorstylep ) 
	  printfileline () ;
	  else printnl ( 65544L ) ;
	  print ( 66023L ) ;
	} 
	printesc ( 65856L ) ;
	{
	  helpptr = 2 ;
	  helpline [1 ]= 66657L ;
	  helpline [0 ]= 66658L ;
	} 
	error () ;
      } 
    } 
    break ;
  case 80 : 
    alteraux () ;
    break ;
  case 81 : 
    alterprevgraf () ;
    break ;
  case 82 : 
    alterpagesofar () ;
    break ;
  case 83 : 
    alterinteger () ;
    break ;
  case 84 : 
    alterboxdimen () ;
    break ;
  case 85 : 
    {
      q = curchr ;
      scanoptionalequals () ;
      scanint () ;
      n = curval ;
      if ( n <= 0 ) 
      p = -268435455L ;
      else if ( q > 2253271L ) 
      {
	n = ( curval / 2 ) + 1 ;
	p = getnode ( 2 * n + 1 ) ;
	mem [p ].hh .v.LH = n ;
	n = curval ;
	mem [p + 1 ].cint = n ;
	{register integer for_end; j = p + 2 ;for_end = p + n + 1 ; if ( j 
	<= for_end) do 
	  {
	    scanint () ;
	    mem [j ].cint = curval ;
	  } 
	while ( j++ < for_end ) ;} 
	if ( ! odd ( n ) ) 
	mem [p + n + 2 ].cint = 0 ;
      } 
      else {
	  
	p = getnode ( 2 * n + 1 ) ;
	mem [p ].hh .v.LH = n ;
	{register integer for_end; j = 1 ;for_end = n ; if ( j <= for_end) 
	do 
	  {
	    scandimen ( false , false , false ) ;
	    mem [p + 2 * j - 1 ].cint = curval ;
	    scandimen ( false , false , false ) ;
	    mem [p + 2 * j ].cint = curval ;
	  } 
	while ( j++ < for_end ) ;} 
      } 
      if ( ( a >= 4 ) ) 
      geqdefine ( q , 120 , p ) ;
      else eqdefine ( q , 120 , p ) ;
    } 
    break ;
  case 101 : 
    if ( curchr == 1 ) 
    {
	;
#ifdef INITEX
      if ( iniversion ) 
      {
	newpatterns () ;
	goto lab30 ;
      } 
#endif /* INITEX */
      {
	if ( interaction == 3 ) 
	;
	if ( filelineerrorstylep ) 
	printfileline () ;
	else printnl ( 65544L ) ;
	print ( 66662L ) ;
      } 
      helpptr = 0 ;
      error () ;
      do {
	  gettoken () ;
      } while ( ! ( curcmd == 2 ) ) ;
      return ;
    } 
    else {
	
      newhyphexceptions () ;
      goto lab30 ;
    } 
    break ;
  case 78 : 
    {
      findfontdimen ( true ) ;
      k = curval ;
      scanoptionalequals () ;
      scandimen ( false , false , false ) ;
      fontinfo [k ].cint = curval ;
    } 
    break ;
  case 79 : 
    {
      n = curchr ;
      scanfontident () ;
      f = curval ;
      if ( n < 2 ) 
      {
	scanoptionalequals () ;
	scanint () ;
	if ( n == 0 ) 
	hyphenchar [f ]= curval ;
	else skewchar [f ]= curval ;
      } 
      else {
	  
	if ( ( ( fontarea [f ]== 65535L ) || ( fontarea [f ]== 65534L ) ) 
	) 
	scanglyphnumber ( f ) ;
	else scancharnum () ;
	p = curval ;
	scanoptionalequals () ;
	scanint () ;
	switch ( n ) 
	{case 2 : 
	  setcpcode ( f , p , 0 , curval ) ;
	  break ;
	case 3 : 
	  setcpcode ( f , p , 1 , curval ) ;
	  break ;
	} 
      } 
    } 
    break ;
  case 90 : 
    newfont ( a ) ;
    break ;
  case 102 : 
    newinteraction () ;
    break ;
    default: 
    confusion ( 66606L ) ;
    break ;
  } 
  lab30: if ( aftertoken != 0 ) 
  {
    curtok = aftertoken ;
    backinput () ;
    aftertoken = 0 ;
  } 
} 
#ifdef INITEX
void 
storefmtfile ( void ) 
{
  /* 41 42 31 32 */ storefmtfile_regmem 
  integer j, k, l  ;
  halfword p, q  ;
  integer x  ;
  char * formatengine  ;
  if ( saveptr != 0 ) 
  {
    {
      if ( interaction == 3 ) 
      ;
      if ( filelineerrorstylep ) 
      printfileline () ;
      else printnl ( 65544L ) ;
      print ( 66704L ) ;
    } 
    {
      helpptr = 1 ;
      helpline [0 ]= 66705L ;
    } 
    {
      if ( interaction == 3 ) 
      interaction = 2 ;
      if ( logopened ) 
      error () ;
	;
#ifdef TEXMF_DEBUG
      if ( interaction > 0 ) 
      debughelp () ;
#endif /* TEXMF_DEBUG */
      history = 3 ;
      jumpout () ;
    } 
  } 
  selector = 21 ;
  print ( 66725L ) ;
  print ( jobname ) ;
  printchar ( 32 ) ;
  printint ( eqtb [8939263L ].cint ) ;
  printchar ( 46 ) ;
  printint ( eqtb [8939262L ].cint ) ;
  printchar ( 46 ) ;
  printint ( eqtb [8939261L ].cint ) ;
  printchar ( 41 ) ;
  if ( interaction == 0 ) 
  selector = 18 ;
  else selector = 19 ;
  {
    if ( poolptr + 1 > poolsize ) 
    overflow ( 65539L , poolsize - initpoolptr ) ;
  } 
  formatident = makestring () ;
  packjobname ( 66168L ) ;
  while ( ! wopenout ( fmtfile ) ) promptfilename ( 66726L , 66168L ) ;
  printnl ( 66727L ) ;
  print ( wmakenamestring ( fmtfile ) ) ;
  {
    decr ( strptr ) ;
    poolptr = strstart [( strptr ) - 65536L ];
  } 
  printnl ( 65626L ) ;
  print ( formatident ) ;
  dumpint ( 1462916184L ) ;
  x = strlen ( enginename ) ;
  formatengine = xmallocarray ( char , x + 4 ) ;
  strcpy ( stringcast ( formatengine ) , enginename ) ;
  {register integer for_end; k = x ;for_end = x + 3 ; if ( k <= for_end) do 
    formatengine [k ]= 0 ;
  while ( k++ < for_end ) ;} 
  x = x + 4 - ( x % 4 ) ;
  dumpint ( x ) ;
  dumpthings ( formatengine [0 ], x ) ;
  libcfree ( formatengine ) ;
  dumpint ( 71683443L ) ;
  dumpint ( 1073741823L ) ;
  dumpint ( hashhigh ) ;
  dumpint ( eTeXmode ) ;
  while ( pseudofiles != -268435455L ) pseudoclose () ;
  dumpint ( membot ) ;
  dumpint ( memtop ) ;
  dumpint ( 10053969L ) ;
  dumpint ( 8501 ) ;
  dumpint ( 607 ) ;
  dumpint ( 1296847960L ) ;
  if ( mltexp ) 
  dumpint ( 1 ) ;
  else dumpint ( 0 ) ;
  dumpint ( poolptr ) ;
  dumpint ( strptr ) ;
  dumpthings ( strstart [( 65536L ) - 65536L ], strptr - 65535L ) ;
  dumpthings ( strpool [0 ], poolptr ) ;
  println () ;
  printint ( strptr ) ;
  print ( 66706L ) ;
  printint ( poolptr ) ;
  sortavail () ;
  varused = 0 ;
  dumpint ( lomemmax ) ;
  dumpint ( rover ) ;
  if ( ( eTeXmode == 1 ) ) 
  {register integer for_end; k = 0 ;for_end = 6 ; if ( k <= for_end) do 
    dumpint ( saroot [k ]) ;
  while ( k++ < for_end ) ;} 
  p = membot ;
  q = rover ;
  x = 0 ;
  do {
      dumpthings ( mem [p ], q + 2 - p ) ;
    x = x + q + 2 - p ;
    varused = varused + q - p ;
    p = q + mem [q ].hh .v.LH ;
    q = mem [q + 1 ].hh .v.RH ;
  } while ( ! ( q == rover ) ) ;
  varused = varused + lomemmax - p ;
  dynused = memend + 1 - himemmin ;
  dumpthings ( mem [p ], lomemmax + 1 - p ) ;
  x = x + lomemmax + 1 - p ;
  dumpint ( himemmin ) ;
  dumpint ( avail ) ;
  dumpthings ( mem [himemmin ], memend + 1 - himemmin ) ;
  x = x + memend + 1 - himemmin ;
  p = avail ;
  while ( p != -268435455L ) {
      
    decr ( dynused ) ;
    p = mem [p ].hh .v.RH ;
  } 
  dumpint ( varused ) ;
  dumpint ( dynused ) ;
  println () ;
  printint ( x ) ;
  print ( 66707L ) ;
  printint ( varused ) ;
  printchar ( 38 ) ;
  printint ( dynused ) ;
  k = 1 ;
  do {
      j = k ;
    while ( j < 8939239L ) {
	
      if ( ( eqtb [j ].hh .v.RH == eqtb [j + 1 ].hh .v.RH ) && ( eqtb [j 
      ].hh.b0 == eqtb [j + 1 ].hh.b0 ) && ( eqtb [j ].hh.b1 == eqtb [j + 
      1 ].hh.b1 ) ) 
      goto lab41 ;
      incr ( j ) ;
    } 
    l = 8939240L ;
    goto lab31 ;
    lab41: incr ( j ) ;
    l = j ;
    while ( j < 8939239L ) {
	
      if ( ( eqtb [j ].hh .v.RH != eqtb [j + 1 ].hh .v.RH ) || ( eqtb [j 
      ].hh.b0 != eqtb [j + 1 ].hh.b0 ) || ( eqtb [j ].hh.b1 != eqtb [j + 
      1 ].hh.b1 ) ) 
      goto lab31 ;
      incr ( j ) ;
    } 
    lab31: dumpint ( l - k ) ;
    dumpthings ( eqtb [k ], l - k ) ;
    k = j + 1 ;
    dumpint ( k - l ) ;
  } while ( ! ( k == 8939240L ) ) ;
  do {
      j = k ;
    while ( j < 10053969L ) {
	
      if ( eqtb [j ].cint == eqtb [j + 1 ].cint ) 
      goto lab42 ;
      incr ( j ) ;
    } 
    l = 10053970L ;
    goto lab32 ;
    lab42: incr ( j ) ;
    l = j ;
    while ( j < 10053969L ) {
	
      if ( eqtb [j ].cint != eqtb [j + 1 ].cint ) 
      goto lab32 ;
      incr ( j ) ;
    } 
    lab32: dumpint ( l - k ) ;
    dumpthings ( eqtb [k ], l - k ) ;
    k = j + 1 ;
    dumpint ( k - l ) ;
  } while ( ! ( k > 10053969L ) ) ;
  if ( hashhigh > 0 ) 
  dumpthings ( eqtb [10053970L ], hashhigh ) ;
  dumpint ( parloc ) ;
  dumpint ( writeloc ) ;
  {register integer for_end; p = 0 ;for_end = 500 ; if ( p <= for_end) do 
    dumphh ( prim [p ]) ;
  while ( p++ < for_end ) ;} 
  dumpint ( hashused ) ;
  cscount = 2243225L - hashused + hashhigh ;
  {register integer for_end; p = 2228226L ;for_end = hashused ; if ( p <= 
  for_end) do 
    if ( hash [p ].v.RH != 0 ) 
    {
      dumpint ( p ) ;
      dumphh ( hash [p ]) ;
      incr ( cscount ) ;
    } 
  while ( p++ < for_end ) ;} 
  dumpthings ( hash [hashused + 1 ], 2252738L - hashused ) ;
  if ( hashhigh > 0 ) 
  dumpthings ( hash [10053970L ], hashhigh ) ;
  dumpint ( cscount ) ;
  println () ;
  printint ( cscount ) ;
  print ( 66708L ) ;
  dumpint ( fmemptr ) ;
  dumpthings ( fontinfo [0 ], fmemptr ) ;
  dumpint ( fontptr ) ;
  {
    dumpthings ( fontcheck [0 ], fontptr + 1 ) ;
    dumpthings ( fontsize [0 ], fontptr + 1 ) ;
    dumpthings ( fontdsize [0 ], fontptr + 1 ) ;
    dumpthings ( fontparams [0 ], fontptr + 1 ) ;
    dumpthings ( hyphenchar [0 ], fontptr + 1 ) ;
    dumpthings ( skewchar [0 ], fontptr + 1 ) ;
    dumpthings ( fontname [0 ], fontptr + 1 ) ;
    dumpthings ( fontarea [0 ], fontptr + 1 ) ;
    dumpthings ( fontbc [0 ], fontptr + 1 ) ;
    dumpthings ( fontec [0 ], fontptr + 1 ) ;
    dumpthings ( charbase [0 ], fontptr + 1 ) ;
    dumpthings ( widthbase [0 ], fontptr + 1 ) ;
    dumpthings ( heightbase [0 ], fontptr + 1 ) ;
    dumpthings ( depthbase [0 ], fontptr + 1 ) ;
    dumpthings ( italicbase [0 ], fontptr + 1 ) ;
    dumpthings ( ligkernbase [0 ], fontptr + 1 ) ;
    dumpthings ( kernbase [0 ], fontptr + 1 ) ;
    dumpthings ( extenbase [0 ], fontptr + 1 ) ;
    dumpthings ( parambase [0 ], fontptr + 1 ) ;
    dumpthings ( fontglue [0 ], fontptr + 1 ) ;
    dumpthings ( bcharlabel [0 ], fontptr + 1 ) ;
    dumpthings ( fontbchar [0 ], fontptr + 1 ) ;
    dumpthings ( fontfalsebchar [0 ], fontptr + 1 ) ;
    {register integer for_end; k = 0 ;for_end = fontptr ; if ( k <= for_end) 
    do 
      {
	printnl ( 66712L ) ;
	printesc ( hash [2243738L + k ].v.RH ) ;
	printchar ( 61 ) ;
	if ( ( ( fontarea [k ]== 65535L ) || ( fontarea [k ]== 65534L ) ) 
	|| ( fontmapping [k ]!= 0 ) ) 
	{
	  printfilename ( fontname [k ], 65626L , 65626L ) ;
	  {
	    if ( interaction == 3 ) 
	    ;
	    if ( filelineerrorstylep ) 
	    printfileline () ;
	    else printnl ( 65544L ) ;
	    print ( 66713L ) ;
	  } 
	  {
	    helpptr = 3 ;
	    helpline [2 ]= 66714L ;
	    helpline [1 ]= 66715L ;
	    helpline [0 ]= 66716L ;
	  } 
	  error () ;
	} 
	else printfilename ( fontname [k ], fontarea [k ], 65626L ) ;
	if ( fontsize [k ]!= fontdsize [k ]) 
	{
	  print ( 66124L ) ;
	  printscaled ( fontsize [k ]) ;
	  print ( 65697L ) ;
	} 
      } 
    while ( k++ < for_end ) ;} 
  } 
  println () ;
  printint ( fmemptr - 7 ) ;
  print ( 66709L ) ;
  printint ( fontptr - 0 ) ;
  if ( fontptr != 1 ) 
  print ( 66710L ) ;
  else print ( 66711L ) ;
  dumpint ( hyphcount ) ;
  if ( hyphnext <= 607 ) 
  hyphnext = hyphsize ;
  dumpint ( hyphnext ) ;
  {register integer for_end; k = 0 ;for_end = hyphsize ; if ( k <= for_end) 
  do 
    if ( hyphword [k ]!= 0 ) 
    {
      dumpint ( k + 65536L * hyphlink [k ]) ;
      dumpint ( hyphword [k ]) ;
      dumpint ( hyphlist [k ]) ;
    } 
  while ( k++ < for_end ) ;} 
  println () ;
  printint ( hyphcount ) ;
  if ( hyphcount != 1 ) 
  print ( 66717L ) ;
  else print ( 66718L ) ;
  if ( trienotready ) 
  inittrie () ;
  dumpint ( triemax ) ;
  dumpint ( hyphstart ) ;
  dumpthings ( trietrl [0 ], triemax + 1 ) ;
  dumpthings ( trietro [0 ], triemax + 1 ) ;
  dumpthings ( trietrc [0 ], triemax + 1 ) ;
  dumpint ( maxhyphchar ) ;
  dumpint ( trieopptr ) ;
  dumpthings ( hyfdistance [1 ], trieopptr ) ;
  dumpthings ( hyfnum [1 ], trieopptr ) ;
  dumpthings ( hyfnext [1 ], trieopptr ) ;
  printnl ( 66719L ) ;
  printint ( triemax ) ;
  print ( 66720L ) ;
  printint ( trieopptr ) ;
  if ( trieopptr != 1 ) 
  print ( 66721L ) ;
  else print ( 66722L ) ;
  print ( 66723L ) ;
  printint ( trieopsize ) ;
  {register integer for_end; k = 255 ;for_end = 0 ; if ( k >= for_end) do 
    if ( trieused [k ]> 0 ) 
    {
      printnl ( 66185L ) ;
      printint ( trieused [k ]) ;
      print ( 66724L ) ;
      printint ( k ) ;
      dumpint ( k ) ;
      dumpint ( trieused [k ]) ;
    } 
  while ( k-- > for_end ) ;} 
  dumpint ( interaction ) ;
  dumpint ( formatident ) ;
  dumpint ( 69069L ) ;
  eqtb [8939271L ].cint = 0 ;
  wclose ( fmtfile ) ;
} 
#endif /* INITEX */
boolean 
loadfmtfile ( void ) 
{
  /* 6666 10 */ register boolean Result; loadfmtfile_regmem 
  integer j, k  ;
  halfword p, q  ;
  integer x  ;
  char * formatengine  ;
	;
#ifdef INITEX
  if ( iniversion ) 
  {
    libcfree ( fontinfo ) ;
    libcfree ( strpool ) ;
    libcfree ( strstart ) ;
    libcfree ( yhash ) ;
    libcfree ( zeqtb ) ;
    libcfree ( yzmem ) ;
  } 
#endif /* INITEX */
  undumpint ( x ) ;
  if ( debugformatfile ) 
  {
    fprintf ( stderr , "%s%s",  "fmtdebug:" , "format magic number" ) ;
    fprintf ( stderr , "%s%ld\n",  " = " , (long)x ) ;
  } 
  if ( x != 1462916184L ) 
  goto lab6666 ;
  undumpint ( x ) ;
  if ( debugformatfile ) 
  {
    fprintf ( stderr , "%s%s",  "fmtdebug:" , "engine name size" ) ;
    fprintf ( stderr , "%s%ld\n",  " = " , (long)x ) ;
  } 
  if ( ( x < 0 ) || ( x > 256 ) ) 
  goto lab6666 ;
  formatengine = xmallocarray ( char , x ) ;
  undumpthings ( formatengine [0 ], x ) ;
  formatengine [x - 1 ]= 0 ;
  if ( strcmp ( enginename , stringcast ( formatengine ) ) ) 
  {
    ;
    fprintf ( stdout , "%s%s%s%s\n",  "---! " , stringcast ( nameoffile + 1 ) ,     " was written by " , formatengine ) ;
    libcfree ( formatengine ) ;
    goto lab6666 ;
  } 
  libcfree ( formatengine ) ;
  undumpint ( x ) ;
  if ( debugformatfile ) 
  {
    fprintf ( stderr , "%s%s",  "fmtdebug:" , "string pool checksum" ) ;
    fprintf ( stderr , "%s%ld\n",  " = " , (long)x ) ;
  } 
  if ( x != 71683443L ) 
  {
    ;
    fprintf ( stdout , "%s%s%s%s\n",  "---! " , stringcast ( nameoffile + 1 ) ,     " doesn't match " , poolname ) ;
    goto lab6666 ;
  } 
  undumpint ( x ) ;
  if ( x != 1073741823L ) 
  goto lab6666 ;
  undumpint ( hashhigh ) ;
  if ( ( hashhigh < 0 ) || ( hashhigh > suphashextra ) ) 
  goto lab6666 ;
  if ( hashextra < hashhigh ) 
  hashextra = hashhigh ;
  eqtbtop = 10053969L + hashextra ;
  if ( hashextra == 0 ) 
  hashtop = 2252739L ;
  else hashtop = eqtbtop ;
  yhash = xmallocarray ( twohalves , 1 + hashtop - hashoffset ) ;
  hash = yhash - hashoffset ;
  hash [2228226L ].v.LH = 0 ;
  hash [2228226L ].v.RH = 0 ;
  {register integer for_end; x = 2228227L ;for_end = hashtop ; if ( x <= 
  for_end) do 
    hash [x ]= hash [2228226L ];
  while ( x++ < for_end ) ;} 
  zeqtb = xmallocarray ( memoryword , eqtbtop + 1 ) ;
  eqtb = zeqtb ;
  eqtb [2252739L ].hh.b0 = 103 ;
  eqtb [2252739L ].hh .v.RH = -268435455L ;
  eqtb [2252739L ].hh.b1 = 0 ;
  {register integer for_end; x = 10053970L ;for_end = eqtbtop ; if ( x <= 
  for_end) do 
    eqtb [x ]= eqtb [2252739L ];
  while ( x++ < for_end ) ;} 
  {
    undumpint ( x ) ;
    if ( ( x < 0 ) || ( x > 1 ) ) 
    goto lab6666 ;
    else eTeXmode = x ;
  } 
  if ( ( eTeXmode == 1 ) ) 
  {
    maxregnum = 32767 ;
    maxreghelpline = 66954L ;
  } 
  else {
      
    maxregnum = 255 ;
    maxreghelpline = 66953L ;
  } 
  undumpint ( x ) ;
  if ( debugformatfile ) 
  {
    fprintf ( stderr , "%s%s",  "fmtdebug:" , "mem_bot" ) ;
    fprintf ( stderr , "%s%ld\n",  " = " , (long)x ) ;
  } 
  if ( x != membot ) 
  goto lab6666 ;
  undumpint ( memtop ) ;
  if ( debugformatfile ) 
  {
    fprintf ( stderr , "%s%s",  "fmtdebug:" , "mem_top" ) ;
    fprintf ( stderr , "%s%ld\n",  " = " , (long)memtop ) ;
  } 
  if ( membot + 1100 > memtop ) 
  goto lab6666 ;
  curlist .headfield = memtop - 1 ;
  curlist .tailfield = memtop - 1 ;
  pagetail = memtop - 2 ;
  memmin = membot - extramembot ;
  memmax = memtop + extramemtop ;
  yzmem = xmallocarray ( memoryword , memmax - memmin + 1 ) ;
  zmem = yzmem - memmin ;
  mem = zmem ;
  undumpint ( x ) ;
  if ( x != 10053969L ) 
  goto lab6666 ;
  undumpint ( x ) ;
  if ( x != 8501 ) 
  goto lab6666 ;
  undumpint ( x ) ;
  if ( x != 607 ) 
  goto lab6666 ;
  undumpint ( x ) ;
  if ( x != 1296847960L ) 
  goto lab6666 ;
  undumpint ( x ) ;
  if ( x == 1 ) 
  mltexenabledp = true ;
  else if ( x != 0 ) 
  goto lab6666 ;
  {
    undumpint ( x ) ;
    if ( x < 0 ) 
    goto lab6666 ;
    if ( x > suppoolsize - poolfree ) 
    {
      ;
      fprintf ( stdout , "%s%s\n",  "---! Must increase the " , "string pool size" ) ;
      goto lab6666 ;
    } 
    else if ( debugformatfile ) 
    {
      fprintf ( stderr , "%s%s",  "fmtdebug:" , "string pool size" ) ;
      fprintf ( stderr , "%s%ld\n",  " = " , (long)x ) ;
    } 
    poolptr = x ;
  } 
  if ( poolsize < poolptr + poolfree ) 
  poolsize = poolptr + poolfree ;
  {
    undumpint ( x ) ;
    if ( x < 0 ) 
    goto lab6666 ;
    if ( x > supmaxstrings - stringsfree ) 
    {
      ;
      fprintf ( stdout , "%s%s\n",  "---! Must increase the " , "sup strings" ) ;
      goto lab6666 ;
    } 
    else if ( debugformatfile ) 
    {
      fprintf ( stderr , "%s%s",  "fmtdebug:" , "sup strings" ) ;
      fprintf ( stderr , "%s%ld\n",  " = " , (long)x ) ;
    } 
    strptr = x ;
  } 
  if ( maxstrings < strptr + stringsfree ) 
  maxstrings = strptr + stringsfree ;
  strstart = xmallocarray ( poolpointer , maxstrings ) ;
  undumpcheckedthings ( 0 , poolptr , strstart [( 65536L ) - 65536L ], 
  strptr - 65535L ) ;
  strpool = xmallocarray ( packedUTF16code , poolsize ) ;
  undumpthings ( strpool [0 ], poolptr ) ;
  initstrptr = strptr ;
  initpoolptr = poolptr ;
  {
    undumpint ( x ) ;
    if ( ( x < membot + 1019 ) || ( x > memtop - 15 ) ) 
    goto lab6666 ;
    else lomemmax = x ;
  } 
  {
    undumpint ( x ) ;
    if ( ( x < membot + 20 ) || ( x > lomemmax ) ) 
    goto lab6666 ;
    else rover = x ;
  } 
  if ( ( eTeXmode == 1 ) ) 
  {register integer for_end; k = 0 ;for_end = 6 ; if ( k <= for_end) do 
    {
      undumpint ( x ) ;
      if ( ( x < -268435455L ) || ( x > lomemmax ) ) 
      goto lab6666 ;
      else saroot [k ]= x ;
    } 
  while ( k++ < for_end ) ;} 
  p = membot ;
  q = rover ;
  do {
      undumpthings ( mem [p ], q + 2 - p ) ;
    p = q + mem [q ].hh .v.LH ;
    if ( ( p > lomemmax ) || ( ( q >= mem [q + 1 ].hh .v.RH ) && ( mem [q + 
    1 ].hh .v.RH != rover ) ) ) 
    goto lab6666 ;
    q = mem [q + 1 ].hh .v.RH ;
  } while ( ! ( q == rover ) ) ;
  undumpthings ( mem [p ], lomemmax + 1 - p ) ;
  if ( memmin < membot - 2 ) 
  {
    p = mem [rover + 1 ].hh .v.LH ;
    q = memmin + 1 ;
    mem [memmin ].hh .v.RH = -268435455L ;
    mem [memmin ].hh .v.LH = -268435455L ;
    mem [p + 1 ].hh .v.RH = q ;
    mem [rover + 1 ].hh .v.LH = q ;
    mem [q + 1 ].hh .v.RH = rover ;
    mem [q + 1 ].hh .v.LH = p ;
    mem [q ].hh .v.RH = 1073741823L ;
    mem [q ].hh .v.LH = membot - q ;
  } 
  {
    undumpint ( x ) ;
    if ( ( x < lomemmax + 1 ) || ( x > memtop - 14 ) ) 
    goto lab6666 ;
    else himemmin = x ;
  } 
  {
    undumpint ( x ) ;
    if ( ( x < -268435455L ) || ( x > memtop ) ) 
    goto lab6666 ;
    else avail = x ;
  } 
  memend = memtop ;
  undumpthings ( mem [himemmin ], memend + 1 - himemmin ) ;
  undumpint ( varused ) ;
  undumpint ( dynused ) ;
  k = 1 ;
  do {
      undumpint ( x ) ;
    if ( ( x < 1 ) || ( k + x > 10053970L ) ) 
    goto lab6666 ;
    undumpthings ( eqtb [k ], x ) ;
    k = k + x ;
    undumpint ( x ) ;
    if ( ( x < 0 ) || ( k + x > 10053970L ) ) 
    goto lab6666 ;
    {register integer for_end; j = k ;for_end = k + x - 1 ; if ( j <= 
    for_end) do 
      eqtb [j ]= eqtb [k - 1 ];
    while ( j++ < for_end ) ;} 
    k = k + x ;
  } while ( ! ( k > 10053969L ) ) ;
  if ( hashhigh > 0 ) 
  undumpthings ( eqtb [10053970L ], hashhigh ) ;
  {
    undumpint ( x ) ;
    if ( ( x < 2228226L ) || ( x > hashtop ) ) 
    goto lab6666 ;
    else parloc = x ;
  } 
  partoken = 33554431L + parloc ;
  {
    undumpint ( x ) ;
    if ( ( x < 2228226L ) || ( x > hashtop ) ) 
    goto lab6666 ;
    else
    writeloc = x ;
  } 
  {register integer for_end; p = 0 ;for_end = 500 ; if ( p <= for_end) do 
    undumphh ( prim [p ]) ;
  while ( p++ < for_end ) ;} 
  {
    undumpint ( x ) ;
    if ( ( x < 2228226L ) || ( x > 2243226L ) ) 
    goto lab6666 ;
    else hashused = x ;
  } 
  p = 2228225L ;
  do {
      { 
      undumpint ( x ) ;
      if ( ( x < p + 1 ) || ( x > hashused ) ) 
      goto lab6666 ;
      else p = x ;
    } 
    undumphh ( hash [p ]) ;
  } while ( ! ( p == hashused ) ) ;
  undumpthings ( hash [hashused + 1 ], 2252738L - hashused ) ;
  if ( debugformatfile ) 
  {
    printcsnames ( 2228226L , 2252738L ) ;
  } 
  if ( hashhigh > 0 ) 
  {
    undumpthings ( hash [10053970L ], hashhigh ) ;
    if ( debugformatfile ) 
    {
      printcsnames ( 10053970L , hashhigh - ( 10053970L ) ) ;
    } 
  } 
  undumpint ( cscount ) ;
  {
    undumpint ( x ) ;
    if ( x < 7 ) 
    goto lab6666 ;
    if ( x > supfontmemsize ) 
    {
      ;
      fprintf ( stdout , "%s%s\n",  "---! Must increase the " , "font mem size" ) ;
      goto lab6666 ;
    } 
    else if ( debugformatfile ) 
    {
      fprintf ( stderr , "%s%s",  "fmtdebug:" , "font mem size" ) ;
      fprintf ( stderr , "%s%ld\n",  " = " , (long)x ) ;
    } 
    fmemptr = x ;
  } 
  if ( fmemptr > fontmemsize ) 
  fontmemsize = fmemptr ;
  fontinfo = xmallocarray ( fmemoryword , fontmemsize ) ;
  undumpthings ( fontinfo [0 ], fmemptr ) ;
  {
    undumpint ( x ) ;
    if ( x < 0 ) 
    goto lab6666 ;
    if ( x > 9000 ) 
    {
      ;
      fprintf ( stdout , "%s%s\n",  "---! Must increase the " , "font max" ) ;
      goto lab6666 ;
    } 
    else if ( debugformatfile ) 
    {
      fprintf ( stderr , "%s%s",  "fmtdebug:" , "font max" ) ;
      fprintf ( stderr , "%s%ld\n",  " = " , (long)x ) ;
    } 
    fontptr = x ;
  } 
  {
    fontmapping = xmallocarray ( voidpointer , fontmax ) ;
    fontlayoutengine = xmallocarray ( voidpointer , fontmax ) ;
    fontflags = xmallocarray ( char , fontmax ) ;
    fontletterspace = xmallocarray ( scaled , fontmax ) ;
    fontcheck = xmallocarray ( fourquarters , fontmax ) ;
    fontsize = xmallocarray ( scaled , fontmax ) ;
    fontdsize = xmallocarray ( scaled , fontmax ) ;
    fontparams = xmallocarray ( fontindex , fontmax ) ;
    fontname = xmallocarray ( strnumber , fontmax ) ;
    fontarea = xmallocarray ( strnumber , fontmax ) ;
    fontbc = xmallocarray ( UTF16code , fontmax ) ;
    fontec = xmallocarray ( UTF16code , fontmax ) ;
    fontglue = xmallocarray ( halfword , fontmax ) ;
    hyphenchar = xmallocarray ( integer , fontmax ) ;
    skewchar = xmallocarray ( integer , fontmax ) ;
    bcharlabel = xmallocarray ( fontindex , fontmax ) ;
    fontbchar = xmallocarray ( ninebits , fontmax ) ;
    fontfalsebchar = xmallocarray ( ninebits , fontmax ) ;
    charbase = xmallocarray ( integer , fontmax ) ;
    widthbase = xmallocarray ( integer , fontmax ) ;
    heightbase = xmallocarray ( integer , fontmax ) ;
    depthbase = xmallocarray ( integer , fontmax ) ;
    italicbase = xmallocarray ( integer , fontmax ) ;
    ligkernbase = xmallocarray ( integer , fontmax ) ;
    kernbase = xmallocarray ( integer , fontmax ) ;
    extenbase = xmallocarray ( integer , fontmax ) ;
    parambase = xmallocarray ( integer , fontmax ) ;
    {register integer for_end; k = 0 ;for_end = fontptr ; if ( k <= for_end) 
    do 
      fontmapping [k ]= 0 ;
    while ( k++ < for_end ) ;} 
    undumpthings ( fontcheck [0 ], fontptr + 1 ) ;
    undumpthings ( fontsize [0 ], fontptr + 1 ) ;
    undumpthings ( fontdsize [0 ], fontptr + 1 ) ;
    undumpcheckedthings ( -268435455L , 1073741823L , fontparams [0 ], 
    fontptr + 1 ) ;
    undumpthings ( hyphenchar [0 ], fontptr + 1 ) ;
    undumpthings ( skewchar [0 ], fontptr + 1 ) ;
    undumpuppercheckthings ( strptr , fontname [0 ], fontptr + 1 ) ;
    undumpuppercheckthings ( strptr , fontarea [0 ], fontptr + 1 ) ;
    undumpthings ( fontbc [0 ], fontptr + 1 ) ;
    undumpthings ( fontec [0 ], fontptr + 1 ) ;
    undumpthings ( charbase [0 ], fontptr + 1 ) ;
    undumpthings ( widthbase [0 ], fontptr + 1 ) ;
    undumpthings ( heightbase [0 ], fontptr + 1 ) ;
    undumpthings ( depthbase [0 ], fontptr + 1 ) ;
    undumpthings ( italicbase [0 ], fontptr + 1 ) ;
    undumpthings ( ligkernbase [0 ], fontptr + 1 ) ;
    undumpthings ( kernbase [0 ], fontptr + 1 ) ;
    undumpthings ( extenbase [0 ], fontptr + 1 ) ;
    undumpthings ( parambase [0 ], fontptr + 1 ) ;
    undumpcheckedthings ( -268435455L , lomemmax , fontglue [0 ], fontptr + 
    1 ) ;
    undumpcheckedthings ( 0 , fmemptr - 1 , bcharlabel [0 ], fontptr + 1 ) ;
    undumpcheckedthings ( 0 , 65536L , fontbchar [0 ], fontptr + 1 ) ;
    undumpcheckedthings ( 0 , 65536L , fontfalsebchar [0 ], fontptr + 1 ) ;
  } 
  {
    undumpint ( x ) ;
    if ( x < 0 ) 
    goto lab6666 ;
    if ( x > hyphsize ) 
    {
      ;
      fprintf ( stdout , "%s%s\n",  "---! Must increase the " , "hyph_size" ) ;
      goto lab6666 ;
    } 
    else if ( debugformatfile ) 
    {
      fprintf ( stderr , "%s%s",  "fmtdebug:" , "hyph_size" ) ;
      fprintf ( stderr , "%s%ld\n",  " = " , (long)x ) ;
    } 
    hyphcount = x ;
  } 
  {
    undumpint ( x ) ;
    if ( x < 607 ) 
    goto lab6666 ;
    if ( x > hyphsize ) 
    {
      ;
      fprintf ( stdout , "%s%s\n",  "---! Must increase the " , "hyph_size" ) ;
      goto lab6666 ;
    } 
    else if ( debugformatfile ) 
    {
      fprintf ( stderr , "%s%s",  "fmtdebug:" , "hyph_size" ) ;
      fprintf ( stderr , "%s%ld\n",  " = " , (long)x ) ;
    } 
    hyphnext = x ;
  } 
  j = 0 ;
  {register integer for_end; k = 1 ;for_end = hyphcount ; if ( k <= for_end) 
  do 
    {
      undumpint ( j ) ;
      if ( j < 0 ) 
      goto lab6666 ;
      if ( j > 65535L ) 
      {
	hyphnext = j / 65536L ;
	j = j - hyphnext * 65536L ;
      } 
      else hyphnext = 0 ;
      if ( ( j >= hyphsize ) || ( hyphnext > hyphsize ) ) 
      goto lab6666 ;
      hyphlink [j ]= hyphnext ;
      {
	undumpint ( x ) ;
	if ( ( x < 0 ) || ( x > strptr ) ) 
	goto lab6666 ;
	else hyphword [j ]= x ;
      } 
      {
	undumpint ( x ) ;
	if ( ( x < -268435455L ) || ( x > 1073741823L ) ) 
	goto lab6666 ;
	else hyphlist [j ]= x ;
      } 
    } 
  while ( k++ < for_end ) ;} 
  incr ( j ) ;
  if ( j < 607 ) 
  j = 607 ;
  hyphnext = j ;
  if ( hyphnext >= hyphsize ) 
  hyphnext = 607 ;
  else if ( hyphnext >= 607 ) 
  incr ( hyphnext ) ;
  {
    undumpint ( x ) ;
    if ( x < 0 ) 
    goto lab6666 ;
    if ( x > triesize ) 
    {
      ;
      fprintf ( stdout , "%s%s\n",  "---! Must increase the " , "trie size" ) ;
      goto lab6666 ;
    } 
    else if ( debugformatfile ) 
    {
      fprintf ( stderr , "%s%s",  "fmtdebug:" , "trie size" ) ;
      fprintf ( stderr , "%s%ld\n",  " = " , (long)x ) ;
    } 
    j = x ;
  } 
	;
#ifdef INITEX
  triemax = j ;
#endif /* INITEX */
  {
    undumpint ( x ) ;
    if ( ( x < 0 ) || ( x > j ) ) 
    goto lab6666 ;
    else hyphstart = x ;
  } 
  if ( ! trietrl ) 
  trietrl = xmallocarray ( triepointer , j + 1 ) ;
  undumpthings ( trietrl [0 ], j + 1 ) ;
  if ( ! trietro ) 
  trietro = xmallocarray ( triepointer , j + 1 ) ;
  undumpthings ( trietro [0 ], j + 1 ) ;
  if ( ! trietrc ) 
  trietrc = xmallocarray ( quarterword , j + 1 ) ;
  undumpthings ( trietrc [0 ], j + 1 ) ;
  undumpint ( maxhyphchar ) ;
  {
    undumpint ( x ) ;
    if ( x < 0 ) 
    goto lab6666 ;
    if ( x > trieopsize ) 
    {
      ;
      fprintf ( stdout , "%s%s\n",  "---! Must increase the " , "trie op size" ) ;
      goto lab6666 ;
    } 
    else if ( debugformatfile ) 
    {
      fprintf ( stderr , "%s%s",  "fmtdebug:" , "trie op size" ) ;
      fprintf ( stderr , "%s%ld\n",  " = " , (long)x ) ;
    } 
    j = x ;
  } 
	;
#ifdef INITEX
  trieopptr = j ;
#endif /* INITEX */
  undumpthings ( hyfdistance [1 ], j ) ;
  undumpthings ( hyfnum [1 ], j ) ;
  undumpuppercheckthings ( maxtrieop , hyfnext [1 ], j ) ;
	;
#ifdef INITEX
  {register integer for_end; k = 0 ;for_end = 255 ; if ( k <= for_end) do 
    trieused [k ]= 0 ;
  while ( k++ < for_end ) ;} 
#endif /* INITEX */
  k = 256 ;
  while ( j > 0 ) {
      
    {
      undumpint ( x ) ;
      if ( ( x < 0 ) || ( x > k - 1 ) ) 
      goto lab6666 ;
      else k = x ;
    } 
    {
      undumpint ( x ) ;
      if ( ( x < 1 ) || ( x > j ) ) 
      goto lab6666 ;
      else x = x ;
    } 
	;
#ifdef INITEX
    trieused [k ]= x ;
#endif /* INITEX */
    j = j - x ;
    opstart [k ]= j ;
  } 
	;
#ifdef INITEX
  trienotready = false 
#endif /* INITEX */
  ;
  {
    undumpint ( x ) ;
    if ( ( x < 0 ) || ( x > 3 ) ) 
    goto lab6666 ;
    else interaction = x ;
  } 
  if ( interactionoption != 4 ) 
  interaction = interactionoption ;
  {
    undumpint ( x ) ;
    if ( ( x < 0 ) || ( x > strptr ) ) 
    goto lab6666 ;
    else formatident = x ;
  } 
  undumpint ( x ) ;
  if ( x != 69069L ) 
  goto lab6666 ;
  Result = true ;
  return Result ;
  lab6666: ;
  fprintf ( stdout , "%s\n",  "(Fatal format file error; I'm stymied)" ) ;
  Result = false ;
  return Result ;
} 
void 
finalcleanup ( void ) 
{
  /* 10 */ finalcleanup_regmem 
  smallnumber c  ;
  c = curchr ;
  if ( jobname == 0 ) 
  openlogfile () ;
  while ( inputptr > 0 ) if ( curinput .statefield == 0 ) 
  endtokenlist () ;
  else endfilereading () ;
  while ( openparens > 0 ) {
      
    print ( 66729L ) ;
    decr ( openparens ) ;
  } 
  if ( curlevel > 1 ) 
  {
    printnl ( 40 ) ;
    printesc ( 66730L ) ;
    print ( 66731L ) ;
    printint ( curlevel - 1 ) ;
    printchar ( 41 ) ;
    if ( ( eTeXmode == 1 ) ) 
    showsavegroups () ;
  } 
  while ( condptr != -268435455L ) {
      
    printnl ( 40 ) ;
    printesc ( 66730L ) ;
    print ( 66732L ) ;
    printcmdchr ( 107 , curif ) ;
    if ( ifline != 0 ) 
    {
      print ( 66733L ) ;
      printint ( ifline ) ;
    } 
    print ( 66734L ) ;
    ifline = mem [condptr + 1 ].cint ;
    curif = mem [condptr ].hh.b1 ;
    tempptr = condptr ;
    condptr = mem [condptr ].hh .v.RH ;
    freenode ( tempptr , 2 ) ;
  } 
  if ( history != 0 ) {
      
    if ( ( ( history == 1 ) || ( interaction < 3 ) ) ) {
	
      if ( selector == 19 ) 
      {
	selector = 17 ;
	printnl ( 66735L ) ;
	selector = 19 ;
      } 
    } 
  } 
  if ( c == 1 ) 
  {
	;
#ifdef INITEX
    if ( iniversion ) 
    {
      {register integer for_end; c = 0 ;for_end = 4 ; if ( c <= for_end) do 
	if ( curmark [c ]!= -268435455L ) 
	deletetokenref ( curmark [c ]) ;
      while ( c++ < for_end ) ;} 
      if ( saroot [7 ]!= -268435455L ) {
	  
	if ( domarks ( 3 , 0 , saroot [7 ]) ) 
	saroot [7 ]= -268435455L ;
      } 
      {register integer for_end; c = 2 ;for_end = 3 ; if ( c <= for_end) do 
	flushnodelist ( discptr [c ]) ;
      while ( c++ < for_end ) ;} 
      if ( lastglue != 1073741823L ) 
      deleteglueref ( lastglue ) ;
      storefmtfile () ;
      return ;
    } 
#endif /* INITEX */
    printnl ( 66736L ) ;
    return ;
  } 
} 
#ifdef INITEX
void 
initprim ( void ) 
{
  initprim_regmem 
  nonewcontrolsequence = false ;
  first = 0 ;
  primitive ( 65675L , 76 , 2252740L ) ;
  primitive ( 65676L , 76 , 2252741L ) ;
  primitive ( 65677L , 76 , 2252742L ) ;
  primitive ( 65678L , 76 , 2252743L ) ;
  primitive ( 65679L , 76 , 2252744L ) ;
  primitive ( 65680L , 76 , 2252745L ) ;
  primitive ( 65681L , 76 , 2252746L ) ;
  primitive ( 65682L , 76 , 2252747L ) ;
  primitive ( 65683L , 76 , 2252748L ) ;
  primitive ( 65684L , 76 , 2252749L ) ;
  primitive ( 65685L , 76 , 2252750L ) ;
  primitive ( 65686L , 76 , 2252751L ) ;
  primitive ( 65687L , 76 , 2252752L ) ;
  primitive ( 65688L , 76 , 2252753L ) ;
  primitive ( 65689L , 76 , 2252754L ) ;
  primitive ( 65690L , 76 , 2252755L ) ;
  primitive ( 65691L , 77 , 2252756L ) ;
  primitive ( 65692L , 77 , 2252757L ) ;
  primitive ( 65693L , 77 , 2252758L ) ;
  primitive ( 65698L , 73 , 2253272L ) ;
  primitive ( 65699L , 73 , 2253273L ) ;
  primitive ( 65700L , 73 , 2253274L ) ;
  primitive ( 65701L , 73 , 2253275L ) ;
  primitive ( 65702L , 73 , 2253276L ) ;
  primitive ( 65703L , 73 , 2253277L ) ;
  primitive ( 65704L , 73 , 2253278L ) ;
  primitive ( 65705L , 73 , 2253279L ) ;
  primitive ( 65706L , 73 , 2253280L ) ;
  primitive ( 65720L , 74 , 8939240L ) ;
  primitive ( 65721L , 74 , 8939241L ) ;
  primitive ( 65722L , 74 , 8939242L ) ;
  primitive ( 65723L , 74 , 8939243L ) ;
  primitive ( 65724L , 74 , 8939244L ) ;
  primitive ( 65725L , 74 , 8939245L ) ;
  primitive ( 65726L , 74 , 8939246L ) ;
  primitive ( 65727L , 74 , 8939247L ) ;
  primitive ( 65728L , 74 , 8939248L ) ;
  primitive ( 65729L , 74 , 8939249L ) ;
  primitive ( 65730L , 74 , 8939250L ) ;
  primitive ( 65731L , 74 , 8939251L ) ;
  primitive ( 65732L , 74 , 8939252L ) ;
  primitive ( 65733L , 74 , 8939253L ) ;
  primitive ( 65734L , 74 , 8939254L ) ;
  primitive ( 65735L , 74 , 8939255L ) ;
  primitive ( 65736L , 74 , 8939256L ) ;
  primitive ( 65737L , 74 , 8939257L ) ;
  primitive ( 65738L , 74 , 8939258L ) ;
  primitive ( 65739L , 74 , 8939259L ) ;
  primitive ( 65740L , 74 , 8939260L ) ;
  primitive ( 65741L , 74 , 8939261L ) ;
  primitive ( 65742L , 74 , 8939262L ) ;
  primitive ( 65743L , 74 , 8939263L ) ;
  primitive ( 65744L , 74 , 8939264L ) ;
  primitive ( 65745L , 74 , 8939265L ) ;
  primitive ( 65746L , 74 , 8939266L ) ;
  primitive ( 65747L , 74 , 8939267L ) ;
  primitive ( 65748L , 74 , 8939268L ) ;
  primitive ( 65749L , 74 , 8939269L ) ;
  primitive ( 65750L , 74 , 8939270L ) ;
  primitive ( 65751L , 74 , 8939271L ) ;
  primitive ( 65752L , 74 , 8939272L ) ;
  primitive ( 65753L , 74 , 8939273L ) ;
  primitive ( 65754L , 74 , 8939274L ) ;
  primitive ( 65755L , 74 , 8939275L ) ;
  primitive ( 65756L , 74 , 8939276L ) ;
  primitive ( 65757L , 74 , 8939277L ) ;
  primitive ( 65758L , 74 , 8939278L ) ;
  primitive ( 65759L , 74 , 8939279L ) ;
  primitive ( 65760L , 74 , 8939280L ) ;
  primitive ( 65761L , 74 , 8939281L ) ;
  primitive ( 65762L , 74 , 8939282L ) ;
  primitive ( 65763L , 74 , 8939283L ) ;
  primitive ( 65764L , 74 , 8939284L ) ;
  primitive ( 65765L , 74 , 8939285L ) ;
  primitive ( 65766L , 74 , 8939286L ) ;
  primitive ( 65767L , 74 , 8939287L ) ;
  primitive ( 65768L , 74 , 8939288L ) ;
  primitive ( 65769L , 74 , 8939289L ) ;
  primitive ( 65770L , 74 , 8939290L ) ;
  primitive ( 65771L , 74 , 8939291L ) ;
  primitive ( 65772L , 74 , 8939292L ) ;
  primitive ( 65773L , 74 , 8939293L ) ;
  primitive ( 65774L , 74 , 8939294L ) ;
  if ( mltexp ) 
  {
    mltexenabledp = true ;
    if ( false ) 
    primitive ( 65775L , 74 , 8939295L ) ;
    primitive ( 65776L , 74 , 8939296L ) ;
    primitive ( 65777L , 74 , 8939297L ) ;
  } 
  primitive ( 65778L , 74 , 8939309L ) ;
  primitive ( 65779L , 74 , 8939310L ) ;
  primitive ( 65783L , 75 , 10053691L ) ;
  primitive ( 65784L , 75 , 10053692L ) ;
  primitive ( 65785L , 75 , 10053693L ) ;
  primitive ( 65786L , 75 , 10053694L ) ;
  primitive ( 65787L , 75 , 10053695L ) ;
  primitive ( 65788L , 75 , 10053696L ) ;
  primitive ( 65789L , 75 , 10053697L ) ;
  primitive ( 65790L , 75 , 10053698L ) ;
  primitive ( 65791L , 75 , 10053699L ) ;
  primitive ( 65792L , 75 , 10053700L ) ;
  primitive ( 65793L , 75 , 10053701L ) ;
  primitive ( 65794L , 75 , 10053702L ) ;
  primitive ( 65795L , 75 , 10053703L ) ;
  primitive ( 65796L , 75 , 10053704L ) ;
  primitive ( 65797L , 75 , 10053705L ) ;
  primitive ( 65798L , 75 , 10053706L ) ;
  primitive ( 65799L , 75 , 10053707L ) ;
  primitive ( 65800L , 75 , 10053708L ) ;
  primitive ( 65801L , 75 , 10053709L ) ;
  primitive ( 65802L , 75 , 10053710L ) ;
  primitive ( 65803L , 75 , 10053711L ) ;
  primitive ( 65804L , 75 , 10053712L ) ;
  primitive ( 65805L , 75 , 10053713L ) ;
  primitive ( 32 , 64 , 0 ) ;
  primitive ( 47 , 44 , 0 ) ;
  primitive ( 65817L , 45 , 0 ) ;
  primitive ( 65818L , 92 , 0 ) ;
  primitive ( 65819L , 40 , 0 ) ;
  primitive ( 65820L , 41 , 0 ) ;
  primitive ( 65821L , 61 , 0 ) ;
  primitive ( 65822L , 16 , 0 ) ;
  primitive ( 65813L , 109 , 0 ) ;
  primitive ( 65823L , 15 , 0 ) ;
  primitive ( 65824L , 15 , 1 ) ;
  primitive ( 65825L , 15 , 1 ) ;
  primitive ( 65826L , 94 , 0 ) ;
  primitive ( 65814L , 67 , 0 ) ;
  primitive ( 65827L , 62 , 0 ) ;
  hash [2243228L ].v.RH = 65827L ;
  eqtb [2243228L ]= eqtb [curval ];
  primitive ( 65828L , 104 , 0 ) ;
  primitive ( 65829L , 90 , 0 ) ;
  primitive ( 65830L , 78 , 0 ) ;
  primitive ( 65831L , 32 , 0 ) ;
  primitive ( 65832L , 36 , 0 ) ;
  primitive ( 65833L , 39 , 0 ) ;
  primitive ( 65618L , 37 , 0 ) ;
  primitive ( 65641L , 18 , 0 ) ;
  primitive ( 65834L , 46 , 0 ) ;
  primitive ( 65835L , 46 , 1 ) ;
  primitive ( 65836L , 46 , 1 ) ;
  primitive ( 65837L , 17 , 0 ) ;
  primitive ( 65838L , 17 , 1 ) ;
  primitive ( 65839L , 17 , 1 ) ;
  primitive ( 65840L , 17 , 2 ) ;
  primitive ( 65841L , 17 , 2 ) ;
  primitive ( 65842L , 54 , 0 ) ;
  primitive ( 65843L , 93 , 0 ) ;
  primitive ( 65844L , 34 , 0 ) ;
  primitive ( 65845L , 65 , 0 ) ;
  primitive ( 65846L , 105 , 0 ) ;
  primitive ( 65810L , 105 , 1 ) ;
  primitive ( 65623L , 55 , 0 ) ;
  primitive ( 65847L , 63 , 0 ) ;
  primitive ( 65848L , 85 , 2253271L ) ;
  primitive ( 65849L , 42 , 0 ) ;
  primitive ( 65850L , 81 , 0 ) ;
  primitive ( 65851L , 66 , 0 ) ;
  primitive ( 65852L , 66 , 1 ) ;
  primitive ( 65853L , 66 , 1 ) ;
  primitive ( 65854L , 98 , 0 ) ;
  primitive ( 65855L , 0 , 1114112L ) ;
  hash [2243233L ].v.RH = 65855L ;
  eqtb [2243233L ]= eqtb [curval ];
  primitive ( 65856L , 100 , 0 ) ;
  primitive ( 65857L , 111 , 0 ) ;
  primitive ( 65707L , 72 , membot ) ;
  primitive ( 65642L , 38 , 0 ) ;
  primitive ( 65858L , 33 , 0 ) ;
  primitive ( 65859L , 56 , 0 ) ;
  primitive ( 65860L , 35 , 0 ) ;
  primitive ( 65921L , 13 , 1114112L ) ;
  parloc = curval ;
  partoken = 33554431L + parloc ;
  primitive ( 65956L , 106 , 0 ) ;
  primitive ( 65957L , 106 , 1 ) ;
  primitive ( 65958L , 112 , 0 ) ;
  primitive ( 65959L , 112 , 1 ) ;
  primitive ( 65960L , 112 , 2 ) ;
  primitive ( 65961L , 112 , 3 ) ;
  primitive ( 65962L , 112 , 4 ) ;
  primitive ( 65781L , 91 , membot + 0 ) ;
  primitive ( 65807L , 91 , membot + 1 ) ;
  primitive ( 65695L , 91 , membot + 2 ) ;
  primitive ( 65696L , 91 , membot + 3 ) ;
  primitive ( 66006L , 80 , 104 ) ;
  primitive ( 66007L , 80 , 1 ) ;
  primitive ( 66008L , 83 , 0 ) ;
  primitive ( 66009L , 83 , 1 ) ;
  primitive ( 66010L , 84 , 1 ) ;
  primitive ( 66011L , 84 , 3 ) ;
  primitive ( 66012L , 84 , 2 ) ;
  primitive ( 66013L , 71 , 0 ) ;
  primitive ( 66014L , 71 , 1 ) ;
  primitive ( 66015L , 71 , 2 ) ;
  primitive ( 66016L , 71 , 4 ) ;
  primitive ( 66017L , 71 , 5 ) ;
  primitive ( 66018L , 71 , 12 ) ;
  primitive ( 66019L , 71 , 13 ) ;
  primitive ( 66020L , 71 , 16 ) ;
  primitive ( 66021L , 71 , 17 ) ;
  primitive ( 66022L , 71 , 18 ) ;
  primitive ( 66090L , 110 , 0 ) ;
  primitive ( 66091L , 110 , 1 ) ;
  primitive ( 66092L , 110 , 2 ) ;
  primitive ( 66093L , 110 , 3 ) ;
  primitive ( 66094L , 110 , 4 ) ;
  primitive ( 66095L , 110 , 6 ) ;
  primitive ( 66096L , 110 , 16 ) ;
  primitive ( 66097L , 110 , 17 ) ;
  primitive ( 66098L , 110 , 22 ) ;
  primitive ( 66099L , 110 , 23 ) ;
  primitive ( 66100L , 110 , 24 ) ;
  primitive ( 66101L , 110 , 25 ) ;
  primitive ( 66102L , 110 , 26 ) ;
  primitive ( 66103L , 110 , 18 ) ;
  primitive ( 66104L , 110 , 29 ) ;
  primitive ( 66105L , 110 , 30 ) ;
  primitive ( 66106L , 110 , 40 ) ;
  primitive ( 66107L , 110 , 38 ) ;
  primitive ( 66108L , 110 , 39 ) ;
  primitive ( 66139L , 107 , 0 ) ;
  primitive ( 66140L , 107 , 1 ) ;
  primitive ( 66141L , 107 , 2 ) ;
  primitive ( 66142L , 107 , 3 ) ;
  primitive ( 66143L , 107 , 4 ) ;
  primitive ( 66144L , 107 , 5 ) ;
  primitive ( 66145L , 107 , 6 ) ;
  primitive ( 66146L , 107 , 7 ) ;
  primitive ( 66147L , 107 , 8 ) ;
  primitive ( 66148L , 107 , 9 ) ;
  primitive ( 66149L , 107 , 10 ) ;
  primitive ( 66150L , 107 , 11 ) ;
  primitive ( 66151L , 107 , 12 ) ;
  primitive ( 66152L , 107 , 13 ) ;
  primitive ( 66153L , 107 , 14 ) ;
  primitive ( 66154L , 107 , 15 ) ;
  primitive ( 66155L , 107 , 16 ) ;
  primitive ( 66156L , 107 , 21 ) ;
  primitive ( 66158L , 108 , 2 ) ;
  hash [2243230L ].v.RH = 66158L ;
  eqtb [2243230L ]= eqtb [curval ];
  primitive ( 66159L , 108 , 4 ) ;
  primitive ( 66160L , 108 , 3 ) ;
  primitive ( 66186L , 89 , 0 ) ;
  hash [2243738L ].v.RH = 66186L ;
  eqtb [2243738L ]= eqtb [curval ];
  primitive ( 66315L , 4 , 1114113L ) ;
  primitive ( 66316L , 5 , 1114114L ) ;
  hash [2243227L ].v.RH = 66316L ;
  eqtb [2243227L ]= eqtb [curval ];
  primitive ( 66317L , 5 , 1114115L ) ;
  hash [2243231L ].v.RH = 66318L ;
  hash [2243232L ].v.RH = 66318L ;
  eqtb [2243232L ].hh.b0 = 9 ;
  eqtb [2243232L ].hh .v.RH = memtop - 11 ;
  eqtb [2243232L ].hh.b1 = 1 ;
  eqtb [2243231L ]= eqtb [2243232L ];
  eqtb [2243231L ].hh.b0 = 117 ;
  primitive ( 66395L , 82 , 0 ) ;
  primitive ( 66396L , 82 , 1 ) ;
  primitive ( 66397L , 82 , 2 ) ;
  primitive ( 66398L , 82 , 3 ) ;
  primitive ( 66399L , 82 , 4 ) ;
  primitive ( 66400L , 82 , 5 ) ;
  primitive ( 66401L , 82 , 6 ) ;
  primitive ( 66402L , 82 , 7 ) ;
  primitive ( 65631L , 14 , 0 ) ;
  primitive ( 66449L , 14 , 1 ) ;
  primitive ( 66450L , 26 , 4 ) ;
  primitive ( 66451L , 26 , 0 ) ;
  primitive ( 66452L , 26 , 1 ) ;
  primitive ( 66453L , 26 , 2 ) ;
  primitive ( 66454L , 26 , 3 ) ;
  primitive ( 66455L , 27 , 4 ) ;
  primitive ( 66456L , 27 , 0 ) ;
  primitive ( 66457L , 27 , 1 ) ;
  primitive ( 66458L , 27 , 2 ) ;
  primitive ( 66459L , 27 , 3 ) ;
  primitive ( 65624L , 28 , 5 ) ;
  primitive ( 65603L , 29 , 1 ) ;
  primitive ( 65630L , 30 , 99 ) ;
  primitive ( 66477L , 21 , 1 ) ;
  primitive ( 66478L , 21 , 0 ) ;
  primitive ( 66479L , 22 , 1 ) ;
  primitive ( 66480L , 22 , 0 ) ;
  primitive ( 65709L , 20 , 0 ) ;
  primitive ( 66481L , 20 , 1 ) ;
  primitive ( 66482L , 20 , 2 ) ;
  primitive ( 66390L , 20 , 3 ) ;
  primitive ( 66483L , 20 , 4 ) ;
  primitive ( 66392L , 20 , 5 ) ;
  primitive ( 66484L , 20 , 108 ) ;
  primitive ( 66485L , 31 , 99 ) ;
  primitive ( 66486L , 31 , 100 ) ;
  primitive ( 66487L , 31 , 101 ) ;
  primitive ( 66488L , 31 , 102 ) ;
  primitive ( 66504L , 43 , 1 ) ;
  primitive ( 66505L , 43 , 0 ) ;
  primitive ( 66515L , 25 , 12 ) ;
  primitive ( 66516L , 25 , 11 ) ;
  primitive ( 66517L , 25 , 10 ) ;
  primitive ( 66518L , 23 , 0 ) ;
  primitive ( 66519L , 23 , 1 ) ;
  primitive ( 66520L , 24 , 0 ) ;
  primitive ( 66521L , 24 , 1 ) ;
  primitive ( 45 , 47 , 1 ) ;
  primitive ( 65639L , 47 , 0 ) ;
  primitive ( 66552L , 48 , 0 ) ;
  primitive ( 66553L , 48 , 1 ) ;
  primitive ( 66265L , 50 , 16 ) ;
  primitive ( 66266L , 50 , 17 ) ;
  primitive ( 66267L , 50 , 18 ) ;
  primitive ( 66268L , 50 , 19 ) ;
  primitive ( 66269L , 50 , 20 ) ;
  primitive ( 66270L , 50 , 21 ) ;
  primitive ( 66271L , 50 , 22 ) ;
  primitive ( 66272L , 50 , 23 ) ;
  primitive ( 66274L , 50 , 26 ) ;
  primitive ( 66273L , 50 , 27 ) ;
  primitive ( 66554L , 51 , 0 ) ;
  primitive ( 66278L , 51 , 1 ) ;
  primitive ( 66279L , 51 , 2 ) ;
  primitive ( 66260L , 53 , 0 ) ;
  primitive ( 66261L , 53 , 2 ) ;
  primitive ( 66262L , 53 , 4 ) ;
  primitive ( 66263L , 53 , 6 ) ;
  primitive ( 66574L , 52 , 0 ) ;
  primitive ( 66575L , 52 , 1 ) ;
  primitive ( 66576L , 52 , 2 ) ;
  primitive ( 66577L , 52 , 3 ) ;
  primitive ( 66578L , 52 , 4 ) ;
  primitive ( 66579L , 52 , 5 ) ;
  primitive ( 66275L , 49 , 30 ) ;
  primitive ( 66276L , 49 , 31 ) ;
  hash [2243229L ].v.RH = 66276L ;
  eqtb [2243229L ]= eqtb [curval ];
  primitive ( 66599L , 95 , 1 ) ;
  primitive ( 66600L , 95 , 2 ) ;
  primitive ( 66601L , 95 , 4 ) ;
  primitive ( 66602L , 99 , 0 ) ;
  primitive ( 66603L , 99 , 1 ) ;
  primitive ( 66604L , 99 , 2 ) ;
  primitive ( 66605L , 99 , 3 ) ;
  primitive ( 66622L , 96 , 0 ) ;
  primitive ( 66623L , 96 , 1 ) ;
  primitive ( 66624L , 97 , 0 ) ;
  primitive ( 66625L , 97 , 1 ) ;
  primitive ( 66626L , 97 , 8 ) ;
  primitive ( 66627L , 97 , 8 ) ;
  primitive ( 66628L , 97 , 9 ) ;
  primitive ( 66629L , 97 , 9 ) ;
  primitive ( 66630L , 97 , 2 ) ;
  primitive ( 66631L , 97 , 3 ) ;
  primitive ( 66632L , 97 , 4 ) ;
  primitive ( 66633L , 97 , 5 ) ;
  primitive ( 66634L , 97 , 6 ) ;
  if ( mltexp ) 
  {
    primitive ( 66635L , 97 , 7 ) ;
  } 
  primitive ( 65715L , 86 , 2254568L ) ;
  primitive ( 65719L , 86 , 6711016L ) ;
  primitive ( 66640L , 87 , 6711016L ) ;
  primitive ( 66641L , 87 , 6711016L ) ;
  primitive ( 66642L , 87 , 6711017L ) ;
  primitive ( 66643L , 87 , 6711017L ) ;
  primitive ( 65716L , 86 , 3368680L ) ;
  primitive ( 65717L , 86 , 4482792L ) ;
  primitive ( 65718L , 86 , 5596904L ) ;
  primitive ( 66644L , 87 , 5596904L ) ;
  primitive ( 65782L , 86 , 8939579L ) ;
  primitive ( 66645L , 87 , 8939579L ) ;
  primitive ( 66646L , 87 , 8939579L ) ;
  primitive ( 66647L , 87 , 8939580L ) ;
  primitive ( 66648L , 87 , 8939580L ) ;
  primitive ( 65712L , 88 , 2253800L ) ;
  primitive ( 65713L , 88 , 2254056L ) ;
  primitive ( 65714L , 88 , 2254312L ) ;
  primitive ( 66366L , 101 , 0 ) ;
  primitive ( 66378L , 101 , 1 ) ;
  primitive ( 66663L , 79 , 0 ) ;
  primitive ( 66664L , 79 , 1 ) ;
  primitive ( 66665L , 79 , 2 ) ;
  primitive ( 66666L , 79 , 3 ) ;
  primitive ( 65554L , 102 , 0 ) ;
  primitive ( 65555L , 102 , 1 ) ;
  primitive ( 65556L , 102 , 2 ) ;
  primitive ( 66675L , 102 , 3 ) ;
  primitive ( 66676L , 60 , 1 ) ;
  primitive ( 66677L , 60 , 0 ) ;
  primitive ( 66678L , 58 , 0 ) ;
  primitive ( 66679L , 58 , 1 ) ;
  primitive ( 66685L , 57 , 3368680L ) ;
  primitive ( 66686L , 57 , 4482792L ) ;
  primitive ( 66687L , 19 , 0 ) ;
  primitive ( 66688L , 19 , 1 ) ;
  primitive ( 66689L , 19 , 2 ) ;
  primitive ( 66690L , 19 , 3 ) ;
  primitive ( 66738L , 59 , 0 ) ;
  primitive ( 65918L , 59 , 1 ) ;
  writeloc = curval ;
  primitive ( 66739L , 59 , 2 ) ;
  primitive ( 66740L , 59 , 3 ) ;
  hash [2243236L ].v.RH = 66740L ;
  eqtb [2243236L ]= eqtb [curval ];
  primitive ( 66741L , 59 , 4 ) ;
  primitive ( 66742L , 59 , 5 ) ;
  primitive ( 66743L , 59 , 31 ) ;
  primitive ( 66744L , 59 , 33 ) ;
  nonewcontrolsequence = true ;
} 
#endif /* INITEX */
void 
mainbody ( void ) 
{
  mainbody_regmem 
  bounddefault = 0 ;
  boundname = "mem_bot" ;
  setupboundvariable ( addressof ( membot ) , boundname , bounddefault ) ;
  bounddefault = 250000L ;
  boundname = "main_memory" ;
  setupboundvariable ( addressof ( mainmemory ) , boundname , bounddefault ) ;
  bounddefault = 0 ;
  boundname = "extra_mem_top" ;
  setupboundvariable ( addressof ( extramemtop ) , boundname , bounddefault ) 
  ;
  bounddefault = 0 ;
  boundname = "extra_mem_bot" ;
  setupboundvariable ( addressof ( extramembot ) , boundname , bounddefault ) 
  ;
  bounddefault = 200000L ;
  boundname = "pool_size" ;
  setupboundvariable ( addressof ( poolsize ) , boundname , bounddefault ) ;
  bounddefault = 75000L ;
  boundname = "string_vacancies" ;
  setupboundvariable ( addressof ( stringvacancies ) , boundname , 
  bounddefault ) ;
  bounddefault = 5000 ;
  boundname = "pool_free" ;
  setupboundvariable ( addressof ( poolfree ) , boundname , bounddefault ) ;
  bounddefault = 15000 ;
  boundname = "max_strings" ;
  setupboundvariable ( addressof ( maxstrings ) , boundname , bounddefault ) ;
  maxstrings = maxstrings + 65536L ;
  bounddefault = 100 ;
  boundname = "strings_free" ;
  setupboundvariable ( addressof ( stringsfree ) , boundname , bounddefault ) 
  ;
  bounddefault = 100000L ;
  boundname = "font_mem_size" ;
  setupboundvariable ( addressof ( fontmemsize ) , boundname , bounddefault ) 
  ;
  bounddefault = 500 ;
  boundname = "font_max" ;
  setupboundvariable ( addressof ( fontmax ) , boundname , bounddefault ) ;
  bounddefault = 20000 ;
  boundname = "trie_size" ;
  setupboundvariable ( addressof ( triesize ) , boundname , bounddefault ) ;
  bounddefault = 659 ;
  boundname = "hyph_size" ;
  setupboundvariable ( addressof ( hyphsize ) , boundname , bounddefault ) ;
  bounddefault = 3000 ;
  boundname = "buf_size" ;
  setupboundvariable ( addressof ( bufsize ) , boundname , bounddefault ) ;
  bounddefault = 50 ;
  boundname = "nest_size" ;
  setupboundvariable ( addressof ( nestsize ) , boundname , bounddefault ) ;
  bounddefault = 15 ;
  boundname = "max_in_open" ;
  setupboundvariable ( addressof ( maxinopen ) , boundname , bounddefault ) ;
  bounddefault = 60 ;
  boundname = "param_size" ;
  setupboundvariable ( addressof ( paramsize ) , boundname , bounddefault ) ;
  bounddefault = 4000 ;
  boundname = "save_size" ;
  setupboundvariable ( addressof ( savesize ) , boundname , bounddefault ) ;
  bounddefault = 300 ;
  boundname = "stack_size" ;
  setupboundvariable ( addressof ( stacksize ) , boundname , bounddefault ) ;
  bounddefault = 16384 ;
  boundname = "dvi_buf_size" ;
  setupboundvariable ( addressof ( dvibufsize ) , boundname , bounddefault ) ;
  bounddefault = 79 ;
  boundname = "error_line" ;
  setupboundvariable ( addressof ( errorline ) , boundname , bounddefault ) ;
  bounddefault = 50 ;
  boundname = "half_error_line" ;
  setupboundvariable ( addressof ( halferrorline ) , boundname , bounddefault 
  ) ;
  bounddefault = 79 ;
  boundname = "max_print_line" ;
  setupboundvariable ( addressof ( maxprintline ) , boundname , bounddefault ) 
  ;
  bounddefault = 0 ;
  boundname = "hash_extra" ;
  setupboundvariable ( addressof ( hashextra ) , boundname , bounddefault ) ;
  bounddefault = 10000 ;
  boundname = "expand_depth" ;
  setupboundvariable ( addressof ( expanddepth ) , boundname , bounddefault ) 
  ;
  {
    if ( membot < infmembot ) 
    membot = infmembot ;
    else if ( membot > supmembot ) 
    membot = supmembot ;
  } 
  {
    if ( mainmemory < infmainmemory ) 
    mainmemory = infmainmemory ;
    else if ( mainmemory > supmainmemory ) 
    mainmemory = supmainmemory ;
  } 
	;
#ifdef INITEX
  if ( iniversion ) 
  {
    extramemtop = 0 ;
    extramembot = 0 ;
  } 
#endif /* INITEX */
  if ( extramembot > supmainmemory ) 
  extramembot = supmainmemory ;
  if ( extramemtop > supmainmemory ) 
  extramemtop = supmainmemory ;
  memtop = membot + mainmemory - 1 ;
  memmin = membot ;
  memmax = memtop ;
  {
    if ( triesize < inftriesize ) 
    triesize = inftriesize ;
    else if ( triesize > suptriesize ) 
    triesize = suptriesize ;
  } 
  {
    if ( hyphsize < infhyphsize ) 
    hyphsize = infhyphsize ;
    else if ( hyphsize > suphyphsize ) 
    hyphsize = suphyphsize ;
  } 
  {
    if ( bufsize < infbufsize ) 
    bufsize = infbufsize ;
    else if ( bufsize > supbufsize ) 
    bufsize = supbufsize ;
  } 
  {
    if ( nestsize < infnestsize ) 
    nestsize = infnestsize ;
    else if ( nestsize > supnestsize ) 
    nestsize = supnestsize ;
  } 
  {
    if ( maxinopen < infmaxinopen ) 
    maxinopen = infmaxinopen ;
    else if ( maxinopen > supmaxinopen ) 
    maxinopen = supmaxinopen ;
  } 
  {
    if ( paramsize < infparamsize ) 
    paramsize = infparamsize ;
    else if ( paramsize > supparamsize ) 
    paramsize = supparamsize ;
  } 
  {
    if ( savesize < infsavesize ) 
    savesize = infsavesize ;
    else if ( savesize > supsavesize ) 
    savesize = supsavesize ;
  } 
  {
    if ( stacksize < infstacksize ) 
    stacksize = infstacksize ;
    else if ( stacksize > supstacksize ) 
    stacksize = supstacksize ;
  } 
  {
    if ( dvibufsize < infdvibufsize ) 
    dvibufsize = infdvibufsize ;
    else if ( dvibufsize > supdvibufsize ) 
    dvibufsize = supdvibufsize ;
  } 
  {
    if ( poolsize < infpoolsize ) 
    poolsize = infpoolsize ;
    else if ( poolsize > suppoolsize ) 
    poolsize = suppoolsize ;
  } 
  {
    if ( stringvacancies < infstringvacancies ) 
    stringvacancies = infstringvacancies ;
    else if ( stringvacancies > supstringvacancies ) 
    stringvacancies = supstringvacancies ;
  } 
  {
    if ( poolfree < infpoolfree ) 
    poolfree = infpoolfree ;
    else if ( poolfree > suppoolfree ) 
    poolfree = suppoolfree ;
  } 
  {
    if ( maxstrings < infmaxstrings ) 
    maxstrings = infmaxstrings ;
    else if ( maxstrings > supmaxstrings ) 
    maxstrings = supmaxstrings ;
  } 
  {
    if ( stringsfree < infstringsfree ) 
    stringsfree = infstringsfree ;
    else if ( stringsfree > supstringsfree ) 
    stringsfree = supstringsfree ;
  } 
  {
    if ( fontmemsize < inffontmemsize ) 
    fontmemsize = inffontmemsize ;
    else if ( fontmemsize > supfontmemsize ) 
    fontmemsize = supfontmemsize ;
  } 
  {
    if ( fontmax < inffontmax ) 
    fontmax = inffontmax ;
    else if ( fontmax > supfontmax ) 
    fontmax = supfontmax ;
  } 
  {
    if ( hashextra < infhashextra ) 
    hashextra = infhashextra ;
    else if ( hashextra > suphashextra ) 
    hashextra = suphashextra ;
  } 
  if ( errorline > 255 ) 
  errorline = 255 ;
  buffer = xmallocarray ( UnicodeScalar , bufsize ) ;
  nest = xmallocarray ( liststaterecord , nestsize ) ;
  savestack = xmallocarray ( memoryword , savesize ) ;
  inputstack = xmallocarray ( instaterecord , stacksize ) ;
  inputfile = xmallocarray ( unicodefile , maxinopen ) ;
  linestack = xmallocarray ( integer , maxinopen ) ;
  eofseen = xmallocarray ( boolean , maxinopen ) ;
  grpstack = xmallocarray ( savepointer , maxinopen ) ;
  ifstack = xmallocarray ( halfword , maxinopen ) ;
  sourcefilenamestack = xmallocarray ( strnumber , maxinopen ) ;
  fullsourcefilenamestack = xmallocarray ( strnumber , maxinopen ) ;
  paramstack = xmallocarray ( halfword , paramsize ) ;
  dvibuf = xmallocarray ( eightbits , dvibufsize ) ;
  hyphword = xmallocarray ( strnumber , hyphsize ) ;
  hyphlist = xmallocarray ( halfword , hyphsize ) ;
  hyphlink = xmallocarray ( hyphpointer , hyphsize ) ;
	;
#ifdef INITEX
  if ( iniversion ) 
  {
    yzmem = xmallocarray ( memoryword , memtop - membot + 1 ) ;
    zmem = yzmem - membot ;
    eqtbtop = 10053969L + hashextra ;
    if ( hashextra == 0 ) 
    hashtop = 2252739L ;
    else hashtop = eqtbtop ;
    yhash = xmallocarray ( twohalves , 1 + hashtop - hashoffset ) ;
    hash = yhash - hashoffset ;
    hash [2228226L ].v.LH = 0 ;
    hash [2228226L ].v.RH = 0 ;
    {register integer for_end; hashused = 2228227L ;for_end = hashtop ; if ( 
    hashused <= for_end) do 
      hash [hashused ]= hash [2228226L ];
    while ( hashused++ < for_end ) ;} 
    zeqtb = xmallocarray ( memoryword , eqtbtop ) ;
    eqtb = zeqtb ;
    strstart = xmallocarray ( poolpointer , maxstrings ) ;
    strpool = xmallocarray ( packedUTF16code , poolsize ) ;
    fontinfo = xmallocarray ( fmemoryword , fontmemsize ) ;
  } 
#endif /* INITEX */
  history = 3 ;
  if ( readyalready == 314159L ) 
  goto lab1 ;
  bad = 0 ;
  if ( ( halferrorline < 30 ) || ( halferrorline > errorline - 15 ) ) 
  bad = 1 ;
  if ( maxprintline < 60 ) 
  bad = 2 ;
  if ( dvibufsize % 8 != 0 ) 
  bad = 3 ;
  if ( membot + 1100 > memtop ) 
  bad = 4 ;
  if ( 8501 > 15000 ) 
  bad = 5 ;
  if ( maxinopen >= 128 ) 
  bad = 6 ;
  if ( memtop < 267 ) 
  bad = 7 ;
	;
#ifdef INITEX
  if ( ( memmin != membot ) || ( memmax != memtop ) ) 
  bad = 10 ;
#endif /* INITEX */
  if ( ( memmin > membot ) || ( memmax < memtop ) ) 
  bad = 10 ;
  if ( ( 0 > 0 ) || ( 65535L < 32767 ) ) 
  bad = 11 ;
  if ( ( -268435455L > 0 ) || ( 1073741823L < 1073741823L ) ) 
  bad = 12 ;
  if ( ( 0 < -268435455L ) || ( 65535L > 1073741823L ) ) 
  bad = 13 ;
  if ( ( membot - supmainmemory < -268435455L ) || ( memtop + supmainmemory >= 
  1073741823L ) ) 
  bad = 14 ;
  if ( ( 9000 < -268435455L ) || ( 9000 > 1073741823L ) ) 
  bad = 15 ;
  if ( fontmax > 9000 ) 
  bad = 16 ;
  if ( ( savesize > 1073741823L ) || ( maxstrings > 1073741823L ) ) 
  bad = 17 ;
  if ( bufsize > 1073741823L ) 
  bad = 18 ;
  if ( 65535L < 65535L ) 
  bad = 19 ;
  if ( 43608400L + hashextra > 1073741823L ) 
  bad = 21 ;
  if ( ( hashoffset < 0 ) || ( hashoffset > 2228226L ) ) 
  bad = 42 ;
  if ( formatdefaultlength > maxint ) 
  bad = 31 ;
  if ( 2 * 1073741823L < memtop - memmin ) 
  bad = 41 ;
  if ( bad > 0 ) 
  {
    fprintf ( stdout , "%s%s%ld\n",  "Ouch---my internal constants have been clobbered!" ,     "---case " , (long)bad ) ;
    goto lab9999 ;
  } 
  initialize () ;
	;
#ifdef INITEX
  if ( iniversion ) 
  {
    if ( ! getstringsstarted () ) 
    goto lab9999 ;
    initprim () ;
    initstrptr = strptr ;
    initpoolptr = poolptr ;
    dateandtime ( eqtb [8939260L ].cint , eqtb [8939261L ].cint , eqtb [
    8939262L ].cint , eqtb [8939263L ].cint ) ;
  } 
#endif /* INITEX */
  readyalready = 314159L ;
  lab1: selector = 17 ;
  tally = 0 ;
  termoffset = 0 ;
  fileoffset = 0 ;
  if ( srcspecialsp || filelineerrorstylep || parsefirstlinep ) 
  fprintf ( stdout , "%s%s%s",  "This is XeTeX, Version 3.14159265" , "-2.6" , "-0.999992"   ) ;
  else
  fprintf ( stdout , "%s%s%s",  "This is XeTeX, Version 3.14159265" , "-2.6" ,   "-0.999992" ) ;
  Fputs ( stdout ,  versionstring ) ;
  if ( formatident == 0 ) 
  fprintf ( stdout , "%s%s%c\n",  " (preloaded format=" , dumpname , ')' ) ;
  else {
      
    print ( formatident ) ;
    println () ;
  } 
  if ( shellenabledp ) 
  {
    putc ( ' ' ,  stdout );
    if ( restrictedshell ) 
    {
      Fputs ( stdout ,  "restricted " ) ;
    } 
    fprintf ( stdout , "%s\n",  "\\write18 enabled." ) ;
  } 
  if ( srcspecialsp ) 
  {
    fprintf ( stdout , "%s\n",  " Source specials enabled." ) ;
  } 
  if ( translatefilename ) 
  {
    Fputs ( stdout ,  " (WARNING: translate-file \"" ) ;
    fputs ( translatefilename , stdout ) ;
    fprintf ( stdout , "%s\n",  "\" ignored)" ) ;
  } 
  fflush ( stdout ) ;
  jobname = 0 ;
  nameinprogress = false ;
  logopened = false ;
  outputfilename = 0 ;
  if ( nopdfoutput ) 
  outputfileextension = 66178L ;
  else outputfileextension = 66179L ;
  {
    {
      inputptr = 0 ;
      maxinstack = 0 ;
      sourcefilenamestack [0 ]= 0 ;
      fullsourcefilenamestack [0 ]= 0 ;
      inopen = 0 ;
      openparens = 0 ;
      maxbufstack = 0 ;
      grpstack [0 ]= 0 ;
      ifstack [0 ]= -268435455L ;
      paramptr = 0 ;
      maxparamstack = 0 ;
      first = bufsize ;
      do {
	  buffer [first ]= 0 ;
	decr ( first ) ;
      } while ( ! ( first == 0 ) ) ;
      scannerstatus = 0 ;
      warningindex = -268435455L ;
      first = 1 ;
      curinput .statefield = 33 ;
      curinput .startfield = 1 ;
      curinput .indexfield = 0 ;
      line = 0 ;
      curinput .namefield = 0 ;
      forceeof = false ;
      alignstate = 1000000L ;
      if ( ! initterminal () ) 
      goto lab9999 ;
      curinput .limitfield = last ;
      first = last + 1 ;
    } 
	;
#ifdef INITEX
    if ( ( etexp || ( buffer [curinput .locfield ]== 42 ) ) && ( formatident 
    == 66703L ) ) 
    {
      nonewcontrolsequence = false ;
      primitive ( 66745L , 59 , 41 ) ;
      primitive ( 66746L , 59 , 42 ) ;
      primitive ( 66747L , 59 , 43 ) ;
      primitive ( 66748L , 59 , 46 ) ;
      primitive ( 66749L , 73 , 2253282L ) ;
      primitive ( 66750L , 59 , 21 ) ;
      primitive ( 66806L , 71 , 3 ) ;
      primitive ( 66807L , 71 , 19 ) ;
      primitive ( 66109L , 110 , 5 ) ;
      primitive ( 66808L , 71 , 27 ) ;
      primitive ( 66809L , 110 , 33 ) ;
      primitive ( 66810L , 71 , 28 ) ;
      primitive ( 66811L , 71 , 29 ) ;
      primitive ( 66812L , 71 , 30 ) ;
      primitive ( 66813L , 71 , 31 ) ;
      primitive ( 66814L , 71 , 32 ) ;
      primitive ( 66815L , 71 , 33 ) ;
      primitive ( 66816L , 71 , 34 ) ;
      primitive ( 66817L , 71 , 35 ) ;
      primitive ( 66818L , 71 , 36 ) ;
      primitive ( 66819L , 71 , 37 ) ;
      primitive ( 66820L , 71 , 38 ) ;
      primitive ( 66821L , 71 , 39 ) ;
      primitive ( 66822L , 71 , 40 ) ;
      primitive ( 66823L , 71 , 41 ) ;
      primitive ( 66824L , 71 , 42 ) ;
      primitive ( 66825L , 110 , 34 ) ;
      primitive ( 66826L , 110 , 35 ) ;
      primitive ( 66827L , 110 , 36 ) ;
      primitive ( 66828L , 71 , 43 ) ;
      primitive ( 66829L , 71 , 44 ) ;
      primitive ( 66830L , 71 , 45 ) ;
      primitive ( 66831L , 71 , 46 ) ;
      primitive ( 66832L , 71 , 47 ) ;
      primitive ( 66833L , 71 , 48 ) ;
      primitive ( 66834L , 71 , 49 ) ;
      primitive ( 66835L , 71 , 50 ) ;
      primitive ( 66836L , 71 , 55 ) ;
      primitive ( 66837L , 110 , 37 ) ;
      primitive ( 66838L , 71 , 51 ) ;
      primitive ( 66839L , 71 , 52 ) ;
      primitive ( 66840L , 71 , 53 ) ;
      primitive ( 66841L , 71 , 54 ) ;
      primitive ( 66851L , 73 , 2253281L ) ;
      primitive ( 66852L , 74 , 8939298L ) ;
      primitive ( 66853L , 74 , 8939299L ) ;
      primitive ( 66854L , 74 , 8939300L ) ;
      primitive ( 66855L , 74 , 8939301L ) ;
      primitive ( 66856L , 74 , 8939302L ) ;
      primitive ( 66857L , 74 , 8939303L ) ;
      primitive ( 66858L , 74 , 8939304L ) ;
      primitive ( 66859L , 74 , 8939305L ) ;
      primitive ( 66860L , 74 , 8939306L ) ;
      primitive ( 66874L , 71 , 20 ) ;
      primitive ( 66875L , 71 , 21 ) ;
      primitive ( 66876L , 71 , 22 ) ;
      primitive ( 66877L , 71 , 23 ) ;
      primitive ( 66878L , 71 , 24 ) ;
      primitive ( 66879L , 71 , 56 ) ;
      primitive ( 66880L , 71 , 57 ) ;
      primitive ( 66881L , 71 , 58 ) ;
      primitive ( 66882L , 71 , 59 ) ;
      primitive ( 66883L , 71 , 60 ) ;
      primitive ( 66884L , 71 , 61 ) ;
      primitive ( 66885L , 71 , 62 ) ;
      primitive ( 66886L , 19 , 4 ) ;
      primitive ( 66888L , 19 , 5 ) ;
      primitive ( 66889L , 111 , 1 ) ;
      primitive ( 66890L , 111 , 5 ) ;
      primitive ( 66891L , 19 , 6 ) ;
      primitive ( 66895L , 83 , 2 ) ;
      primitive ( 66277L , 49 , 1 ) ;
      primitive ( 66899L , 74 , 8939307L ) ;
      primitive ( 66900L , 74 , 8939311L ) ;
      primitive ( 66901L , 74 , 8939313L ) ;
      primitive ( 66902L , 74 , 8939314L ) ;
      primitive ( 66903L , 74 , 8939315L ) ;
      primitive ( 66904L , 74 , 8939312L ) ;
      primitive ( 66905L , 74 , 8939316L ) ;
      primitive ( 66906L , 74 , 8939319L ) ;
      primitive ( 66907L , 74 , 8939320L ) ;
      primitive ( 66908L , 74 , 8939321L ) ;
      primitive ( 66909L , 74 , 8939322L ) ;
      primitive ( 66751L , 59 , 44 ) ;
      primitive ( 66752L , 59 , 45 ) ;
      primitive ( 66910L , 33 , 6 ) ;
      primitive ( 66911L , 33 , 7 ) ;
      primitive ( 66912L , 33 , 10 ) ;
      primitive ( 66913L , 33 , 11 ) ;
      primitive ( 66922L , 106 , 2 ) ;
      primitive ( 66924L , 98 , 1 ) ;
      primitive ( 66157L , 104 , 1 ) ;
      primitive ( 66925L , 107 , 17 ) ;
      primitive ( 66926L , 107 , 18 ) ;
      primitive ( 66927L , 107 , 19 ) ;
      primitive ( 66928L , 107 , 20 ) ;
      primitive ( 66613L , 95 , 8 ) ;
      primitive ( 66934L , 71 , 67 ) ;
      primitive ( 66935L , 71 , 68 ) ;
      primitive ( 66936L , 71 , 69 ) ;
      primitive ( 66937L , 71 , 70 ) ;
      primitive ( 66941L , 71 , 25 ) ;
      primitive ( 66942L , 71 , 26 ) ;
      primitive ( 66943L , 71 , 63 ) ;
      primitive ( 66944L , 71 , 64 ) ;
      primitive ( 66945L , 71 , 65 ) ;
      primitive ( 66946L , 71 , 66 ) ;
      primitive ( 66947L , 18 , 5 ) ;
      primitive ( 66948L , 112 , 5 ) ;
      primitive ( 66949L , 112 , 6 ) ;
      primitive ( 66950L , 112 , 7 ) ;
      primitive ( 66951L , 112 , 8 ) ;
      primitive ( 66952L , 112 , 9 ) ;
      primitive ( 66957L , 24 , 2 ) ;
      primitive ( 66958L , 24 , 3 ) ;
      primitive ( 66959L , 85 , 2253539L ) ;
      primitive ( 66960L , 85 , 2253540L ) ;
      primitive ( 66961L , 85 , 2253541L ) ;
      primitive ( 66962L , 85 , 2253542L ) ;
      if ( buffer [curinput .locfield ]== 42 ) 
      incr ( curinput .locfield ) ;
      eTeXmode = 1 ;
      maxregnum = 32767 ;
      maxreghelpline = 66954L ;
    } 
#endif /* INITEX */
    if ( ! nonewcontrolsequence ) 
    nonewcontrolsequence = true ;
    else if ( ( formatident == 0 ) || ( buffer [curinput .locfield ]== 38 ) 
    || dumpline ) 
    {
      if ( formatident != 0 ) 
      initialize () ;
      if ( ! openfmtfile () ) 
      goto lab9999 ;
      if ( ! loadfmtfile () ) 
      {
	wclose ( fmtfile ) ;
	goto lab9999 ;
      } 
      wclose ( fmtfile ) ;
      eqtb = zeqtb ;
      while ( ( curinput .locfield < curinput .limitfield ) && ( buffer [
      curinput .locfield ]== 32 ) ) incr ( curinput .locfield ) ;
    } 
    if ( ( eTeXmode == 1 ) ) 
    fprintf ( stdout , "%s\n",  "entering extended mode" ) ;
    if ( ( eqtb [8939288L ].cint < 0 ) || ( eqtb [8939288L ].cint > 255 ) 
    ) 
    decr ( curinput .limitfield ) ;
    else buffer [curinput .limitfield ]= eqtb [8939288L ].cint ;
    if ( mltexenabledp ) 
    {
      fprintf ( stdout , "%s\n",  "MLTeX v2.2 enabled" ) ;
    } 
    dateandtime ( eqtb [8939260L ].cint , eqtb [8939261L ].cint , eqtb [
    8939262L ].cint , eqtb [8939263L ].cint ) ;
	;
#ifdef INITEX
    if ( trienotready ) 
    {
      trietrl = xmallocarray ( triepointer , triesize ) ;
      trietro = xmallocarray ( triepointer , triesize ) ;
      trietrc = xmallocarray ( quarterword , triesize ) ;
      triec = xmallocarray ( packedUTF16code , triesize ) ;
      trieo = xmallocarray ( trieopcode , triesize ) ;
      triel = xmallocarray ( triepointer , triesize ) ;
      trier = xmallocarray ( triepointer , triesize ) ;
      triehash = xmallocarray ( triepointer , triesize ) ;
      trietaken = xmallocarray ( boolean , triesize ) ;
      triel [0 ]= 0 ;
      triec [0 ]= 0 ;
      trieptr = 0 ;
      trier [0 ]= 0 ;
      hyphstart = 0 ;
      fontmapping = xmallocarray ( voidpointer , fontmax ) ;
      fontlayoutengine = xmallocarray ( voidpointer , fontmax ) ;
      fontflags = xmallocarray ( char , fontmax ) ;
      fontletterspace = xmallocarray ( scaled , fontmax ) ;
      fontcheck = xmallocarray ( fourquarters , fontmax ) ;
      fontsize = xmallocarray ( scaled , fontmax ) ;
      fontdsize = xmallocarray ( scaled , fontmax ) ;
      fontparams = xmallocarray ( fontindex , fontmax ) ;
      fontname = xmallocarray ( strnumber , fontmax ) ;
      fontarea = xmallocarray ( strnumber , fontmax ) ;
      fontbc = xmallocarray ( UTF16code , fontmax ) ;
      fontec = xmallocarray ( UTF16code , fontmax ) ;
      fontglue = xmallocarray ( halfword , fontmax ) ;
      hyphenchar = xmallocarray ( integer , fontmax ) ;
      skewchar = xmallocarray ( integer , fontmax ) ;
      bcharlabel = xmallocarray ( fontindex , fontmax ) ;
      fontbchar = xmallocarray ( ninebits , fontmax ) ;
      fontfalsebchar = xmallocarray ( ninebits , fontmax ) ;
      charbase = xmallocarray ( integer , fontmax ) ;
      widthbase = xmallocarray ( integer , fontmax ) ;
      heightbase = xmallocarray ( integer , fontmax ) ;
      depthbase = xmallocarray ( integer , fontmax ) ;
      italicbase = xmallocarray ( integer , fontmax ) ;
      ligkernbase = xmallocarray ( integer , fontmax ) ;
      kernbase = xmallocarray ( integer , fontmax ) ;
      extenbase = xmallocarray ( integer , fontmax ) ;
      parambase = xmallocarray ( integer , fontmax ) ;
      fontptr = 0 ;
      fmemptr = 7 ;
      fontname [0 ]= 66186L ;
      fontarea [0 ]= 65626L ;
      hyphenchar [0 ]= 45 ;
      skewchar [0 ]= -1 ;
      bcharlabel [0 ]= 0 ;
      fontbchar [0 ]= 65536L ;
      fontfalsebchar [0 ]= 65536L ;
      fontbc [0 ]= 1 ;
      fontec [0 ]= 0 ;
      fontsize [0 ]= 0 ;
      fontdsize [0 ]= 0 ;
      charbase [0 ]= 0 ;
      widthbase [0 ]= 0 ;
      heightbase [0 ]= 0 ;
      depthbase [0 ]= 0 ;
      italicbase [0 ]= 0 ;
      ligkernbase [0 ]= 0 ;
      kernbase [0 ]= 0 ;
      extenbase [0 ]= 0 ;
      fontglue [0 ]= -268435455L ;
      fontparams [0 ]= 7 ;
      fontmapping [0 ]= 0 ;
      parambase [0 ]= -1 ;
      {register integer for_end; fontk = 0 ;for_end = 6 ; if ( fontk <= 
      for_end) do 
	fontinfo [fontk ].cint = 0 ;
      while ( fontk++ < for_end ) ;} 
    } 
#endif /* INITEX */
    fontused = xmallocarray ( boolean , fontmax ) ;
    {register integer for_end; fontk = 0 ;for_end = fontmax ; if ( fontk <= 
    for_end) do 
      fontused [fontk ]= false ;
    while ( fontk++ < for_end ) ;} 
    randomseed = ( microseconds * 1000 ) + ( epochseconds % 1000000L ) ;
    initrandoms ( randomseed ) ;
    magicoffset = strstart [( 66309L ) - 65536L ]- 9 * 16 ;
    if ( interaction == 0 ) 
    selector = 16 ;
    else selector = 17 ;
    if ( ( curinput .locfield < curinput .limitfield ) && ( eqtb [2254568L + 
    buffer [curinput .locfield ]].hh .v.RH != 0 ) ) 
    startinput () ;
  } 
  history = 0 ;
  maincontrol () ;
  finalcleanup () ;
  closefilesandterminate () ;
  lab9999: {
      
    fflush ( stdout ) ;
    readyalready = 0 ;
    if ( ( history != 0 ) && ( history != 1 ) ) 
    uexit ( 1 ) ;
    else uexit ( 0 ) ;
  } 
} 
