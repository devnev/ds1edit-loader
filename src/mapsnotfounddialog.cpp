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

#include "mapsnotfounddialog.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QDialogButtonBox>
#include <QTreeWidgetItem>
#include "maplistwidget.h"
#include "mapdialog.h"

MapsNotFoundDialog::MapsNotFoundDialog(const QStringList fileNames, QWidget* parent)
: QDialog(parent/*, Qt::MSWindowsFixedSizeDialogHint*/)
{
	QVBoxLayout* topLevelLayout = new QVBoxLayout(this);

	QLabel* explanation = new QLabel(
		tr("The following files were not found in maplist.txt\n"
		"Double-click a file to manually enter its parameters."), this);
	topLevelLayout->addWidget(explanation);

	fileList = new MapListWidget(true, this);
	connect(fileList, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)),
		SLOT(itemDoubleClicked(QTreeWidgetItem*, int)));
	for (QStringList::const_iterator fileIter = fileNames.begin();
		fileIter != fileNames.end(); ++fileIter)
		fileList->AddMap(*fileIter, tr("Unknown"), QString(), QString(), QString(), QString());
	topLevelLayout->addWidget(fileList);


	QDialogButtonBox* buttonBox = new QDialogButtonBox(
		QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
		Qt::Horizontal, this);
	connect(buttonBox, SIGNAL(accepted()), SLOT(accept()));
	connect(buttonBox, SIGNAL(rejected()), SLOT(reject()));
	topLevelLayout->addWidget(buttonBox);

	setWindowTitle(tr("Maps not found"));
}

void MapsNotFoundDialog::itemDoubleClicked(QTreeWidgetItem* item, int column)
{
	MapDialog dlg(false, this);
	//dlg.setFile(item->text(0));
	dlg.setWindowTitle(tr("Enter Parameters for %1").arg(item->text(0)));
	dlg.fromMapListItem(item);
	dlg.setShowFileName(false);
	if (dlg.exec() == Accepted)
	{
		item->setText(1, dlg.name());
		item->setText(2, dlg.path());
		item->setText(3, dlg.typeId());
		item->setText(4, dlg.prestDef());
		item->setText(5, dlg.comment());
	}
}

QList<Ds1Info> MapsNotFoundDialog::newMaps()
{
	QList<QTreeWidgetItem*> modifiedMaps =
		fileList->findItems(".+", Qt::MatchRegExp, 3); // LvlType ID non-empty
	// the MapDialog should have made sure the rest of the fields are filled approprietly

	QList<Ds1Info> resultList;
	Ds1Info map;
	for (QList<QTreeWidgetItem*>::iterator mapIter = modifiedMaps.begin();
		mapIter != modifiedMaps.end(); ++mapIter)
	{
		map.file = (*mapIter)->text(0);
		map.name = (*mapIter)->text(1);
		map.path = (*mapIter)->text(2);
		map.typeId = (*mapIter)->text(3).toInt();
		map.prestDef = (*mapIter)->text(4).toInt();
		map.comment = (*mapIter)->text(5);
		resultList.append(map);
	}
	return resultList;
}
