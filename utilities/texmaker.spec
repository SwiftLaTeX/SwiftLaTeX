Name: texmaker
Summary: LaTeX editor
Version: 5.0.3
Release: xm1
License: GPLv2+
Group: Applications/Publishing
Source: texmaker-%{version}.tar.bz2
Packager: Pascal Brachet
Url: http://www.xm1math.net/texmaker/


%if 0%{?fedora}
%define debug_package %{nil}
BuildRequires: gcc-c++
BuildRequires: qt5-devel  
BuildRequires: qt5-qtbase-devel 
BuildRequires: qt5-qtscript-devel 
BuildRequires: qt5-qtbase-private-devel 
BuildRequires: qt5-qttools-devel
%endif

%if 0%{?suse_version}
BuildRequires: libQt5Core-devel
BuildRequires: libQt5Core-private-headers-devel
BuildRequires: libQt5Gui-devel
BuildRequires: libQt5Concurrent-devel
BuildRequires: libQt5Core-devel
BuildRequires: libQt5Network-devel
BuildRequires: libQt5PrintSupport-devel
BuildRequires: libQt5Widgets-devel
BuildRequires: libqt5-qtbase-devel 
BuildRequires: libqt5-qtbase-common-devel
BuildRequires: libqt5-qtscript-devel
BuildRequires: libqt5-qttools-devel
%endif

%description
Texmaker is a clean, highly configurable LaTeX editor with good hot key 
 support and extensive LaTeX documentation. Texmaker integrates many tools 
 needed to develop documents with LaTeX, in just one application.

%prep
%setup 

%build
PREFIX=%{buildroot}%{_prefix}
%if 0%{?suse_version}
qmake-qt5 texmaker.pro
%endif
%if 0%{?fedora}
qmake-qt5 CONFIG-=debug texmaker.pro
%endif
make INSTALL_ROOT=%{buildroot}

%install
make INSTALL_ROOT=%{buildroot} install

%files
%defattr(-,root,root,-)
%{_bindir}/texmaker
%{_datadir}/applications/texmaker.desktop
%{_datadir}/pixmaps/texmaker.png
%{_datadir}/texmaker/titlecase_selection.tms
%{_datadir}/texmaker/hardwordwrap_selection_80col.tms
%{_datadir}/texmaker/doc10.png
%{_datadir}/texmaker/doc11.png
%{_datadir}/texmaker/doc12.png
%{_datadir}/texmaker/doc13.png
%{_datadir}/texmaker/doc14.png
%{_datadir}/texmaker/doc15.png
%{_datadir}/texmaker/doc16.png
%{_datadir}/texmaker/doc17.png
%{_datadir}/texmaker/doc1.png
%{_datadir}/texmaker/doc20.png
%{_datadir}/texmaker/doc21.png
%{_datadir}/texmaker/doc22.png
%{_datadir}/texmaker/doc2.png
%{_datadir}/texmaker/doc3.png
%{_datadir}/texmaker/doc4.png
%{_datadir}/texmaker/doc5.png
%{_datadir}/texmaker/doc6bis.png
%{_datadir}/texmaker/doc6.png
%{_datadir}/texmaker/doc7.png
%{_datadir}/texmaker/doc8.png
%{_datadir}/texmaker/doc9.png
%{_datadir}/texmaker/latexhelp.html
%{_datadir}/texmaker/usermanual_en.html
%{_datadir}/texmaker/usermanual_fr.html
%{_datadir}/texmaker/AUTHORS
%{_datadir}/texmaker/COPYING
%{_datadir}/texmaker/CHANGELOG.txt
%{_datadir}/texmaker/texmaker16x16.png
%{_datadir}/texmaker/texmaker22x22.png
%{_datadir}/texmaker/texmaker32x32.png
%{_datadir}/texmaker/texmaker48x48.png
%{_datadir}/texmaker/texmaker64x64.png
%{_datadir}/texmaker/texmaker128x128.png
%{_datadir}/texmaker/texmaker.svg
%{_datadir}/texmaker/qt_ar.qm
%{_datadir}/texmaker/qt_ca.qm
%{_datadir}/texmaker/qt_cs.qm
%{_datadir}/texmaker/qt_da.qm
%{_datadir}/texmaker/qt_de.qm
%{_datadir}/texmaker/qt_en.qm
%{_datadir}/texmaker/qt_es.qm
%{_datadir}/texmaker/qt_fa.qm
%{_datadir}/texmaker/qt_fi.qm
%{_datadir}/texmaker/qt_fr.qm
%{_datadir}/texmaker/qt_gl.qm
%{_datadir}/texmaker/qt_he.qm
%{_datadir}/texmaker/qt_hu.qm
%{_datadir}/texmaker/qt_it.qm
%{_datadir}/texmaker/qt_ja.qm
%{_datadir}/texmaker/qt_ko.qm
%{_datadir}/texmaker/qt_lt.qm
%{_datadir}/texmaker/qt_pl.qm
%{_datadir}/texmaker/qt_pt.qm
%{_datadir}/texmaker/qt_ru.qm
%{_datadir}/texmaker/qt_sk.qm
%{_datadir}/texmaker/qt_sl.qm
%{_datadir}/texmaker/qt_sv.qm
%{_datadir}/texmaker/qt_uk.qm
%{_datadir}/texmaker/qt_zh_CN.qm
%{_datadir}/texmaker/qt_zh_TW.qm
%{_datadir}/texmaker/texmaker_pl.qm
%{_datadir}/texmaker/texmaker_hu.qm
%{_datadir}/texmaker/texmaker_de.qm
%{_datadir}/texmaker/texmaker_es.qm
%{_datadir}/texmaker/texmaker_fa.qm
%{_datadir}/texmaker/texmaker_fr.qm
%{_datadir}/texmaker/texmaker_gl.qm
%{_datadir}/texmaker/texmaker_it.qm
%{_datadir}/texmaker/texmaker_pt.qm
%{_datadir}/texmaker/texmaker_pt_BR.qm
%{_datadir}/texmaker/texmaker_ru.qm
%{_datadir}/texmaker/texmaker_zh_CN.qm
%{_datadir}/texmaker/texmaker_zh_TW.qm
%{_datadir}/texmaker/texmaker_cs.qm
%{_datadir}/texmaker/texmaker_nl.qm
%{_datadir}/texmaker/texmaker_sr.qm
%{_datadir}/texmaker/texmaker_el.qm
%{_datadir}/texmaker/texmaker_se.qm
%{_datadir}/texmaker/texmaker_ar.qm
%{_datadir}/texmaker/texmaker_lv.qm
%{_datadir}/texmaker/texmaker_uk.qm
%{_datadir}/texmaker/texmaker_vi_VN.qm
%{_datadir}/texmaker/nl_NL.aff
%{_datadir}/texmaker/nl_NL.dic
%{_datadir}/texmaker/de_DE.aff
%{_datadir}/texmaker/de_DE.dic
%{_datadir}/texmaker/en_GB.aff
%{_datadir}/texmaker/en_GB.dic
%{_datadir}/texmaker/en_US.aff
%{_datadir}/texmaker/en_US.dic
%{_datadir}/texmaker/es_ES.aff
%{_datadir}/texmaker/es_ES.dic
%{_datadir}/texmaker/fr_FR.aff
%{_datadir}/texmaker/fr_FR.dic
%{_datadir}/texmaker/it_IT.aff
%{_datadir}/texmaker/it_IT.dic
%{_datadir}/texmaker/it_IT_README.txt
%{_datadir}/texmaker/hu_HU.aff
%{_datadir}/texmaker/hu_HU.dic
%{_datadir}/texmaker/README_hu_HU.txt
%{_datadir}/texmaker/cs_CZ.aff
%{_datadir}/texmaker/cs_CZ.dic
%{_datadir}/texmaker/pl_PL.aff
%{_datadir}/texmaker/pl_PL.dic
%{_datadir}/texmaker/README_pl.txt
%{_datadir}/texmaker/README_cs_CZ.txt
%{_datadir}/texmaker/README_de_DE_frami.txt
%{_datadir}/texmaker/README_DIC_fr_FR.txt
%{_datadir}/texmaker/README_es_ES.txt
%{_datadir}/texmaker/README_en_US.txt
%{_datadir}/texmaker/pt_PT.aff
%{_datadir}/texmaker/pt_PT.dic
%{_datadir}/texmaker/ca_CA.aff
%{_datadir}/texmaker/ca_CA.dic
%{_datadir}/texmaker/README_pt_PT.txt
