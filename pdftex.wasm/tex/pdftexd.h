#undef TRIP
#undef TRAP
#define STAT
#define INI
#define INITEX
#define TeX
#define pdfTeX
#ifdef TEXMF_DEBUG
#endif /* TEXMF_DEBUG */
#define PDFTEXCOERCE
#include "texmfmp.h"
#define hashoffset ( 514 ) 
#define trieopsize ( 35111L ) 
#define negtrieopsize ( -35111L ) 
#define mintrieop ( 0 ) 
#define maxtrieop ( 65535L ) 
#define poolname ( TEXMFPOOLNAME ) 
#define enginename ( TEXMFENGINENAME ) 
#define infmembot ( 0 ) 
#define supmembot ( 1 ) 
#define infmainmemory ( 3000 ) 
#define supmainmemory ( 256000000L ) 
#define inftriesize ( 8000 ) 
#define suptriesize ( 4194303L ) 
#define infmaxstrings ( 3000 ) 
#define supmaxstrings ( 2097151L ) 
#define infstringsfree ( 100 ) 
#define supstringsfree ( supmaxstrings ) 
#define infbufsize ( 500 ) 
#define supbufsize ( 30000000L ) 
#define infnestsize ( 40 ) 
#define supnestsize ( 4000 ) 
#define infmaxinopen ( 6 ) 
#define supmaxinopen ( 127 ) 
#define infparamsize ( 60 ) 
#define supparamsize ( 32767 ) 
#define infsavesize ( 600 ) 
#define supsavesize ( 80000L ) 
#define infstacksize ( 200 ) 
#define supstacksize ( 30000 ) 
#define infdvibufsize ( 800 ) 
#define supdvibufsize ( 65536L ) 
#define inffontmemsize ( 20000 ) 
#define supfontmemsize ( 147483647L ) 
#define supfontmax ( 9000 ) 
#define inffontmax ( 50 ) 
#define infpoolsize ( 32000 ) 
#define suppoolsize ( 40000000L ) 
#define infpoolfree ( 1000 ) 
#define suppoolfree ( suppoolsize ) 
#define infstringvacancies ( 8000 ) 
#define supstringvacancies ( suppoolsize - 23000 ) 
#define suphashextra ( supmaxstrings ) 
#define infhashextra ( 0 ) 
#define suphyphsize ( 65535L ) 
#define infhyphsize ( 610 ) 
#define infexpanddepth ( 10 ) 
#define supexpanddepth ( 10000000L ) 
#define infpdfmemsize ( 10000 ) 
#define suppdfmemsize ( 10000000L ) 
#define pdfopbufsize ( 16384 ) 
#define infpdfosbufsize ( 1 ) 
#define suppdfosbufsize ( 5000000L ) 
#define pdfosmaxobjs ( 100 ) 
#define infobjtabsize ( 1000 ) 
#define supobjtabsize ( 8388607L ) 
#define infdestnamessize ( 1000 ) 
#define supdestnamessize ( 500000L ) 
#define infpkdpi ( 72 ) 
#define suppkdpi ( 8000 ) 
#define pdfobjtypemax ( 9 ) 
#define vfmaxrecursion ( 10 ) 
#define vfstacksize ( 100 ) 
#define pdfmaxlinklevel ( 10 ) 
typedef unsigned char ASCIIcode  ;
typedef unsigned char eightbits  ;
typedef text /* of  ASCIIcode */ alphafile  ;
typedef text /* of  eightbits */ bytefile  ;
typedef integer poolpointer  ;
typedef integer strnumber  ;
typedef unsigned char packedASCIIcode  ;
typedef integer scaled  ;
typedef integer nonnegativeinteger  ;
typedef unsigned char smallnumber  ;
typedef unsigned char quarterword  ;
typedef integer halfword  ;
typedef unsigned char twochoices  ;
typedef unsigned char fourchoices  ;
#include "texmfmem.h"
typedef text wordfile  ;
typedef unsigned char glueord  ;
typedef struct {
    short modefield ;
  halfword headfield, tailfield ;
  halfword eTeXauxfield ;
  integer pgfield, mlfield ;
  memoryword auxfield ;
} liststaterecord  ;
typedef unsigned char groupcode  ;
typedef struct {
    quarterword statefield, indexfield ;
  halfword startfield, locfield, limitfield, namefield ;
} instaterecord  ;
typedef integer internalfontnumber  ;
typedef integer fontindex  ;
typedef short ninebits  ;
typedef integer dviindex  ;
typedef struct {
    integer int0, int1 ;
  longinteger int2 ;
  integer int3, int4 ;
} objentry  ;
typedef eightbits charusedarray [32] ;
typedef eightbits charmaparray [33] ;
typedef integer * fmentryptr  ;
typedef integer vfstackindex  ;
typedef struct {
    scaled stackh, stackv, stackw, stackx, stacky, stackz ;
} vfstackrecord  ;
typedef integer triepointer  ;
typedef unsigned short trieopcode  ;
typedef unsigned short hyphpointer  ;
typedef struct {
    strnumber objname ;
  integer objnum ;
} destnameentry  ;
typedef struct {
    integer nestinglevel ;
  halfword linknode ;
  halfword reflinknode ;
} pdflinkstackrecord  ;
typedef integer savepointer  ;
EXTERN integer bad  ;
EXTERN ASCIIcode xord[256]  ;
EXTERN ASCIIcode xchr[256]  ;
EXTERN ASCIIcode xprn[256]  ;
EXTERN halfword mubyteread[256]  ;
EXTERN strnumber mubytewrite[256]  ;
EXTERN halfword mubytecswrite[128]  ;
EXTERN integer mubyteskip  ;
EXTERN integer mubytekeep  ;
EXTERN integer mubyteskeep  ;
EXTERN integer mubyteprefix  ;
EXTERN boolean mubytetablein  ;
EXTERN boolean mubytetableout  ;
EXTERN boolean mubyterelax  ;
EXTERN boolean mubytestart  ;
EXTERN boolean mubytesstart  ;
EXTERN halfword mubytetoken  ;
EXTERN halfword mubytestoken  ;
EXTERN integer mubytesout  ;
EXTERN integer mubyteslog  ;
EXTERN integer specsout  ;
EXTERN boolean noconvert  ;
EXTERN boolean activenoconvert  ;
EXTERN boolean writenoexpanding  ;
EXTERN boolean csconverting  ;
EXTERN boolean specialprinting  ;
EXTERN boolean messageprinting  ;
EXTERN ASCIIcode * nameoffile  ;
EXTERN integer namelength  ;
EXTERN ASCIIcode * buffer  ;
EXTERN integer first  ;
EXTERN integer last  ;
EXTERN integer maxbufstack  ;
#ifdef INITEX
EXTERN boolean iniversion  ;
EXTERN boolean dumpoption  ;
EXTERN boolean dumpline  ;
#endif /* INITEX */
EXTERN constcstring dumpname  ;
EXTERN integer bounddefault  ;
EXTERN constcstring boundname  ;
EXTERN integer membot  ;
EXTERN integer mainmemory  ;
EXTERN integer extramembot  ;
EXTERN integer memmin  ;
EXTERN integer memtop  ;
EXTERN integer extramemtop  ;
EXTERN integer memmax  ;
EXTERN integer errorline  ;
EXTERN integer halferrorline  ;
EXTERN integer maxprintline  ;
EXTERN integer maxstrings  ;
EXTERN integer stringsfree  ;
EXTERN integer stringvacancies  ;
EXTERN integer poolsize  ;
EXTERN integer poolfree  ;
EXTERN integer fontmemsize  ;
EXTERN integer fontmax  ;
EXTERN integer fontk  ;
EXTERN integer hyphsize  ;
EXTERN integer triesize  ;
EXTERN integer bufsize  ;
EXTERN integer stacksize  ;
EXTERN integer maxinopen  ;
EXTERN integer paramsize  ;
EXTERN integer nestsize  ;
EXTERN integer savesize  ;
EXTERN integer dvibufsize  ;
EXTERN integer expanddepth  ;
EXTERN cinttype parsefirstlinep  ;
EXTERN cinttype filelineerrorstylep  ;
EXTERN cinttype eightbitp  ;
EXTERN cinttype haltonerrorp  ;
EXTERN boolean quotedfilename  ;
EXTERN boolean srcspecialsp  ;
EXTERN boolean insertsrcspecialauto  ;
EXTERN boolean insertsrcspecialeverypar  ;
EXTERN boolean insertsrcspecialeveryparend  ;
EXTERN boolean insertsrcspecialeverycr  ;
EXTERN boolean insertsrcspecialeverymath  ;
EXTERN boolean insertsrcspecialeveryhbox  ;
EXTERN boolean insertsrcspecialeveryvbox  ;
EXTERN boolean insertsrcspecialeverydisplay  ;
EXTERN packedASCIIcode * strpool  ;
EXTERN poolpointer * strstart  ;
EXTERN poolpointer poolptr  ;
EXTERN strnumber strptr  ;
EXTERN poolpointer initpoolptr  ;
EXTERN strnumber initstrptr  ;
#ifdef INITEX
EXTERN alphafile poolfile  ;
#endif /* INITEX */
EXTERN alphafile logfile  ;
EXTERN unsigned char selector  ;
EXTERN unsigned char dig[23]  ;
EXTERN integer tally  ;
EXTERN integer termoffset  ;
EXTERN integer fileoffset  ;
EXTERN ASCIIcode trickbuf[256]  ;
EXTERN integer trickcount  ;
EXTERN integer firstcount  ;
EXTERN unsigned char interaction  ;
EXTERN unsigned char interactionoption  ;
EXTERN boolean deletionsallowed  ;
EXTERN boolean setboxallowed  ;
EXTERN unsigned char history  ;
EXTERN schar errorcount  ;
EXTERN strnumber helpline[6]  ;
EXTERN unsigned char helpptr  ;
EXTERN boolean useerrhelp  ;
EXTERN integer interrupt  ;
EXTERN boolean OKtointerrupt  ;
EXTERN boolean aritherror  ;
EXTERN scaled texremainder  ;
EXTERN integer randoms[55]  ;
EXTERN unsigned char jrandom  ;
EXTERN scaled randomseed  ;
EXTERN integer twotothe[31]  ;
EXTERN integer speclog[29]  ;
EXTERN halfword tempptr  ;
EXTERN memoryword * yzmem  ;
EXTERN memoryword * zmem  ;
EXTERN halfword lomemmax  ;
EXTERN halfword himemmin  ;
EXTERN integer varused, dynused  ;
EXTERN halfword avail  ;
EXTERN halfword memend  ;
EXTERN halfword rover  ;
#ifdef TEXMF_DEBUG
EXTERN boolean freearr[10]  ;
EXTERN boolean wasfree[10]  ;
EXTERN halfword wasmemend, waslomax, washimin  ;
EXTERN boolean panicking  ;
#endif /* TEXMF_DEBUG */
EXTERN integer fontinshortdisplay  ;
EXTERN integer depththreshold  ;
EXTERN integer breadthmax  ;
EXTERN liststaterecord * nest  ;
EXTERN integer nestptr  ;
EXTERN integer maxneststack  ;
EXTERN liststaterecord curlist  ;
EXTERN short shownmode  ;
EXTERN halfword savetail  ;
EXTERN unsigned char oldsetting  ;
EXTERN memoryword * zeqtb  ;
EXTERN quarterword 
#define xeqlevel (zzzaa -29277)
  zzzaa[908]  ;
EXTERN twohalves * hash  ;
EXTERN twohalves * yhash  ;
EXTERN halfword hashused  ;
EXTERN halfword hashextra  ;
EXTERN halfword hashtop  ;
EXTERN halfword eqtbtop  ;
EXTERN halfword hashhigh  ;
EXTERN boolean nonewcontrolsequence  ;
EXTERN integer cscount  ;
EXTERN twohalves prim[2101]  ;
EXTERN halfword primused  ;
EXTERN memoryword * savestack  ;
EXTERN integer saveptr  ;
EXTERN integer maxsavestack  ;
EXTERN quarterword curlevel  ;
EXTERN groupcode curgroup  ;
EXTERN integer curboundary  ;
EXTERN integer magset  ;
EXTERN eightbits curcmd  ;
EXTERN halfword curchr  ;
EXTERN halfword curcs  ;
EXTERN halfword curtok  ;
EXTERN instaterecord * inputstack  ;
EXTERN integer inputptr  ;
EXTERN integer maxinstack  ;
EXTERN instaterecord curinput  ;
EXTERN integer inopen  ;
EXTERN integer openparens  ;
EXTERN alphafile * inputfile  ;
EXTERN integer line  ;
EXTERN integer * linestack  ;
EXTERN strnumber * sourcefilenamestack  ;
EXTERN strnumber * fullsourcefilenamestack  ;
EXTERN unsigned char scannerstatus  ;
EXTERN halfword warningindex  ;
EXTERN halfword defref  ;
EXTERN halfword * paramstack  ;
EXTERN integer paramptr  ;
EXTERN integer maxparamstack  ;
EXTERN integer alignstate  ;
EXTERN integer baseptr  ;
EXTERN halfword parloc  ;
EXTERN halfword partoken  ;
EXTERN boolean forceeof  ;
EXTERN boolean isincsname  ;
EXTERN halfword curmark[5]  ;
EXTERN unsigned char longstate  ;
EXTERN halfword pstack[9]  ;
EXTERN integer curval  ;
EXTERN unsigned char curvallevel  ;
EXTERN smallnumber radix  ;
EXTERN glueord curorder  ;
EXTERN alphafile readfile[16]  ;
EXTERN unsigned char readopen[17]  ;
EXTERN halfword condptr  ;
EXTERN unsigned char iflimit  ;
EXTERN smallnumber curif  ;
EXTERN integer ifline  ;
EXTERN integer skipline  ;
EXTERN strnumber curname  ;
EXTERN strnumber curarea  ;
EXTERN strnumber curext  ;
EXTERN poolpointer areadelimiter  ;
EXTERN poolpointer extdelimiter  ;
EXTERN integer formatdefaultlength  ;
EXTERN cstring TEXformatdefault  ;
EXTERN boolean nameinprogress  ;
EXTERN strnumber jobname  ;
EXTERN boolean logopened  ;
EXTERN bytefile dvifile  ;
EXTERN strnumber outputfilename  ;
EXTERN strnumber texmflogname  ;
EXTERN bytefile tfmfile  ;
EXTERN fmemoryword * fontinfo  ;
EXTERN fontindex fmemptr  ;
EXTERN internalfontnumber fontptr  ;
EXTERN fourquarters * fontcheck  ;
EXTERN scaled * fontsize  ;
EXTERN scaled * fontdsize  ;
EXTERN fontindex * fontparams  ;
EXTERN strnumber * fontname  ;
EXTERN strnumber * fontarea  ;
EXTERN eightbits * fontbc  ;
EXTERN eightbits * fontec  ;
EXTERN halfword * fontglue  ;
EXTERN boolean * fontused  ;
EXTERN integer * hyphenchar  ;
EXTERN integer * skewchar  ;
EXTERN fontindex * bcharlabel  ;
EXTERN ninebits * fontbchar  ;
EXTERN ninebits * fontfalsebchar  ;
EXTERN integer * charbase  ;
EXTERN integer * widthbase  ;
EXTERN integer * heightbase  ;
EXTERN integer * depthbase  ;
EXTERN integer * italicbase  ;
EXTERN integer * ligkernbase  ;
EXTERN integer * kernbase  ;
EXTERN integer * extenbase  ;
EXTERN integer * parambase  ;
EXTERN fourquarters nullcharacter  ;
EXTERN integer totalpages  ;
EXTERN scaled maxv  ;
EXTERN scaled maxh  ;
EXTERN integer maxpush  ;
EXTERN integer lastbop  ;
EXTERN integer deadcycles  ;
EXTERN boolean doingleaders  ;
EXTERN quarterword c  ;
EXTERN internalfontnumber f  ;
EXTERN scaled ruleht, ruledp, rulewd  ;
EXTERN halfword g  ;
EXTERN integer lq, lr  ;
EXTERN eightbits * dvibuf  ;
EXTERN integer halfbuf  ;
EXTERN integer dvilimit  ;
EXTERN integer dviptr  ;
EXTERN integer dvioffset  ;
EXTERN integer dvigone  ;
EXTERN halfword downptr, rightptr  ;
EXTERN scaled dvih, dviv  ;
EXTERN scaled curh, curv  ;
EXTERN internalfontnumber dvif  ;
EXTERN integer curs  ;
EXTERN integer pdfmemsize  ;
EXTERN integer * pdfmem  ;
EXTERN integer pdfmemptr  ;
EXTERN bytefile pdffile  ;
EXTERN eightbits * pdfbuf  ;
EXTERN integer pdfbufsize  ;
EXTERN integer pdfptr  ;
EXTERN eightbits * pdfopbuf  ;
EXTERN eightbits * pdfosbuf  ;
EXTERN integer pdfosbufsize  ;
EXTERN integer * pdfosobjnum  ;
EXTERN integer * pdfosobjoff  ;
EXTERN halfword pdfosobjidx  ;
EXTERN integer pdfoscntr  ;
EXTERN integer pdfopptr  ;
EXTERN integer pdfosptr  ;
EXTERN boolean pdfosmode  ;
EXTERN boolean pdfosenable  ;
EXTERN integer pdfoscurobjnum  ;
EXTERN longinteger pdfgone  ;
EXTERN longinteger pdfsaveoffset  ;
EXTERN integer zipwritestate  ;
EXTERN integer fixedpdfmajorversion  ;
EXTERN integer fixedpdfminorversion  ;
EXTERN integer fixedpdfobjcompresslevel  ;
EXTERN boolean pdfversionwritten  ;
EXTERN integer fixedpdfoutput  ;
EXTERN boolean fixedpdfoutputset  ;
EXTERN integer fixedgamma  ;
EXTERN integer fixedimagegamma  ;
EXTERN boolean fixedimagehicolor  ;
EXTERN integer fixedimageapplygamma  ;
EXTERN integer epochseconds  ;
EXTERN integer microseconds  ;
EXTERN integer fixedpdfdraftmode  ;
EXTERN boolean fixedpdfdraftmodeset  ;
EXTERN integer pdfpagegroupval  ;
EXTERN scaled onebp  ;
EXTERN scaled onehundredbp  ;
EXTERN scaled onehundredinch  ;
EXTERN integer tenpow[10]  ;
EXTERN integer scaledout  ;
EXTERN boolean initpdfoutput  ;
EXTERN internalfontnumber pdff  ;
EXTERN scaled pdfh  ;
EXTERN scaled pdfv  ;
EXTERN scaled pdftjstarth  ;
EXTERN scaled curdeltah  ;
EXTERN scaled pdfdeltah  ;
EXTERN scaled pdforiginh  ;
EXTERN scaled pdforiginv  ;
EXTERN boolean pdfdoingstring  ;
EXTERN boolean pdfdoingtext  ;
EXTERN scaled minbpval  ;
EXTERN scaled minfontval  ;
EXTERN integer fixedpkresolution  ;
EXTERN integer fixeddecimaldigits  ;
EXTERN integer fixedgentounicode  ;
EXTERN integer fixedinclusioncopyfont  ;
EXTERN integer pkscalefactor  ;
EXTERN integer pdfoutputoption  ;
EXTERN integer pdfoutputvalue  ;
EXTERN integer pdfdraftmodeoption  ;
EXTERN integer pdfdraftmodevalue  ;
EXTERN integer pdfcurTma  ;
EXTERN internalfontnumber pdflastf  ;
EXTERN internalfontnumber pdflastfs  ;
EXTERN internalfontnumber pdfdummyfont  ;
EXTERN integer objtabsize  ;
EXTERN objentry * objtab  ;
EXTERN integer headtab[10]  ;
EXTERN integer pagestail  ;
EXTERN integer objptr  ;
EXTERN integer sysobjptr  ;
EXTERN integer pdflastpages  ;
EXTERN integer pdflastpage  ;
EXTERN integer pdflaststream  ;
EXTERN longinteger pdfstreamlength  ;
EXTERN longinteger pdfstreamlengthoffset  ;
EXTERN boolean pdfseekwritelength  ;
EXTERN eightbits pdflastbyte  ;
EXTERN integer pdfappendlistarg  ;
EXTERN integer ff  ;
EXTERN integer pdfboxspecmedia  ;
EXTERN integer pdfboxspeccrop  ;
EXTERN integer pdfboxspecbleed  ;
EXTERN integer pdfboxspectrim  ;
EXTERN integer pdfboxspecart  ;
EXTERN integer pdfimageprocset  ;
EXTERN boolean pdftextprocset  ;
EXTERN eightbits * pdffonttype  ;
EXTERN strnumber * pdffontattr  ;
EXTERN boolean * pdffontnobuiltintounicode  ;
EXTERN charusedarray * pdfcharused  ;
EXTERN scaled * pdffontsize  ;
EXTERN integer * pdffontnum  ;
EXTERN fmentryptr * pdffontmap  ;
EXTERN halfword pdffontlist  ;
EXTERN strnumber pdfresnameprefix  ;
EXTERN strnumber lasttokensstring  ;
EXTERN integer * vfpacketbase  ;
EXTERN internalfontnumber * vfdefaultfont  ;
EXTERN internalfontnumber * vflocalfontnum  ;
EXTERN integer vfpacketlength  ;
EXTERN bytefile vffile  ;
EXTERN internalfontnumber vfnf  ;
EXTERN integer * vfefnts  ;
EXTERN internalfontnumber * vfifnts  ;
EXTERN memoryword tmpw  ;
EXTERN integer vfcurs  ;
EXTERN vfstackrecord vfstack[vfstacksize + 1]  ;
EXTERN vfstackindex vfstackptr  ;
EXTERN integer savedpdfcurform  ;
EXTERN strnumber pdftexbanner  ;
EXTERN scaled totalstretch[4], totalshrink[4]  ;
EXTERN integer lastbadness  ;
EXTERN halfword adjusttail  ;
EXTERN internalfontnumber * pdffontblink  ;
EXTERN internalfontnumber * pdffontelink  ;
EXTERN integer * pdffontstretch  ;
EXTERN integer * pdffontshrink  ;
EXTERN integer * pdffontstep  ;
EXTERN integer * pdffontexpandratio  ;
EXTERN boolean * pdffontautoexpand  ;
EXTERN integer * pdffontlpbase  ;
EXTERN integer * pdffontrpbase  ;
EXTERN integer * pdffontefbase  ;
EXTERN integer * pdffontknbsbase  ;
EXTERN integer * pdffontstbsbase  ;
EXTERN integer * pdffontshbsbase  ;
EXTERN integer * pdffontknbcbase  ;
EXTERN integer * pdffontknacbase  ;
EXTERN integer fontexpandratio  ;
EXTERN halfword lastleftmostchar  ;
EXTERN halfword lastrightmostchar  ;
EXTERN halfword hliststack[513]  ;
EXTERN short hliststacklevel  ;
EXTERN halfword preadjusttail  ;
EXTERN integer packbeginline  ;
EXTERN twohalves emptyfield  ;
EXTERN fourquarters nulldelimiter  ;
EXTERN halfword curmlist  ;
EXTERN smallnumber curstyle  ;
EXTERN smallnumber cursize  ;
EXTERN scaled curmu  ;
EXTERN boolean mlistpenalties  ;
EXTERN internalfontnumber curf  ;
EXTERN quarterword curc  ;
EXTERN fourquarters curi  ;
EXTERN integer magicoffset  ;
EXTERN halfword curalign  ;
EXTERN halfword curspan  ;
EXTERN halfword curloop  ;
EXTERN halfword alignptr  ;
EXTERN halfword curhead, curtail  ;
EXTERN halfword curprehead, curpretail  ;
EXTERN halfword justbox  ;
EXTERN halfword passive  ;
EXTERN halfword printednode  ;
EXTERN halfword passnumber  ;
EXTERN scaled activewidth[9]  ;
EXTERN scaled curactivewidth[9]  ;
EXTERN scaled background[9]  ;
EXTERN scaled breakwidth[9]  ;
EXTERN boolean autobreaking  ;
EXTERN halfword prevp  ;
EXTERN halfword firstp  ;
EXTERN halfword prevcharp  ;
EXTERN halfword nextcharp  ;
EXTERN boolean tryprevbreak  ;
EXTERN halfword prevlegal  ;
EXTERN halfword prevprevlegal  ;
EXTERN boolean prevautobreaking  ;
EXTERN scaled prevactivewidth[9]  ;
EXTERN halfword rejectedcurp  ;
EXTERN boolean beforerejectedcurp  ;
EXTERN integer maxstretchratio  ;
EXTERN integer maxshrinkratio  ;
EXTERN integer curfontstep  ;
EXTERN boolean noshrinkerroryet  ;
EXTERN halfword curp  ;
EXTERN boolean secondpass  ;
EXTERN boolean finalpass  ;
EXTERN integer threshold  ;
EXTERN integer minimaldemerits[4]  ;
EXTERN integer minimumdemerits  ;
EXTERN halfword bestplace[4]  ;
EXTERN halfword bestplline[4]  ;
EXTERN scaled discwidth[9]  ;
EXTERN halfword easyline  ;
EXTERN halfword lastspecialline  ;
EXTERN scaled firstwidth  ;
EXTERN scaled secondwidth  ;
EXTERN scaled firstindent  ;
EXTERN scaled secondindent  ;
EXTERN halfword bestbet  ;
EXTERN integer fewestdemerits  ;
EXTERN halfword bestline  ;
EXTERN integer actuallooseness  ;
EXTERN integer linediff  ;
EXTERN short hc[66]  ;
EXTERN smallnumber hn  ;
EXTERN halfword ha, hb  ;
EXTERN internalfontnumber hf  ;
EXTERN short hu[64]  ;
EXTERN integer hyfchar  ;
EXTERN ASCIIcode curlang, initcurlang  ;
EXTERN integer lhyf, rhyf, initlhyf, initrhyf  ;
EXTERN halfword hyfbchar  ;
EXTERN unsigned char hyf[65]  ;
EXTERN halfword initlist  ;
EXTERN boolean initlig  ;
EXTERN boolean initlft  ;
EXTERN smallnumber hyphenpassed  ;
EXTERN halfword curl, curr  ;
EXTERN halfword curq  ;
EXTERN halfword ligstack  ;
EXTERN boolean ligaturepresent  ;
EXTERN boolean lfthit, rthit  ;
EXTERN triepointer * trietrl  ;
EXTERN triepointer * trietro  ;
EXTERN quarterword * trietrc  ;
EXTERN smallnumber hyfdistance[trieopsize + 1]  ;
EXTERN smallnumber hyfnum[trieopsize + 1]  ;
EXTERN trieopcode hyfnext[trieopsize + 1]  ;
EXTERN integer opstart[256]  ;
EXTERN strnumber * hyphword  ;
EXTERN halfword * hyphlist  ;
EXTERN hyphpointer * hyphlink  ;
EXTERN integer hyphcount  ;
EXTERN integer hyphnext  ;
#ifdef INITEX
EXTERN integer 
#define trieophash (zzzab - (int)(negtrieopsize))
  zzzab[trieopsize - negtrieopsize + 1]  ;
EXTERN trieopcode trieused[256]  ;
EXTERN ASCIIcode trieoplang[trieopsize + 1]  ;
EXTERN trieopcode trieopval[trieopsize + 1]  ;
EXTERN integer trieopptr  ;
#endif /* INITEX */
EXTERN trieopcode maxopused  ;
EXTERN boolean smallop  ;
#ifdef INITEX
EXTERN packedASCIIcode * triec  ;
EXTERN trieopcode * trieo  ;
EXTERN triepointer * triel  ;
EXTERN triepointer * trier  ;
EXTERN triepointer trieptr  ;
EXTERN triepointer * triehash  ;
#endif /* INITEX */
#ifdef INITEX
EXTERN boolean * trietaken  ;
EXTERN triepointer triemin[256]  ;
EXTERN triepointer triemax  ;
EXTERN boolean trienotready  ;
#endif /* INITEX */
EXTERN scaled bestheightplusdepth  ;
EXTERN halfword pagetail  ;
EXTERN unsigned char pagecontents  ;
EXTERN scaled pagemaxdepth  ;
EXTERN halfword bestpagebreak  ;
EXTERN integer leastpagecost  ;
EXTERN scaled bestsize  ;
EXTERN scaled pagesofar[8]  ;
EXTERN halfword lastglue  ;
EXTERN integer lastpenalty  ;
EXTERN scaled lastkern  ;
EXTERN integer lastnodetype  ;
EXTERN integer insertpenalties  ;
EXTERN boolean outputactive  ;
EXTERN internalfontnumber mainf  ;
EXTERN fourquarters maini  ;
EXTERN fourquarters mainj  ;
EXTERN fontindex maink  ;
EXTERN halfword mainp  ;
EXTERN integer mains  ;
EXTERN halfword bchar  ;
EXTERN halfword falsebchar  ;
EXTERN boolean cancelboundary  ;
EXTERN boolean insdisc  ;
EXTERN halfword curbox  ;
EXTERN halfword aftertoken  ;
EXTERN boolean longhelpseen  ;
EXTERN strnumber formatident  ;
EXTERN wordfile fmtfile  ;
EXTERN integer readyalready  ;
EXTERN alphafile writefile[16]  ;
EXTERN boolean writeopen[18]  ;
EXTERN halfword writeloc  ;
EXTERN integer pdflastobj  ;
EXTERN integer pdflastxform  ;
EXTERN integer pdflastximage  ;
EXTERN integer pdflastximagepages  ;
EXTERN integer pdflastximagecolordepth  ;
EXTERN halfword altrule  ;
EXTERN boolean warnpdfpagebox  ;
EXTERN integer pdflastannot  ;
EXTERN integer pdflastlink  ;
EXTERN integer pdflastxpos  ;
EXTERN integer pdflastypos  ;
EXTERN integer pdfsnapxrefpos  ;
EXTERN integer pdfsnapyrefpos  ;
EXTERN integer countdosnapy  ;
EXTERN integer pdfretval  ;
EXTERN scaled curpagewidth  ;
EXTERN scaled curpageheight  ;
EXTERN scaled curhoffset  ;
EXTERN scaled curvoffset  ;
EXTERN halfword pdfobjlist  ;
EXTERN halfword pdfxformlist  ;
EXTERN halfword pdfximagelist  ;
EXTERN halfword lastthread  ;
EXTERN scaled pdfthreadht, pdfthreaddp, pdfthreadwd  ;
EXTERN halfword pdflastthreadid  ;
EXTERN boolean pdflastthreadnamedid  ;
EXTERN integer pdfthreadlevel  ;
EXTERN halfword pdfannotlist  ;
EXTERN halfword pdflinklist  ;
EXTERN halfword pdfdestlist  ;
EXTERN halfword pdfbeadlist  ;
EXTERN integer pdfobjcount  ;
EXTERN integer pdfxformcount  ;
EXTERN integer pdfximagecount  ;
EXTERN integer pdfcurform  ;
EXTERN integer pdffirstoutline, pdflastoutline, pdfparentoutline  ;
EXTERN scaled pdfxformwidth, pdfxformheight, pdfxformdepth  ;
EXTERN halfword pdfinfotoks  ;
EXTERN halfword pdfcatalogtoks  ;
EXTERN integer pdfcatalogopenaction  ;
EXTERN halfword pdfnamestoks  ;
EXTERN integer pdfdestnamesptr  ;
EXTERN integer destnamessize  ;
EXTERN destnameentry * destnames  ;
EXTERN integer pkdpi  ;
EXTERN integer imageorigx, imageorigy  ;
EXTERN halfword pdftrailertoks  ;
EXTERN halfword pdftraileridtoks  ;
EXTERN boolean genfakedinterwordspace  ;
EXTERN pdflinkstackrecord pdflinkstack[pdfmaxlinklevel + 1]  ;
EXTERN smallnumber pdflinkstackptr  ;
EXTERN boolean isshippingpage  ;
EXTERN unsigned char eTeXmode  ;
EXTERN boolean etexp  ;
EXTERN boolean * eofseen  ;
EXTERN halfword LRptr  ;
EXTERN integer LRproblems  ;
EXTERN smallnumber curdir  ;
EXTERN halfword pseudofiles  ;
EXTERN savepointer * grpstack  ;
EXTERN halfword * ifstack  ;
EXTERN halfword maxregnum  ;
EXTERN strnumber maxreghelpline  ;
EXTERN halfword saroot[7]  ;
EXTERN halfword curptr  ;
EXTERN memoryword sanull  ;
EXTERN halfword sachain  ;
EXTERN quarterword salevel  ;
EXTERN halfword lastlinefill  ;
EXTERN boolean dolastlinefit  ;
EXTERN smallnumber activenodesize  ;
EXTERN scaled fillwidth[3]  ;
EXTERN scaled bestplshort[4]  ;
EXTERN scaled bestplglue[4]  ;
EXTERN triepointer hyphstart  ;
EXTERN triepointer hyphindex  ;
EXTERN halfword discptr[4]  ;
EXTERN poolpointer editnamestart  ;
EXTERN integer editnamelength, editline  ;
EXTERN cinttype ipcon  ;
EXTERN boolean stopatspace  ;
EXTERN strnumber savestrptr  ;
EXTERN poolpointer savepoolptr  ;
EXTERN cinttype shellenabledp  ;
EXTERN cinttype restrictedshell  ;
EXTERN char * outputcomment  ;
EXTERN unsigned char k, l  ;
EXTERN boolean debugformatfile  ;
EXTERN boolean mltexp  ;
EXTERN boolean mltexenabledp  ;
EXTERN integer accentc, basec, replacec  ;
EXTERN fourquarters iac, ibc  ;
EXTERN real baseslant, accentslant  ;
EXTERN scaled basexheight  ;
EXTERN scaled basewidth, baseheight  ;
EXTERN scaled accentwidth, accentheight  ;
EXTERN scaled delta  ;
EXTERN boolean enctexp  ;
EXTERN boolean enctexenabledp  ;
EXTERN integer expanddepthcount  ;

#include "pdftexcoerce.h"
