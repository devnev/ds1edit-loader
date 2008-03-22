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

#include "initialconfigdialog.h"
#include <QLineEdit>
#include <QCheckBox>
#include <QTreeWidget>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QGridLayout>
#include <QPushButton>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QLabel>
#include <QFileDialog>
#include <QPalette>
#include <QBrush>
#include <cassert>
#include "iniparser.h"
#include "loaderparams.h"
#include "datasync.h"
#include "loaderdialog.h"

InitialConfigDialog::InitialConfigDialog(QWidget* parent)
: QDialog(parent)
{
	FNTRACE("", "InitialConfigDialog", "InitialConfigDialog", QString("parent %1 0").arg(parent?"!=":"=="));

	setupWidgets();

	setWindowFlags(windowFlags() & (~Qt::WindowContextHelpButtonHint));
	setWindowTitle(tr("Setup Loader"));
}

void InitialConfigDialog::setupWidgets()
{
	FNTRACE("", "InitialConfigDialog", "setupWidgets", "");

	QVBoxLayout* topLevelLayout = new QVBoxLayout(this);

	QLabel* explanationText = new QLabel(tr("The loader is not configured.\n"
		"Please enter the basic configuration below."), this);
	topLevelLayout->addWidget(explanationText);

	QGroupBox* groupBox = new QGroupBox(tr("Editor Path && Name"), this);

	QGridLayout* gridLayout = new QGridLayout(groupBox);

	editorPath = new QLineEdit(groupBox);
	editorPath->setReadOnly(true);
	editorPath->setStyleSheet(editorPath->styleSheet() + "\nQWidget { background: #CCC; }\n");
	gridLayout->addWidget(editorPath, 0, 0, 1, 1);

	editorName = new QLineEdit(groupBox);
	editorName->setReadOnly(true);
	editorName->setStyleSheet(editorName->styleSheet() + "\nQWidget { background: #CCC; }\n");
	gridLayout->addWidget(editorName, 1, 0, 1, 1);

	QPushButton* pushButton = new QPushButton(tr("Browse..."), groupBox);
	connect(pushButton, SIGNAL(clicked()), SLOT(browseEditor()));
	gridLayout->addWidget(pushButton, 0, 1, 2, 1);

	topLevelLayout->addWidget(groupBox);

#ifdef Q_WS_WIN
	QHBoxLayout* hboxLayout = new QHBoxLayout;

	hboxLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Preferred));

	registerExtension = new QCheckBox(tr("Open .ds1 files with Ds1edit Loader"));
	hboxLayout->addWidget(registerExtension);

	hboxLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Preferred));

	topLevelLayout->addLayout(hboxLayout);
#endif

	buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
	buttonBox->button(QDialogButtonBox::Ok)->setDefault(true);
	buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
	connect(buttonBox, SIGNAL(accepted()), SLOT(accept()));
	connect(buttonBox, SIGNAL(rejected()), SLOT(reject()));
	topLevelLayout->addWidget(buttonBox);
}

void InitialConfigDialog::saveData()
{
	FNTRACE("", "InitialConfigDialog", "saveData", "");

	ini::AttributeMap data;
	lineedit("ds1editpath", editorPath, "", false, data);
	lineedit("ds1editname", editorName, "", false, data);

	// make sure file exists
	QFile iniFile("ds1edit_loader.ini");
	if (!iniFile.exists())
	{
		iniFile.open(QIODevice::ReadWrite | QIODevice::Text);
		iniFile.close();
	}
	ini::loader::writeData(data, ini::full);
}

void InitialConfigDialog::browseEditor()
{
	FNTRACE("", "InitialConfigDialog", "browseEditor", "");

	 QString executablePath =
		QFileDialog::getOpenFileName(this, "Select ds1edit executable",
			editorPath->text(), "Executable Files (*.exe);;All Files (*.*)");

	if (executablePath.length())
	{
		QFileInfo executable(executablePath);
		assert(executable.exists());
		editorPath->setText(executable.absolutePath());
		editorName->setText(executable.fileName());
	}

	if (!editorPath->text().isEmpty() && !editorName->text().isEmpty())
	{
		buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
	}
}

void InitialConfigDialog::accept()
{
	FNTRACE("", "InitialConfigDialog", "accept", "");

	if (editorPath->text().isEmpty() || editorName->text().isEmpty())
	{
		QMessageBox::information(this, tr("Empty field"),
			tr("The Editor's Name and Path must not be empty.\n"
			"Use the \"Browse...\" button to select the editor."));
		return;
	}

	try {
		saveData();
	} catch (Exception& e) {
		QMessageBox::critical(this, "Error", e.what());
		return;
	}

#ifdef Q_WS_WIN
	if (registerExtension->isChecked())
		registerExtension(this);
#endif

	QDialog::accept();
}
