COMPILEUSB=no
INTERNALBROWSER=no
AUTHORIZELINUXQSTYLES=yes


TEMPLATE	= app
LANGUAGE	= C++
TARGET	 = texmaker


TEXMAKERVERSION=5.0.3
DEFINES += TEXMAKERVERSION=$${TEXMAKERVERSION}


equals(QT_MAJOR_VERSION, 5):greaterThan(QT_MINOR_VERSION, 6) {
QT += core widgets xml 
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
    USB_VERSION \
    FT2_BUILD_LIBRARY

# QMAKE_CXXFLAGS += -g

###############################

HEADERS	+= texmaker.h \
    theme.h \
    dropshadowlabel.h \
	geticon.h \
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
	refdialog.h \
	configdialog.h \
	aboutdialog.h \
	spellerdialog.h \
	xmltagslistwidget.h \
	blockdata.h \
	keysequencedialog.h \
	usercompletiondialog.h \
	textblockselection.h \
	usertagslistwidget.h \
	addtagdialog.h \
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
	
	
SOURCES	+= main.cpp \
    dropshadowlabel.cpp \
	geticon.cpp \
	texmakerapp.cpp \
	texmaker.cpp \
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
	refdialog.cpp \
	configdialog.cpp \
	aboutdialog.cpp \
	spellerdialog.cpp \
	xmltagslistwidget.cpp \
	blockdata.cpp \
	keysequencedialog.cpp \
	usercompletiondialog.cpp \
	usertagslistwidget.cpp \
	addtagdialog.cpp \
	quickbeamerdialog.cpp \
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
	refdialog.ui \
	aboutdialog.ui \
	configdialog.ui \
	spellerdialog.ui \
	keysequencedialog.ui \
	usercompletiondialog.ui \
	addtagdialog.ui \
	quickbeamerdialog.ui 
TRANSLATIONS += trans/texmaker_fr.ts \
	trans/texmaker_de.ts \
	trans/texmaker_es.ts \
	trans/texmaker_zh_CN.ts \
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
SOURCES	+= x11fontdialog.cpp 
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
