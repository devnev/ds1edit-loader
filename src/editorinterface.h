/***************************************************************************
 *   Copyright (C) 2007 by Mark Nevill                                     *
 *   mark.nevill@gmail.com                                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef EDITORINTEFACE_H_INCLUDED
#define EDITORINTEFACE_H_INCLUDED

#include <QObject>
#include <QProcess>
#include <QList>
#include <QString>
#include <QDir>
#include "ds1info.h"

class LoaderDialog;
class QTextEdit;

class EditorInterface : public QObject
{
	Q_OBJECT

public:
	EditorInterface(LoaderDialog* dialog, const QString& path, const QString& name);

	void run(); ///< Run editor directly
	void makeBatch(QString& batchFileName); ///< Make batch files

signals:
	void finished();

private slots:
	void terminated(int exitCode, QProcess::ExitStatus exitStatus);
	void editorOutputReady();

private:
	typedef QList<Ds1Info> MapList;

	void makeIni();
	QStringList makeArgs();
	void makeRelativePaths();

	bool resize, forceDt1, forcePalette, noCheckAct, autoPrestDef;
	int act, x, y;
	QStringList dt1List;
	QDir editorDir;
	QString editorName, fileName;
	MapList mapList;
	QTextEdit* output;
	QProcess* editorProcess;
	bool consoleAllocated;
};

#endif //EDITORINTEFACE_H_INCLUDED
