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

#ifndef EXCEPTION_H_INCLUDED
#define EXCEPTION_H_INCLUDED

#include <exception>
#include <QString>
#include "stacktrace.h"

class Exception : public std::exception
{
public:
	Exception(const QString& message)
	: _message(message)
#ifndef NDEBUG
	, _trace(trace::FunctionStack::top())
#endif
	{
	}

	virtual ~Exception() throw() { }

	virtual const char* what() const throw()
	{
#ifndef NDEBUG
		QString message(_message);
		message += "\n\nStack trace (most recent call first):\n";
		trace::FunctionCallRef trace_(_trace);
		for ( ; trace_; trace_ = trace_.call()->_caller )
		{
			message += trace_.call()->toString();
			message += "\n";
		}
		message += "Stack trace entry point.";
		if (file())
		{
			message += "\nException thrown in file ";
			message += file();
			message += ", line ";
			message += QString::number(line());
			message += ".";
		}
		return message.toAscii().data();
#else
		return _message.toAscii().data();
#endif
	}

private:
	QString _message;
#ifndef NDEBUG
	trace::FunctionCallRef _trace;
#endif

#ifndef NDEBUG
public:
	template<class T>
	static void throw_(T e, const char* file_, size_t line_)
	{
		((void)throw_type_must_be_derived_from_Exception<T>::valid);

		e.setFile(file_);
		e.setLine(line_);
		throw e;
	}

	size_t line() const throw() { return _line; }
	const char* file() const throw() { return _file; }

private:
	template<class T>
	struct throw_type_must_be_derived_from_Exception
	{
		static Exception* _pE;
		static T* _pT;
		static int _test() { _pE = _pT; return 0; }
		static const int _eval = sizeof(_test());
		static const int valid = 1;
	};

	const char* _file;
	size_t _line;

	void setFile(const char* file) { _file = file; }
	void setLine(size_t line) { _line = line; }
#else
public: // provide stubs
	size_t line() const throw() { return 0; }
	const char* file() const throw() { return 0; }
#endif

};

#ifndef NDEBUG
#define ETHROW(e) ::Exception::throw_(( e ), __FILE__, __LINE__)
#else
#define ETHROW(e) throw ( e )
#endif

#endif //EXCEPTION_H_INCLUDED
