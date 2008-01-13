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

#ifndef DS1INFO_H
#define DS1INFO_H

#include <QString>
//#include <QMetaType>

struct Ds1Info {
	QString file; ///< map file name
	QString name; ///< human readable map name. Does not influence comparison.
	QString path; ///< path to map in data/global/... wherever.
	int typeId; ///< ID column of LvlType.txt
	int prestDef; ///< DEF column of LvlPrest.txt
	QString comment; ///< Additional comment on map. Does not influence comparison.
	/*Ds1Info() : typeId(0), prestDef(0) { }
	Ds1Info(const char *mapName, const char *mapPath, int type_id, int def_id)
	 : mapname(mapname),path(path),typeId(type_id),prestDef(def_id) { }
	Ds1Info(Ds1Info& other)
	 : mapname(other.mapname), path(other.path), typeId(other.typeId), prestDef(other.prestDef) { }
	Ds1Info& operator=(Ds1Info &rhs)
	 { mapname=rhs.mapname; path=rhs.path; typeId=rhs.typeId; prestDef=rhs.prestDef; return *this; }*/
	/// reset members
	void clear() { file.clear(); name.clear(); path.clear(); typeId = prestDef = 0; comment.clear(); }
	/*bool operator<(Ds1Info& other) {
		if (file != other.file) return file < other.file;
		else if (path != other.path) return path < other.path;
		else if (typeId != other.typeId) return typeId < other.typeId;
		else return prestDef < other.prestDef;
	}
	bool operator==(Ds1Info& other) {
		return typeId == other.typeId &&
			prestDef == other.prestDef &&
			file == other.file &&
			path == other.path;
	}*/
};

//Q_DECLARE_METATYPE(Ds1Info);

//static const int Ds1ItemRole = Qt::UserRole + 1;

#endif
