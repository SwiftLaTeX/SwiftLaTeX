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

#ifndef SVNHELPER_H
#define SVNHELPER_H

#include <QProcess>
class SvnHelper : public QObject
{
    Q_OBJECT
public:
    SvnHelper(QString filename, QString svnPath);
    ~SvnHelper();


signals:
    void uncommittedLines(QList<int>);

private slots:
    void parseUnified(int exitCode, QProcess::ExitStatus exitStatus);
private:
    QProcess _process;
    QList<int> _uncommitLines;
};

#endif // SVNHELPER_H
