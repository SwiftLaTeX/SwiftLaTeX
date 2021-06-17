#ifndef HTMLBACKEND
#define HTMLBACKEND

#ifdef __cplusplus
extern "C" {
#endif
    unsigned int read_charglyph_map(unsigned int f, unsigned int glyph);
    void store_charglyph_map(unsigned int f, unsigned int glyph, unsigned int character);
    void clean_charglyph_map();
    void processColorSpecial(const char* bytes);
    void processBackgroundSpecial(const char* bytes);
	int processPDFSpecial(const char* bytes, double x, double y, char *buf, unsigned int bufsize);
    void getBackgroundColor(int *r, int *g, int *b);
    void getDocumentColor(int *r, int *g, int *b);
    // unsigned long long readmeta(unsigned int addr);
    // void storemeta(unsigned int addr, unsigned long long meta);
    // void copymeta(unsigned int dst, unsigned int src);
    // void clearmeta(unsigned int addr);
#ifdef __cplusplus
};
#endif


#endif