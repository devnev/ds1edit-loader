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

#include "datasync.h"
#include <QString>
#include <QCheckBox>
#include <QLineEdit>
#include <QComboBox>
#include "iniparser.h"

const char * const YES = "YES";
const char * const NO = "NO";

bool checkbox(const QString& key, QCheckBox* check, bool def, bool invert,
	bool read, ini::AttributeMap& data)
{
	if (read)
	{
		if (data.contains(key))
		{
			if (data.value(key).compare(NO, Qt::CaseInsensitive) == 0 ||
				data.value(key).compare(YES, Qt::CaseInsensitive) == 0)
				check->setChecked((data.value(key).compare(YES, Qt::CaseInsensitive) == 0) xor invert);
			else
				check->setChecked(def);
			return true;
		}
		else
		{
			check->setChecked(def);
			return false;
		}
	}
	else
	{
		data.insert(key, (check->isChecked() xor invert ? YES : NO));
		return true;
	}
}

bool lineedit(const QString& key, QLineEdit* text, const QString& def,
	bool read, ini::AttributeMap& data)
{
	if (read)
	{
		if (data.contains(key))
		{
			if (text->validator())
			{
				QString value = data.value(key); int pos;
				if (text->validator()->validate(value, pos) == QValidator::Acceptable)
					text->setText(data.value(key));
				else
					text->setText(def);
			}
			else
				text->setText(data.value(key));
			return true;
		}
		else
		{
			text->setText(def);
			return false;
		}
	}
	else
	{
		data.insert(key, text->text());
		return true;
	}
}

bool combobox(const QString& key, QComboBox* combo, int def,
	bool read, ini::AttributeMap& data)
{
	if (read)
	{
		if (data.contains(key))
		{
			int index;
			if ((index = combo->findText(data.value(key), Qt::MatchFixedString)) != -1)
				combo->setCurrentIndex(index);
			else
				combo->setCurrentIndex(def);
			return true;
		}
		else
		{
			combo->setCurrentIndex(def);
			return false;
		}
	}
	else
	{
		data.insert(key, combo->currentText());
		return true;
	}
}

void readBoolVar(ini::AttributeMap& data, const char* name, bool& var)
{
	if (data.contains(name))
	{
		if (data.value(name).compare("YES", Qt::CaseInsensitive) == 0)
			var = true;
		else if (data.value(name).compare("NO", Qt::CaseInsensitive) == 0)
			var = false;
		//else do nothing
	}
	//else do nothing
}
