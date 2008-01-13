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

#include <QApplication>
#include <QStringList>
#include <QFileInfo>
#include <QMessageBox>
#include <QDir>
#include "loaderdialog.h"
#include "stacktrace.h"
#include "exception.h"
#if !defined(NDEBUG) || !defined(Q_WS_WIN)
#include <iostream>
#endif

#ifdef NDEBUG
#define DEBUG_ONLY(exp) ((void)0)
#else
#define DEBUG_ONLY(exp) exp
#endif

int main(int argc, char ** argv)
{
#ifndef NDEBUG
	QString args(argv[0]);
	for (int i = 1; i < argc; ++i) { args.append(' ').append(argv[i]); }
	FNTRACE("", "", "main", QString("%1, %2").arg(argc).arg(args));
#else
	FNTRACE("", "", "main", QString("%1, %2").arg(argc).arg(argv[0]));
#endif
	//Q_INIT_RESOURCE(ds1editloader);

	DEBUG_ONLY(std::cout << "Current directory: " << QDir::current().absolutePath().toStdString() << '\n');

	QStringList files;
	DEBUG_ONLY(std::cout << argv[0] << '\n');
	for (int i = 1; i < argc; ++i)
	{
		DEBUG_ONLY(std::cout << argv[i] << '\n');
		if (argv[i][0] == '-') { ++i; continue; }
		files << QFileInfo(argv[i]).absoluteFilePath();
	}
	QDir::setCurrent(QFileInfo(argv[0]).absolutePath());

	try {
		QApplication app(argc, argv);
		LoaderDialog dlg;
		if (files.size())
		{
			DEBUG_ONLY(std::cout << '"' << files.join("\" \"").toStdString() << '"' << '\n');
			dlg.addFileList(files);
		}
		dlg.show();
		return app.exec();
	} catch (Exception& e) {
#if defined(Q_WS_WIN ) && defined(NDEBUG)
		QMessageBox::critical(0, "Error", e.what());
#else
		std::cout << e.what() << std::endl;
#endif
		return 1;
	}
}
