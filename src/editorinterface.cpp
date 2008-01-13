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

#include "editorinterface.h"
#include "iniparser.h"
#include "loaderdialog.h"
#include "parseerror.h"
#include <QTextEdit>
#include <QTreeWidget>
#include <QCheckBox>
#include <QApplication>
#include <QLineEdit>
#include <QTextStream>
#include <QMessageBox>
#include <cassert>
#ifdef Q_WS_WIN
#include <windows.h>
#endif

EditorInterface::EditorInterface(LoaderDialog* dialog, const QString& path, const QString& name)
: editorDir(path), editorName(name), editorProcess(0)
{
	FNTRACE("", "EditorInterface", "EditorInterface", "LoaderDialog* dialog");
	assert(dialog->mapList->topLevelItemCount() > 0);

#define SYNCCHECKED(name) name = dialog-> name ->isChecked()
	SYNCCHECKED(resize);
	SYNCCHECKED(forceDt1);
	SYNCCHECKED(forcePalette);
	SYNCCHECKED(noCheckAct);
	SYNCCHECKED(autoPrestDef);
#undef SYNCCHECKED

#define GETNUMVAL(name) dialog-> name ->text().toInt()
	act = GETNUMVAL(forcePalAct);
	x = GETNUMVAL(resizeX);
	y = GETNUMVAL(resizeY);
#undef GETNUMVAL

	dt1List = dialog->dt1List;
	Ds1Info map;
	QTreeWidgetItem* item;
	for (int i = 0, itemCount = dialog->mapList->topLevelItemCount(); i < itemCount; ++i)
	{
		map.clear();
		item = dialog->mapList->topLevelItem(i);
		map.file = item->text(0);
		map.name = item->text(1);
		map.path = item->text(2);
		map.typeId = item->text(3).toInt();
		if (autoPrestDef)
			map.prestDef = -1;
		else
			map.prestDef = item->text(4).toInt();
		map.comment = item->text(5).toInt();
		mapList.append(map);
	}
	output = dialog->editorOutput;
}

void EditorInterface::makeRelativePaths()
{
	FNTRACE("", "EditorInterface", "makeRelativePaths", "");

	QString editorPath = editorDir.absolutePath();
	for (MapList::iterator mapIter = mapList.begin();
		mapIter != mapList.end(); ++mapIter)
	{
		if (mapIter->file.startsWith(editorPath))
			mapIter->file = mapIter->file.mid(editorPath.length()+1); // +1 for /
	}
	for (QStringList::iterator dt1Iter = dt1List.begin();
		dt1Iter != dt1List.end(); ++dt1Iter)
	{
		if (dt1Iter->startsWith(editorPath))
			(*dt1Iter) = dt1Iter->mid(editorPath.length()+1);
	}
	if (fileName.startsWith(editorPath))
		fileName = fileName.mid(editorPath.length()+1);
}

void EditorInterface::makeIni()
{
	FNTRACE("", "EditorInterface", "makeIni", "");
	assert(!fileName.isEmpty());

	QFile iniFile(fileName+".ini");
	if (!iniFile.open(QIODevice::WriteOnly | QIODevice::Text))
		ETHROW(Exception(QString("Could not open %1 for writing.").arg(iniFile.fileName())));
	QTextStream iniOut(&iniFile);

	for (MapList::iterator mapIter = mapList.begin();
		mapIter != mapList.end(); ++mapIter)
	{
		iniOut << mapIter->typeId << ' ' << mapIter->prestDef << ' ' << mapIter->file << '\n';
	}
}

QStringList EditorInterface::makeArgs()
{
	FNTRACE("", "EditorInterface", "makeCommandLine", "");

	QStringList arguments;

	if (mapList.size() == 1)
	{
		Ds1Info& map = mapList.front();
		arguments << map.file;
		if (!forceDt1)
			arguments << QString::number(map.typeId) << QString::number(map.prestDef);
		if (resize)
			arguments << "-resize" << QString::number(x) << QString::number(y);
		if (forcePalette)
			arguments << "-force_pal" << QString::number(act);
		if (noCheckAct)
			arguments << "-no_check_act";
		if (forceDt1)
		{
			if (dt1List.size() < 1)
				ETHROW(Exception("You must enter at least 1 .dt1 in the list"));
			arguments << "-force_dt1";
			arguments << dt1List;
		}
	}
	else
	{
		arguments << fileName+".ini";
		makeIni();
	}

	return arguments;
}

void EditorInterface::makeBatch(QString& batchFileName)
{
	FNTRACE("", "EditorInterface", "batchFileName", batchFileName);

	if (batchFileName.endsWith(".bat", Qt::CaseInsensitive))
		fileName = batchFileName.left(batchFileName.length()-4);
	else
		fileName = batchFileName;

	makeRelativePaths();

	batchFileName = fileName+".bat";
	QString debugFileName = fileName + "_debug.txt";

	QStringList args = makeArgs();

	QFile batchFile(batchFileName);
	if (!batchFile.open(QIODevice::WriteOnly | QIODevice::Text))
		ETHROW(Exception(QString("Failed to open %1 for writing").arg(batchFileName)));
	QTextStream batch(&batchFile);

	batch << "@echo off\n";
	if (QDir::drives().size()>1)
		batch << editorDir.absolutePath().left(2) << "\n";
	batch << "cd \"" << editorDir.absolutePath() << "\"\n" << editorName;
	for (QStringList::iterator arg = args.begin(); arg != args.end(); ++arg)
	{
		batch << ' ';
		if (arg->contains(' '))
			batch << '"'+(*arg)+'"';
		else
			batch << (*arg);
	}
	batch << " > \"" << debugFileName << "\"\n"
			"If ERRORLEVEL 0 goto DONE\n"
			"pause\n"
			":DONE\n";
}

void EditorInterface::run()
{
	FNTRACE("", "EditorInterface", "run", "");
	assert(!editorProcess || editorProcess->state() == QProcess::NotRunning);

	if (!(QFileInfo(editorDir.filePath(editorName))).exists())
		ETHROW(Exception("The Loader can't find the Ds1 Editor. "
			"You must setup the Loader's ini to select where the Ds1 Editor .exe is. "
			"Use the menu \"Settings->Configure Loader\"."));

	QDir temp = QDir::temp();
	int i = 0;
	// find first tempX.ini that doesn't exist in temporary directory
	while (QFileInfo(temp.filePath(QString("temp%1.ini").arg(i))).exists())
		++i;

	if (!editorProcess)
		editorProcess = new QProcess(this);

	fileName = temp.filePath(QString("temp%1").arg(i));
	makeRelativePaths();
	QStringList args = makeArgs();

	output->clear();
	output->insertPlainText("ds1edit Loader: starting editor using program\n");
	output->insertPlainText(editorDir.filePath(editorName));
	output->insertPlainText("\nand arguments\n\"");
	output->insertPlainText(args.join("\" \""));

	connect(editorProcess, SIGNAL(finished(int, QProcess::ExitStatus)),
		SLOT(terminated(int, QProcess::ExitStatus)));
	connect(editorProcess, SIGNAL(readyReadStandardOutput()),
		SLOT(editorOutputReady()));

	editorProcess->setWorkingDirectory(editorDir.absolutePath());
	output->insertPlainText("\"\nStarting from directory: ");
	output->insertPlainText(editorProcess->workingDirectory());
	output->insertPlainText("\n\n");

	editorProcess->start(editorDir.filePath(editorName), args, QIODevice::ReadOnly | QIODevice::Text);
	//if (!editorProcess->waitForStarted())
	//	ETHROW(Exception("Editor process failed to start"));
}

void EditorInterface::editorOutputReady()
{
	if (!output) return; // don't throw
	output->insertPlainText(QString(editorProcess->readAllStandardOutput()));
	output->moveCursor(QTextCursor::End);
	output->ensureCursorVisible();
}

void EditorInterface::terminated(int exitCode, QProcess::ExitStatus exitStatus)
{
	output->insertPlainText("\nEnd of Standard Output.\n"
		"--------------------------------------------------\nFrom stderr.txt:\n\n");

	QFile err(editorDir.filePath("stderr.txt"));
	if (!err.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		output->insertPlainText(QString("Error: failed to open %1 for reading")
			.arg(editorDir.filePath("stderr.txt")));
	}
	else
	{
		output->insertPlainText(QString(err.readAll()));
		output->insertPlainText(QString("\nEnd of stderr.txt\n\n"
			"Editor's Process exited with code %1 (0x%2)\n")
			.arg(exitCode).arg(exitCode, 2, 16, QLatin1Char('0')));
	}
	output->moveCursor(QTextCursor::End);
	output->ensureCursorVisible();

	editorProcess = 0;

	emit finished();

	((void)exitStatus); // anti-unused-param
}







