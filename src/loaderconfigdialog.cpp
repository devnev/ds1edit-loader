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

#include "loaderconfigdialog.h"
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
#include "mapdialog.h"

LoaderConfigDialog::LoaderConfigDialog(QWidget* parent)
: QDialog(parent)
{
	FNTRACE("", "LoaderConfigDialog", "LoaderConfigDialog", QString("parent %1 0").arg(parent?"!=":"=="));

	setupWidgets();

	// make sure ini exists
	QFile iniFile("ds1edit_loader.ini");
	if (!iniFile.exists())
	{
		iniFile.open(QIODevice::ReadWrite | QIODevice::Text);
		iniFile.close();
		mode = ini::full;
	}
	else
		mode = ini::preserve;
	syncData(false);

	setWindowFlags(windowFlags() & (~Qt::WindowContextHelpButtonHint));
	setWindowTitle(tr("Configure Loader"));
}

void LoaderConfigDialog::setupWidgets()
{
	FNTRACE("", "LoaderConfigDialog", "setupWidgets", "");

	QVBoxLayout* topLevelLayout = new QVBoxLayout(this);

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

	groupBox = new QGroupBox(tr("Display"), this);

	QVBoxLayout* vboxLayout = new QVBoxLayout(groupBox);

	QHBoxLayout* hboxLayout = new QHBoxLayout;

	useLoaderTabs = new QCheckBox(tr("Use tab layout for loader"), groupBox);
	hboxLayout->addWidget(useLoaderTabs);

	useEditorConfigTabs = new QCheckBox(tr("Use tab layout for editor config"), groupBox);
	hboxLayout->addWidget(useEditorConfigTabs);

	vboxLayout->addLayout(hboxLayout);

	switchToOutput = new QCheckBox(tr("Show output window when running editor"), groupBox);
	vboxLayout->addWidget(switchToOutput);

	topLevelLayout->addWidget(groupBox);

	groupBox = new QGroupBox(tr("Map List"), this);

	vboxLayout = new QVBoxLayout(groupBox);

	mapList = new QTreeWidget(groupBox);
	mapList->setColumnCount(6);
	mapList->setHeaderLabels(QStringList()
		<< "File"
		<< "Name"
		<< "Path"
		<< "LvlType ID"
		<< "LvlPrest Def"
		<< "Comment");
	mapList->setSelectionMode(QAbstractItemView::ExtendedSelection);
	vboxLayout->addWidget(mapList);

	hboxLayout = new QHBoxLayout;

	hboxLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Preferred));

#define ADDMAPLISTBUTTON(text, slot) \
	pushButton = new QPushButton(tr(text), groupBox); \
	connect(pushButton, SIGNAL(clicked()), SLOT(slot)); \
	hboxLayout->addWidget(pushButton); \

	ADDMAPLISTBUTTON("Add...", addMap());
	ADDMAPLISTBUTTON("Modify...", modifyMaps());
	ADDMAPLISTBUTTON("Duplicate...", duplicateMaps());
	ADDMAPLISTBUTTON("Remove", removeMaps());
#undef ADDMAPLISTBUTTON

	hboxLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Preferred));

	vboxLayout->addLayout(hboxLayout);

	topLevelLayout->addWidget(groupBox);

	buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
	buttonBox->button(QDialogButtonBox::Ok)->setDefault(true);
	connect(buttonBox, SIGNAL(accepted()), SLOT(accept()));
	connect(buttonBox, SIGNAL(rejected()), SLOT(reject()));
	topLevelLayout->addWidget(buttonBox);
}

void LoaderConfigDialog::syncData(bool saveDialogData)
{
	FNTRACE("", "LoaderConfigDialog", "syncData", QString("saveDialogData: %1").arg(saveDialogData?"true":"false"));

	bool read = !saveDialogData;

	// sync ini
	ini::AttributeMap data;
	if (read)
		ini::loader::readData(data);
	if (!lineedit("ds1editpath", editorPath, "", read, data) ||
		!lineedit("ds1editname", editorName, "", read, data))
		mode = ini::full;
	checkbox("loader_use_tabs", useLoaderTabs, false, false, read, data);
	checkbox("editor_config_use_tabs", useEditorConfigTabs, true, false, read, data);
	checkbox("show_output_on_run", switchToOutput, true, false, read, data);
	if (!read)
	{
		if (mode == ini::preserve)
		{
			// try to write in "preserving" mode, if that fails, try in "full" mode
			try {
				ini::loader::writeData(data, ini::preserve);
			} catch (Exception) {
				// no try/catch here, pass exceptions on
				ini::loader::writeData(data, ini::full);
			}
		}
		else
			ini::loader::writeData(data, mode);
	}

	// sync maplist
	if (read)
	{
		params::data_t maps = params::readParams();
		mapList->clear();
		for (params::data_t::iterator mapsIter = maps.begin();
			mapsIter != maps.end(); ++mapsIter)
		{
			QTreeWidgetItem* item = new QTreeWidgetItem(QStringList()
				<< mapsIter->file
				<< mapsIter->name
				<< mapsIter->path
				<< QString::number(mapsIter->typeId)
				<< QString::number(mapsIter->prestDef)
				<< mapsIter->comment);
			mapList->addTopLevelItem(item);
		}
	}
	else
	{
		QFile mapFile("maplist.txt");
		if (!mapFile.exists())
			ETHROW(Exception("Could not find maplist.txt."));
		if (!mapFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
			ETHROW(Exception("Could not open maplist.txt for writing."));
		QTextStream out(&mapFile);

		typedef QList<QTreeWidgetItem*> ItemList;
		ItemList items = mapList->findItems("", Qt::MatchContains);
		assert(items.size() == mapList->topLevelItemCount());

		for (ItemList::iterator itemIter = items.begin();
			itemIter != items.end(); ++itemIter)
		{
			out << (*itemIter)->text(0) << '\t'
				<< (*itemIter)->text(1) << '\t'
				<< (*itemIter)->text(2) << '\t'
				<< (*itemIter)->text(3) << '\t'
				<< (*itemIter)->text(4);
			if ((*itemIter)->text(5).length())
				out << '\t' << (*itemIter)->text(5);
			out << '\n';
		}
	}
}

void LoaderConfigDialog::browseEditor()
{
	FNTRACE("", "LoaderConfigDialog", "browseEditor", "");

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
}

void LoaderConfigDialog::addMap()
{
	FNTRACE("", "LoaderConfigDialog", "addMap", "");

	MapDialog dlg(false, this);
	dlg.setWindowTitle(tr("Add Map"));
	if (dlg.exec() == QDialog::Accepted)
	{
		QTreeWidgetItem* item = new QTreeWidgetItem(QStringList()
			<< dlg.file()
			<< dlg.name()
			<< dlg.path()
			<< dlg.typeId()
			<< dlg.prestDef()
			<< dlg.comment());
		mapList->addTopLevelItem(item);
	}
}

void LoaderConfigDialog::updateSelection(bool duplicate)
{
	FNTRACE("", "LoaderConfigDialog", "updateSelection",
		QString("duplicate: %1").arg(duplicate?"true":"false"));

	QList<QTreeWidgetItem*> selection = mapList->selectedItems();
	QTreeWidgetItem* item, *newitem;
	QString title = (duplicate ?
		tr("Duplicate Map(s)", "", selection.size()) :
		tr("Modify Map(s)", "", selection.size()));
	if (selection.size() == 0)
		return;
	if (selection.size() == 1)
	{
		MapDialog dlg(false, this);
		dlg.setWindowTitle(title);
		item = selection.first();
		dlg.setFile(item->text(0));
		dlg.setName(item->text(1));
		dlg.setPath(item->text(2));
		dlg.setTypeId(item->text(3));
		dlg.setPrestDef(item->text(4));
		dlg.setComment(item->text(5));
		if (dlg.exec() == QDialog::Accepted)
		{
			if (duplicate) newitem = new QTreeWidgetItem;
			else newitem = item;
			newitem->setText(0, dlg.file());
			newitem->setText(1, dlg.name());
			newitem->setText(2, dlg.path());
			newitem->setText(3, dlg.typeId());
			newitem->setText(4, dlg.prestDef());
			newitem->setText(5, dlg.comment());
			if (duplicate)
				mapList->insertTopLevelItem(mapList->indexOfTopLevelItem(item)+1, newitem);
		}
	}
	else
	{
		MapDialog dlg(true, this);
		dlg.setWindowTitle(title);
		if (dlg.exec() == QDialog::Accepted)
		{
			bool update[6] = {
				dlg.file().length()!=0,
				dlg.name().length()!=0,
				dlg.path().length()!=0,
				dlg.typeId().length()!=0,
				dlg.prestDef().length()!=0,
				dlg.comment().length()!=0
			};
			for (QList<QTreeWidgetItem*>::iterator selIter = selection.begin();
				selIter != selection.end(); ++selIter)
			{
				item = (*selIter);
				if (duplicate)
					newitem = new QTreeWidgetItem(*(*selIter));
				else
					newitem = item;

				if (update[0]) newitem->setText(0, dlg.file());
				if (update[1]) newitem->setText(1, dlg.name());
				if (update[2]) newitem->setText(2, dlg.path());
				if (update[3]) newitem->setText(3, dlg.typeId());
				if (update[4]) newitem->setText(4, dlg.prestDef());
				if (update[5]) newitem->setText(5, dlg.comment());

				if (duplicate)
					mapList->insertTopLevelItem(mapList->indexOfTopLevelItem(item)+1, newitem);
			}
		}
	}
}

void LoaderConfigDialog::modifyMaps()
{
	FNTRACE("", "LoaderConfigDialog", "modifyMaps", "");

	updateSelection(false);
}

void LoaderConfigDialog::duplicateMaps()
{
	FNTRACE("", "LoaderConfigDialog", "duplicateMaps", "");

	updateSelection(true);
}

void LoaderConfigDialog::removeMaps()
{
	FNTRACE("", "LoaderConfigDialog", "removeMaps", "");

	QList<QTreeWidgetItem*> selection = mapList->selectedItems();
	for (QList<QTreeWidgetItem*>::iterator selIter = selection.begin();
		selIter != selection.end(); ++selIter)
	{
		delete mapList->takeTopLevelItem(mapList->indexOfTopLevelItem(*selIter));
	}
}

void LoaderConfigDialog::accept()
{
	FNTRACE("", "LoaderConfigDialog", "accept", "");

	if (editorPath->text().isEmpty() || editorName->text().isEmpty())
	{
		QMessageBox::information(this, tr("Empty field"),
			tr("The Editor's Name and Path must not be empty.\n"
			"Use the \"Browse...\" button to select the editor."));
		return;
	}

	try {
		syncData(true);
	} catch (Exception& e) {
		QMessageBox::critical(this, "Error", e.what());
		return;
	}

	QDialog::accept();
}
