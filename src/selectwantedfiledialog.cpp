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

#include "selectwantedfiledialog.h"
#include <QFileInfo>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QList>
#include <QPushButton>
#include <QMessageBox>
#include "maplistwidget.h"
#include "loaderparams.h"
#include "mapdialog.h"
#include "exception.h"

SelectWantedFileDialog::SelectWantedFileDialog(const QString& fileName, QWidget* parent)
: QDialog(parent), mapFile(fileName)
{
	QVBoxLayout* dialogLayout = new QVBoxLayout(this);

	fileList = new MapListWidget(false, this);
	dialogLayout->addWidget(fileList);

	QPushButton* addButton = new QPushButton(tr("Add..."));
	connect(addButton, SIGNAL(clicked()), SLOT(addMap()));

	buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Ignore, Qt::Horizontal);
	buttonBox->addButton(addButton, QDialogButtonBox::ActionRole);
	dialogLayout->addWidget(buttonBox);

	fileList->setSelectionMode(QAbstractItemView::SingleSelection);
	fileList->setCurrentItem(0);

	buttonBox->setDisabled(true);

	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
	connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
	connect(buttonBox->button(QDialogButtonBox::Ignore), SIGNAL(clicked()), this, SLOT(skip()));

	setWindowFlags(windowFlags() & (~Qt::WindowContextHelpButtonHint));
	setWindowTitle(tr("Select %1").arg(fileName));
}

void SelectWantedFileDialog::addDs1(Ds1Info& item)
{
	fileList->AddMap(item);
	if (!fileList->currentItem())
		fileList->setCurrentItem(fileList->topLevelItem(0));
	buttonBox->setEnabled(true);
}

void SelectWantedFileDialog::enableSkip(bool enable)
{
	buttonBox->button(QDialogButtonBox::Ignore)->setEnabled(enable);
}

void SelectWantedFileDialog::disableSkip(bool disable)
{
	enableSkip(!disable);
}

void SelectWantedFileDialog::enableCancel(bool enable)
{
	buttonBox->button(QDialogButtonBox::Cancel)->setEnabled(enable);
}

void SelectWantedFileDialog::disableCancel(bool disable)
{
	enableCancel(!disable);
}

void SelectWantedFileDialog::skip()
{
	emit skipped();
	done(Skipped);
}

int SelectWantedFileDialog::selectWantedFile(const QString& fileName, Ds1Info& out,
	QWidget* parent, bool autoSelectSingle)
{
	SelectWantedFileDialog dlg(fileName, parent);

	QFileInfo fileInfo(fileName);
	QString name = fileInfo.fileName();

	size_t count = 0;
	params::data_t maps = params::readParams();
	for (params::data_t::iterator mapsIter = maps.begin();
		mapsIter != maps.end(); mapsIter++)
	{
		if (mapsIter->file.compare(name, Qt::CaseInsensitive) == 0)
		{
			dlg.addDs1(*mapsIter);
			count++;
		}
	}

	if (count == 0)
	{
		return NotFound;
	}
	else if (count == 1 && autoSelectSingle)
	{
		QTreeWidgetItem* selectedItem = dlg.fileList->topLevelItem(0);
		dlg.fileList->getMap(selectedItem, out);
		out.file = fileName;
		return Accepted;
	}
	else
	{
		int result = dlg.exec();
		if (result == Accepted)
		{
			QTreeWidgetItem* selectedItem = dlg.fileList->currentItem();
			//out = dlg.fileList->currentItem()->data(0, Ds1ItemRole).value<Ds1Info>();
			dlg.fileList->getMap(selectedItem, out);
			out.file = fileName;
		}
		return result;
	}
}

void SelectWantedFileDialog::addMap()
{
	MapDialog dlg(false, this);
	dlg.setFile(mapFile);
	dlg.setShowFileName(false);
	dlg.setWindowTitle(tr("Enter Parameters for %1").arg(mapFile));
	if (dlg.exec() == QDialog::Accepted)
	{
		Ds1Info map = dlg.toMap();
		if (QMessageBox::question(this, tr("Save new map?"),
			tr("Would you like to save the new map to maplist.txt?"),
			QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes)
			== QMessageBox::Yes)
			params::appendParams(map);
		fileList->AddMap(map);
	}
}
