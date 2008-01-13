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

#ifndef LOADERCONFIGDIALOG_H_INCLUDED
#define LOADERCONFIGDIALOG_H_INCLUDED

#include <QDialog>
#include "iniparser.h"

class QLineEdit;
class QCheckBox;
class QTreeWidget;
class QDialogButtonBox;

/// Dialog for editing Loader's .ini
class LoaderConfigDialog : public QDialog
{
	Q_OBJECT

private:
	//QVBoxLayout *topLevelLayout;
	//QGroupBox *editorLocGroup;
	//QGridLayout *editorLocGroupLayout;
	QLineEdit *editorPath;
	QLineEdit *editorName;
	//QPushButton *browseButton;
	//QGroupBox *mapListGroup;
	//QVBoxLayout *mapListGroupLayout;
	QTreeWidget *mapList;
	//QHBoxLayout *mapListButtonsLayout;
	//QSpacerItem *spacerItem1;
	//QPushButton *addButton;
	//QPushButton *modifyButton;
	//QPushButton *duplicateButton;
	//QPushButton *removeButton;
	//QSpacerItem *spacerItem2;
	QDialogButtonBox *buttonBox;

	/// Set up dialog widgets
	void setupWidgets();

	/// Read/write data from/to .ini
	void syncData(bool saveDialogData);

	/// Prompt user for changes to selected items
	void updateSelection(bool duplicate);

	ini::writeMode mode;

public:
	LoaderConfigDialog(QWidget* parent = 0);

private slots:
	void browseEditor();
	void addMap();
	void modifyMaps();
	void duplicateMaps();
	void removeMaps();

public slots:
	virtual void accept();
};

#endif //LOADERCONFIGDIALOG_H_INCLUDED
