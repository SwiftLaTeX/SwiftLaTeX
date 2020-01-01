#!/bin/sh
LD_LIBRARY_PATH=/home/xm1/compil/qt-everywhere-opensource-src-5.9.1/qtbase/lib${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}
export LD_LIBRARY_PATH
QT_PLUGIN_PATH=/home/xm1/compil/qt-everywhere-opensource-src-5.9.1/qtbase/plugins${QT_PLUGIN_PATH:+:$QT_PLUGIN_PATH}
export QT_PLUGIN_PATH
exec /home/xm1/compil/qt-everywhere-opensource-src-5.9.1/qtbase/bin/uic "$@"
