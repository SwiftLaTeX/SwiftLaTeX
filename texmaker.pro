COMPILEUSB=no
INTERNALBROWSER=no
AUTHORIZELINUXQSTYLES=yes


TEMPLATE	= app
LANGUAGE	= C++
TARGET	 = texmaker


TEXMAKERVERSION=5.0.3
DEFINES += TEXMAKERVERSION=$${TEXMAKERVERSION}


equals(QT_MAJOR_VERSION, 5):greaterThan(QT_MINOR_VERSION, 6) {
QT += core gui widgets xml network printsupport concurrent core-private script
equals(INTERNALBROWSER,yes){
QT += webenginewidgets
}  
} else {
message("Qt>=5.7 is required.")
}


CONFIG	+= qt hide_symbols warn_off rtti_off exceptions_off c++11 release
CONFIG -= precompile_header

gcc {
    QMAKE_CXXFLAGS_WARN_ON += -Wno-unused-parameter
}

msvc {
    QMAKE_CXXFLAGS_WARN_ON += -wd"4100"
}

DEFINES += \
    OPJ_STATIC \
    PNG_PREFIX \
    PNG_USE_READ_MACROS \
    V8_DEPRECATION_WARNINGS \
    NOMINMAX \
    FT2_BUILD_LIBRARY


INCLUDEPATH += \
    pdfium/ \
    pdfium \
    pdfium/third_party/ \
    pdfium/third_party/freetype/include \
    pdfium/third_party/freetype/include/freetype \
    pdfium/fpdfsdk/include \
    pdfium/third_party/zlib_v128

###############################

HEADERS	+= texmaker.h \
    theme.h \
    dropshadowlabel.h \
	geticon.h \
	documentview.h \
	pageitem.h \
	presentationview.h \
	texmakerapp.h \
	minisplitter.h \
	playerbutton.h \
	symbollistwidget.h \
	icondelegate.h \
	latexeditor.h \
	latexhighlighter.h \
	latexeditorview.h \
	linenumberwidget.h \
	lightlatexeditor.h \
	lightlatexhighlighter.h \
	lightlinenumberwidget.h \
	sourceview.h \
	logeditor.h \
	loghighlighter.h \
	findwidget.h \
	gotolinewidget.h \
	lightfindwidget.h \
	lightgotolinewidget.h \
	replacewidget.h \
	structdialog.h \
	filechooser.h \
	graphicfilechooser.h \
	tabbingdialog.h \
	arraydialog.h \
	tabdialog.h \
	letterdialog.h \
	addoptiondialog.h \
	quickdocumentdialog.h \
	quickxelatexdialog.h \
	usermenudialog.h \
	usertooldialog.h \
	refdialog.h \
	configdialog.h \
	aboutdialog.h \
	spellerdialog.h \
	xmltagslistwidget.h \
	blockdata.h \
	keysequencedialog.h \
	pdfviewerwidget.h \
	pdfviewer.h \
	userquickdialog.h \
	encodingdialog.h \
	usercompletiondialog.h \
	texdocdialog.h \
	textblockselection.h \
	scandialog.h \
	exportdialog.h \
	synctex_parser.h \
	synctex_parser_utils.h \
	usertagslistwidget.h \
	addtagdialog.h \
	versiondialog.h \
	unicodedialog.h \
	unicodeview.h \
	svnhelper.h \
	quickbeamerdialog.h \
	hunspell/affentry.hxx \
	hunspell/affixmgr.hxx \
	hunspell/atypes.hxx \
	hunspell/baseaffix.hxx \
	hunspell/csutil.hxx \
	hunspell/dictmgr.hxx \
	hunspell/hashmgr.hxx \
	hunspell/htypes.hxx \
	hunspell/hunspell.hxx \
	hunspell/hunspell.h \
	hunspell/langnum.hxx \
	hunspell/license.hunspell \
	hunspell/phonet.hxx \
	hunspell/suggestmgr.hxx \
	hunspell/license.myspell \
	hunspell/filemgr.hxx \
	hunspell/hunzip.hxx \
	hunspell/replist.hxx \
	hunspell/w_char.hxx \
	singleapp/qtlocalpeer.h \
	singleapp/qtlockedfile.h \
	singleapp/qtsingleapplication.h \
	singleapp/qtsinglecoreapplication.h \
	encodingprober/CharDistribution.h \
	encodingprober/ChineseGroupProber.h \
	encodingprober/ctype_test_p.h \
	encodingprober/JapaneseGroupProber.h \
	encodingprober/JpCntx.h \
	encodingprober/nsBig5Prober.h \
	encodingprober/nsCharSetProber.h \
	encodingprober/nsCodingStateMachine.h \
	encodingprober/nsEscCharsetProber.h \
	encodingprober/nsEUCJPProber.h \
	encodingprober/nsEUCKRProber.h \
	encodingprober/nsEUCTWProber.h \
	encodingprober/nsGB2312Prober.h \
	encodingprober/nsHebrewProber.h \
	encodingprober/nsLatin1Prober.h \
	encodingprober/nsMBCSGroupProber.h \
	encodingprober/nsPkgInt.h \
	encodingprober/nsSBCharSetProber.h \
	encodingprober/nsSBCSGroupProber.h \
	encodingprober/nsSJISProber.h \
	encodingprober/nsUniversalDetector.h \
	encodingprober/qencodingprober.h \
	encodingprober/UnicodeGroupProber.h \
    qpdfdocument.h \
    qpdfdocument_p.h \
    qpdfdocumentrenderoptions.h \
    qpdfnamespace.h \
    qpdfbookmarkmodel.h \
    pdfium/fpdfsdk/cfx_systemhandler.h \
    pdfium/fpdfsdk/include/fsdk_actionhandler.h \
    pdfium/fpdfsdk/include/fsdk_annothandler.h \
    pdfium/fpdfsdk/include/fsdk_baseannot.h \
    pdfium/fpdfsdk/include/fsdk_baseform.h \
    pdfium/public/fpdf_dataavail.h \
    pdfium/public/fpdf_doc.h \
    pdfium/public/fpdf_edit.h \
    pdfium/public/fpdf_ext.h \
    pdfium/public/fpdf_flatten.h \
    pdfium/public/fpdf_formfill.h \
    pdfium/public/fpdf_fwlevent.h \
    pdfium/public/fpdf_ppo.h \
    pdfium/public/fpdf_progressive.h \
    pdfium/public/fpdf_save.h \
    pdfium/public/fpdf_searchex.h \
    pdfium/public/fpdf_sysfontinfo.h \
    pdfium/public/fpdf_text.h \
    pdfium/public/fpdf_transformpage.h \
    pdfium/public/fpdfview.h \
    pdfium/third_party/bigint/BigInteger.hh \
    pdfium/third_party/bigint/BigIntegerLibrary.hh \
    pdfium/third_party/bigint/BigIntegerUtils.hh \
    pdfium/third_party/bigint/BigUnsigned.hh \
    pdfium/third_party/bigint/NumberlikeArray.hh \
    pdfium/third_party/bigint/BigUnsignedInABase.hh \
    pdfium/third_party/base/logging.h \
    pdfium/third_party/base/macros.h \
    pdfium/third_party/base/numerics/safe_conversions.h \
    pdfium/third_party/base/numerics/safe_conversions_impl.h \
    pdfium/third_party/base/numerics/safe_math.h \
    pdfium/third_party/base/numerics/safe_math_impl.h \
    pdfium/third_party/base/stl_util.h \
    pdfium/core/fdrm/crypto/include/fx_crypt.h \
    pdfium/core/fpdfdoc/clines.h \
    pdfium/core/fpdfdoc/include/fpdf_doc.h \
    pdfium/core/fpdfdoc/include/fpdf_tagged.h \
    pdfium/core/fpdfdoc/cpvt_color.h \
    pdfium/core/fpdfdoc/cpvt_dash.h \
    pdfium/core/fpdfdoc/cpvt_floatrect.h \
    pdfium/core/fpdfdoc/cpvt_fontmap.h \
    pdfium/core/fpdfdoc/cpvt_generateap.h \
    pdfium/core/fpdfdoc/cpvt_lineinfo.h \
    pdfium/core/fpdfdoc/cpvt_sectioninfo.h \
    pdfium/core/fpdfdoc/cpvt_wordinfo.h \
    pdfium/core/fpdfdoc/csection.h \
    pdfium/core/fpdfdoc/ctypeset.h \
    pdfium/core/fpdfdoc/doc_utils.h \
    pdfium/core/fpdfdoc/include/cpdf_variabletext.h \
    pdfium/core/fpdfdoc/include/cpvt_line.h \
    pdfium/core/fpdfdoc/include/cpvt_secprops.h \
    pdfium/core/fpdfdoc/include/cpvt_section.h \
    pdfium/core/fpdfdoc/include/cpvt_word.h \
    pdfium/core/fpdfdoc/include/cpvt_wordplace.h \
    pdfium/core/fpdfdoc/include/cpvt_wordprops.h \
    pdfium/core/fpdfdoc/include/cpvt_wordrange.h \
    pdfium/core/fpdfdoc/include/ipvt_fontmap.h \
    pdfium/core/fpdfdoc/pdf_vt.h \
    pdfium/core/fpdfdoc/tagged_int.h \
    pdfium/core/fpdfapi/fpdf_cmaps/cmap_int.h \
    pdfium/core/fpdfapi/fpdf_edit/editint.h \
    pdfium/core/fpdfapi/fpdf_edit/include/cpdf_creator.h \
    pdfium/core/fpdfapi/fpdf_edit/include/cpdf_pagecontentgenerator.h \
    pdfium/core/fpdfapi/fpdf_font/cpdf_cidfont.h \
    pdfium/core/fpdfapi/fpdf_font/cpdf_simplefont.h \
    pdfium/core/fpdfapi/fpdf_font/cpdf_truetypefont.h \
    pdfium/core/fpdfapi/fpdf_font/cpdf_type1font.h \
    pdfium/core/fpdfapi/fpdf_font/cpdf_type3char.h \
    pdfium/core/fpdfapi/fpdf_font/cpdf_type3font.h \
    pdfium/core/fpdfapi/fpdf_font/font_int.h \
    pdfium/core/fpdfapi/fpdf_font/include/cpdf_font.h \
    pdfium/core/fpdfapi/fpdf_font/include/cpdf_fontencoding.h \
    pdfium/core/fpdfapi/fpdf_font/ttgsubtable.h \
    pdfium/core/fpdfapi/fpdf_page/cpdf_allstates.h \
    pdfium/core/fpdfapi/fpdf_page/cpdf_clippathdata.h \
    pdfium/core/fpdfapi/fpdf_page/cpdf_colorstate.h \
    pdfium/core/fpdfapi/fpdf_page/cpdf_colorstatedata.h \
    pdfium/core/fpdfapi/fpdf_page/cpdf_contentmark.h \
    pdfium/core/fpdfapi/fpdf_page/cpdf_contentmarkdata.h \
    pdfium/core/fpdfapi/fpdf_page/cpdf_contentmarkitem.h \
    pdfium/core/fpdfapi/fpdf_page/cpdf_countedobject.h \
    pdfium/core/fpdfapi/fpdf_page/cpdf_graphicstates.h \
    pdfium/core/fpdfapi/fpdf_page/cpdf_graphstate.h \
    pdfium/core/fpdfapi/fpdf_page/cpdf_meshstream.h \
    pdfium/core/fpdfapi/fpdf_page/cpdf_pagemodule.h \
    pdfium/core/fpdfapi/fpdf_page/cpdf_pageobjectlist.h \
    pdfium/core/fpdfapi/fpdf_page/cpdf_pattern.h \
    pdfium/core/fpdfapi/fpdf_page/cpdf_shadingpattern.h \
    pdfium/core/fpdfapi/fpdf_page/cpdf_textstate.h \
    pdfium/core/fpdfapi/fpdf_page/cpdf_tilingpattern.h \
    pdfium/core/fpdfapi/fpdf_page/include/cpdf_clippath.h \
    pdfium/core/fpdfapi/fpdf_page/include/cpdf_color.h \
    pdfium/core/fpdfapi/fpdf_page/include/cpdf_colorspace.h \
    pdfium/core/fpdfapi/fpdf_page/include/cpdf_form.h \
    pdfium/core/fpdfapi/fpdf_page/include/cpdf_formobject.h \
    pdfium/core/fpdfapi/fpdf_page/include/cpdf_generalstatedata.h \
    pdfium/core/fpdfapi/fpdf_page/include/cpdf_image.h \
    pdfium/core/fpdfapi/fpdf_page/include/cpdf_imageobject.h \
    pdfium/core/fpdfapi/fpdf_page/include/cpdf_page.h \
    pdfium/core/fpdfapi/fpdf_page/include/cpdf_pageobject.h \
    pdfium/core/fpdfapi/fpdf_page/include/cpdf_pageobjectholder.h \
    pdfium/core/fpdfapi/fpdf_page/include/cpdf_path.h \
    pdfium/core/fpdfapi/fpdf_page/include/cpdf_pathobject.h \
    pdfium/core/fpdfapi/fpdf_page/include/cpdf_shadingobject.h \
    pdfium/core/fpdfapi/fpdf_page/include/cpdf_textobject.h \
    pdfium/core/fpdfapi/fpdf_page/include/cpdf_textstatedata.h \
    pdfium/core/fpdfapi/fpdf_page/pageint.h \
    pdfium/core/fpdfapi/fpdf_parser/cpdf_boolean.h \
    pdfium/core/fpdfapi/fpdf_parser/cpdf_crypto_handler.h \
    pdfium/core/fpdfapi/fpdf_parser/cpdf_hint_tables.h \
    pdfium/core/fpdfapi/fpdf_parser/cpdf_null.h \
    pdfium/core/fpdfapi/fpdf_parser/cpdf_security_handler.h \
    pdfium/core/fpdfapi/fpdf_parser/cpdf_syntax_parser.h \
    pdfium/core/fpdfapi/fpdf_parser/fpdf_parser_utility.h \
    pdfium/core/fpdfapi/fpdf_parser/include/cfdf_document.h \
    pdfium/core/fpdfapi/fpdf_parser/include/cpdf_array.h \
    pdfium/core/fpdfapi/fpdf_parser/include/cpdf_data_avail.h \
    pdfium/core/fpdfapi/fpdf_parser/include/cpdf_dictionary.h \
    pdfium/core/fpdfapi/fpdf_parser/include/cpdf_document.h \
    pdfium/core/fpdfapi/fpdf_parser/include/cpdf_indirect_object_holder.h \
    pdfium/core/fpdfapi/fpdf_parser/include/cpdf_name.h \
    pdfium/core/fpdfapi/fpdf_parser/include/cpdf_number.h \
    pdfium/core/fpdfapi/fpdf_parser/include/cpdf_object.h \
    pdfium/core/fpdfapi/fpdf_parser/include/cpdf_parser.h \
    pdfium/core/fpdfapi/fpdf_parser/include/cpdf_reference.h \
    pdfium/core/fpdfapi/fpdf_parser/include/cpdf_simple_parser.h \
    pdfium/core/fpdfapi/fpdf_parser/include/cpdf_stream.h \
    pdfium/core/fpdfapi/fpdf_parser/include/cpdf_stream_acc.h \
    pdfium/core/fpdfapi/fpdf_parser/include/cpdf_string.h \
    pdfium/core/fpdfapi/fpdf_parser/include/fpdf_parser_decode.h \
    pdfium/core/fpdfapi/fpdf_render/cpdf_pagerendercache.h \
    pdfium/core/fpdfapi/fpdf_render/include/cpdf_progressiverenderer.h \
    pdfium/core/fpdfapi/fpdf_render/include/cpdf_rendercontext.h \
    pdfium/core/fpdfapi/fpdf_render/include/cpdf_renderoptions.h \
    pdfium/core/fpdfapi/fpdf_render/include/cpdf_textrenderer.h \
    pdfium/core/fpdfapi/fpdf_render/render_int.h \
    pdfium/core/fpdfapi/include/cpdf_modulemgr.h \
    pdfium/core/fpdftext/include/cpdf_linkextract.h \
    pdfium/core/fpdftext/include/cpdf_textpage.h \
    pdfium/core/fpdftext/include/cpdf_textpagefind.h \
    pdfium/core/fpdftext/unicodenormalizationdata.h \
    pdfium/fpdfsdk/formfiller/cba_fontmap.h \
    pdfium/fpdfsdk/formfiller/cffl_checkbox.h \
    pdfium/fpdfsdk/formfiller/cffl_combobox.h \
    pdfium/fpdfsdk/formfiller/cffl_formfiller.h \
    pdfium/fpdfsdk/formfiller/cffl_iformfiller.h \
    pdfium/fpdfsdk/formfiller/cffl_listbox.h \
    pdfium/fpdfsdk/formfiller/cffl_pushbutton.h \
    pdfium/fpdfsdk/formfiller/cffl_radiobutton.h \
    pdfium/fpdfsdk/formfiller/cffl_textfield.h \
    pdfium/core/fxcodec/include/fx_codec.h \
    pdfium/core/fxcodec/include/fx_codec_def.h \
    pdfium/core/fxcodec/codec/ccodec_basicmodule.h \
    pdfium/core/fxcodec/codec/ccodec_faxmodule.h \
    pdfium/core/fxcodec/codec/ccodec_flatemodule.h \
    pdfium/core/fxcodec/codec/ccodec_iccmodule.h \
    pdfium/core/fxcodec/codec/ccodec_jbig2module.h \
    pdfium/core/fxcodec/codec/ccodec_jpegmodule.h \
    pdfium/core/fxcodec/codec/ccodec_jpxmodule.h \
    pdfium/core/fxcodec/codec/ccodec_scanlinedecoder.h \
    pdfium/core/fxcodec/codec/codec_int.h \
    pdfium/core/fxcodec/jbig2/JBig2_ArithDecoder.h \
    pdfium/core/fxcodec/jbig2/JBig2_ArithIntDecoder.h \
    pdfium/core/fxcodec/jbig2/JBig2_BitStream.h \
    pdfium/core/fxcodec/jbig2/JBig2_Context.h \
    pdfium/core/fxcodec/jbig2/JBig2_Define.h \
    pdfium/core/fxcodec/jbig2/JBig2_GrdProc.h \
    pdfium/core/fxcodec/jbig2/JBig2_GrrdProc.h \
    pdfium/core/fxcodec/jbig2/JBig2_GsidProc.h \
    pdfium/core/fxcodec/jbig2/JBig2_HtrdProc.h \
    pdfium/core/fxcodec/jbig2/JBig2_HuffmanDecoder.h \
    pdfium/core/fxcodec/jbig2/JBig2_HuffmanTable.h \
    pdfium/core/fxcodec/jbig2/JBig2_HuffmanTable_Standard.h \
    pdfium/core/fxcodec/jbig2/JBig2_Image.h \
    pdfium/core/fxcodec/jbig2/JBig2_List.h \
    pdfium/core/fxcodec/jbig2/JBig2_Page.h \
    pdfium/core/fxcodec/jbig2/JBig2_PatternDict.h \
    pdfium/core/fxcodec/jbig2/JBig2_PddProc.h \
    pdfium/core/fxcodec/jbig2/JBig2_SddProc.h \
    pdfium/core/fxcodec/jbig2/JBig2_Segment.h \
    pdfium/core/fxcodec/jbig2/JBig2_SymbolDict.h \
    pdfium/core/fxcodec/jbig2/JBig2_TrdProc.h \
    pdfium/third_party/libjpeg/cderror.h \
    pdfium/third_party/libjpeg/cdjpeg.h \
    pdfium/third_party/libjpeg/jchuff.h \
    pdfium/third_party/libjpeg/jconfig.h \
    pdfium/third_party/libjpeg/jdct.h \
    pdfium/third_party/libjpeg/jdhuff.h \
    pdfium/third_party/libjpeg/jerror.h \
    pdfium/third_party/libjpeg/jinclude.h \
    pdfium/third_party/libjpeg/jmemsys.h \
    pdfium/third_party/libjpeg/jmorecfg.h \
    pdfium/third_party/libjpeg/jpegint.h \
    pdfium/third_party/libjpeg/jpeglib.h \
    pdfium/third_party/libjpeg/jversion.h \
    pdfium/third_party/libjpeg/transupp.h \
    pdfium/third_party/lcms2-2.6/include/lcms2.h \
    pdfium/third_party/lcms2-2.6/include/lcms2_plugin.h \
    pdfium/core/fxcrt/cfx_string_c_template.h \
    pdfium/core/fxcrt/cfx_string_data_template.h \
    pdfium/core/fxcrt/extension.h \
    pdfium/core/fxcrt/fx_bidi.h \
    pdfium/core/fxcrt/fxcrt_posix.h \
    pdfium/core/fxcrt/fxcrt_windows.h \
    pdfium/core/fxcrt/include/cfx_retain_ptr.h \
    pdfium/core/fxcrt/include/fx_basic.h \
    pdfium/core/fxcrt/include/fx_coordinates.h \
    pdfium/core/fxcrt/include/fx_ext.h \
    pdfium/core/fxcrt/include/fx_memory.h \
    pdfium/core/fxcrt/include/fx_safe_types.h \
    pdfium/core/fxcrt/include/fx_stream.h \
    pdfium/core/fxcrt/include/fx_string.h \
    pdfium/core/fxcrt/include/fx_system.h \
    pdfium/core/fxcrt/include/fx_ucd.h \
    pdfium/core/fxcrt/include/fx_xml.h \
    pdfium/core/fxcrt/plex.h \
    pdfium/core/fxcrt/xml_int.h \
    pdfium/fpdfsdk/fxedit/include/fx_edit.h \
    pdfium/fpdfsdk/fxedit/include/fxet_edit.h \
    pdfium/fpdfsdk/fxedit/include/fxet_list.h \
    pdfium/core/fxge/agg/fx_agg_driver.h \
    pdfium/core/fxge/android/fpf_skiafont.h \
    pdfium/core/fxge/android/fpf_skiafontmgr.h \
    pdfium/core/fxge/android/fpf_skiamodule.h \
    pdfium/core/fxge/android/fx_android_font.h \
    pdfium/core/fxge/apple/apple_int.h \
    pdfium/core/fxge/dib/dib_int.h \
    pdfium/core/fxge/fontdata/chromefontdata/chromefontdata.h \
    pdfium/core/fxge/ge/fx_text_int.h \
    pdfium/core/fxge/include/fx_dib.h \
    pdfium/core/fxge/include/fx_font.h \
    pdfium/core/fxge/include/fx_freetype.h \
    pdfium/core/fxge/include/fx_ge.h \
    pdfium/core/fxge/include/fx_ge_apple.h \
    pdfium/core/fxge/include/fx_ge_win32.h \
    pdfium/core/fxge/include/ifx_renderdevicedriver.h \
    pdfium/third_party/agg23/agg_basics.h \
    pdfium/third_party/agg23/agg_clip_liang_barsky.h \
    pdfium/third_party/agg23/agg_conv_dash.h \
    pdfium/third_party/agg23/agg_conv_stroke.h \
    pdfium/third_party/agg23/agg_curves.h \
    pdfium/third_party/agg23/agg_path_storage.h \
    pdfium/third_party/agg23/agg_rasterizer_scanline_aa.h \
    pdfium/third_party/agg23/agg_renderer_scanline.h \
    pdfium/third_party/agg23/agg_rendering_buffer.h \
    pdfium/third_party/agg23/agg_scanline_u.h \
    pdfium/fpdfsdk/pdfwindow/PWL_Button.h \
    pdfium/fpdfsdk/pdfwindow/PWL_Caret.h \
    pdfium/fpdfsdk/pdfwindow/PWL_ComboBox.h \
    pdfium/fpdfsdk/pdfwindow/PWL_Edit.h \
    pdfium/fpdfsdk/pdfwindow/PWL_EditCtrl.h \
    pdfium/fpdfsdk/pdfwindow/PWL_FontMap.h \
    pdfium/fpdfsdk/pdfwindow/PWL_Icon.h \
    pdfium/fpdfsdk/pdfwindow/PWL_ListBox.h \
    pdfium/fpdfsdk/pdfwindow/PWL_ScrollBar.h \
    pdfium/fpdfsdk/pdfwindow/PWL_SpecialButton.h \
    pdfium/fpdfsdk/pdfwindow/PWL_Utils.h \
    pdfium/fpdfsdk/pdfwindow/PWL_Wnd.h \
    pdfium/third_party/freetype/include/freetype/freetype.h \
    pdfium/third_party/freetype/include/freetype/ftmm.h \
    pdfium/third_party/freetype/include/freetype/ftotval.h \
    pdfium/third_party/freetype/include/freetype/ftoutln.h \
    pdfium/third_party/freetype/include/freetype/internal/ftobjs.h \
    pdfium/third_party/freetype/include/freetype/internal/ftstream.h \
    pdfium/third_party/freetype/include/freetype/internal/tttypes.h \
    pdfium/third_party/freetype/include/freetype/tttables.h \
    pdfium/third_party/freetype/include/ft2build.h \
    pdfium/third_party/freetype/src/base/ftbase.h \
    pdfium/third_party/freetype/src/cff/cffobjs.h \
    pdfium/third_party/freetype/src/cff/cfftypes.h

SOURCES	+= main.cpp \
    dropshadowlabel.cpp \
	geticon.cpp \
	texmakerapp.cpp \
	texmaker.cpp \
	documentview.cpp \
	pageitem.cpp \
	presentationview.cpp \
	minisplitter.cpp \
	playerbutton.cpp \
	symbollistwidget.cpp \
	icondelegate.cpp \
	latexeditor.cpp \
	latexhighlighter.cpp \
	latexeditorview.cpp \
	linenumberwidget.cpp \
	lightlatexeditor.cpp \
	lightlatexhighlighter.cpp \
	lightlinenumberwidget.cpp \
	sourceview.cpp \
	logeditor.cpp \
	loghighlighter.cpp \
	findwidget.cpp \
	gotolinewidget.cpp \
	lightfindwidget.cpp \
	lightgotolinewidget.cpp \
	replacewidget.cpp \
	structdialog.cpp \
	filechooser.cpp \
	graphicfilechooser.cpp \
	tabbingdialog.cpp \
	arraydialog.cpp \
	tabdialog.cpp \
	letterdialog.cpp \
	addoptiondialog.cpp \
	quickdocumentdialog.cpp \
	quickxelatexdialog.cpp \
	usermenudialog.cpp \
	usertooldialog.cpp \
	refdialog.cpp \
	configdialog.cpp \
	aboutdialog.cpp \
	spellerdialog.cpp \
	xmltagslistwidget.cpp \
	blockdata.cpp \
	keysequencedialog.cpp \
	pdfviewerwidget.cpp \
	pdfviewer.cpp \
	userquickdialog.cpp \
	encodingdialog.cpp \
	usercompletiondialog.cpp \
	texdocdialog.cpp \
	scandialog.cpp \
	exportdialog.cpp \
	synctex_parser.c \
	synctex_parser_utils.c \
	usertagslistwidget.cpp \
	addtagdialog.cpp \
	versiondialog.cpp \
	unicodedialog.cpp \
	unicodeview.cpp \
	quickbeamerdialog.cpp \
	svnhelper.cpp \
	hunspell/affentry.cxx \
	hunspell/affixmgr.cxx \
	hunspell/csutil.cxx \
	hunspell/dictmgr.cxx \
	hunspell/hashmgr.cxx \
	hunspell/hunspell.cxx \
	hunspell/phonet.cxx \
	hunspell/suggestmgr.cxx \
	hunspell/utf_info.cxx \
	hunspell/filemgr.cxx \
	hunspell/replist.cxx \
	hunspell/hunzip.cxx \
	singleapp/qtlocalpeer.cpp \
	singleapp/qtlockedfile.cpp \
	singleapp/qtsingleapplication.cpp \
	singleapp/qtsinglecoreapplication.cpp \
	encodingprober/CharDistribution.cpp \
	encodingprober/ChineseGroupProber.cpp \
	encodingprober/JapaneseGroupProber.cpp \
	encodingprober/JpCntx.cpp \
	encodingprober/LangBulgarianModel.cpp \
	encodingprober/LangCyrillicModel.cpp \
	encodingprober/LangGreekModel.cpp \
	encodingprober/LangHebrewModel.cpp \
	encodingprober/LangHungarianModel.cpp \
	encodingprober/LangThaiModel.cpp \
	encodingprober/nsBig5Prober.cpp \
	encodingprober/nsCharSetProber.cpp \
	encodingprober/nsEscCharsetProber.cpp \
	encodingprober/nsEscSM.cpp \
	encodingprober/nsEUCJPProber.cpp \
	encodingprober/nsEUCKRProber.cpp \
	encodingprober/nsEUCTWProber.cpp \
	encodingprober/nsGB2312Prober.cpp \
	encodingprober/nsHebrewProber.cpp \
	encodingprober/nsLatin1Prober.cpp \
	encodingprober/nsMBCSGroupProber.cpp \
	encodingprober/nsMBCSSM.cpp \
	encodingprober/nsSBCharSetProber.cpp \
	encodingprober/nsSBCSGroupProber.cpp \
	encodingprober/nsSJISProber.cpp \
	encodingprober/nsUniversalDetector.cpp \
	encodingprober/qencodingprober.cpp \
	encodingprober/UnicodeGroupProber.cpp \
    jsbridge.cpp \
    qpdfdocument.cpp \
    qpdfbookmarkmodel.cpp \
    pdfium/fpdfsdk/cfx_systemhandler.cpp \
    pdfium/fpdfsdk/fpdfdoc.cpp \
    pdfium/fpdfsdk/fpdfeditimg.cpp \
    pdfium/fpdfsdk/fpdfeditpage.cpp \
    pdfium/fpdfsdk/fpdfformfill.cpp \
    pdfium/fpdfsdk/fpdfppo.cpp \
    pdfium/fpdfsdk/fpdfsave.cpp \
    pdfium/fpdfsdk/fpdftext.cpp \
    pdfium/fpdfsdk/fpdfview.cpp \
    pdfium/fpdfsdk/fpdf_dataavail.cpp \
    pdfium/fpdfsdk/fpdf_ext.cpp \
    pdfium/fpdfsdk/fpdf_flatten.cpp \
    pdfium/fpdfsdk/fpdf_progressive.cpp \
    pdfium/fpdfsdk/fpdf_searchex.cpp \
    pdfium/fpdfsdk/fpdf_sysfontinfo.cpp \
    pdfium/fpdfsdk/fpdf_transformpage.cpp \
    pdfium/fpdfsdk/fsdk_actionhandler.cpp \
    pdfium/fpdfsdk/fsdk_annothandler.cpp \
    pdfium/fpdfsdk/fsdk_baseannot.cpp \
    pdfium/fpdfsdk/fsdk_baseform.cpp \
    pdfium/fpdfsdk/fsdk_mgr.cpp \
    pdfium/fpdfsdk/fsdk_rendercontext.cpp \
    pdfium/third_party/bigint/BigInteger.cc \
    pdfium/third_party/bigint/BigIntegerUtils.cc \
    pdfium/third_party/bigint/BigUnsigned.cc \
    pdfium/third_party/bigint/BigUnsignedInABase.cc \
    pdfium/core/fdrm/crypto/fx_crypt.cpp \
    pdfium/core/fdrm/crypto/fx_crypt_aes.cpp \
    pdfium/core/fdrm/crypto/fx_crypt_sha.cpp \
    pdfium/core/fpdfdoc/clines.cpp \
    pdfium/core/fpdfdoc/cpdf_variabletext.cpp \
    pdfium/core/fpdfdoc/cpvt_color.cpp \
    pdfium/core/fpdfdoc/cpvt_fontmap.cpp \
    pdfium/core/fpdfdoc/cpvt_generateap.cpp \
    pdfium/core/fpdfdoc/cpvt_sectioninfo.cpp \
    pdfium/core/fpdfdoc/cpvt_wordinfo.cpp \
    pdfium/core/fpdfdoc/csection.cpp \
    pdfium/core/fpdfdoc/ctypeset.cpp \
    pdfium/core/fpdfdoc/doc_action.cpp \
    pdfium/core/fpdfdoc/doc_annot.cpp \
    pdfium/core/fpdfdoc/doc_basic.cpp \
    pdfium/core/fpdfdoc/doc_bookmark.cpp \
    pdfium/core/fpdfdoc/doc_form.cpp \
    pdfium/core/fpdfdoc/doc_formcontrol.cpp \
    pdfium/core/fpdfdoc/doc_formfield.cpp \
    pdfium/core/fpdfdoc/doc_link.cpp \
    pdfium/core/fpdfdoc/doc_metadata.cpp \
    pdfium/core/fpdfdoc/doc_ocg.cpp \
    pdfium/core/fpdfdoc/doc_tagged.cpp \
    pdfium/core/fpdfdoc/doc_utils.cpp \
    pdfium/core/fpdfdoc/doc_viewerPreferences.cpp \
    pdfium/core/fpdfdoc/doc_vt.cpp \
    pdfium/core/fpdfapi/cpdf_modulemgr.cpp \
    pdfium/core/fpdfapi/fpdf_cmaps/CNS1/Adobe-CNS1-UCS2_5.cpp \
    pdfium/core/fpdfapi/fpdf_cmaps/CNS1/B5pc-H_0.cpp \
    pdfium/core/fpdfapi/fpdf_cmaps/CNS1/B5pc-V_0.cpp \
    pdfium/core/fpdfapi/fpdf_cmaps/CNS1/CNS-EUC-H_0.cpp \
    pdfium/core/fpdfapi/fpdf_cmaps/CNS1/CNS-EUC-V_0.cpp \
    pdfium/core/fpdfapi/fpdf_cmaps/CNS1/ETen-B5-H_0.cpp \
    pdfium/core/fpdfapi/fpdf_cmaps/CNS1/ETen-B5-V_0.cpp \
    pdfium/core/fpdfapi/fpdf_cmaps/CNS1/ETenms-B5-H_0.cpp \
    pdfium/core/fpdfapi/fpdf_cmaps/CNS1/ETenms-B5-V_0.cpp \
    pdfium/core/fpdfapi/fpdf_cmaps/CNS1/HKscs-B5-H_5.cpp \
    pdfium/core/fpdfapi/fpdf_cmaps/CNS1/HKscs-B5-V_5.cpp \
    pdfium/core/fpdfapi/fpdf_cmaps/CNS1/UniCNS-UCS2-H_3.cpp \
    pdfium/core/fpdfapi/fpdf_cmaps/CNS1/UniCNS-UCS2-V_3.cpp \
    pdfium/core/fpdfapi/fpdf_cmaps/CNS1/UniCNS-UTF16-H_0.cpp \
    pdfium/core/fpdfapi/fpdf_cmaps/CNS1/cmaps_cns1.cpp \
    pdfium/core/fpdfapi/fpdf_cmaps/GB1/Adobe-GB1-UCS2_5.cpp \
    pdfium/core/fpdfapi/fpdf_cmaps/GB1/GB-EUC-H_0.cpp \
    pdfium/core/fpdfapi/fpdf_cmaps/GB1/GB-EUC-V_0.cpp \
    pdfium/core/fpdfapi/fpdf_cmaps/GB1/GBK-EUC-H_2.cpp \
    pdfium/core/fpdfapi/fpdf_cmaps/GB1/GBK-EUC-V_2.cpp \
    pdfium/core/fpdfapi/fpdf_cmaps/GB1/GBK2K-H_5.cpp \
    pdfium/core/fpdfapi/fpdf_cmaps/GB1/GBK2K-V_5.cpp \
    pdfium/core/fpdfapi/fpdf_cmaps/GB1/GBKp-EUC-H_2.cpp \
    pdfium/core/fpdfapi/fpdf_cmaps/GB1/GBKp-EUC-V_2.cpp \
    pdfium/core/fpdfapi/fpdf_cmaps/GB1/GBpc-EUC-H_0.cpp \
    pdfium/core/fpdfapi/fpdf_cmaps/GB1/GBpc-EUC-V_0.cpp \
    pdfium/core/fpdfapi/fpdf_cmaps/GB1/UniGB-UCS2-H_4.cpp \
    pdfium/core/fpdfapi/fpdf_cmaps/GB1/UniGB-UCS2-V_4.cpp \
    pdfium/core/fpdfapi/fpdf_cmaps/GB1/cmaps_gb1.cpp \
    pdfium/core/fpdfapi/fpdf_cmaps/Japan1/83pv-RKSJ-H_1.cpp \
    pdfium/core/fpdfapi/fpdf_cmaps/Japan1/90ms-RKSJ-H_2.cpp \
    pdfium/core/fpdfapi/fpdf_cmaps/Japan1/90ms-RKSJ-V_2.cpp \
    pdfium/core/fpdfapi/fpdf_cmaps/Japan1/90msp-RKSJ-H_2.cpp \
    pdfium/core/fpdfapi/fpdf_cmaps/Japan1/90msp-RKSJ-V_2.cpp \
    pdfium/core/fpdfapi/fpdf_cmaps/Japan1/90pv-RKSJ-H_1.cpp \
    pdfium/core/fpdfapi/fpdf_cmaps/Japan1/Add-RKSJ-H_1.cpp \
    pdfium/core/fpdfapi/fpdf_cmaps/Japan1/Add-RKSJ-V_1.cpp \
    pdfium/core/fpdfapi/fpdf_cmaps/Japan1/Adobe-Japan1-UCS2_4.cpp \
    pdfium/core/fpdfapi/fpdf_cmaps/Japan1/EUC-H_1.cpp \
    pdfium/core/fpdfapi/fpdf_cmaps/Japan1/EUC-V_1.cpp \
    pdfium/core/fpdfapi/fpdf_cmaps/Japan1/Ext-RKSJ-H_2.cpp \
    pdfium/core/fpdfapi/fpdf_cmaps/Japan1/Ext-RKSJ-V_2.cpp \
    pdfium/core/fpdfapi/fpdf_cmaps/Japan1/H_1.cpp \
    pdfium/core/fpdfapi/fpdf_cmaps/Japan1/UniJIS-UCS2-HW-H_4.cpp \
    pdfium/core/fpdfapi/fpdf_cmaps/Japan1/UniJIS-UCS2-HW-V_4.cpp \
    pdfium/core/fpdfapi/fpdf_cmaps/Japan1/UniJIS-UCS2-H_4.cpp \
    pdfium/core/fpdfapi/fpdf_cmaps/Japan1/UniJIS-UCS2-V_4.cpp \
    pdfium/core/fpdfapi/fpdf_cmaps/Japan1/V_1.cpp \
    pdfium/core/fpdfapi/fpdf_cmaps/Japan1/cmaps_japan1.cpp \
    pdfium/core/fpdfapi/fpdf_cmaps/Korea1/Adobe-Korea1-UCS2_2.cpp \
    pdfium/core/fpdfapi/fpdf_cmaps/Korea1/KSC-EUC-H_0.cpp \
    pdfium/core/fpdfapi/fpdf_cmaps/Korea1/KSC-EUC-V_0.cpp \
    pdfium/core/fpdfapi/fpdf_cmaps/Korea1/KSCms-UHC-HW-H_1.cpp \
    pdfium/core/fpdfapi/fpdf_cmaps/Korea1/KSCms-UHC-HW-V_1.cpp \
    pdfium/core/fpdfapi/fpdf_cmaps/Korea1/KSCms-UHC-H_1.cpp \
    pdfium/core/fpdfapi/fpdf_cmaps/Korea1/KSCms-UHC-V_1.cpp \
    pdfium/core/fpdfapi/fpdf_cmaps/Korea1/KSCpc-EUC-H_0.cpp \
    pdfium/core/fpdfapi/fpdf_cmaps/Korea1/UniKS-UCS2-H_1.cpp \
    pdfium/core/fpdfapi/fpdf_cmaps/Korea1/UniKS-UCS2-V_1.cpp \
    pdfium/core/fpdfapi/fpdf_cmaps/Korea1/UniKS-UTF16-H_0.cpp \
    pdfium/core/fpdfapi/fpdf_cmaps/Korea1/cmaps_korea1.cpp \
    pdfium/core/fpdfapi/fpdf_cmaps/fpdf_cmaps.cpp \
    pdfium/core/fpdfapi/fpdf_edit/cpdf_pagecontentgenerator.cpp \
    pdfium/core/fpdfapi/fpdf_edit/fpdf_edit_create.cpp \
    pdfium/core/fpdfapi/fpdf_font/cpdf_cidfont.cpp \
    pdfium/core/fpdfapi/fpdf_font/cpdf_font.cpp \
    pdfium/core/fpdfapi/fpdf_font/cpdf_fontencoding.cpp \
    pdfium/core/fpdfapi/fpdf_font/cpdf_simplefont.cpp \
    pdfium/core/fpdfapi/fpdf_font/cpdf_truetypefont.cpp \
    pdfium/core/fpdfapi/fpdf_font/cpdf_type1font.cpp \
    pdfium/core/fpdfapi/fpdf_font/cpdf_type3char.cpp \
    pdfium/core/fpdfapi/fpdf_font/cpdf_type3font.cpp \
    pdfium/core/fpdfapi/fpdf_font/fpdf_font.cpp \
    pdfium/core/fpdfapi/fpdf_font/fpdf_font_cid.cpp \
    pdfium/core/fpdfapi/fpdf_font/ttgsubtable.cpp \
    pdfium/core/fpdfapi/fpdf_page/cpdf_allstates.cpp \
    pdfium/core/fpdfapi/fpdf_page/cpdf_clippath.cpp \
    pdfium/core/fpdfapi/fpdf_page/cpdf_clippathdata.cpp \
    pdfium/core/fpdfapi/fpdf_page/cpdf_color.cpp \
    pdfium/core/fpdfapi/fpdf_page/cpdf_colorspace.cpp \
    pdfium/core/fpdfapi/fpdf_page/cpdf_colorstate.cpp \
    pdfium/core/fpdfapi/fpdf_page/cpdf_colorstatedata.cpp \
    pdfium/core/fpdfapi/fpdf_page/cpdf_contentmark.cpp \
    pdfium/core/fpdfapi/fpdf_page/cpdf_contentmarkdata.cpp \
    pdfium/core/fpdfapi/fpdf_page/cpdf_contentmarkitem.cpp \
    pdfium/core/fpdfapi/fpdf_page/cpdf_form.cpp \
    pdfium/core/fpdfapi/fpdf_page/cpdf_formobject.cpp \
    pdfium/core/fpdfapi/fpdf_page/cpdf_generalstate.cpp \
    pdfium/core/fpdfapi/fpdf_page/cpdf_generalstatedata.cpp \
    pdfium/core/fpdfapi/fpdf_page/cpdf_graphicstates.cpp \
    pdfium/core/fpdfapi/fpdf_page/cpdf_image.cpp \
    pdfium/core/fpdfapi/fpdf_page/cpdf_imageobject.cpp \
    pdfium/core/fpdfapi/fpdf_page/cpdf_meshstream.cpp \
    pdfium/core/fpdfapi/fpdf_page/cpdf_page.cpp \
    pdfium/core/fpdfapi/fpdf_page/cpdf_pagemodule.cpp \
    pdfium/core/fpdfapi/fpdf_page/cpdf_pageobject.cpp \
    pdfium/core/fpdfapi/fpdf_page/cpdf_pageobjectholder.cpp \
    pdfium/core/fpdfapi/fpdf_page/cpdf_pageobjectlist.cpp \
    pdfium/core/fpdfapi/fpdf_page/cpdf_pathobject.cpp \
    pdfium/core/fpdfapi/fpdf_page/cpdf_pattern.cpp \
    pdfium/core/fpdfapi/fpdf_page/cpdf_shadingobject.cpp \
    pdfium/core/fpdfapi/fpdf_page/cpdf_shadingpattern.cpp \
    pdfium/core/fpdfapi/fpdf_page/cpdf_textobject.cpp \
    pdfium/core/fpdfapi/fpdf_page/cpdf_textstate.cpp \
    pdfium/core/fpdfapi/fpdf_page/cpdf_textstatedata.cpp \
    pdfium/core/fpdfapi/fpdf_page/cpdf_tilingpattern.cpp \
    pdfium/core/fpdfapi/fpdf_page/fpdf_page_colors.cpp \
    pdfium/core/fpdfapi/fpdf_page/fpdf_page_doc.cpp \
    pdfium/core/fpdfapi/fpdf_page/fpdf_page_func.cpp \
    pdfium/core/fpdfapi/fpdf_page/fpdf_page_parser.cpp \
    pdfium/core/fpdfapi/fpdf_page/fpdf_page_parser_old.cpp \
    pdfium/core/fpdfapi/fpdf_parser/cfdf_document.cpp \
    pdfium/core/fpdfapi/fpdf_parser/cpdf_array.cpp \
    pdfium/core/fpdfapi/fpdf_parser/cpdf_boolean.cpp \
    pdfium/core/fpdfapi/fpdf_parser/cpdf_crypto_handler.cpp \
    pdfium/core/fpdfapi/fpdf_parser/cpdf_data_avail.cpp \
    pdfium/core/fpdfapi/fpdf_parser/cpdf_dictionary.cpp \
    pdfium/core/fpdfapi/fpdf_parser/cpdf_document.cpp \
    pdfium/core/fpdfapi/fpdf_parser/cpdf_hint_tables.cpp \
    pdfium/core/fpdfapi/fpdf_parser/cpdf_indirect_object_holder.cpp \
    pdfium/core/fpdfapi/fpdf_parser/cpdf_name.cpp \
    pdfium/core/fpdfapi/fpdf_parser/cpdf_null.cpp \
    pdfium/core/fpdfapi/fpdf_parser/cpdf_number.cpp \
    pdfium/core/fpdfapi/fpdf_parser/cpdf_object.cpp \
    pdfium/core/fpdfapi/fpdf_parser/cpdf_parser.cpp \
    pdfium/core/fpdfapi/fpdf_parser/cpdf_reference.cpp \
    pdfium/core/fpdfapi/fpdf_parser/cpdf_security_handler.cpp \
    pdfium/core/fpdfapi/fpdf_parser/cpdf_simple_parser.cpp \
    pdfium/core/fpdfapi/fpdf_parser/cpdf_stream.cpp \
    pdfium/core/fpdfapi/fpdf_parser/cpdf_stream_acc.cpp \
    pdfium/core/fpdfapi/fpdf_parser/cpdf_string.cpp \
    pdfium/core/fpdfapi/fpdf_parser/cpdf_syntax_parser.cpp \
    pdfium/core/fpdfapi/fpdf_parser/fpdf_parser_decode.cpp \
    pdfium/core/fpdfapi/fpdf_parser/fpdf_parser_utility.cpp \
    pdfium/core/fpdfapi/fpdf_render/fpdf_render.cpp \
    pdfium/core/fpdfapi/fpdf_render/fpdf_render_cache.cpp \
    pdfium/core/fpdfapi/fpdf_render/fpdf_render_image.cpp \
    pdfium/core/fpdfapi/fpdf_render/fpdf_render_loadimage.cpp \
    pdfium/core/fpdfapi/fpdf_render/fpdf_render_pattern.cpp \
    pdfium/core/fpdfapi/fpdf_render/fpdf_render_text.cpp \
    pdfium/core/fpdftext/fpdf_text_int.cpp \
    pdfium/core/fpdftext/unicodenormalizationdata.cpp \
    pdfium/fpdfsdk/formfiller/cba_fontmap.cpp \
    pdfium/fpdfsdk/formfiller/cffl_checkbox.cpp \
    pdfium/fpdfsdk/formfiller/cffl_combobox.cpp \
    pdfium/fpdfsdk/formfiller/cffl_formfiller.cpp \
    pdfium/fpdfsdk/formfiller/cffl_iformfiller.cpp \
    pdfium/fpdfsdk/formfiller/cffl_listbox.cpp \
    pdfium/fpdfsdk/formfiller/cffl_pushbutton.cpp \
    pdfium/fpdfsdk/formfiller/cffl_radiobutton.cpp \
    pdfium/fpdfsdk/formfiller/cffl_textfield.cpp \
    pdfium/core/fxcodec/codec/fx_codec.cpp \
    pdfium/core/fxcodec/codec/fx_codec_fax.cpp \
    pdfium/core/fxcodec/codec/fx_codec_flate.cpp \
    pdfium/core/fxcodec/codec/fx_codec_icc.cpp \
    pdfium/core/fxcodec/codec/fx_codec_jbig.cpp \
    pdfium/core/fxcodec/codec/fx_codec_jpeg.cpp \
    pdfium/core/fxcodec/codec/fx_codec_jpx_opj.cpp \
    pdfium/core/fxcodec/jbig2/JBig2_ArithDecoder.cpp \
    pdfium/core/fxcodec/jbig2/JBig2_ArithIntDecoder.cpp \
    pdfium/core/fxcodec/jbig2/JBig2_BitStream.cpp \
    pdfium/core/fxcodec/jbig2/JBig2_Context.cpp \
    pdfium/core/fxcodec/jbig2/JBig2_GrdProc.cpp \
    pdfium/core/fxcodec/jbig2/JBig2_GrrdProc.cpp \
    pdfium/core/fxcodec/jbig2/JBig2_GsidProc.cpp \
    pdfium/core/fxcodec/jbig2/JBig2_HtrdProc.cpp \
    pdfium/core/fxcodec/jbig2/JBig2_HuffmanDecoder.cpp \
    pdfium/core/fxcodec/jbig2/JBig2_HuffmanTable.cpp \
    pdfium/core/fxcodec/jbig2/JBig2_HuffmanTable_Standard.cpp \
    pdfium/core/fxcodec/jbig2/JBig2_Image.cpp \
    pdfium/core/fxcodec/jbig2/JBig2_PatternDict.cpp \
    pdfium/core/fxcodec/jbig2/JBig2_PddProc.cpp \
    pdfium/core/fxcodec/jbig2/JBig2_SddProc.cpp \
    pdfium/core/fxcodec/jbig2/JBig2_Segment.cpp \
    pdfium/core/fxcodec/jbig2/JBig2_SymbolDict.cpp \
    pdfium/core/fxcodec/jbig2/JBig2_TrdProc.cpp \
    pdfium/third_party/libjpeg/fpdfapi_jcapimin.c \
    pdfium/third_party/libjpeg/fpdfapi_jcapistd.c \
    pdfium/third_party/libjpeg/fpdfapi_jccoefct.c \
    pdfium/third_party/libjpeg/fpdfapi_jccolor.c \
    pdfium/third_party/libjpeg/fpdfapi_jcdctmgr.c \
    pdfium/third_party/libjpeg/fpdfapi_jchuff.c \
    pdfium/third_party/libjpeg/fpdfapi_jcinit.c \
    pdfium/third_party/libjpeg/fpdfapi_jcmainct.c \
    pdfium/third_party/libjpeg/fpdfapi_jcmarker.c \
    pdfium/third_party/libjpeg/fpdfapi_jcmaster.c \
    pdfium/third_party/libjpeg/fpdfapi_jcomapi.c \
    pdfium/third_party/libjpeg/fpdfapi_jcparam.c \
    pdfium/third_party/libjpeg/fpdfapi_jcphuff.c \
    pdfium/third_party/libjpeg/fpdfapi_jcprepct.c \
    pdfium/third_party/libjpeg/fpdfapi_jcsample.c \
    pdfium/third_party/libjpeg/fpdfapi_jctrans.c \
    pdfium/third_party/libjpeg/fpdfapi_jdapimin.c \
    pdfium/third_party/libjpeg/fpdfapi_jdapistd.c \
    pdfium/third_party/libjpeg/fpdfapi_jdcoefct.c \
    pdfium/third_party/libjpeg/fpdfapi_jdcolor.c \
    pdfium/third_party/libjpeg/fpdfapi_jddctmgr.c \
    pdfium/third_party/libjpeg/fpdfapi_jdhuff.c \
    pdfium/third_party/libjpeg/fpdfapi_jdinput.c \
    pdfium/third_party/libjpeg/fpdfapi_jdmainct.c \
    pdfium/third_party/libjpeg/fpdfapi_jdmarker.c \
    pdfium/third_party/libjpeg/fpdfapi_jdmaster.c \
    pdfium/third_party/libjpeg/fpdfapi_jdmerge.c \
    pdfium/third_party/libjpeg/fpdfapi_jdphuff.c \
    pdfium/third_party/libjpeg/fpdfapi_jdpostct.c \
    pdfium/third_party/libjpeg/fpdfapi_jdsample.c \
    pdfium/third_party/libjpeg/fpdfapi_jdtrans.c \
    pdfium/third_party/libjpeg/fpdfapi_jerror.c \
    pdfium/third_party/libjpeg/fpdfapi_jfdctfst.c \
    pdfium/third_party/libjpeg/fpdfapi_jfdctint.c \
    pdfium/third_party/libjpeg/fpdfapi_jidctfst.c \
    pdfium/third_party/libjpeg/fpdfapi_jidctint.c \
    pdfium/third_party/libjpeg/fpdfapi_jidctred.c \
    pdfium/third_party/libjpeg/fpdfapi_jmemmgr.c \
    pdfium/third_party/libjpeg/fpdfapi_jmemnobs.c \
    pdfium/third_party/libjpeg/fpdfapi_jutils.c \
    pdfium/third_party/lcms2-2.6/src/cmscam02.c \
    pdfium/third_party/lcms2-2.6/src/cmscgats.c \
    pdfium/third_party/lcms2-2.6/src/cmscnvrt.c \
    pdfium/third_party/lcms2-2.6/src/cmserr.c \
    pdfium/third_party/lcms2-2.6/src/cmsgamma.c \
    pdfium/third_party/lcms2-2.6/src/cmsgmt.c \
    pdfium/third_party/lcms2-2.6/src/cmshalf.c \
    pdfium/third_party/lcms2-2.6/src/cmsintrp.c \
    pdfium/third_party/lcms2-2.6/src/cmsio0.c \
    pdfium/third_party/lcms2-2.6/src/cmsio1.c \
    pdfium/third_party/lcms2-2.6/src/cmslut.c \
    pdfium/third_party/lcms2-2.6/src/cmsmd5.c \
    pdfium/third_party/lcms2-2.6/src/cmsmtrx.c \
    pdfium/third_party/lcms2-2.6/src/cmsnamed.c \
    pdfium/third_party/lcms2-2.6/src/cmsopt.c \
    pdfium/third_party/lcms2-2.6/src/cmspack.c \
    pdfium/third_party/lcms2-2.6/src/cmspcs.c \
    pdfium/third_party/lcms2-2.6/src/cmsplugin.c \
    pdfium/third_party/lcms2-2.6/src/cmsps2.c \
    pdfium/third_party/lcms2-2.6/src/cmssamp.c \
    pdfium/third_party/lcms2-2.6/src/cmssm.c \
    pdfium/third_party/lcms2-2.6/src/cmstypes.c \
    pdfium/third_party/lcms2-2.6/src/cmsvirt.c \
    pdfium/third_party/lcms2-2.6/src/cmswtpnt.c \
    pdfium/third_party/lcms2-2.6/src/cmsxform.c \
    pdfium/third_party/libopenjpeg20/bio.c \
    pdfium/third_party/libopenjpeg20/cio.c \
    pdfium/third_party/libopenjpeg20/dwt.c \
    pdfium/third_party/libopenjpeg20/event.c \
    pdfium/third_party/libopenjpeg20/function_list.c \
    pdfium/third_party/libopenjpeg20/image.c \
    pdfium/third_party/libopenjpeg20/invert.c \
    pdfium/third_party/libopenjpeg20/j2k.c \
    pdfium/third_party/libopenjpeg20/jp2.c \
    pdfium/third_party/libopenjpeg20/mct.c \
    pdfium/third_party/libopenjpeg20/mqc.c \
    pdfium/third_party/libopenjpeg20/openjpeg.c \
    pdfium/third_party/libopenjpeg20/opj_clock.c \
    pdfium/third_party/libopenjpeg20/pi.c \
    pdfium/third_party/libopenjpeg20/raw.c \
    pdfium/third_party/libopenjpeg20/t1.c \
    pdfium/third_party/libopenjpeg20/t2.c \
    pdfium/third_party/libopenjpeg20/tcd.c \
    pdfium/third_party/libopenjpeg20/tgt.c \
    pdfium/third_party/zlib_v128/adler32.c \
    pdfium/third_party/zlib_v128/compress.c \
    pdfium/third_party/zlib_v128/crc32.c \
    pdfium/third_party/zlib_v128/deflate.c \
    pdfium/third_party/zlib_v128/gzclose.c \
    pdfium/third_party/zlib_v128/gzlib.c \
    pdfium/third_party/zlib_v128/gzread.c \
    pdfium/third_party/zlib_v128/gzwrite.c \
    pdfium/third_party/zlib_v128/infback.c \
    pdfium/third_party/zlib_v128/inffast.c \
    pdfium/third_party/zlib_v128/inflate.c \
    pdfium/third_party/zlib_v128/inftrees.c \
    pdfium/third_party/zlib_v128/trees.c \
    pdfium/third_party/zlib_v128/uncompr.c \
    pdfium/third_party/zlib_v128/zutil.c \
    pdfium/core/fxcrt/fx_basic_array.cpp \
    pdfium/core/fxcrt/fx_basic_bstring.cpp \
    pdfium/core/fxcrt/fx_basic_buffer.cpp \
    pdfium/core/fxcrt/fx_basic_coords.cpp \
    pdfium/core/fxcrt/fx_basic_gcc.cpp \
    pdfium/core/fxcrt/fx_basic_list.cpp \
    pdfium/core/fxcrt/fx_basic_memmgr.cpp \
    pdfium/core/fxcrt/fx_basic_plex.cpp \
    pdfium/core/fxcrt/fx_basic_utf.cpp \
    pdfium/core/fxcrt/fx_basic_util.cpp \
    pdfium/core/fxcrt/fx_basic_wstring.cpp \
    pdfium/core/fxcrt/fx_bidi.cpp \
    pdfium/core/fxcrt/fx_extension.cpp \
    pdfium/core/fxcrt/fx_ucddata.cpp \
    pdfium/core/fxcrt/fx_unicode.cpp \
    pdfium/core/fxcrt/fx_xml_composer.cpp \
    pdfium/core/fxcrt/fx_xml_parser.cpp \
    pdfium/core/fxcrt/fxcrt_posix.cpp \
    pdfium/core/fxcrt/fxcrt_stream.cpp \
    pdfium/core/fxcrt/fxcrt_windows.cpp \
    pdfium/fpdfsdk/fxedit/fxet_ap.cpp \
    pdfium/fpdfsdk/fxedit/fxet_edit.cpp \
    pdfium/fpdfsdk/fxedit/fxet_list.cpp \
    pdfium/core/fxge/agg/fx_agg_driver.cpp \
    pdfium/core/fxge/android/fpf_skiafont.cpp \
    pdfium/core/fxge/android/fpf_skiafontmgr.cpp \
    pdfium/core/fxge/android/fpf_skiamodule.cpp \
    pdfium/core/fxge/android/fx_android_font.cpp \
    pdfium/core/fxge/android/fx_android_imp.cpp \
    pdfium/core/fxge/apple/fx_apple_platform.cpp \
    pdfium/core/fxge/apple/fx_mac_imp.cpp \
    pdfium/core/fxge/apple/fx_quartz_device.cpp \
    pdfium/core/fxge/dib/fx_dib_composite.cpp \
    pdfium/core/fxge/dib/fx_dib_convert.cpp \
    pdfium/core/fxge/dib/fx_dib_engine.cpp \
    pdfium/core/fxge/dib/fx_dib_main.cpp \
    pdfium/core/fxge/dib/fx_dib_transform.cpp \
    pdfium/core/fxge/fontdata/chromefontdata/FoxitDingbats.cpp \
    pdfium/core/fxge/fontdata/chromefontdata/FoxitFixed.cpp \
    pdfium/core/fxge/fontdata/chromefontdata/FoxitFixedBold.cpp \
    pdfium/core/fxge/fontdata/chromefontdata/FoxitFixedBoldItalic.cpp \
    pdfium/core/fxge/fontdata/chromefontdata/FoxitFixedItalic.cpp \
    pdfium/core/fxge/fontdata/chromefontdata/FoxitSans.cpp \
    pdfium/core/fxge/fontdata/chromefontdata/FoxitSansBold.cpp \
    pdfium/core/fxge/fontdata/chromefontdata/FoxitSansBoldItalic.cpp \
    pdfium/core/fxge/fontdata/chromefontdata/FoxitSansItalic.cpp \
    pdfium/core/fxge/fontdata/chromefontdata/FoxitSansMM.cpp \
    pdfium/core/fxge/fontdata/chromefontdata/FoxitSerif.cpp \
    pdfium/core/fxge/fontdata/chromefontdata/FoxitSerifBold.cpp \
    pdfium/core/fxge/fontdata/chromefontdata/FoxitSerifBoldItalic.cpp \
    pdfium/core/fxge/fontdata/chromefontdata/FoxitSerifItalic.cpp \
    pdfium/core/fxge/fontdata/chromefontdata/FoxitSerifMM.cpp \
    pdfium/core/fxge/fontdata/chromefontdata/FoxitSymbol.cpp \
    pdfium/core/fxge/freetype/fx_freetype.cpp \
    pdfium/core/fxge/ge/fx_ge.cpp \
    pdfium/core/fxge/ge/fx_ge_device.cpp \
    pdfium/core/fxge/ge/fx_ge_font.cpp \
    pdfium/core/fxge/ge/fx_ge_fontmap.cpp \
    pdfium/core/fxge/ge/fx_ge_linux.cpp \
    pdfium/core/fxge/ge/fx_ge_path.cpp \
    pdfium/core/fxge/ge/fx_ge_text.cpp \
    pdfium/core/fxge/ifx_renderdevicedriver.cpp \
    pdfium/third_party/agg23/agg_curves.cpp \
    pdfium/third_party/agg23/agg_path_storage.cpp \
    pdfium/third_party/agg23/agg_rasterizer_scanline_aa.cpp \
    pdfium/third_party/agg23/agg_vcgen_dash.cpp \
    pdfium/third_party/agg23/agg_vcgen_stroke.cpp \
    pdfium/fpdfsdk/pdfwindow/PWL_Button.cpp \
    pdfium/fpdfsdk/pdfwindow/PWL_Caret.cpp \
    pdfium/fpdfsdk/pdfwindow/PWL_ComboBox.cpp \
    pdfium/fpdfsdk/pdfwindow/PWL_Edit.cpp \
    pdfium/fpdfsdk/pdfwindow/PWL_EditCtrl.cpp \
    pdfium/fpdfsdk/pdfwindow/PWL_FontMap.cpp \
    pdfium/fpdfsdk/pdfwindow/PWL_Icon.cpp \
    pdfium/fpdfsdk/pdfwindow/PWL_ListBox.cpp \
    pdfium/fpdfsdk/pdfwindow/PWL_ScrollBar.cpp \
    pdfium/fpdfsdk/pdfwindow/PWL_SpecialButton.cpp \
    pdfium/fpdfsdk/pdfwindow/PWL_Utils.cpp \
    pdfium/fpdfsdk/pdfwindow/PWL_Wnd.cpp \
    pdfium/third_party/freetype/src/base/ftbase.c \
    pdfium/third_party/freetype/src/base/ftbitmap.c \
    pdfium/third_party/freetype/src/base/ftglyph.c \
    pdfium/third_party/freetype/src/base/ftinit.c \
    pdfium/third_party/freetype/src/base/ftlcdfil.c \
    pdfium/third_party/freetype/src/base/ftmm.c \
    pdfium/third_party/freetype/src/base/ftsystem.c \
    pdfium/third_party/freetype/src/cff/cff.c \
    pdfium/third_party/freetype/src/cid/type1cid.c \
    pdfium/third_party/freetype/src/psaux/psaux.c \
    pdfium/third_party/freetype/src/pshinter/pshinter.c \
    pdfium/third_party/freetype/src/psnames/psmodule.c \
    pdfium/third_party/freetype/src/raster/raster.c \
    pdfium/third_party/freetype/src/sfnt/sfnt.c \
    pdfium/third_party/freetype/src/smooth/smooth.c \
    pdfium/third_party/freetype/src/truetype/truetype.c \
    pdfium/third_party/freetype/src/type1/type1.c
    
equals(INTERNALBROWSER,yes){
DEFINES += INTERNAL_BROWSER
HEADERS	+= browser.h
SOURCES	+= browser.cpp
}   

RESOURCES += texmaker.qrc
FORMS   += findwidget.ui\
	gotolinewidget.ui \
	replacewidget.ui \
	structdialog.ui \
	filechooser.ui \
	graphicfilechooser.ui \
	tabbingdialog.ui \
	arraydialog.ui \
	tabdialog.ui \
	letterdialog.ui \
	addoptiondialog.ui \
	quickdocumentdialog.ui \
	quickxelatexdialog.ui \
	usermenudialog.ui \
	usertooldialog.ui \
	refdialog.ui \
	aboutdialog.ui \
	configdialog.ui \
	spellerdialog.ui \
	keysequencedialog.ui \
	userquickdialog.ui \
	encodingdialog.ui \
	usercompletiondialog.ui \
	texdocdialog.ui \
	scandialog.ui \
	exportdialog.ui \
	addtagdialog.ui \
	versiondialog.ui \
	unicodedialog.ui \
	quickbeamerdialog.ui 
TRANSLATIONS += trans/texmaker_fr.ts \
	trans/texmaker_de.ts \
	trans/texmaker_es.ts \
	trans/texmaker_gl.ts \
	trans/texmaker_it.ts \
	trans/texmaker_ru.ts \
	trans/texmaker_zh_CN.ts \
	trans/texmaker_zh_TW.ts \
	trans/texmaker_cs.ts \
	trans/texmaker_pt.ts \
	trans/texmaker_pt_BR.ts \
	trans/texmaker_nl.ts  \
	trans/texmaker_hu.ts  \
	trans/texmaker_fa.ts  \
	trans/texmaker_pl.ts  \
	trans/texmaker_vi_VN.ts \
	trans/texmaker_da.ts \
	trans/texmaker_ca.ts \
	trans/texmaker_sr.ts \
	trans/texmaker_el.ts \
	trans/texmaker_se.ts \
	trans/texmaker_ar.ts \
	trans/texmaker_lv.ts \
	trans/texmaker_uk.ts
################################
unix:!macx {
UI_DIR = .ui
MOC_DIR = .moc
OBJECTS_DIR = .obj
isEmpty( PREFIX ) {
    PREFIX=/usr
}
isEmpty( DESKTOPDIR ) {
    DESKTOPDIR=/usr/share/applications
}
isEmpty( ICONDIR ) {
    ICONDIR=/usr/share/pixmaps
}
isEmpty( METAINFODIR ) {
    METAINFODIR=/usr/share/metainfo
}

DEFINES += PREFIX=\\\"$${PREFIX}\\\"

INCLUDEPATH +=$${QTDIR}/include/
 

equals(AUTHORIZELINUXQSTYLES,yes){
DEFINES += AUTHORIZE_LINUX_QSTYLES
}


target.path = $${PREFIX}/bin
utilities.path = $${PREFIX}/share/texmaker
desktop.path = $${DESKTOPDIR}
icon.path = $${ICONDIR}
#not for openSUSE :
metainfo.path = $${METAINFODIR}


INSTALLS = target
HEADERS	+= x11fontdialog.h 
SOURCES	+= x11fontdialog.cpp \
	  singleapp/qtlockedfile_unix.cpp
FORMS += x11fontdialog.ui

utilities.files = doc/doc10.png \
	doc/doc11.png \
	doc/doc12.png \
	doc/doc13.png \
	doc/doc14.png \
	doc/doc15.png \
	doc/doc16.png \
	doc/doc17.png \
	doc/doc1.png \
	doc/doc20.png \
	doc/doc21.png \
	doc/doc22.png \
	doc/doc2.png \
	doc/doc3.png \
	doc/doc4.png \
	doc/doc5.png \
	doc/doc6bis.png \
	doc/doc6.png \
	doc/doc7.png \
	doc/doc8.png \
	doc/doc9.png \
	doc/latexhelp.html \
	doc/usermanual_en.html \
	doc/usermanual_fr.html \
	utilities/AUTHORS \
	utilities/COPYING \
	utilities/CHANGELOG.txt \
	utilities/texmaker16x16.png \
	utilities/texmaker22x22.png \
	utilities/texmaker32x32.png \
	utilities/texmaker48x48.png \
	utilities/texmaker64x64.png \
	utilities/texmaker128x128.png \
	utilities/texmaker.svg \
	scripts/titlecase_selection.tms \
	scripts/hardwordwrap_selection_80col.tms \
	locale/qt_ar.qm \
    locale/qt_ca.qm \
    locale/qt_cs.qm \
    locale/qt_da.qm \
    locale/qt_de.qm \
    locale/qt_en.qm \
    locale/qt_es.qm \
    locale/qt_fa.qm \
    locale/qt_fi.qm \
    locale/qt_fr.qm \
    locale/qt_gl.qm \
    locale/qt_he.qm \
    locale/qt_hu.qm \
    locale/qt_it.qm \
    locale/qt_ja.qm \
    locale/qt_ko.qm \
    locale/qt_lt.qm \
    locale/qt_pl.qm \
    locale/qt_pt.qm \
    locale/qt_ru.qm \
    locale/qt_sk.qm \
    locale/qt_sl.qm \
    locale/qt_sv.qm \
    locale/qt_uk.qm \
    locale/qt_zh_CN.qm \
    locale/qt_zh_TW.qm \
	locale/texmaker_de.qm \
	locale/texmaker_es.qm \
	locale/texmaker_fr.qm \
	locale/texmaker_gl.qm \
	locale/texmaker_it.qm \
	locale/texmaker_pt.qm \
	locale/texmaker_pt_BR.qm \
	locale/texmaker_ru.qm \
	locale/texmaker_zh_CN.qm \
	locale/texmaker_zh_TW.qm \
	locale/texmaker_cs.qm \
	locale/texmaker_nl.qm \
	locale/texmaker_fa.qm \
	locale/texmaker_pl.qm \
	locale/texmaker_hu.qm \
	locale/texmaker_sr.qm \
	locale/texmaker_el.qm \
	locale/texmaker_se.qm \
	locale/texmaker_ar.qm \
	locale/texmaker_lv.qm \
	locale/texmaker_uk.qm \
	locale/texmaker_vi_VN.qm \
	dictionaries/nl_NL.aff \
	dictionaries/nl_NL.dic \
	dictionaries/de_DE.aff \
	dictionaries/de_DE.dic \
	dictionaries/en_GB.aff \
	dictionaries/en_GB.dic \
	dictionaries/en_US.aff \
	dictionaries/en_US.dic \
	dictionaries/es_ES.aff \
	dictionaries/es_ES.dic \
	dictionaries/fr_FR.aff \
	dictionaries/fr_FR.dic \
	dictionaries/it_IT.aff \
	dictionaries/it_IT.dic \
	dictionaries/it_IT_README.txt \
	dictionaries/hu_HU.aff \
	dictionaries/hu_HU.dic \
	dictionaries/README_hu_HU.txt \
	dictionaries/cs_CZ.aff \
	dictionaries/cs_CZ.dic \
	dictionaries/pl_PL.aff \
	dictionaries/pl_PL.dic \
	dictionaries/pt_PT.aff \
	dictionaries/pt_PT.dic \
	dictionaries/ca_CA.aff \
	dictionaries/ca_CA.dic \
	dictionaries/README_pt_PT.txt \
	dictionaries/README_pl.txt \
	dictionaries/README_cs_CZ.txt \
	dictionaries/README_de_DE_frami.txt \
	dictionaries/README_DIC_fr_FR.txt \
	dictionaries/README_es_ES.txt \
	dictionaries/README_en_US.txt
INSTALLS += utilities

desktop.files = utilities/texmaker.desktop
INSTALLS += desktop

icon.files = utilities/texmaker.png
INSTALLS += icon

#not for openSUSE :
metainfo.files = utilities/texmaker.appdata.xml
INSTALLS += metainfo
}
################################
win32 {
UI_DIR = .ui
MOC_DIR = .moc
OBJECTS_DIR = .obj

SOURCES	+= singleapp/qtlockedfile_win.cpp


RC_FILE = win.rc

}
###############################
macx {
UI_DIR = .ui
MOC_DIR = .moc
OBJECTS_DIR = .obj

LIBS_PRIVATE += -framework AppKit -framework CoreFoundation

QMAKE_MAC_SDK=macosx

#QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.8
target.path = Texmaker


INSTALLS = target
SOURCES	+= singleapp/qtlockedfile_unix.cpp
utilities.path = Contents/Resources
utilities.files = utilities/qt_menu.nib \
	utilities/openTerminal.command \
    doc/doc10.png \
	doc/doc11.png \
	doc/doc12.png \
	doc/doc13.png \
	doc/doc14.png \
	doc/doc15.png \
	doc/doc16.png \
	doc/doc17.png \
	doc/doc1.png \
	doc/doc20.png \
	doc/doc21.png \
	doc/doc22.png \
	doc/doc2.png \
	doc/doc3.png \
	doc/doc4.png \
	doc/doc5.png \
	doc/doc6bis.png \
	doc/doc6.png \
	doc/doc7.png \
	doc/doc8.png \
	doc/doc9.png \
	doc/latexhelp.html \
	doc/usermanual_en.html \
	doc/usermanual_fr.html \
	scripts/titlecase_selection.tms \
	scripts/hardwordwrap_selection_80col.tms \
	utilities/AUTHORS \
	utilities/COPYING \
	utilities/CHANGELOG.txt \
	locale/qt_ar.qm \
    locale/qt_ca.qm \
    locale/qt_cs.qm \
    locale/qt_da.qm \
    locale/qt_de.qm \
    locale/qt_en.qm \
    locale/qt_es.qm \
    locale/qt_fa.qm \
    locale/qt_fi.qm \
    locale/qt_fr.qm \
    locale/qt_gl.qm \
    locale/qt_he.qm \
    locale/qt_hu.qm \
    locale/qt_it.qm \
    locale/qt_ja.qm \
    locale/qt_ko.qm \
    locale/qt_lt.qm \
    locale/qt_pl.qm \
    locale/qt_pt.qm \
    locale/qt_ru.qm \
    locale/qt_sk.qm \
    locale/qt_sl.qm \
    locale/qt_sv.qm \
    locale/qt_uk.qm \
    locale/qt_zh_CN.qm \
    locale/qt_zh_TW.qm \
	locale/texmaker_de.qm \
	locale/texmaker_es.qm \
	locale/texmaker_fr.qm \
	locale/texmaker_gl.qm \
	locale/texmaker_it.qm \
	locale/texmaker_pt.qm \
	locale/texmaker_pt_BR.qm \
	locale/texmaker_ru.qm \
	locale/texmaker_zh_CN.qm \
	locale/texmaker_zh_TW.qm \
	locale/texmaker_cs.qm \
	locale/texmaker_nl.qm \
	locale/texmaker_fa.qm \
	locale/texmaker_pl.qm \
	locale/texmaker_hu.qm \
	locale/texmaker_sr.qm \
	locale/texmaker_el.qm \
	locale/texmaker_se.qm \
	locale/texmaker_ar.qm \
	locale/texmaker_lv.qm \
	locale/texmaker_uk.qm \
	locale/texmaker_vi_VN.qm \
	dictionaries/nl_NL.aff \
	dictionaries/nl_NL.dic \
	dictionaries/de_DE.aff \
	dictionaries/de_DE.dic \
	dictionaries/en_GB.aff \
	dictionaries/en_GB.dic \
	dictionaries/en_US.aff \
	dictionaries/en_US.dic \
	dictionaries/es_ES.aff \
	dictionaries/es_ES.dic \
	dictionaries/fr_FR.aff \
	dictionaries/fr_FR.dic \
	dictionaries/it_IT.aff \
	dictionaries/it_IT.dic \
	dictionaries/it_IT_README.txt \
	dictionaries/hu_HU.aff \
	dictionaries/hu_HU.dic \
	dictionaries/README_hu_HU.txt \
	dictionaries/cs_CZ.aff \
	dictionaries/cs_CZ.dic \
	dictionaries/pl_PL.aff \
	dictionaries/pl_PL.dic \
	dictionaries/pt_PT.aff \
	dictionaries/pt_PT.dic \
	dictionaries/ca_CA.aff \
	dictionaries/ca_CA.dic \
	dictionaries/README_pt_PT.txt \
	dictionaries/README_pl.txt \
	dictionaries/README_cs_CZ.txt \
	dictionaries/README_de_DE_frami.txt \
	dictionaries/README_DIC_fr_FR.txt \
	dictionaries/README_es_ES.txt \
	dictionaries/README_en_US.txt
QMAKE_BUNDLE_DATA += utilities
INSTALLS += utilities
ICON = texmaker.icns
QMAKE_INFO_PLIST =Info.plist
}
