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

#ifndef PARSEERROR_H_INCLUDED
#define PARSEERROR_H_INCLUDED

#include "exception.h"

class ParseError : public Exception
{
public:
	/** Construct a ParseError.
	 * @param filename name of file in which parsing failed.
	 * @param line line number where parsing failed.
	 * @param message reason for failure.
	 */
	ParseError(const QString& filename, size_t line, const QString& message)
	: Exception(QString("Error while parsing %1, line %2:\n%3")
		.arg(filename).arg(line).arg(message)) { }
};

#endif //PARSEERROR_H_INCLUDED
