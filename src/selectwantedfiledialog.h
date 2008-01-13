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

#ifndef SELECTWANTEDFILEDIALOG_H_INCLUDED
#define SELECTWANTEDFILEDIALOG_H_INCLUDED

#include <QDialog>
#include "ds1info.h"

class MapListWidget;
class QDialogButtonBox;

class SelectWantedFileDialog : public QDialog
{
	Q_OBJECT

private:

	MapListWidget* fileList;
	QDialogButtonBox* buttonBox;
	//QVBoxLayout* dialogLayout;

	QString mapFile;

	void addDs1(Ds1Info& item);
	//void removeDs1(Ds1Info* item); // don't really need it
	//void clearList() { fileList->clear(); }

	SelectWantedFileDialog(const QString& fileName, QWidget* parent = 0);
public:
	virtual ~SelectWantedFileDialog() { }

	static const int Skipped = ((int)Accepted)+1;
	static const int NotFound = Skipped+1;

	static int selectWantedFile(const QString& fileName, Ds1Info& out,
		QWidget* parent = 0, bool autoSelectSingle = true);

public slots:
	void enableSkip(bool enable);
	void disableSkip(bool disable);
	void enableCancel(bool enable);
	void disableCancel(bool disable);

	void skip();

	void addMap();

signals:
	void skipped();
};

#endif // SELECTWANTEDFILEDIALOG_H_INCLUDED
