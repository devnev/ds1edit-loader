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

#ifndef LOADERPARAMS_H_INCLUDED
#define LOADERPARAMS_H_INCLUDED

#include <QList>
#include <QString>
#include "ds1info.h"
#include "exception.h"
#include "parseerror.h"

/// Parser for tab-delimited map lists (e.g. maplist.txt)
namespace params
{

	extern QString fileName;

	typedef QList<Ds1Info> data_t;

	data_t readParams() throw (ParseError, Exception);

	void writeParams(const data_t& data) throw(Exception);

	void appendParams(const data_t& data) throw(Exception);
	void appendParams(const Ds1Info& data) throw(Exception);

}

#endif //LOADERPARAMS_H_INCLUDED
