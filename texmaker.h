/***************************************************************************
 *   copyright       : (C) 2003-2017 by Pascal Brachet                     *
 *   http://www.xm1math.net/texmaker/                                      *
 *   addons by Luis Silvestre                           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef TEXMAKER_H
#define TEXMAKER_H

#include <QMainWindow>
#include <QDockWidget>
#include <QMenuBar>
#include <QToolBar>
#include <QListWidget>
#include <QStackedWidget>
#include <QTabWidget>
#include <QTreeWidget>
#include <QListWidget>
#include <QLabel>
#include <QAction>
#include <QActionGroup>
#include <QTreeWidgetItem>
#include <QListWidgetItem>
#include <QTableWidgetItem>
#include <QPointer>
#include <QProcess>
#include <QPushButton>
#include <QColor>
#include <QCompleter>
#include <QTextTable>
#include <QVBoxLayout>
#include <QTableWidget>
#include <QTranslator>
#include <QToolButton>
#include <QTimer>
#include <QKeySequence>
#include <QWidgetAction>

#include "latexeditorview.h"
#include "minisplitter.h"
#include "playerbutton.h"
#include "symbollistwidget.h"
#include "usertagslistwidget.h"
#include "xmltagslistwidget.h"
#include "logeditor.h"
#include "hunspell/hunspell.hxx"
#include "pdfviewerwidget.h"
#include "pdfviewer.h"
#include "sourceview.h"
#include "encodingprober/qencodingprober.h"
#include "scandialog.h"
#include "dropshadowlabel.h"

#ifdef INTERNAL_BROWSER
#include "browser.h"
#endif

typedef  QMap<LatexEditorView*, QString> FilesMap;
typedef  QMap<QString,QString> KeysMap;
typedef  QString Userlist[10];
typedef  QString UserCd[5];
typedef int SymbolList[412];
typedef QString foldlist[5];
const foldlist foldClasses=
{
"\\part",
"\\chapter",
"\\section",
"\\subsection",
"\\subsubsection"
};


class Texmaker : public QMainWindow
{
    Q_OBJECT

public:
    Texmaker(QWidget *parent = 0);
    ~Texmaker();
QString getName();
QFont EditorFont;
QByteArray windowstate,splitter1state,splitter2state,splitter3state,fullscreenstate;
void mainWindowActivated();
//virtual QMenu* createPopupMenu();

public slots:
void load( const QString &f );
void setLine( const QString &line );
void ToggleMode();
void insertFromCommandLine(const QString &entity);
void onOtherInstanceMessage(const QString &);  // For messages for the single instance

private:
void setupMenus();
void setupToolBars();
void createStatusBar();
bool FileAlreadyOpen(QString f);
void closeEvent(QCloseEvent *e);
int LevelItem(const QTreeWidgetItem *item);
int LineItem(const QTreeWidgetItem *item);
bool currentfileSaved();
bool isCurrentModifiedOutside();
bool copyFile(QString origin,QString destination);
//void removeDir(QDir thedir);
void createBuildSubdirectory(QString fn);
QString outputName(QString finame,QString extension);
QString outputBaseName(QString finame);
bool gtkSession();
void setMasterDocument(const QString &fn);

int untitled_id;

FilesMap filenames;
KeysMap shortcuts, actionstext;
//gui
QFrame *LeftPanelFrameBis, *Outputframe, *separatorline;

MiniSplitter *splitter1, *splitter2, *splitter3 ;

PlayerButton *toggleStructureButton, *toggleLogButton, *togglePdfButton, *toggleSourceButton; 
bool embedinternalpdf,winmaximized;

QStackedWidget *EditorView;
QStackedWidget *StackedViewers;
LogEditor* OutputTextEdit;

QStackedWidget *LeftPanelStackedWidget;
QListWidget *OpenedFilesListWidget;
UserTagsListWidget *usertagsListWidget;
XmlTagsListWidget *MpListWidget, *PsListWidget, *leftrightWidget, *tikzWidget, *asyWidget;
SymbolListWidget *RelationListWidget, *ArrowListWidget, *MiscellaneousListWidget, *DelimitersListWidget, *GreekListWidget, *MostUsedListWidget, *FavoriteListWidget;
QTreeWidget *StructureTreeWidget;
QVBoxLayout *OutputLayoutV, *CentralLayoutBis,*LeftPanelLayoutBis;
QHBoxLayout *OutputLayoutH, *LeftPanelLayout, *CentralLayout;
QTableWidget *OutputTableWidget;
//menu-toolbar
QMenu *fileMenu, *recentMenu, *editMenu, *toolMenu, *sessionMenu;
QMenu *latex1Menu, *latex11Menu, *latex12Menu, *latex13Menu, *latex14Menu, *latex15Menu, *latex16Menu, *latex17Menu, *latex18Menu ;
QMenu *math1Menu, *math11Menu, *math12Menu, *math13Menu, *math14Menu;

QMenu *foldMenu, *unfoldMenu;

QMenu *wizardMenu;
QMenu *bibMenu, *bibtexMenu, *biblatexMenu;
QMenu *user1Menu, *user11Menu, *user12Menu;
QMenu *viewMenu;
QMenu *optionsMenu, *translationMenu, /**appearanceMenu,*/ *settingsMenu, *scriptMenu;
QMenu *helpMenu;
QMenu *sectionMenu, *sizeMenu, *refMenu;

QToolBar *fileToolBar, *editToolBar, *runToolBar, /* *formatToolBar ,*/ *logToolBar, *LeftPanelToolBar,*LeftPanelToolBarBis, *centralToolBar, *centralToolBarBis;
QAction *recentFileActs[10];
QAction *ToggleAct, *StopAct, *UndoAct, *RedoAct, *SaveAct, *CutAct, *CopyAct,*PasteAct, *ToggleDocAct, *ViewStructurePanelAct, *ViewLogPanelAct, *ViewPdfPanelAct, *ViewSourcePanelAct, *FullScreenAct, *NextDocAct, *PrevDocAct, *ViewOpenedFilesPanelAct ;

QAction *relationAct, *arrowAct, *miscAct, *delimAct, *greekAct, *usedAct, *favAct, *leftrightAct, *pstricksAct, *mpAct, *tikzAct, *asyAct, *userpanelAct;
bool showPstricks, showMp, showTikz, showAsy;
QAction *viewPstricksAct, *viewMpAct, *viewTikzAct, *viewAsyAct;

QAction *emphasisAct, *newlineAct, *mathmodeAct, *indiceAct, *puissanceAct, *smallfracAct, *dfracAct, *racineAct;
QAction *showemphasisAct, *shownewlineAct, *showmathmodeAct, *showindiceAct, *showpuissanceAct, *showsmallfracAct, *showdfracAct, *showracineAct;
bool showEmphasis, showNewline, showMathmode, showIndice, showPuissance, showSmallfrac, showDfrac, showRacine;


QComboBox *comboCompil, *comboView, *comboFiles;
DropShadowLabel *titleLeftPanel, *posLabel, *stat1, *stat2, *stat3;

QPushButton *pb1, *pb2, *pb3;
QString MasterName;
bool logpresent;
QStringList recentFilesList;
QString sessionTempFile;

//settings
bool eraseSettings, replaceSettings;
QString settingsFileName;
int split1_right, split1_left, split2_top, split2_bottom, quickmode, tabwidth;
bool singlemode, wordwrap, parenmatch, showline, showoutputview, showstructview, showpdfview, showsourceview, showfilesview, ams_packages, makeidx_package, completion, inlinespellcheck, modern_style, new_gui, builtinpdfview, singleviewerinstance, babel_package, geometry_package, graphicx_package, watchfiles, autosave, tabspaces, useoutputdir ;
bool lmodern_package, kpfonts_package, fourier_package;
QString document_class, typeface_size, paper_size, document_encoding, author, geometry_options, babel_default;
QString latex_command, viewdvi_command, dvips_command, dvipdf_command, metapost_command, psize, xelatex_command, lualatex_command, htlatex_command, htlatex_options;
QString viewps_command, ps2pdf_command, makeindex_command, bibtex_command, pdflatex_command, viewpdf_command, userquick_command, ghostscript_command, asymptote_command, latexmk_command, sweave_command, texdoc_command, quick_asy_command, lp_options;
QString spell_dic, spell_ignored_words;
QString lastDocument, input_encoding, lastChild, lastTemplate, lastScript;
QString struct_level1, struct_level2, struct_level3, struct_level4, struct_level5;
QStringList userClassList, userPaperList, userEncodingList, userOptionsList, userCompletionList, userBabelList;
QStringList labelitem, bibitem, listbibfiles, listchildfiles;
Userlist UserMenuName, UserMenuTag;
UserCd UserToolName, UserToolCommand;
QString keyToggleFocus;
QString extra_path;
QString beamer_theme, beamer_size, beamer_encoding, beamer_author, beamer_babel;
qreal lastScale;
bool clean_exit;

bool svnEnable;
QString svnPath;
//dialogs
QPointer<ScanDialog> scanDialog;
#ifdef INTERNAL_BROWSER
QPointer<Browser> browserWindow, diffWindow;
#endif
QPointer<PdfViewerWidget> pdfviewerWidget;
QPointer<PdfViewer> pdfviewerWindow;
SourceView* sourceviewerWidget;
//tools
QProcess *proc;
bool FINPROCESS, ERRPROCESS, STOPPROCESS;
QStringList listViewerCommands;
bool checkViewerInstance;
//latex errors
QStringList errorFileList, errorTypeList, errorLineList, errorMessageList, errorLogList;
QList<int> onlyErrorList;
int errorIndex, runIndex, viewIndex;
QString pdfCheckerLang;
//X11
#if defined(Q_OS_UNIX) && !defined(Q_OS_MAC)
QString x11style;
QString x11fontfamily;
int x11fontsize;
#endif
SymbolList symbolScore;
usercodelist symbolMostused;
QList<int> favoriteSymbolList;
QStringList userTagsList;

QColor colorBackground, colorLine, colorHighlight, colorStandard, colorComment, colorMath, colorCommand, colorKeyword, colorVerbatim, colorTodo, colorKeywordGraphic, colorNumberGraphic;

QList<QColor> edcolors() {QList<QColor> e; e << colorBackground << colorLine << colorHighlight << colorStandard; return e;}
QList<QColor> hicolors() {QList<QColor> h; h << colorStandard << colorComment << colorMath << colorCommand << colorKeyword << colorVerbatim << colorTodo << colorKeywordGraphic << colorNumberGraphic; return h;}

QCompleter *completer;
Hunspell * spellChecker;
bool spelldicExist();
QStringList translationList, scriptList;
QActionGroup *translationGroup/*, *appearanceGroup*/;
QTimer *autosaveTimer;

private slots:
LatexEditorView *currentEditorView() const;
void fileNew();
void fileNewFromFile();
void fileOpen();
void checkModifiedOutsideAll();
void fileSave();
void fileSaveAll();
void fileBackupAll();
void fileSaveAs();
void fileSaveACopy();
void fileClose();
void fileCloseRequested( int index);
void fileCloseAll();
void fileExit();
void fileOpenRecent();
void AddRecentFile(const QString &f);
void UpdateRecentFile();
void CleanRecent();
void filePrint();
void fileOpenAndGoto(const QString &f, int line, bool focus);
void getFocusToEditor();
void fileReload();
void allReload();
void listSelectionActivated(int index);
void ComboFilesInsert(const QString & file);
void ToggleMasterCurrent();

void editUndo();
void editRedo();
void editCut();
void editCopy();
void editPaste();
void editSelectAll();
void editFind();
void editFindNext();
void editFindInDirectory();
void editReplace();
void editGotoLine();
void editComment();
void editUncomment();
void editIndent();
void editUnindent();
void editSpell();
void editTipTab();
void editRunScript();
void editRunFurnishedScript();


void doFoldAll(int i);
void doUnfoldAll(int i);
void foldAll();
void unfoldAll();
void foldAllUnder();
void unfoldAllUnder();
void foldEnclosing();
void unfoldEnclosing();

void ReadSettings();
void SaveSettings();
void DeleteSettings();
void CopySettings();
void ReplaceSettings();
void setPrintPaperSize(const QString &p);

void NewDocumentStatus(bool m);
void UpdateCaption();

void UpdateStructure();
void UpdateChildsLabels(QStringList listfiles);
void UpdateBibliography();
void ParseTree(QTreeWidgetItem *item);
void ItemToRange(QTreeWidgetItem *item);

void ShowOpenedFiles();
void OpenedFileActivated(QListWidgetItem *item);
void ShowStructure();
void ShowRelation(); //RelationListWidget
void ShowArrow(); //ArrowListWidget
void ShowMisc(); //MiscellaneousListWidget
void ShowDelim(); //DelimitersListWidget
void ShowGreek(); //GreekListWidget
void ShowMostUsed(); //MostUsedListWidget
void ShowFavorite(); //FavoriteListWidget
void ShowPstricks(); //PsListWidget
void ShowLeftRight(); //leftrightWidget
void ShowMplist(); //MpListWidget
void ShowTikz(); //tikzWidget
void ShowAsy(); //asyWidget
void ShowUserPanel(); //userPanel
void ClickedOnStructure(QTreeWidgetItem *item,int);

void InsertTag(QString Entity, int dx, int dy);
void InsertSymbol(QTableWidgetItem *item);
void InsertXmlTag(QListWidgetItem *item);
void InsertUserElement(QListWidgetItem *item);
void InsertFromAction();
void InsertWithSelectionFromAction();
void InsertWithSelectionFromString(const QString& text);
void InsertFromString(const QString& text);
void InsertBib();
void InsertBibLatex();
void InsertStruct();
void InsertStructFromString(const QString& text);
void InsertImage();
void InsertInclude();
void InsertInput();

void InsertBib1();
void InsertBib2();
void InsertBib3();
void InsertBib4();
void InsertBib5();
void InsertBib6();
void InsertBib7();
void InsertBib8();
void InsertBib9();
void InsertBib10();
void InsertBib11();
void InsertBib12();
void InsertBib13();

void InsertBibLatex1(); //Article in Journal
void InsertBibLatex2(); //Single-volume book
void InsertBibLatex3(); //Multi-volume book
void InsertBibLatex4(); //Part of a book
void InsertBibLatex5(); //Booklet
void InsertBibLatex6(); //Single-volume collection
void InsertBibLatex7(); //Multi-volume collection
void InsertBibLatex8(); //Part of a collection
void InsertBibLatex9(); //Technical documentation
void InsertBibLatex10(); //Miscellaneous
void InsertBibLatex11(); //Online resource
void InsertBibLatex12(); //Issue of a periodical
void InsertBibLatex13(); //Single-volume conference proceedings
void InsertBibLatex14(); //Multi-volume conference proceedings
void InsertBibLatex15(); //Article in conference proceedings
void InsertBibLatex16(); //Technical report
void InsertBibLatex17(); //Thesis
void InsertBibLatex18(); //Patent

void CleanBib();

void InsertUserTag(QString Entity);
void InsertUserTag1();
void InsertUserTag2();
void InsertUserTag3();
void InsertUserTag4();
void InsertUserTag5();
void InsertUserTag6();
void InsertUserTag7();
void InsertUserTag8();
void InsertUserTag9();
void InsertUserTag10();
void EditUserMenu();

void SectionCommand();
void OtherCommand();
void InsertCite();
void InsertRef();
void InsertPageRef();
void SizeCommand();

void ShowSectionMenu();
void ShowRefMenu();
void ShowSizeMenu();

void QuickTabular();
void QuickArray();
void QuickTabbing();
void QuickLetter();
void QuickDocument();
void QuickXelatex();
void QuickBeamer();

void RunCommand(QString comd,bool waitendprocess);
void readFromStderr();
void stopProcess();
void readFromStdoutput();
void SlotEndProcess(int err);
void SlotEndViewerProcess(int err);
void QuickBuild();
void Latex();
void ViewDvi();
void DviToPS();
void ViewPS();
void PDFLatex();
void ViewPDF();
void CleanAll();
void AutoCleanAll();
void MakeBib();
void MakeIndex();
void PStoPDF();
void DVItoPDF();
void MetaPost();
void Asymptote();
void LatexMk();
void Sweave();
void Xelatex();
void Lualatex();
void AsyFile(QString asyfile);
void UserTool1();
void UserTool2();
void UserTool3();
void UserTool4();
void UserTool5();
void EditUserTool();
void doCompile();
void doView();
void jumpToPdfline(int line);
void OpenTerminal();
void Export();
void ConvertToUnicode();

void LoadLog();
void ViewLog();
void ClickedOnOutput(int l);
void ClickedOnLogLine(QTableWidgetItem *item);
void LatexError();
void DisplayLatexError();
void NextError();
void PreviousError();
bool NoLatexErrors();
bool LogExists();

/////
void LatexHelp();
void UserManualHelp();
void TexDocHelp();
void HelpAbout();
void CheckVersion();
void Doculatex();
//void Docufrlatex();

void GeneralOptions();

void gotoNextDocument();
void gotoPrevDocument();

void SetInterfaceFont();

void gotoBookmark1();
void gotoBookmark2();
void gotoBookmark3();

void SetMostUsedSymbols();
void InsertFavoriteSymbols();
void RemoveFavoriteSymbols();

void RemoveUserTag();
void AddUserTag();
void ChangeUserTag();
void UpdateUserTag();

void ModifyShortcuts();

void initCompleter();
void updateCompleter();
void updateTranslation();
// void updateAppearance();

void disableToolsActions();
void enableToolsActions();

void clipboardDataChanged();
void refreshAll();
void refreshAllFromCursor(int newnumlines);
void refreshRange();
void jumpToStructure(int line);


void ToggleStructurePanel();
void ToggleLogPanel();
void TogglePdfPanel();
void ToggleSourcePanel();
void ToggleFilesPanel();
void ShowStructView(bool change);
void ShowOutputView(bool change);
void ShowPdfView(bool change);
void ShowSourceView(bool change);
void ShowFilesView(bool change); 
void ToggleFullScreen(); 
void EditUserCompletion();
void addBibFiles(QString param);
void addIncludeFiles(QString param);
void loadIncludeFiles(QString param, QString extension);
void showCursorPos(int li, int col);
void customContentsMenuStructure( const QPoint &pos );
void customContentsMenuMain( const QPoint &pos );
void TogglePstricks();
void ToggleMetapost();
void ToggleTikz();
void ToggleAsymptote();
void ToggleEmphasis();
void ToggleNewline();
void ToggleMathmode();
void ToggleIndice();
void TogglePuissance();
void ToggleSmallfrac();
void ToggleDfrac();
void ToggleRacine();
void splitter2Changed();

void SaveSession();
void LoadSessionFile(const QString &fn);
void LoadSession();
void SaveLastSession();
void LoadLastSession();

void compareDocuments();

protected:
void dragEnterEvent(QDragEnterEvent *event);
void dropEvent(QDropEvent *event);
void keyPressEvent(QKeyEvent * event);
   
};

#endif
