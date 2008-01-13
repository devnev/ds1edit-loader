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

#ifndef MAPLISTWIDGET_H_INCLUDED
#define MAPLISTWIDGET_H_INCLUDED

#include <QTreeWidget>
#include <QString>
#include "ds1info.h"

class MapListWidget : public QTreeWidget
{
	Q_OBJECT

private:
	const bool showFileColumn;

public:
	MapListWidget(bool showFileNames = true, QWidget* parent = 0);
	void AddMap(const QString& file, const QString& name,
		const QString& path, const QString& typeId,
		const QString& prestDef, const QString& comment = QString());
	void AddMap(const QString& file, const QString& name,
		const QString& path, int typeId, int prestDef,
		const QString& comment = QString()) {
		AddMap(file, name, path, QString::number(typeId),
			QString::number(prestDef), comment);
	}
	void AddMap(Ds1Info& map) {
		AddMap(map.file, map.name, map.path, map.typeId,
			map.prestDef, map.comment);
	}
	void getMap(QTreeWidgetItem* item, Ds1Info& out);
	Ds1Info getMap(QTreeWidgetItem* item) {
		Ds1Info map;
		getMap(item, map);
		return map;
	}
};

#endif // MAPLISTWIDGET_H_INCLUDED
