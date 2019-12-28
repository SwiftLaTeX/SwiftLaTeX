/***************************************************************************
 *   copyright       : (C) 2003-2017 by Pascal Brachet                     *
 *   http://www.xm1math.net/texmaker/                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "svnhelper.h"
#include <QProcess>
#include <QDebug>


SvnHelper::SvnHelper(QString filename,QString svnPath)
{
    QStringList args;
    args << "diff";
    args << "-x" << "--ignore-eol-style";
    args << filename;
    args << "-r" << "BASE";
    args <<"--non-interactive";

    //add svn to path
    QString extraPath = svnPath;
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
#ifdef OS_WINDOWS
    if (!extraPath.isEmpty())
    {
        env.insert("PATH", env.value("PATH") + ";"+extraPath);
        _process.setProcessEnvironment(env);
    }
#endif
#ifdef OS_LINUX
    if (!extraPath.isEmpty())
    {
        env.insert("PATH", env.value("PATH") + ":"+extraPath);
        _process.setProcessEnvironment(env);
    }
#endif
    //Very Important, everything is in there because process->setEnvironement is for child process
    qputenv("PATH", env.value("PATH").toLatin1());



    connect(&_process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(parseUnified(int,QProcess::ExitStatus)));
#ifdef OS_WINDOWS
    _process.start("svn.exe", args);
#else
    _process.start("svn", args);
#endif

}

SvnHelper::~SvnHelper()
{

}

void SvnHelper::parseUnified(int exitCode, QProcess::ExitStatus exitStatus)
{
    if(exitCode == QProcess::CrashExit)
    {
        //qDebug()<<"svn error: "<<_process.errorString();
        this->deleteLater();
        return;
    }
    _process.readLine();// "Index: ...
    _process.readLine();// "====== ...
    _process.readLine();// "--- ...
    _process.readLine();// "+++ ...


    while(!_process.atEnd())
    {
        QString c = _process.read(1);
        if(c == "\\")
        {
            break;
        }
        QString position = _process.readLine();
        QRegExp positionRegex("^\\@ \\-(\\d+),(\\d+) \\+(\\d+),(\\d+) \\@\\@");
        if(!position.contains(positionRegex))
        {
            //qDebug()<<position;
            qWarning("Wrong diff output. Expecting unified format");
            return;
        }
        int line1 = positionRegex.cap(1).toInt();
        int length1 = positionRegex.cap(2).toInt();
        int line2 = positionRegex.cap(3).toInt();
        int length2 = positionRegex.cap(4).toInt();
        while(length2 > 0 || length1 > 0)
        {
            c = _process.read(1);
            _process.readLine();
            if(c == " ")
            {
                length1--;
                line1++;
                length2--;
                line2++;
                continue;
            }
            if(c == "+")
            {
                this->_uncommitLines << line2;
                length2--;
                line2++;
                continue;
            }
            if(c == "-")
            {
                length1--;
                line1++;
                continue;
            }

        }
    }
    emit uncommittedLines(_uncommitLines);
    this->deleteLater();
}

