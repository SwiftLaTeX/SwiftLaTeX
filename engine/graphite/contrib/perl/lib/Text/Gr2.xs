#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"

#include "ppport.h"

#include <graphite2/Font.h>
#include <graphite2/Segment.h>
#include <graphite2/Log.h>

typedef gr_face              Text_Gr2_Face;
typedef gr_font              Text_Gr2_Font;
typedef const gr_feature_ref Text_Gr2_FeatureRef;
typedef gr_feature_val       Text_Gr2_FeatureVal;
typedef const gr_char_info   Text_Gr2_CharInfo;
typedef gr_segment           Text_Gr2_Segment;
typedef const gr_slot        Text_Gr2_Slot;

#define gr_face_DESTROY gr_face_destroy
#define gr_font_DESTROY gr_font_destroy
#define gr_featureval_DESTROY gr_featureval_destroy
#define gr_seg_DESTROY gr_seg_destroy

#define gr_fref__id gr_fref_id

MODULE = Text::Gr2  PACKAGE = Text::Gr2 PREFIX = graphite_

bool gr_start_logging(SV* class, Text_Gr2_Face *face, char* logFile)
    C_ARGS:
        face, logFile

void gr_stop_logging(SV* class, Text_Gr2_Face *face)
    C_ARGS:
        face

MODULE = Text::Gr2  PACKAGE = Text::Gr2 PREFIX = gr_

gr_uint32 gr_str_to_tag(SV* class, const char *str)
    C_ARGS:
        str

char* gr_tag_to_str(SV* class, gr_uint32 tag)
    PREINIT:
        char str[5] = {0,0,0,0,0};
    CODE:
        gr_tag_to_str(tag,str);
        RETVAL = str;
    OUTPUT:
        RETVAL

MODULE = Text::Gr2  PACKAGE = Text::Gr2::Face PREFIX = gr_

#ifndef GRAPHITE2_NFILEFACE
Text_Gr2_Face* gr_make_file_face(char* classname, char* filename, unsigned int options)
    CODE:
        RETVAL = gr_make_file_face(filename, options);
        if (!RETVAL)
            croak("Couldn't open file");
    OUTPUT:
        RETVAL

Text_Gr2_Face* gr_make_file_face_with_seg_cache(char* classname, char* filename, unsigned int segCacheMaxSize, unsigned int options)
    CODE:
        RETVAL = gr_make_file_face_with_seg_cache(filename, segCacheMaxSize, options);
        if (!RETVAL)
            croak("Couldn't open file");
    OUTPUT:
        RETVAL

#endif

Text_Gr2_Font* gr_make_font(Text_Gr2_Face* gr_face, float ppm)
    C_ARGS:
        ppm, gr_face

MODULE = Text::Gr2  PACKAGE = Text::Gr2::Face PREFIX = gr_face_

gr_uint16 gr_face_n_fref(Text_Gr2_Face* pFace)

unsigned short gr_face_n_languages(Text_Gr2_Face* pFace)

unsigned short gr_face_n_glyphs(Text_Gr2_Face* pFace)

Text_Gr2_FeatureRef* gr_face_find_fref(Text_Gr2_Face* pFace, gr_uint32 featId)

Text_Gr2_FeatureRef* gr_face_fref(Text_Gr2_Face* pFace, gr_uint32 i)

Text_Gr2_FeatureVal* gr_face_featureval_for_lang(Text_Gr2_Face* pFace, gr_uint32 langname)

gr_uint32 gr_face_lang_by_index(Text_Gr2_Face* pFace, gr_uint32 i)

void gr_face_DESTROY(Text_Gr2_Face* pFace)

MODULE = Text::Gr2  PACKAGE = Text::Gr2::Font PREFIX=gr_font_

void gr_font_DESTROY(Text_Gr2_Font* pFont)

MODULE = Text::Gr2  PACKAGE = Text::Gr2::FeatureRef PREFIX=gr_fref_

gr_uint16 gr_fref_feature_value(Text_Gr2_FeatureRef* pfeatureref, Text_Gr2_FeatureVal* feats)

int gr_fref_set_feature_value(Text_Gr2_FeatureRef* pfeatureref, Text_Gr2_FeatureVal* feats, gr_uint16 val)
    C_ARGS:
        pfeatureref, val, feats

gr_uint32 gr_fref__id(Text_Gr2_FeatureRef* pfeatureref)

gr_uint16 gr_fref_n_values(Text_Gr2_FeatureRef* pfeatureref)

gr_int16 gr_fref_value(Text_Gr2_FeatureRef* pfeatureref, gr_uint16 settingno)

char* gr_fref_value_label(Text_Gr2_FeatureRef* f, gr_int16 j, gr_uint16 langId, IV encoding)
    PREINIT:
        gr_uint32 length;
    CODE:
        RETVAL = gr_fref_value_label(f, j, &langId, encoding, &length);
    OUTPUT:
        RETVAL

char* gr_fref_label(Text_Gr2_FeatureRef* f, gr_uint16 langId, IV encoding)
    PREINIT:
        gr_uint32 length;
    CODE:
        RETVAL = gr_fref_label(f, &langId, encoding, &length);
    OUTPUT:
        RETVAL

MODULE = Text::Gr2  PACKAGE = Text::Gr2::FeatureVal PREFIX=gr_featureval_

Text_Gr2_FeatureVal* gr_featureval_clone(Text_Gr2_FeatureVal* pfeatures)

void gr_featureval_DESTROY(Text_Gr2_FeatureVal* pfeatures)

MODULE = Text::Gr2  PACKAGE = Text::Gr2::CharInfo PREFIX=gr_cinfo_

unsigned int gr_cinfo_unicode_char(Text_Gr2_CharInfo* p)

int gr_cinfo_break_weight(Text_Gr2_CharInfo* p)

MODULE = Text::Gr2  PACKAGE = Text::Gr2::Font PREFIX=gr_

Text_Gr2_Segment* gr_make_seg(font, face, script, pFeats, enc, string, nChars, dir)
    Text_Gr2_Font*       font
    Text_Gr2_Face*       face
    gr_uint32             script
    Text_Gr2_FeatureVal* pFeats
    enum gr_encform enc
    SV* string
    size_t nChars
    int dir
    CODE:
        RETVAL = gr_make_seg(font,face,script,pFeats,enc,(void*)SvPVX(string),nChars, dir);
    OUTPUT:
        RETVAL

MODULE = Text::Gr2  PACKAGE = Text::Gr2::Segment PREFIX=gr_seg_

void gr_seg_DESTROY(Text_Gr2_Segment* pSeg)

float gr_seg_advance_X(Text_Gr2_Segment* pSeg)

float gr_seg_advance_Y(Text_Gr2_Segment* pSeg)

unsigned int gr_seg_n_cinfo(Text_Gr2_Segment* pSeg)

Text_Gr2_CharInfo* gr_seg_cinfo(Text_Gr2_Segment* pSeg, unsigned int index)

Text_Gr2_Slot* gr_seg_first_slot(Text_Gr2_Segment* pSeg)

Text_Gr2_Slot* gr_seg_last_slot(Text_Gr2_Segment* pSeg)

# char_slots is tricky

MODULE = Text::Gr2  PACKAGE = Text::Gr2::Slot PREFIX=gr_slot_

Text_Gr2_Slot* gr_slot_next_in_segment(Text_Gr2_Slot* p)

Text_Gr2_Slot* gr_slot_prev_in_segment(Text_Gr2_Slot* p)

Text_Gr2_Slot* gr_slot_attached_to(Text_Gr2_Slot* p)

Text_Gr2_Slot* gr_slot_first_attachment(Text_Gr2_Slot* p)

Text_Gr2_Slot* gr_slot_next_sibling_attachment(Text_Gr2_Slot* p)

unsigned short gr_slot_gid(Text_Gr2_Slot* p)

float gr_slot_origin_X(Text_Gr2_Slot* p)

float gr_slot_origin_Y(Text_Gr2_Slot* p)

float gr_slot_advance_X(Text_Gr2_Slot* p, Text_Gr2_Face* face, Text_Gr2_Font* font)

float gr_slot_advance_Y(Text_Gr2_Slot* p, Text_Gr2_Face* face, Text_Gr2_Font* font)

int gr_slot_before(Text_Gr2_Slot* p)

int gr_slot_after(Text_Gr2_Slot* p)

int gr_slot_can_insert_before(Text_Gr2_Slot* p)

int gr_slot_original(Text_Gr2_Slot* p)

int gr_slot_attr(Text_Gr2_Slot* p, Text_Gr2_Segment* pSeg, enum gr_attrCode index, gr_uint8 subindex)
