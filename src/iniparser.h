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

#ifndef INIPARSER_H_INCLUDED
#define INIPARSER_H_INCLUDED

#include <QString>
#include <QHash>
#include "exception.h"
#include "parseerror.h"

/// .ini Parser for reading/writing .ini files
namespace ini
{
	/// (name,value)-map
	typedef QHash<QString, QString> AttributeMap;

	enum writeMode {
		/// Write fully commented ini
		full,
		/// Write ini without comments
		sparse,
		/// Preserve existing comments
		preserve
	};

	void writeData(const QString& filename, const QString& defaultFilename,
		const AttributeMap& data, writeMode mode = preserve) throw(ParseError, Exception);
	void readData(const QString& filename, AttributeMap& data) throw(ParseError, Exception);

	/// Parameter adaption for the loader's .ini
	namespace loader
	{
		inline void writeData(const AttributeMap& data, writeMode mode = preserve) {
			::ini::writeData("ds1edit_loader.ini", ":/res/loader_default.ini", data, mode);
		}
		inline void readData(AttributeMap& data) {
			::ini::readData("ds1edit_loader.ini", data);
		}
	}
	/// Parameter adaption for the editor's .ini
	namespace editor
	{
		inline void writeData(const AttributeMap& data, writeMode mode = preserve) {
			::ini::writeData("ds1edit.ini", ":/res/ds1edit_default.ini", data, mode);
		}
		inline void readData(AttributeMap& data) {
			::ini::readData("ds1edit.ini", data);
		}
	}
}

#endif //INIPARSER_H_INCLUDED
