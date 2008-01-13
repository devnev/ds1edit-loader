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

#include "maplistwidget.h"
#include <cassert>

MapListWidget::MapListWidget(bool showFileNames, QWidget* parent)
: QTreeWidget(parent), showFileColumn(showFileNames)
{
	int firstCol = (int)showFileColumn;
	setColumnCount(5+firstCol);
	if (showFileColumn)
		headerItem()->setText(0, tr("File"));
	headerItem()->setText(0+firstCol, tr("Name"));
	headerItem()->setText(1+firstCol, tr("Game Path"));
	headerItem()->setText(2+firstCol, tr("LvlType ID"));
	headerItem()->setText(3+firstCol, tr("LvlPrest Def"));
	headerItem()->setText(4+firstCol, tr("Comment"));
	setSelectionBehavior(QAbstractItemView::SelectRows);
	//setSelectionMode(QAbstractItemView::ExtendedSelection);
}

void MapListWidget::AddMap(const QString& file, const QString& name,
	const QString& path, const QString& typeId,
	const QString& prestDef, const QString& comment)
{
	if (showFileColumn)
		new QTreeWidgetItem(this, QStringList()
			<< file << name << path << typeId << prestDef << comment);
	else
		new QTreeWidgetItem(this, QStringList()
			<< name << path << typeId << prestDef << comment);
}

void MapListWidget::getMap(QTreeWidgetItem* item, Ds1Info& out)
{
	assert(item->treeWidget() == this); // item is in this treewidget
	int firstCol = (int)showFileColumn;
	if (showFileColumn)
		out.file = item->text(0);
	out.name = item->text(0+firstCol);
	out.path = item->text(1+firstCol);
	out.typeId = item->text(2+firstCol).toInt();
	out.prestDef = item->text(3+firstCol).toInt();
	out.comment = item->text(4+firstCol);
}
