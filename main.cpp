/***************************************************************************
 *   copyright       : (C) 2003-2017 by Pascal Brachet                     *
 *   addons by Frederic Devernay <frederic.devernay@m4x.org>               *
 *   addons by Luis Silvestre                                              *
 *   http://www.xm1math.net/texmaker/                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include "texmakerapp.h"
#include <QStringList>
#include <QDebug>
#define STRINGIFY_INTERNAL(x) #x
#define STRINGIFY(x) STRINGIFY_INTERNAL(x)

#define VERSION_STR STRINGIFY(TEXMAKERVERSION)

char** appendCommandLineArguments(int argc, char **argv, const QStringList& args)
{
  size_t newSize = (argc + args.length() + 1) * sizeof(char*);
  char** newArgv = (char**)calloc(1, newSize);
  memcpy(newArgv, argv, (size_t)(argc * sizeof(char*)));

  int pos = argc;
  for(const QString& str : args)
    newArgv[pos++] = qstrdup(str.toUtf8().data());

  return newArgv;
}

int main( int argc, char ** argv )
{
QStringList rawargs;
for ( int i = 0; i < argc; ++i )
	{
	rawargs.append( argv[ i ]);
	}
for (QStringList::Iterator it = ++(rawargs.begin()); it != rawargs.end(); it++)
    {
    if ( ( *it == "-dpiscale") && (++it != rawargs.end())) {qputenv("QT_SCALE_FACTOR", (*it).toUtf8());}
    }

#if !defined(Q_OS_MAC)
QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
QStringList g_qtFlags = {"--disable-gpu"}; 
char **newArgv = appendCommandLineArguments(argc, argv, g_qtFlags);
int newArgc = argc + g_qtFlags.size();

TexmakerApp app("TexMaker", newArgc, newArgv );

QStringList args = QCoreApplication::arguments();
app.setApplicationName("TexMaker");
app.setApplicationVersion(VERSION_STR);
app.setOrganizationName("xm1");
app.setAttribute(Qt::AA_DontShowIconsInMenus, true);
app.setAttribute(Qt::AA_UseHighDpiPixmaps);

#if defined(Q_OS_UNIX) && !defined(Q_OS_MAC)
QApplication::setAttribute(Qt::AA_DontUseNativeMenuBar);
#endif



if ( app.isRunning() && !args.contains("-n")) 
    {
    QString msg;
    msg = args.join("#!#");
    msg += "#!#";
    app.sendMessage( msg );
    return 0;
    }

app.init(args); // Initialization takes place only if there is no other instance running.

QObject::connect( &app, SIGNAL( messageReceived(const QString &) ), 
                  app.mw,   SLOT( onOtherInstanceMessage(const QString &) ) );

return app.exec();
}
