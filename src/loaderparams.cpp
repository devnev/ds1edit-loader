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

#include "stacktrace.h"
#include "exception.h"
#include <QFile>
#include <QByteArray>
#include <QStringList>
#include <QTextStream>
#include <QFileInfo>
#include "loaderparams.h"

namespace params
{

QString fileName = "maplist.txt";

data_t readParams() throw (ParseError, Exception)
{
	FNTRACE("params", "", "readParams", "");

	QFile file(fileName);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		ETHROW(Exception(QString("Unable to open %1 for reading.").arg(fileName)));

	QString line;
	QStringList parts;

	size_t lineNumber = 0;

	data_t result;

	while (!file.atEnd())
	{
		line = QString(file.readLine());
		line.chop(1); // remove newline
		lineNumber++;

		parts = line.split('\t');
		if (parts.size() == 1 && parts[0] == "") // empty line, ignore
			continue;
		else if (parts.size() < 5 || parts.size() > 6)
			ETHROW(ParseError(fileName, lineNumber,
				"Wrong number of columns."));

		Ds1Info map;
		bool conversion;

		map.file = parts[0];
		map.name = parts[1];
		map.path = parts[2];
		map.typeId = parts[3].toInt(&conversion);
		if (!conversion)
			ETHROW(ParseError(fileName, lineNumber,
				"4th column not a valid number."));
		map.prestDef = parts[4].toInt(&conversion);
		if (!conversion)
			ETHROW(ParseError(fileName, lineNumber,
				"5th column not a valid number."));
		if (parts.size() == 6)
			map.comment = parts[5];

		result.append(map);
	}
	return result;
}

void writeParams(const data_t& data) throw(Exception)
{
	FNTRACE("params", "", "writeParams", QString("%2 rows of data").arg(data.size()));

	QFile file(fileName);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
		ETHROW(Exception(QString("Unable to open %1 for writing.").arg(fileName)));

	QTextStream out(&file);
	for (data_t::const_iterator iter = data.begin(); iter != data.end(); ++iter)
	{
		out
			<< QFileInfo(iter->file).fileName() << '\t'
			<< iter->name << '\t'
			<< iter->path << '\t'
			<< iter->typeId << '\t'
			<< iter->prestDef;
		if (iter->comment.length())
			out << '\t' << iter->comment;
		out << '\n';
	}
	//file.resize(file.pos());
}

void appendParams(const data_t& data) throw(Exception)
{
	FNTRACE("params", "", "appendParams", QString("%2 rows of data").arg(data.size()));

	QFile file(fileName);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
		ETHROW(Exception(QString("Unable to open %1 for appending.").arg(fileName)));

	QTextStream out(&file);
	for (data_t::const_iterator iter = data.begin(); iter != data.end(); ++iter)
	{
		out
			<< QFileInfo(iter->file).fileName() << '\t'
			<< iter->name << '\t'
			<< iter->path << '\t'
			<< iter->typeId << '\t'
			<< iter->prestDef;
		if (iter->comment.length())
			out << '\t' << iter->comment;
		out << '\n';
	}
}

void appendParams(const Ds1Info& data) throw(Exception)
{
	FNTRACE("params", "", "appendParams", data.file);

	QFile file(fileName);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
		ETHROW(Exception(QString("Unable to open %1 for appending.").arg(fileName)));

	QTextStream out(&file);
	out << QFileInfo(data.file).fileName() << '\t'
		<< data.name << '\t'
		<< data.path << '\t'
		<< data.typeId << '\t'
		<< data.prestDef;
	if (data.comment.length())
		out << '\t' << data.comment;
	out << '\n';
}

}
