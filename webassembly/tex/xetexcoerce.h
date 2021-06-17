void initialize (void);
#define initialize_regmem register memoryword *mem=zmem, *eqtb=zeqtb;
void println (void);
#define println_regmem
void zprintrawchar (UTF16code s,boolean incroffset);
#define printrawchar(s, incroffset) zprintrawchar((UTF16code) (s), (boolean) (incroffset))
#define printrawchar_regmem
void zprintchar (integer s);
#define printchar(s) zprintchar((integer) (s))
#define printchar_regmem register memoryword *eqtb=zeqtb;
void zprint (integer s);
#define print(s) zprint((integer) (s))
#define print_regmem register memoryword *eqtb=zeqtb;
void zprintnl (strnumber s);
#define printnl(s) zprintnl((strnumber) (s))
#define printnl_regmem
void zprintesc (strnumber s);
#define printesc(s) zprintesc((strnumber) (s))
#define printesc_regmem register memoryword *eqtb=zeqtb;
void zprintthedigs (eightbits k);
#define printthedigs(k) zprintthedigs((eightbits) (k))
#define printthedigs_regmem
void zprintint (integer n);
#define printint(n) zprintint((integer) (n))
#define printint_regmem
void zprintcs (integer p);
#define printcs(p) zprintcs((integer) (p))
#define printcs_regmem register memoryword *eqtb=zeqtb;
void zsprintcs (halfword p);
#define sprintcs(p) zsprintcs((halfword) (p))
#define sprintcs_regmem
void zprintfilename (integer n,integer a,integer e);
#define printfilename(n, a, e) zprintfilename((integer) (n), (integer) (a), (integer) (e))
#define printfilename_regmem
void zprintsize (integer s);
#define printsize(s) zprintsize((integer) (s))
#define printsize_regmem
void zprintwritewhatsit (strnumber s,halfword p);
#define printwritewhatsit(s, p) zprintwritewhatsit((strnumber) (s), (halfword) (p))
#define printwritewhatsit_regmem register memoryword *mem=zmem;
void zprintnativeword (halfword p);
#define printnativeword(p) zprintnativeword((halfword) (p))
#define printnativeword_regmem register memoryword *mem=zmem;
void zprintsanum (halfword q);
#define printsanum(q) zprintsanum((halfword) (q))
#define printsanum_regmem register memoryword *mem=zmem;
void zprintcsnames (integer hstart,integer hfinish);
#define printcsnames(hstart, hfinish) zprintcsnames((integer) (hstart), (integer) (hfinish))
#define printcsnames_regmem
void printfileline (void);
#define printfileline_regmem
WEB2C_NORETURN void jumpout (void);
#define jumpout_regmem
void error (void);
#define error_regmem
WEB2C_NORETURN void zfatalerror (strnumber s);
#define fatalerror(s) zfatalerror((strnumber) (s))
#define fatalerror_regmem
WEB2C_NORETURN void zoverflow (strnumber s,integer n);
#define overflow(s, n) zoverflow((strnumber) (s), (integer) (n))
#define overflow_regmem
WEB2C_NORETURN void zconfusion (strnumber s);
#define confusion(s) zconfusion((strnumber) (s))
#define confusion_regmem
boolean initterminal (void);
#define initterminal_regmem
integer zlength (strnumber s);
#define length(s) zlength((strnumber) (s))
#define length_regmem
strnumber makestring (void);
#define makestring_regmem
void zappendstr (strnumber s);
#define appendstr(s) zappendstr((strnumber) (s))
#define appendstr_regmem
boolean zstreqbuf (strnumber s,integer k);
#define streqbuf(s, k) zstreqbuf((strnumber) (s), (integer) (k))
#define streqbuf_regmem
boolean zstreqstr (strnumber s,strnumber t);
#define streqstr(s, t) zstreqstr((strnumber) (s), (strnumber) (t))
#define streqstr_regmem
strnumber zsearchstring (strnumber search);
#define searchstring(search) zsearchstring((strnumber) (search))
#define searchstring_regmem
strnumber slowmakestring (void);
#define slowmakestring_regmem
boolean getstringsstarted (void);
#define getstringsstarted_regmem
void zprinttwo (integer n);
#define printtwo(n) zprinttwo((integer) (n))
#define printtwo_regmem
void zprinthex (integer n);
#define printhex(n) zprinthex((integer) (n))
#define printhex_regmem
void zprintromanint (integer n);
#define printromanint(n) zprintromanint((integer) (n))
#define printromanint_regmem
void printcurrentstring (void);
#define printcurrentstring_regmem
void terminput (void);
#define terminput_regmem
void zinterror (integer n);
#define interror(n) zinterror((integer) (n))
#define interror_regmem
void normalizeselector (void);
#define normalizeselector_regmem
void pauseforinstructions (void);
#define pauseforinstructions_regmem
integer zhalf (integer x);
#define half(x) zhalf((integer) (x))
#define half_regmem
scaled zrounddecimals (smallnumber k);
#define rounddecimals(k) zrounddecimals((smallnumber) (k))
#define rounddecimals_regmem
void zprintscaled (scaled s);
#define printscaled(s) zprintscaled((scaled) (s))
#define printscaled_regmem
scaled zmultandadd (integer n,scaled x,scaled y,scaled maxanswer);
#define multandadd(n, x, y, maxanswer) zmultandadd((integer) (n), (scaled) (x), (scaled) (y), (scaled) (maxanswer))
#define multandadd_regmem
scaled zxovern (scaled x,integer n);
#define xovern(x, n) zxovern((scaled) (x), (integer) (n))
#define xovern_regmem
scaled zxnoverd (scaled x,integer n,integer d);
#define xnoverd(x, n, d) zxnoverd((scaled) (x), (integer) (n), (integer) (d))
#define xnoverd_regmem
halfword zbadness (scaled t,scaled s);
#define badness(t, s) zbadness((scaled) (t), (scaled) (s))
#define badness_regmem
integer zmakefrac (integer p,integer q);
#define makefrac(p, q) zmakefrac((integer) (p), (integer) (q))
#define makefrac_regmem
integer ztakefrac (integer q,integer f);
#define takefrac(q, f) ztakefrac((integer) (q), (integer) (f))
#define takefrac_regmem
integer zmlog (integer x);
#define mlog(x) zmlog((integer) (x))
#define mlog_regmem
integer zabvscd (integer a,integer b,integer c,integer d);
#define abvscd(a, b, c, d) zabvscd((integer) (a), (integer) (b), (integer) (c), (integer) (d))
#define abvscd_regmem
void newrandoms (void);
#define newrandoms_regmem
void zinitrandoms (integer seed);
#define initrandoms(seed) zinitrandoms((integer) (seed))
#define initrandoms_regmem
integer zunifrand (integer x);
#define unifrand(x) zunifrand((integer) (x))
#define unifrand_regmem
integer normrand (void);
#define normrand_regmem
void zprintword (memoryword w);
#define printword(w) zprintword((memoryword) (w))
#define printword_regmem
void zshowtokenlist (integer p,integer q,integer l);
#define showtokenlist(p, q, l) zshowtokenlist((integer) (p), (integer) (q), (integer) (l))
#define showtokenlist_regmem register memoryword *mem=zmem;
void runaway (void);
#define runaway_regmem register memoryword *mem=zmem;
halfword getavail (void);
#define getavail_regmem register memoryword *mem=zmem;
void zflushlist (halfword p);
#define flushlist(p) zflushlist((halfword) (p))
#define flushlist_regmem register memoryword *mem=zmem;
halfword zgetnode (integer s);
#define getnode(s) zgetnode((integer) (s))
#define getnode_regmem register memoryword *mem=zmem;
void zfreenode (halfword p,halfword s);
#define freenode(p, s) zfreenode((halfword) (p), (halfword) (s))
#define freenode_regmem register memoryword *mem=zmem;
void sortavail (void);
#define sortavail_regmem register memoryword *mem=zmem;
halfword newnullbox (void);
#define newnullbox_regmem register memoryword *mem=zmem;
halfword newrule (void);
#define newrule_regmem register memoryword *mem=zmem;
halfword znewligature (internalfontnumber f,quarterword c,halfword q);
#define newligature(f, c, q) znewligature((internalfontnumber) (f), (quarterword) (c), (halfword) (q))
#define newligature_regmem register memoryword *mem=zmem;
halfword znewligitem (quarterword c);
#define newligitem(c) znewligitem((quarterword) (c))
#define newligitem_regmem register memoryword *mem=zmem;
halfword newdisc (void);
#define newdisc_regmem register memoryword *mem=zmem;
void zcopynativeglyphinfo (halfword src,halfword dest);
#define copynativeglyphinfo(src, dest) zcopynativeglyphinfo((halfword) (src), (halfword) (dest))
#define copynativeglyphinfo_regmem register memoryword *mem=zmem;
halfword znewmath (scaled w,smallnumber s);
#define newmath(w, s) znewmath((scaled) (w), (smallnumber) (s))
#define newmath_regmem register memoryword *mem=zmem;
halfword znewspec (halfword p);
#define newspec(p) znewspec((halfword) (p))
#define newspec_regmem register memoryword *mem=zmem;
halfword znewparamglue (smallnumber n);
#define newparamglue(n) znewparamglue((smallnumber) (n))
#define newparamglue_regmem register memoryword *mem=zmem, *eqtb=zeqtb;
halfword znewglue (halfword q);
#define newglue(q) znewglue((halfword) (q))
#define newglue_regmem register memoryword *mem=zmem;
halfword znewskipparam (smallnumber n);
#define newskipparam(n) znewskipparam((smallnumber) (n))
#define newskipparam_regmem register memoryword *mem=zmem, *eqtb=zeqtb;
halfword znewkern (scaled w);
#define newkern(w) znewkern((scaled) (w))
#define newkern_regmem register memoryword *mem=zmem;
halfword znewpenalty (integer m);
#define newpenalty(m) znewpenalty((integer) (m))
#define newpenalty_regmem register memoryword *mem=zmem;
void zcheckmem (boolean printlocs);
#define checkmem(printlocs) zcheckmem((boolean) (printlocs))
#define checkmem_regmem register memoryword *mem=zmem;
void zsearchmem (halfword p);
#define searchmem(p) zsearchmem((halfword) (p))
#define searchmem_regmem register memoryword *mem=zmem, *eqtb=zeqtb;
void zpdferror (strnumber t,strnumber p);
#define pdferror(t, p) zpdferror((strnumber) (t), (strnumber) (p))
#define pdferror_regmem
halfword zprevrightmost (halfword s,halfword e);
#define prevrightmost(s, e) zprevrightmost((halfword) (s), (halfword) (e))
#define prevrightmost_regmem register memoryword *mem=zmem;
scaled zroundxnoverd (scaled x,integer n,integer d);
#define roundxnoverd(x, n, d) zroundxnoverd((scaled) (x), (integer) (n), (integer) (d))
#define roundxnoverd_regmem
integer getmicrointerval (void);
#define getmicrointerval_regmem
void zshortdisplay (integer p);
#define shortdisplay(p) zshortdisplay((integer) (p))
#define shortdisplay_regmem register memoryword *mem=zmem;
void zprintfontandchar (integer p);
#define printfontandchar(p) zprintfontandchar((integer) (p))
#define printfontandchar_regmem register memoryword *mem=zmem;
void zprintmark (integer p);
#define printmark(p) zprintmark((integer) (p))
#define printmark_regmem register memoryword *mem=zmem;
void zprintruledimen (scaled d);
#define printruledimen(d) zprintruledimen((scaled) (d))
#define printruledimen_regmem
void zprintglue (scaled d,integer order,strnumber s);
#define printglue(d, order, s) zprintglue((scaled) (d), (integer) (order), (strnumber) (s))
#define printglue_regmem
void zprintspec (integer p,strnumber s);
#define printspec(p, s) zprintspec((integer) (p), (strnumber) (s))
#define printspec_regmem register memoryword *mem=zmem;
void zprintfamandchar (halfword p);
#define printfamandchar(p) zprintfamandchar((halfword) (p))
#define printfamandchar_regmem register memoryword *mem=zmem;
void zprintdelimiter (halfword p);
#define printdelimiter(p) zprintdelimiter((halfword) (p))
#define printdelimiter_regmem register memoryword *mem=zmem;
void zprintsubsidiarydata (halfword p,UTF16code c);
#define printsubsidiarydata(p, c) zprintsubsidiarydata((halfword) (p), (UTF16code) (c))
#define printsubsidiarydata_regmem register memoryword *mem=zmem;
void zprintstyle (integer c);
#define printstyle(c) zprintstyle((integer) (c))
#define printstyle_regmem
void zprintskipparam (integer n);
#define printskipparam(n) zprintskipparam((integer) (n))
#define printskipparam_regmem
void zshownodelist (integer p);
#define shownodelist(p) zshownodelist((integer) (p))
#define shownodelist_regmem register memoryword *mem=zmem;
void zshowbox (halfword p);
#define showbox(p) zshowbox((halfword) (p))
#define showbox_regmem register memoryword *eqtb=zeqtb;
void zshortdisplayn (integer p,integer m);
#define shortdisplayn(p, m) zshortdisplayn((integer) (p), (integer) (m))
#define shortdisplayn_regmem
void zdeletetokenref (halfword p);
#define deletetokenref(p) zdeletetokenref((halfword) (p))
#define deletetokenref_regmem register memoryword *mem=zmem;
void zdeleteglueref (halfword p);
#define deleteglueref(p) zdeleteglueref((halfword) (p))
#define deleteglueref_regmem register memoryword *mem=zmem;
void zflushnodelist (halfword p);
#define flushnodelist(p) zflushnodelist((halfword) (p))
#define flushnodelist_regmem register memoryword *mem=zmem;
halfword zcopynodelist (halfword p);
#define copynodelist(p) zcopynodelist((halfword) (p))
#define copynodelist_regmem register memoryword *mem=zmem;
void zprintmode (integer m);
#define printmode(m) zprintmode((integer) (m))
#define printmode_regmem
void zprintinmode (integer m);
#define printinmode(m) zprintinmode((integer) (m))
#define printinmode_regmem
void pushnest (void);
#define pushnest_regmem
void popnest (void);
#define popnest_regmem register memoryword *mem=zmem;
void showactivities (void);
#define showactivities_regmem register memoryword *mem=zmem, *eqtb=zeqtb;
void zprintparam (integer n);
#define printparam(n) zprintparam((integer) (n))
#define printparam_regmem
void begindiagnostic (void);
#define begindiagnostic_regmem register memoryword *eqtb=zeqtb;
void zenddiagnostic (boolean blankline);
#define enddiagnostic(blankline) zenddiagnostic((boolean) (blankline))
#define enddiagnostic_regmem
void zprintlengthparam (integer n);
#define printlengthparam(n) zprintlengthparam((integer) (n))
#define printlengthparam_regmem
void zprintcmdchr (quarterword cmd,halfword chrcode);
#define printcmdchr(cmd, chrcode) zprintcmdchr((quarterword) (cmd), (halfword) (chrcode))
#define printcmdchr_regmem register memoryword *mem=zmem;
void znotaatfonterror (integer cmd,integer c,integer f);
#define notaatfonterror(cmd, c, f) znotaatfonterror((integer) (cmd), (integer) (c), (integer) (f))
#define notaatfonterror_regmem
void znotaatgrfonterror (integer cmd,integer c,integer f);
#define notaatgrfonterror(cmd, c, f) znotaatgrfonterror((integer) (cmd), (integer) (c), (integer) (f))
#define notaatgrfonterror_regmem
void znototfonterror (integer cmd,integer c,integer f);
#define nototfonterror(cmd, c, f) znototfonterror((integer) (cmd), (integer) (c), (integer) (f))
#define nototfonterror_regmem
void znotnativefonterror (integer cmd,integer c,integer f);
#define notnativefonterror(cmd, c, f) znotnativefonterror((integer) (cmd), (integer) (c), (integer) (f))
#define notnativefonterror_regmem
void zshoweqtb (halfword n);
#define showeqtb(n) zshoweqtb((halfword) (n))
#define showeqtb_regmem register memoryword *mem=zmem, *eqtb=zeqtb;
halfword zidlookup (integer j,integer l);
#define idlookup(j, l) zidlookup((integer) (j), (integer) (l))
#define idlookup_regmem
halfword zprimlookup (strnumber s);
#define primlookup(s) zprimlookup((strnumber) (s))
#define primlookup_regmem
void zprimitive (strnumber s,quarterword c,halfword o);
#define primitive(s, c, o) zprimitive((strnumber) (s), (quarterword) (c), (halfword) (o))
#define primitive_regmem register memoryword *eqtb=zeqtb;
void zrestoretrace (halfword p,strnumber s);
#define restoretrace(p, s) zrestoretrace((halfword) (p), (strnumber) (s))
#define restoretrace_regmem
void zprintgroup (boolean e);
#define printgroup(e) zprintgroup((boolean) (e))
#define printgroup_regmem
void zgrouptrace (boolean e);
#define grouptrace(e) zgrouptrace((boolean) (e))
#define grouptrace_regmem
boolean pseudoinput (void);
#define pseudoinput_regmem register memoryword *mem=zmem;
void pseudoclose (void);
#define pseudoclose_regmem register memoryword *mem=zmem;
void groupwarning (void);
#define groupwarning_regmem register memoryword *eqtb=zeqtb;
void ifwarning (void);
#define ifwarning_regmem register memoryword *mem=zmem, *eqtb=zeqtb;
void filewarning (void);
#define filewarning_regmem register memoryword *mem=zmem, *eqtb=zeqtb;
void zdeletesaref (halfword q);
#define deletesaref(q) zdeletesaref((halfword) (q))
#define deletesaref_regmem register memoryword *mem=zmem;
void zshowsa (halfword p,strnumber s);
#define showsa(p, s) zshowsa((halfword) (p), (strnumber) (s))
#define showsa_regmem register memoryword *mem=zmem;
void zsasave (halfword p);
#define sasave(p) zsasave((halfword) (p))
#define sasave_regmem register memoryword *mem=zmem;
void zsadestroy (halfword p);
#define sadestroy(p) zsadestroy((halfword) (p))
#define sadestroy_regmem register memoryword *mem=zmem;
void zsadef (halfword p,halfword e);
#define sadef(p, e) zsadef((halfword) (p), (halfword) (e))
#define sadef_regmem register memoryword *mem=zmem, *eqtb=zeqtb;
void zsawdef (halfword p,integer w);
#define sawdef(p, w) zsawdef((halfword) (p), (integer) (w))
#define sawdef_regmem register memoryword *mem=zmem, *eqtb=zeqtb;
void zgsadef (halfword p,halfword e);
#define gsadef(p, e) zgsadef((halfword) (p), (halfword) (e))
#define gsadef_regmem register memoryword *mem=zmem, *eqtb=zeqtb;
void zgsawdef (halfword p,integer w);
#define gsawdef(p, w) zgsawdef((halfword) (p), (integer) (w))
#define gsawdef_regmem register memoryword *mem=zmem, *eqtb=zeqtb;
void sarestore (void);
#define sarestore_regmem register memoryword *mem=zmem, *eqtb=zeqtb;
void znewsavelevel (groupcode c);
#define newsavelevel(c) znewsavelevel((groupcode) (c))
#define newsavelevel_regmem register memoryword *eqtb=zeqtb;
void zeqdestroy (memoryword w);
#define eqdestroy(w) zeqdestroy((memoryword) (w))
#define eqdestroy_regmem register memoryword *mem=zmem;
void zeqsave (halfword p,quarterword l);
#define eqsave(p, l) zeqsave((halfword) (p), (quarterword) (l))
#define eqsave_regmem register memoryword *eqtb=zeqtb;
void zeqdefine (halfword p,quarterword t,halfword e);
#define eqdefine(p, t, e) zeqdefine((halfword) (p), (quarterword) (t), (halfword) (e))
#define eqdefine_regmem register memoryword *eqtb=zeqtb;
void zeqworddefine (halfword p,integer w);
#define eqworddefine(p, w) zeqworddefine((halfword) (p), (integer) (w))
#define eqworddefine_regmem register memoryword *eqtb=zeqtb;
void zgeqdefine (halfword p,quarterword t,halfword e);
#define geqdefine(p, t, e) zgeqdefine((halfword) (p), (quarterword) (t), (halfword) (e))
#define geqdefine_regmem register memoryword *eqtb=zeqtb;
void zgeqworddefine (halfword p,integer w);
#define geqworddefine(p, w) zgeqworddefine((halfword) (p), (integer) (w))
#define geqworddefine_regmem register memoryword *eqtb=zeqtb;
void zsaveforafter (halfword t);
#define saveforafter(t) zsaveforafter((halfword) (t))
#define saveforafter_regmem
void unsave (void);
#define unsave_regmem register memoryword *mem=zmem, *eqtb=zeqtb;
void preparemag (void);
#define preparemag_regmem register memoryword *eqtb=zeqtb;
void ztokenshow (halfword p);
#define tokenshow(p) ztokenshow((halfword) (p))
#define tokenshow_regmem register memoryword *mem=zmem;
void printmeaning (void);
#define printmeaning_regmem
void showcurcmdchr (void);
#define showcurcmdchr_regmem register memoryword *mem=zmem, *eqtb=zeqtb;
void showcontext (void);
#define showcontext_regmem register memoryword *mem=zmem, *eqtb=zeqtb;
void zbegintokenlist (halfword p,quarterword t);
#define begintokenlist(p, t) zbegintokenlist((halfword) (p), (quarterword) (t))
#define begintokenlist_regmem register memoryword *mem=zmem, *eqtb=zeqtb;
void endtokenlist (void);
#define endtokenlist_regmem
void backinput (void);
#define backinput_regmem register memoryword *mem=zmem;
void backerror (void);
#define backerror_regmem
void inserror (void);
#define inserror_regmem
void beginfilereading (void);
#define beginfilereading_regmem
void endfilereading (void);
#define endfilereading_regmem
void clearforerrorprompt (void);
#define clearforerrorprompt_regmem
void checkoutervalidity (void);
#define checkoutervalidity_regmem register memoryword *mem=zmem;
void getnext (void);
#define getnext_regmem register memoryword *mem=zmem, *eqtb=zeqtb;
void firmuptheline (void);
#define firmuptheline_regmem register memoryword *eqtb=zeqtb;
void gettoken (void);
#define gettoken_regmem
void macrocall (void);
#define macrocall_regmem register memoryword *mem=zmem, *eqtb=zeqtb;
void insertrelax (void);
#define insertrelax_regmem
void znewindex (quarterword i,halfword q);
#define newindex(i, q) znewindex((quarterword) (i), (halfword) (q))
#define newindex_regmem register memoryword *mem=zmem;
void zfindsaelement (smallnumber t,halfword n,boolean w);
#define findsaelement(t, n, w) zfindsaelement((smallnumber) (t), (halfword) (n), (boolean) (w))
#define findsaelement_regmem register memoryword *mem=zmem;
void expand (void);
#define expand_regmem register memoryword *mem=zmem, *eqtb=zeqtb;
void getxtoken (void);
#define getxtoken_regmem
void xtoken (void);
#define xtoken_regmem
void scanleftbrace (void);
#define scanleftbrace_regmem
void scanoptionalequals (void);
#define scanoptionalequals_regmem
boolean zscankeyword (strnumber s);
#define scankeyword(s) zscankeyword((strnumber) (s))
#define scankeyword_regmem register memoryword *mem=zmem;
void muerror (void);
#define muerror_regmem
void zscanglyphnumber (internalfontnumber f);
#define scanglyphnumber(f) zscanglyphnumber((internalfontnumber) (f))
#define scanglyphnumber_regmem
void scancharclass (void);
#define scancharclass_regmem
void scancharclassnotignored (void);
#define scancharclassnotignored_regmem
void scaneightbitint (void);
#define scaneightbitint_regmem
void scanusvnum (void);
#define scanusvnum_regmem
void scancharnum (void);
#define scancharnum_regmem
void scanxetexmathcharint (void);
#define scanxetexmathcharint_regmem
void scanmathclassint (void);
#define scanmathclassint_regmem
void scanmathfamint (void);
#define scanmathfamint_regmem
void scanfourbitint (void);
#define scanfourbitint_regmem
void scanfifteenbitint (void);
#define scanfifteenbitint_regmem
void scandelimiterint (void);
#define scandelimiterint_regmem
void scanregisternum (void);
#define scanregisternum_regmem
void scanfourbitintor18 (void);
#define scanfourbitintor18_regmem
void getxorprotected (void);
#define getxorprotected_regmem register memoryword *mem=zmem;
integer zeffectivechar (boolean errp,internalfontnumber f,quarterword c);
#define effectivechar(errp, f, c) zeffectivechar((boolean) (errp), (internalfontnumber) (f), (quarterword) (c))
#define effectivechar_regmem register memoryword *eqtb=zeqtb;
void scanfontident (void);
#define scanfontident_regmem register memoryword *eqtb=zeqtb;
void zfindfontdimen (boolean writing);
#define findfontdimen(writing) zfindfontdimen((boolean) (writing))
#define findfontdimen_regmem
void zscansomethinginternal (smallnumber level,boolean negative);
#define scansomethinginternal(level, negative) zscansomethinginternal((smallnumber) (level), (boolean) (negative))
#define scansomethinginternal_regmem register memoryword *mem=zmem, *eqtb=zeqtb;
void scanint (void);
#define scanint_regmem register memoryword *eqtb=zeqtb;
void zxetexscandimen (boolean mu,boolean inf,boolean shortcut,boolean requiresunits);
#define xetexscandimen(mu, inf, shortcut, requiresunits) zxetexscandimen((boolean) (mu), (boolean) (inf), (boolean) (shortcut), (boolean) (requiresunits))
#define xetexscandimen_regmem register memoryword *mem=zmem, *eqtb=zeqtb;
void zscandimen (boolean mu,boolean inf,boolean shortcut);
#define scandimen(mu, inf, shortcut) zscandimen((boolean) (mu), (boolean) (inf), (boolean) (shortcut))
#define scandimen_regmem
void scandecimal (void);
#define scandecimal_regmem
void zscanglue (smallnumber level);
#define scanglue(level) zscanglue((smallnumber) (level))
#define scanglue_regmem register memoryword *mem=zmem;
integer zaddorsub (integer x,integer y,integer maxanswer,boolean negative);
#define addorsub(x, y, maxanswer, negative) zaddorsub((integer) (x), (integer) (y), (integer) (maxanswer), (boolean) (negative))
#define addorsub_regmem
integer zquotient (integer n,integer d);
#define quotient(n, d) zquotient((integer) (n), (integer) (d))
#define quotient_regmem
integer zfract (integer x,integer n,integer d,integer maxanswer);
#define fract(x, n, d, maxanswer) zfract((integer) (x), (integer) (n), (integer) (d), (integer) (maxanswer))
#define fract_regmem
void scanexpr (void);
#define scanexpr_regmem register memoryword *mem=zmem;
void scannormalglue (void);
#define scannormalglue_regmem
void scanmuglue (void);
#define scanmuglue_regmem
halfword scanrulespec (void);
#define scanrulespec_regmem register memoryword *mem=zmem;
void scangeneraltext (void);
#define scangeneraltext_regmem register memoryword *mem=zmem;
void pseudostart (void);
#define pseudostart_regmem register memoryword *mem=zmem, *eqtb=zeqtb;
halfword zstrtokscat (poolpointer b,smallnumber cat);
#define strtokscat(b, cat) zstrtokscat((poolpointer) (b), (smallnumber) (cat))
#define strtokscat_regmem register memoryword *mem=zmem;
halfword zstrtoks (poolpointer b);
#define strtoks(b) zstrtoks((poolpointer) (b))
#define strtoks_regmem
halfword thetoks (void);
#define thetoks_regmem register memoryword *mem=zmem;
void insthetoks (void);
#define insthetoks_regmem register memoryword *mem=zmem;
void convtoks (void);
#define convtoks_regmem register memoryword *mem=zmem, *eqtb=zeqtb;
halfword zscantoks (boolean macrodef,boolean xpand);
#define scantoks(macrodef, xpand) zscantoks((boolean) (macrodef), (boolean) (xpand))
#define scantoks_regmem register memoryword *mem=zmem;
void zreadtoks (integer n,halfword r,halfword j);
#define readtoks(n, r, j) zreadtoks((integer) (n), (halfword) (r), (halfword) (j))
#define readtoks_regmem register memoryword *mem=zmem, *eqtb=zeqtb;
void passtext (void);
#define passtext_regmem register memoryword *eqtb=zeqtb;
void zchangeiflimit (smallnumber l,halfword p);
#define changeiflimit(l, p) zchangeiflimit((smallnumber) (l), (halfword) (p))
#define changeiflimit_regmem register memoryword *mem=zmem;
void conditional (void);
#define conditional_regmem register memoryword *mem=zmem, *eqtb=zeqtb;
void beginname (void);
#define beginname_regmem
boolean zmorename (UTF16code c);
#define morename(c) zmorename((UTF16code) (c))
#define morename_regmem
void endname (void);
#define endname_regmem
void zpackfilename (strnumber n,strnumber a,strnumber e);
#define packfilename(n, a, e) zpackfilename((strnumber) (n), (strnumber) (a), (strnumber) (e))
#define packfilename_regmem
void zpackbufferedname (smallnumber n,integer a,integer b);
#define packbufferedname(n, a, b) zpackbufferedname((smallnumber) (n), (integer) (a), (integer) (b))
#define packbufferedname_regmem
strnumber makenamestring (void);
#define makenamestring_regmem
strnumber zzumakenamestring (unicodefile * f);
#define umakenamestring(f) zzumakenamestring((unicodefile *) &(f))
#define umakenamestring_regmem
strnumber zamakenamestring (alphafile f);
#define amakenamestring(f) zamakenamestring((alphafile) (f))
#define amakenamestring_regmem
strnumber zbmakenamestring (bytefile f);
#define bmakenamestring(f) zbmakenamestring((bytefile) (f))
#define bmakenamestring_regmem
strnumber zzwmakenamestring (wordfile * f);
#define wmakenamestring(f) zzwmakenamestring((wordfile *) &(f))
#define wmakenamestring_regmem
void scanfilename (void);
#define scanfilename_regmem
void zpackjobname (strnumber s);
#define packjobname(s) zpackjobname((strnumber) (s))
#define packjobname_regmem
void zpromptfilename (strnumber s,strnumber e);
#define promptfilename(s, e) zpromptfilename((strnumber) (s), (strnumber) (e))
#define promptfilename_regmem
void openlogfile (void);
#define openlogfile_regmem register memoryword *eqtb=zeqtb;
void startinput (void);
#define startinput_regmem register memoryword *eqtb=zeqtb;
fourquarters zeffectivecharinfo (internalfontnumber f,quarterword c);
#define effectivecharinfo(f, c) zeffectivecharinfo((internalfontnumber) (f), (quarterword) (c))
#define effectivecharinfo_regmem register memoryword *eqtb=zeqtb;
void zcharwarning (internalfontnumber f,integer c);
#define charwarning(f, c) zcharwarning((internalfontnumber) (f), (integer) (c))
#define charwarning_regmem register memoryword *eqtb=zeqtb;
halfword znewnativewordnode (internalfontnumber f,integer n);
#define newnativewordnode(f, n) znewnativewordnode((internalfontnumber) (f), (integer) (n))
#define newnativewordnode_regmem register memoryword *mem=zmem, *eqtb=zeqtb;
halfword znewnativecharacter (internalfontnumber f,UnicodeScalar c);
#define newnativecharacter(f, c) znewnativecharacter((internalfontnumber) (f), (UnicodeScalar) (c))
#define newnativecharacter_regmem register memoryword *mem=zmem, *eqtb=zeqtb;
void zfontfeaturewarning (voidpointer featureNameP,integer featLen,voidpointer settingNameP,integer setLen);
#define fontfeaturewarning(featureNameP, featLen, settingNameP, setLen) zfontfeaturewarning((voidpointer) (featureNameP), (integer) (featLen), (voidpointer) (settingNameP), (integer) (setLen))
#define fontfeaturewarning_regmem
void zfontmappingwarning (voidpointer mappingNameP,integer mappingNameLen,integer warningType);
#define fontmappingwarning(mappingNameP, mappingNameLen, warningType) zfontmappingwarning((voidpointer) (mappingNameP), (integer) (mappingNameLen), (integer) (warningType))
#define fontmappingwarning_regmem
void graphitewarning (void);
#define graphitewarning_regmem
internalfontnumber zloadnativefont (halfword u,strnumber nom,strnumber aire,scaled s);
#define loadnativefont(u, nom, aire, s) zloadnativefont((halfword) (u), (strnumber) (nom), (strnumber) (aire), (scaled) (s))
#define loadnativefont_regmem register memoryword *mem=zmem, *eqtb=zeqtb;
void zdolocalelinebreaks (integer s,integer len);
#define dolocalelinebreaks(s, len) zdolocalelinebreaks((integer) (s), (integer) (len))
#define dolocalelinebreaks_regmem register memoryword *mem=zmem, *eqtb=zeqtb;
void badutf8warning (void);
#define badutf8warning_regmem
integer getinputnormalizationstate (void);
#define getinputnormalizationstate_regmem register memoryword *eqtb=zeqtb;
integer gettracingfontsstate (void);
#define gettracingfontsstate_regmem register memoryword *eqtb=zeqtb;
internalfontnumber zreadfontinfo (halfword u,strnumber nom,strnumber aire,scaled s);
#define readfontinfo(u, nom, aire, s) zreadfontinfo((halfword) (u), (strnumber) (nom), (strnumber) (aire), (scaled) (s))
#define readfontinfo_regmem register memoryword *eqtb=zeqtb;
halfword znewcharacter (internalfontnumber f,UTF16code c);
#define newcharacter(f, c) znewcharacter((internalfontnumber) (f), (UTF16code) (c))
#define newcharacter_regmem register memoryword *mem=zmem;
void dviswap (void);
#define dviswap_regmem
void zdvifour (integer x);
#define dvifour(x) zdvifour((integer) (x))
#define dvifour_regmem
void zdvitwo (UTF16code s);
#define dvitwo(s) zdvitwo((UTF16code) (s))
#define dvitwo_regmem
void zdvipop (integer l);
#define dvipop(l) zdvipop((integer) (l))
#define dvipop_regmem
void zdvinativefontdef (internalfontnumber f);
#define dvinativefontdef(f) zdvinativefontdef((internalfontnumber) (f))
#define dvinativefontdef_regmem
void zdvifontdef (internalfontnumber f);
#define dvifontdef(f) zdvifontdef((internalfontnumber) (f))
#define dvifontdef_regmem
void zmovement (scaled w,eightbits o);
#define movement(w, o) zmovement((scaled) (w), (eightbits) (o))
#define movement_regmem register memoryword *mem=zmem;
void zprunemovements (integer l);
#define prunemovements(l) zprunemovements((integer) (l))
#define prunemovements_regmem register memoryword *mem=zmem;
void zspecialout (halfword p);
#define specialout(p) zspecialout((halfword) (p))
#define specialout_regmem register memoryword *mem=zmem;
void zwriteout (halfword p);
#define writeout(p) zwriteout((halfword) (p))
#define writeout_regmem register memoryword *mem=zmem, *eqtb=zeqtb;
void zpicout (halfword p);
#define picout(p) zpicout((halfword) (p))
#define picout_regmem register memoryword *mem=zmem;
void zoutwhat (halfword p);
#define outwhat(p) zoutwhat((halfword) (p))
#define outwhat_regmem register memoryword *mem=zmem, *eqtb=zeqtb;
halfword znewedge (smallnumber s,scaled w);
#define newedge(s, w) znewedge((smallnumber) (s), (scaled) (w))
#define newedge_regmem register memoryword *mem=zmem;
halfword zzreverse (halfword thisbox,halfword t,scaled * curg,real * curglue);
#define reverse(thisbox, t, curg, curglue) zzreverse((halfword) (thisbox), (halfword) (t), (scaled *) &(curg), (real *) &(curglue))
#define reverse_regmem register memoryword *mem=zmem;
void hlistout (void);
#define hlistout_regmem register memoryword *mem=zmem, *eqtb=zeqtb;
void vlistout (void);
#define vlistout_regmem register memoryword *mem=zmem;
void zshipout (halfword p);
#define shipout(p) zshipout((halfword) (p))
#define shipout_regmem register memoryword *mem=zmem, *eqtb=zeqtb;
void zscanspec (groupcode c,boolean threecodes);
#define scanspec(c, threecodes) zscanspec((groupcode) (c), (boolean) (threecodes))
#define scanspec_regmem
scaled zcharpw (halfword p,smallnumber side);
#define charpw(p, side) zcharpw((halfword) (p), (smallnumber) (side))
#define charpw_regmem register memoryword *mem=zmem;
halfword znewmarginkern (scaled w,halfword p,smallnumber side);
#define newmarginkern(w, p, side) znewmarginkern((scaled) (w), (halfword) (p), (smallnumber) (side))
#define newmarginkern_regmem register memoryword *mem=zmem;
halfword zhpack (halfword p,scaled w,smallnumber m);
#define hpack(p, w, m) zhpack((halfword) (p), (scaled) (w), (smallnumber) (m))
#define hpack_regmem register memoryword *mem=zmem, *eqtb=zeqtb;
halfword zvpackage (halfword p,scaled h,smallnumber m,scaled l);
#define vpackage(p, h, m, l) zvpackage((halfword) (p), (scaled) (h), (smallnumber) (m), (scaled) (l))
#define vpackage_regmem register memoryword *mem=zmem, *eqtb=zeqtb;
void zappendtovlist (halfword b);
#define appendtovlist(b) zappendtovlist((halfword) (b))
#define appendtovlist_regmem register memoryword *mem=zmem, *eqtb=zeqtb;
halfword newnoad (void);
#define newnoad_regmem register memoryword *mem=zmem;
halfword znewstyle (smallnumber s);
#define newstyle(s) znewstyle((smallnumber) (s))
#define newstyle_regmem register memoryword *mem=zmem;
halfword newchoice (void);
#define newchoice_regmem register memoryword *mem=zmem;
void showinfo (void);
#define showinfo_regmem register memoryword *mem=zmem;
scaled zmathxheight (integer sizecode);
#define mathxheight(sizecode) zmathxheight((integer) (sizecode))
#define mathxheight_regmem register memoryword *eqtb=zeqtb;
scaled zmathquad (integer sizecode);
#define mathquad(sizecode) zmathquad((integer) (sizecode))
#define mathquad_regmem register memoryword *eqtb=zeqtb;
scaled znum1 (integer sizecode);
#define num1(sizecode) znum1((integer) (sizecode))
#define num1_regmem register memoryword *eqtb=zeqtb;
scaled znum2 (integer sizecode);
#define num2(sizecode) znum2((integer) (sizecode))
#define num2_regmem register memoryword *eqtb=zeqtb;
scaled znum3 (integer sizecode);
#define num3(sizecode) znum3((integer) (sizecode))
#define num3_regmem register memoryword *eqtb=zeqtb;
scaled zdenom1 (integer sizecode);
#define denom1(sizecode) zdenom1((integer) (sizecode))
#define denom1_regmem register memoryword *eqtb=zeqtb;
scaled zdenom2 (integer sizecode);
#define denom2(sizecode) zdenom2((integer) (sizecode))
#define denom2_regmem register memoryword *eqtb=zeqtb;
scaled zsup1 (integer sizecode);
#define sup1(sizecode) zsup1((integer) (sizecode))
#define sup1_regmem register memoryword *eqtb=zeqtb;
scaled zsup2 (integer sizecode);
#define sup2(sizecode) zsup2((integer) (sizecode))
#define sup2_regmem register memoryword *eqtb=zeqtb;
scaled zsup3 (integer sizecode);
#define sup3(sizecode) zsup3((integer) (sizecode))
#define sup3_regmem register memoryword *eqtb=zeqtb;
scaled zsub1 (integer sizecode);
#define sub1(sizecode) zsub1((integer) (sizecode))
#define sub1_regmem register memoryword *eqtb=zeqtb;
scaled zsub2 (integer sizecode);
#define sub2(sizecode) zsub2((integer) (sizecode))
#define sub2_regmem register memoryword *eqtb=zeqtb;
scaled zsupdrop (integer sizecode);
#define supdrop(sizecode) zsupdrop((integer) (sizecode))
#define supdrop_regmem register memoryword *eqtb=zeqtb;
scaled zsubdrop (integer sizecode);
#define subdrop(sizecode) zsubdrop((integer) (sizecode))
#define subdrop_regmem register memoryword *eqtb=zeqtb;
scaled zdelim1 (integer sizecode);
#define delim1(sizecode) zdelim1((integer) (sizecode))
#define delim1_regmem register memoryword *eqtb=zeqtb;
scaled zdelim2 (integer sizecode);
#define delim2(sizecode) zdelim2((integer) (sizecode))
#define delim2_regmem register memoryword *eqtb=zeqtb;
scaled zaxisheight (integer sizecode);
#define axisheight(sizecode) zaxisheight((integer) (sizecode))
#define axisheight_regmem register memoryword *eqtb=zeqtb;
scaled defaultrulethickness (void);
#define defaultrulethickness_regmem register memoryword *eqtb=zeqtb;
scaled bigopspacing1 (void);
#define bigopspacing1_regmem register memoryword *eqtb=zeqtb;
scaled bigopspacing2 (void);
#define bigopspacing2_regmem register memoryword *eqtb=zeqtb;
scaled bigopspacing3 (void);
#define bigopspacing3_regmem register memoryword *eqtb=zeqtb;
scaled bigopspacing4 (void);
#define bigopspacing4_regmem register memoryword *eqtb=zeqtb;
scaled bigopspacing5 (void);
#define bigopspacing5_regmem register memoryword *eqtb=zeqtb;
halfword zfractionrule (scaled t);
#define fractionrule(t) zfractionrule((scaled) (t))
#define fractionrule_regmem register memoryword *mem=zmem;
halfword zoverbar (halfword b,scaled k,scaled t);
#define overbar(b, k, t) zoverbar((halfword) (b), (scaled) (k), (scaled) (t))
#define overbar_regmem register memoryword *mem=zmem;
halfword zcharbox (internalfontnumber f,integer c);
#define charbox(f, c) zcharbox((internalfontnumber) (f), (integer) (c))
#define charbox_regmem register memoryword *mem=zmem;
void zstackintobox (halfword b,internalfontnumber f,quarterword c);
#define stackintobox(b, f, c) zstackintobox((halfword) (b), (internalfontnumber) (f), (quarterword) (c))
#define stackintobox_regmem register memoryword *mem=zmem;
scaled zheightplusdepth (internalfontnumber f,quarterword c);
#define heightplusdepth(f, c) zheightplusdepth((internalfontnumber) (f), (quarterword) (c))
#define heightplusdepth_regmem
void zstackglyphintobox (halfword b,internalfontnumber f,integer g);
#define stackglyphintobox(b, f, g) zstackglyphintobox((halfword) (b), (internalfontnumber) (f), (integer) (g))
#define stackglyphintobox_regmem register memoryword *mem=zmem;
void zstackglueintobox (halfword b,scaled min,scaled max);
#define stackglueintobox(b, min, max) zstackglueintobox((halfword) (b), (scaled) (min), (scaled) (max))
#define stackglueintobox_regmem register memoryword *mem=zmem;
halfword zbuildopentypeassembly (internalfontnumber f,voidpointer a,scaled s,boolean horiz);
#define buildopentypeassembly(f, a, s, horiz) zbuildopentypeassembly((internalfontnumber) (f), (voidpointer) (a), (scaled) (s), (boolean) (horiz))
#define buildopentypeassembly_regmem register memoryword *mem=zmem;
halfword zvardelimiter (halfword d,integer s,scaled v);
#define vardelimiter(d, s, v) zvardelimiter((halfword) (d), (integer) (s), (scaled) (v))
#define vardelimiter_regmem register memoryword *mem=zmem, *eqtb=zeqtb;
halfword zrebox (halfword b,scaled w);
#define rebox(b, w) zrebox((halfword) (b), (scaled) (w))
#define rebox_regmem register memoryword *mem=zmem;
halfword zmathglue (halfword g,scaled m);
#define mathglue(g, m) zmathglue((halfword) (g), (scaled) (m))
#define mathglue_regmem register memoryword *mem=zmem;
void zmathkern (halfword p,scaled m);
#define mathkern(p, m) zmathkern((halfword) (p), (scaled) (m))
#define mathkern_regmem register memoryword *mem=zmem;
void flushmath (void);
#define flushmath_regmem register memoryword *mem=zmem;
halfword zcleanbox (halfword p,smallnumber s);
#define cleanbox(p, s) zcleanbox((halfword) (p), (smallnumber) (s))
#define cleanbox_regmem register memoryword *mem=zmem;
void zfetch (halfword a);
#define fetch(a) zfetch((halfword) (a))
#define fetch_regmem register memoryword *mem=zmem, *eqtb=zeqtb;
void zmakeover (halfword q);
#define makeover(q) zmakeover((halfword) (q))
#define makeover_regmem register memoryword *mem=zmem;
void zmakeunder (halfword q);
#define makeunder(q) zmakeunder((halfword) (q))
#define makeunder_regmem register memoryword *mem=zmem;
void zmakevcenter (halfword q);
#define makevcenter(q) zmakevcenter((halfword) (q))
#define makevcenter_regmem register memoryword *mem=zmem;
void zmakeradical (halfword q);
#define makeradical(q) zmakeradical((halfword) (q))
#define makeradical_regmem register memoryword *mem=zmem, *eqtb=zeqtb;
scaled zcomputeotmathaccentpos (halfword p);
#define computeotmathaccentpos(p) zcomputeotmathaccentpos((halfword) (p))
#define computeotmathaccentpos_regmem register memoryword *mem=zmem;
void zmakemathaccent (halfword q);
#define makemathaccent(q) zmakemathaccent((halfword) (q))
#define makemathaccent_regmem register memoryword *mem=zmem;
void zmakefraction (halfword q);
#define makefraction(q) zmakefraction((halfword) (q))
#define makefraction_regmem register memoryword *mem=zmem;
scaled zmakeop (halfword q);
#define makeop(q) zmakeop((halfword) (q))
#define makeop_regmem register memoryword *mem=zmem;
void zmakeord (halfword q);
#define makeord(q) zmakeord((halfword) (q))
#define makeord_regmem register memoryword *mem=zmem;
halfword zattachhkerntonewhlist (halfword q,scaled delta);
#define attachhkerntonewhlist(q, delta) zattachhkerntonewhlist((halfword) (q), (scaled) (delta))
#define attachhkerntonewhlist_regmem register memoryword *mem=zmem;
void zmakescripts (halfword q,scaled delta);
#define makescripts(q, delta) zmakescripts((halfword) (q), (scaled) (delta))
#define makescripts_regmem register memoryword *mem=zmem, *eqtb=zeqtb;
smallnumber zmakeleftright (halfword q,smallnumber style,scaled maxd,scaled maxh);
#define makeleftright(q, style, maxd, maxh) zmakeleftright((halfword) (q), (smallnumber) (style), (scaled) (maxd), (scaled) (maxh))
#define makeleftright_regmem register memoryword *mem=zmem, *eqtb=zeqtb;
void mlisttohlist (void);
#define mlisttohlist_regmem register memoryword *mem=zmem, *eqtb=zeqtb;
void pushalignment (void);
#define pushalignment_regmem register memoryword *mem=zmem;
void popalignment (void);
#define popalignment_regmem register memoryword *mem=zmem;
void getpreambletoken (void);
#define getpreambletoken_regmem register memoryword *eqtb=zeqtb;
void initalign (void);
#define initalign_regmem register memoryword *mem=zmem, *eqtb=zeqtb;
void zinitspan (halfword p);
#define initspan(p) zinitspan((halfword) (p))
#define initspan_regmem
void initrow (void);
#define initrow_regmem register memoryword *mem=zmem;
void initcol (void);
#define initcol_regmem register memoryword *mem=zmem;
boolean fincol (void);
#define fincol_regmem register memoryword *mem=zmem;
void finrow (void);
#define finrow_regmem register memoryword *mem=zmem, *eqtb=zeqtb;
void finalign (void);
#define finalign_regmem register memoryword *mem=zmem, *eqtb=zeqtb;
void alignpeek (void);
#define alignpeek_regmem
halfword zfiniteshrink (halfword p);
#define finiteshrink(p) zfiniteshrink((halfword) (p))
#define finiteshrink_regmem register memoryword *mem=zmem;
void zpushnode (halfword p);
#define pushnode(p) zpushnode((halfword) (p))
#define pushnode_regmem
halfword popnode (void);
#define popnode_regmem
halfword zfindprotcharleft (halfword l,boolean d);
#define findprotcharleft(l, d) zfindprotcharleft((halfword) (l), (boolean) (d))
#define findprotcharleft_regmem register memoryword *mem=zmem;
halfword zfindprotcharright (halfword l,halfword r);
#define findprotcharright(l, r) zfindprotcharright((halfword) (l), (halfword) (r))
#define findprotcharright_regmem register memoryword *mem=zmem;
scaled ztotalpw (halfword q,halfword p);
#define totalpw(q, p) ztotalpw((halfword) (q), (halfword) (p))
#define totalpw_regmem register memoryword *mem=zmem;
void ztrybreak (integer pi,smallnumber breaktype);
#define trybreak(pi, breaktype) ztrybreak((integer) (pi), (smallnumber) (breaktype))
#define trybreak_regmem register memoryword *mem=zmem, *eqtb=zeqtb;
void zpostlinebreak (boolean d);
#define postlinebreak(d) zpostlinebreak((boolean) (d))
#define postlinebreak_regmem register memoryword *mem=zmem, *eqtb=zeqtb;
smallnumber zreconstitute (smallnumber j,smallnumber n,halfword bchar,halfword hchar);
#define reconstitute(j, n, bchar, hchar) zreconstitute((smallnumber) (j), (smallnumber) (n), (halfword) (bchar), (halfword) (hchar))
#define reconstitute_regmem register memoryword *mem=zmem;
void hyphenate (void);
#define hyphenate_regmem register memoryword *mem=zmem, *eqtb=zeqtb;
integer maxhyphenatablelength (void);
#define maxhyphenatablelength_regmem register memoryword *eqtb=zeqtb;
trieopcode znewtrieop (smallnumber d,smallnumber n,trieopcode v);
#define newtrieop(d, n, v) znewtrieop((smallnumber) (d), (smallnumber) (n), (trieopcode) (v))
#define newtrieop_regmem
triepointer ztrienode (triepointer p);
#define trienode(p) ztrienode((triepointer) (p))
#define trienode_regmem
triepointer zcompresstrie (triepointer p);
#define compresstrie(p) zcompresstrie((triepointer) (p))
#define compresstrie_regmem
void zfirstfit (triepointer p);
#define firstfit(p) zfirstfit((triepointer) (p))
#define firstfit_regmem
void ztriepack (triepointer p);
#define triepack(p) ztriepack((triepointer) (p))
#define triepack_regmem
void ztriefix (triepointer p);
#define triefix(p) ztriefix((triepointer) (p))
#define triefix_regmem
void newpatterns (void);
#define newpatterns_regmem register memoryword *mem=zmem, *eqtb=zeqtb;
void inittrie (void);
#define inittrie_regmem
void zlinebreak (boolean d);
#define linebreak(d) zlinebreak((boolean) (d))
#define linebreak_regmem register memoryword *mem=zmem, *eqtb=zeqtb;
boolean zeTeXenabled (boolean b,quarterword j,halfword k);
#define eTeXenabled(b, j, k) zeTeXenabled((boolean) (b), (quarterword) (j), (halfword) (k))
#define eTeXenabled_regmem
void showsavegroups (void);
#define showsavegroups_regmem register memoryword *mem=zmem;
void newhyphexceptions (void);
#define newhyphexceptions_regmem register memoryword *mem=zmem, *eqtb=zeqtb;
halfword zprunepagetop (halfword p,boolean s);
#define prunepagetop(p, s) zprunepagetop((halfword) (p), (boolean) (s))
#define prunepagetop_regmem register memoryword *mem=zmem;
halfword zvertbreak (halfword p,scaled h,scaled d);
#define vertbreak(p, h, d) zvertbreak((halfword) (p), (scaled) (h), (scaled) (d))
#define vertbreak_regmem register memoryword *mem=zmem;
boolean zdomarks (smallnumber a,smallnumber l,halfword q);
#define domarks(a, l, q) zdomarks((smallnumber) (a), (smallnumber) (l), (halfword) (q))
#define domarks_regmem register memoryword *mem=zmem;
halfword zvsplit (halfword n,scaled h);
#define vsplit(n, h) zvsplit((halfword) (n), (scaled) (h))
#define vsplit_regmem register memoryword *mem=zmem, *eqtb=zeqtb;
void printtotals (void);
#define printtotals_regmem
void zfreezepagespecs (smallnumber s);
#define freezepagespecs(s) zfreezepagespecs((smallnumber) (s))
#define freezepagespecs_regmem register memoryword *eqtb=zeqtb;
void zboxerror (eightbits n);
#define boxerror(n) zboxerror((eightbits) (n))
#define boxerror_regmem register memoryword *eqtb=zeqtb;
void zensurevbox (eightbits n);
#define ensurevbox(n) zensurevbox((eightbits) (n))
#define ensurevbox_regmem register memoryword *mem=zmem, *eqtb=zeqtb;
void zfireup (halfword c);
#define fireup(c) zfireup((halfword) (c))
#define fireup_regmem register memoryword *mem=zmem, *eqtb=zeqtb;
void buildpage (void);
#define buildpage_regmem register memoryword *mem=zmem, *eqtb=zeqtb;
void appspace (void);
#define appspace_regmem register memoryword *mem=zmem, *eqtb=zeqtb;
void insertdollarsign (void);
#define insertdollarsign_regmem
void youcant (void);
#define youcant_regmem
void reportillegalcase (void);
#define reportillegalcase_regmem
boolean privileged (void);
#define privileged_regmem
boolean itsallover (void);
#define itsallover_regmem register memoryword *mem=zmem, *eqtb=zeqtb;
void appendglue (void);
#define appendglue_regmem register memoryword *mem=zmem;
void appendkern (void);
#define appendkern_regmem register memoryword *mem=zmem;
void offsave (void);
#define offsave_regmem register memoryword *mem=zmem;
void extrarightbrace (void);
#define extrarightbrace_regmem
void normalparagraph (void);
#define normalparagraph_regmem register memoryword *eqtb=zeqtb;
void zboxend (integer boxcontext);
#define boxend(boxcontext) zboxend((integer) (boxcontext))
#define boxend_regmem register memoryword *mem=zmem;
void zbeginbox (integer boxcontext);
#define beginbox(boxcontext) zbeginbox((integer) (boxcontext))
#define beginbox_regmem register memoryword *mem=zmem, *eqtb=zeqtb;
void zscanbox (integer boxcontext);
#define scanbox(boxcontext) zscanbox((integer) (boxcontext))
#define scanbox_regmem
void zpackage (smallnumber c);
#define package(c) zpackage((smallnumber) (c))
#define package_regmem register memoryword *mem=zmem, *eqtb=zeqtb;
smallnumber znormmin (integer h);
#define normmin(h) znormmin((integer) (h))
#define normmin_regmem
void znewgraf (boolean indented);
#define newgraf(indented) znewgraf((boolean) (indented))
#define newgraf_regmem register memoryword *mem=zmem, *eqtb=zeqtb;
void indentinhmode (void);
#define indentinhmode_regmem register memoryword *mem=zmem, *eqtb=zeqtb;
void headforvmode (void);
#define headforvmode_regmem
void endgraf (void);
#define endgraf_regmem
void begininsertoradjust (void);
#define begininsertoradjust_regmem
void makemark (void);
#define makemark_regmem register memoryword *mem=zmem;
void appendpenalty (void);
#define appendpenalty_regmem register memoryword *mem=zmem;
void deletelast (void);
#define deletelast_regmem register memoryword *mem=zmem;
void unpackage (void);
#define unpackage_regmem register memoryword *mem=zmem, *eqtb=zeqtb;
void appenditaliccorrection (void);
#define appenditaliccorrection_regmem register memoryword *mem=zmem;
void appenddiscretionary (void);
#define appenddiscretionary_regmem register memoryword *mem=zmem, *eqtb=zeqtb;
void builddiscretionary (void);
#define builddiscretionary_regmem register memoryword *mem=zmem;
void makeaccent (void);
#define makeaccent_regmem register memoryword *mem=zmem, *eqtb=zeqtb;
void alignerror (void);
#define alignerror_regmem
void noalignerror (void);
#define noalignerror_regmem
void omiterror (void);
#define omiterror_regmem
void doendv (void);
#define doendv_regmem
void cserror (void);
#define cserror_regmem
void zpushmath (groupcode c);
#define pushmath(c) zpushmath((groupcode) (c))
#define pushmath_regmem
void zjustcopy (halfword p,halfword h,halfword t);
#define justcopy(p, h, t) zjustcopy((halfword) (p), (halfword) (h), (halfword) (t))
#define justcopy_regmem register memoryword *mem=zmem;
void zjustreverse (halfword p);
#define justreverse(p) zjustreverse((halfword) (p))
#define justreverse_regmem register memoryword *mem=zmem;
void initmath (void);
#define initmath_regmem register memoryword *mem=zmem, *eqtb=zeqtb;
void starteqno (void);
#define starteqno_regmem register memoryword *eqtb=zeqtb;
void zscanmath (halfword p);
#define scanmath(p) zscanmath((halfword) (p))
#define scanmath_regmem register memoryword *mem=zmem, *eqtb=zeqtb;
void zsetmathchar (integer c);
#define setmathchar(c) zsetmathchar((integer) (c))
#define setmathchar_regmem register memoryword *mem=zmem, *eqtb=zeqtb;
void mathlimitswitch (void);
#define mathlimitswitch_regmem register memoryword *mem=zmem;
void zscandelimiter (halfword p,boolean r);
#define scandelimiter(p, r) zscandelimiter((halfword) (p), (boolean) (r))
#define scandelimiter_regmem register memoryword *mem=zmem, *eqtb=zeqtb;
void mathradical (void);
#define mathradical_regmem register memoryword *mem=zmem;
void mathac (void);
#define mathac_regmem register memoryword *mem=zmem, *eqtb=zeqtb;
void appendchoices (void);
#define appendchoices_regmem register memoryword *mem=zmem;
halfword zfinmlist (halfword p);
#define finmlist(p) zfinmlist((halfword) (p))
#define finmlist_regmem register memoryword *mem=zmem;
void buildchoices (void);
#define buildchoices_regmem register memoryword *mem=zmem;
void subsup (void);
#define subsup_regmem register memoryword *mem=zmem;
void mathfraction (void);
#define mathfraction_regmem register memoryword *mem=zmem;
void mathleftright (void);
#define mathleftright_regmem register memoryword *mem=zmem;
void zappdisplay (halfword j,halfword b,scaled d);
#define appdisplay(j, b, d) zappdisplay((halfword) (j), (halfword) (b), (scaled) (d))
#define appdisplay_regmem register memoryword *mem=zmem, *eqtb=zeqtb;
void aftermath (void);
#define aftermath_regmem register memoryword *mem=zmem, *eqtb=zeqtb;
void resumeafterdisplay (void);
#define resumeafterdisplay_regmem register memoryword *eqtb=zeqtb;
void getrtoken (void);
#define getrtoken_regmem
void trapzeroglue (void);
#define trapzeroglue_regmem register memoryword *mem=zmem;
void zdoregistercommand (smallnumber a);
#define doregistercommand(a) zdoregistercommand((smallnumber) (a))
#define doregistercommand_regmem register memoryword *mem=zmem, *eqtb=zeqtb;
void alteraux (void);
#define alteraux_regmem
void alterprevgraf (void);
#define alterprevgraf_regmem
void alterpagesofar (void);
#define alterpagesofar_regmem
void alterinteger (void);
#define alterinteger_regmem
void alterboxdimen (void);
#define alterboxdimen_regmem register memoryword *mem=zmem, *eqtb=zeqtb;
void znewfont (smallnumber a);
#define newfont(a) znewfont((smallnumber) (a))
#define newfont_regmem register memoryword *eqtb=zeqtb;
void newinteraction (void);
#define newinteraction_regmem
void prefixedcommand (void);
#define prefixedcommand_regmem register memoryword *mem=zmem, *eqtb=zeqtb;
void doassignments (void);
#define doassignments_regmem
void openorclosein (void);
#define openorclosein_regmem register memoryword *eqtb=zeqtb;
void issuemessage (void);
#define issuemessage_regmem register memoryword *mem=zmem, *eqtb=zeqtb;
void shiftcase (void);
#define shiftcase_regmem register memoryword *mem=zmem, *eqtb=zeqtb;
void showwhatever (void);
#define showwhatever_regmem register memoryword *mem=zmem, *eqtb=zeqtb;
void storefmtfile (void);
#define storefmtfile_regmem register memoryword *mem=zmem, *eqtb=zeqtb;
void znewwhatsit (smallnumber s,smallnumber w);
#define newwhatsit(s, w) znewwhatsit((smallnumber) (s), (smallnumber) (w))
#define newwhatsit_regmem register memoryword *mem=zmem;
void znewwritewhatsit (smallnumber w);
#define newwritewhatsit(w) znewwritewhatsit((smallnumber) (w))
#define newwritewhatsit_regmem register memoryword *mem=zmem;
void zloadpicture (boolean ispdf);
#define loadpicture(ispdf) zloadpicture((boolean) (ispdf))
#define loadpicture_regmem register memoryword *mem=zmem;
void scanandpackname (void);
#define scanandpackname_regmem
void doextension (void);
#define doextension_regmem register memoryword *mem=zmem, *eqtb=zeqtb;
void fixlanguage (void);
#define fixlanguage_regmem register memoryword *mem=zmem, *eqtb=zeqtb;
void insertsrcspecial (void);
#define insertsrcspecial_regmem register memoryword *mem=zmem;
void appendsrcspecial (void);
#define appendsrcspecial_regmem register memoryword *mem=zmem;
void handlerightbrace (void);
#define handlerightbrace_regmem register memoryword *mem=zmem, *eqtb=zeqtb;
void maincontrol (void);
#define maincontrol_regmem register memoryword *mem=zmem, *eqtb=zeqtb;
void giveerrhelp (void);
#define giveerrhelp_regmem register memoryword *eqtb=zeqtb;
boolean openfmtfile (void);
#define openfmtfile_regmem
boolean loadfmtfile (void);
#define loadfmtfile_regmem register memoryword *mem=zmem, *eqtb=zeqtb;
void closefilesandterminate (void);
#define closefilesandterminate_regmem register memoryword *eqtb=zeqtb;
void finalcleanup (void);
#define finalcleanup_regmem register memoryword *mem=zmem;
void initprim (void);
#define initprim_regmem register memoryword *eqtb=zeqtb;
void debughelp (void);
#define debughelp_regmem register memoryword *mem=zmem, *eqtb=zeqtb;
void mainbody (void);
#define mainbody_regmem register memoryword *eqtb=zeqtb;
void zflushstr (strnumber s);
#define flushstr(s) zflushstr((strnumber) (s))
#define flushstr_regmem
strnumber ztokenstostring (halfword p);
#define tokenstostring(p) ztokenstostring((halfword) (p))
#define tokenstostring_regmem register memoryword *mem=zmem;
void scanpdfexttoks (void);
#define scanpdfexttoks_regmem
void comparestrings (void);
#define comparestrings_regmem
strnumber getnullstr (void);
#define getnullstr_regmem
/* Some definitions that get appended to the `coerce.h' file that web2c
   outputs.  */
/* $Id$ */

/* The C compiler ignores most unnecessary casts (i.e., casts of
   something to its own type).  However, for structures, it doesn't.
   Therefore, we have to redefine these macros so they don't cast
   their argument (of type memoryword or fourquarters, respectively).  */

#ifdef	printword
#undef	printword
#define	printword(x)	zprintword (x)
#endif

#ifdef	tfmqqqq
#undef	tfmqqqq
#define tfmqqqq(x)	ztfmqqqq (x)
#endif

#ifdef	eqdestroy
#undef	eqdestroy
#define	eqdestroy(x)	zeqdestroy(x)
#endif

/* And we use the opportunity to declare a few functions that could not be
   declared in texmfmp.h, because they need typedefs not yet known at that
   point.  */
extern strnumber getjobname (strnumber);

#ifdef XeTeX
/* XeTeX redefines "ASCII" types.... */
typedef packedUTF16code packedASCIIcode;
#endif
extern void calledit (packedASCIIcode *, poolpointer, integer, integer);

#ifdef MF
extern void blankrectangle (screencol, screencol, screenrow, screenrow);
extern void paintrow (screenrow, pixelcolor, transspec, screencol);
#endif

extern strnumber makefullnamestring(void);

#ifdef TeX
extern string gettexstring (strnumber);
/* Prototypes for source-specials functions... */
extern boolean isnewsource (strnumber, int);
extern poolpointer makesrcspecial (strnumber, int);
extern void remembersourceinfo (strnumber, int);

#ifdef pdfTeX
#include <pdftexdir/pdftex.h>
#endif /* pdfTeX */

#ifdef XeTeX
#include <xetexdir/xetex.h>
#endif /* XeTeX */

#ifdef __SyncTeX__
#include <synctexdir/synctex.h>
#endif
#endif /* TeX */
