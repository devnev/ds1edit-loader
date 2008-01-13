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

#ifndef CONFIGDIALOGSYNC_H_INCLUDED
#define CONFIGDIALOGSYNC_H_INCLUDED

#include <QString>
class QCheckBox;
class QLineEdit;
class QComboBox;
#include "iniparser.h"

bool checkbox(const QString& key, QCheckBox* check, bool def, bool invert,
	bool read, ini::AttributeMap& data);
bool lineedit(const QString& key, QLineEdit* text, const QString& def,
	bool read, ini::AttributeMap& data);
bool combobox(const QString& key, QComboBox* combo, int def,
	bool read, ini::AttributeMap& data);
void readBoolVar(ini::AttributeMap& data, const char* name, bool& var);

#endif //CONFIGDIALOGSYNC_H_INCLUDED
