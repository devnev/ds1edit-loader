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

#include "dt1listdialog.h"
#include <QStringListModel>
#include <QListView>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>
#include <QAction>

Dt1ListDialog::Dt1ListDialog(const QStringList& dt1StartupList, QWidget* parent)
: QDialog(parent)
{
	listModel = new QStringListModel(dt1StartupList, this);
	listView = new QListView();
	listView->setModel(listModel);
	listView->setSelectionMode(QAbstractItemView::ExtendedSelection);

	buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	addButton = buttonBox->addButton("&Add", QDialogButtonBox::ActionRole);
	removeButton = buttonBox->addButton("&Remove", QDialogButtonBox::ActionRole);

	layout = new QVBoxLayout(this);
	layout->addWidget(listView);
	layout->addWidget(buttonBox);

	removeAction = new QAction(tr("&Remove"), this);
	removeAction->setShortcut(Qt::Key_Delete);
	addAction(removeAction);

	connect(buttonBox->button(QDialogButtonBox::Ok), SIGNAL(clicked()), SLOT(accept()));
	connect(buttonBox->button(QDialogButtonBox::Cancel), SIGNAL(clicked()), SLOT(reject()));
	connect(addButton, SIGNAL(clicked()), SLOT(addFiles()));
	connect(removeButton, SIGNAL(clicked()), SLOT(removeSelection()));
	connect(removeAction, SIGNAL(triggered()), SLOT(removeSelection()));

	setWindowFlags(windowFlags() & (~Qt::WindowContextHelpButtonHint));
	setWindowTitle(tr("Configure .dt1 List"));
}

QStringList Dt1ListDialog::dt1List()
{
	return listModel->stringList();
}

void Dt1ListDialog::addFiles()
{
	QStringList list = listModel->stringList();
	QStringList newItems = QFileDialog::getOpenFileNames(this, tr("Add .dt1 Files"), QString(),
		tr("dt1 files (*.dt1);;All files (*.*)"));
	QString item;

	foreach(item, newItems)
		list.removeAll(item);
	list = list + newItems;

	if (list.size() > MAX_DT1)
		QMessageBox::warning(this, tr("Maximum exceeded"),
			tr("Adding selected files would exceed the maximum number of files\n"
			"The files have not been added."));
	else
		listModel->setStringList(list);
}

void Dt1ListDialog::removeSelection()
{
	QModelIndexList indexes = listView->selectionModel()->selectedIndexes();
	QModelIndex index;

	foreach(index, indexes)
		listModel->removeRows(index.row(), 1);
}
