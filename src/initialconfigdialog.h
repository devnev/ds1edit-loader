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

#ifndef INITIALCONFIGDIALOG_H_INCLUDED
#define INITIALCONFIGDIALOG_H_INCLUDED

#include <QDialog>
#include "iniparser.h"

class QLineEdit;
class QCheckBox;
class QDialogButtonBox;

/// Dialog for initial setup of Loader's .ini
class InitialConfigDialog : public QDialog
{
	Q_OBJECT

private:
	//QVBoxLayout *topLevelLayout;
	//QLabel *explanationText;
	//QGroupBox *editorLocGroup;
	//QGridLayout *editorLocGroupLayout;
	QLineEdit *editorPath;
	QLineEdit *editorName;
	//QPushButton *browseButton;
#ifdef Q_WS_WIN
	//QHBoxLayout *hboxLayout;
	//QSpacerItem *spacerItem1;
	QCheckBox *registerExtension;
	//QSpacerItem *spacerItem2;
#endif
	QDialogButtonBox *buttonBox;

	/// Set up dialog widgets
	void setupWidgets();

	/// Write data to .ini
	void saveData();

	/// Prompt user for changes to selected items
	void updateSelection(bool duplicate);

public:
	InitialConfigDialog(QWidget* parent = 0);

private slots:
	void browseEditor();

public slots:
	virtual void accept();
};

#endif //INITIALCONFIGDIALOG_H_INCLUDED
