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

namespace trace {

FunctionStack FunctionStack::stack;

RefcountedFunctionCall::RefcountedFunctionCall(QString namespaceName,
	QString className,
	QString functionName,
	QString arguments,
	FunctionCallRef caller)
: _namespaceName(namespaceName)
, _className(className)
, _functionName(functionName)
, _arguments(arguments)
, _caller(caller)
{
}

QString RefcountedFunctionCall::toString() const
{
	return QString()
		+ _namespaceName + (_namespaceName.length() ? "::" : "")
		+ _className + (_className.length() ? "::" : "")
		+ _functionName + "(" + _arguments + ")";
}

void FunctionCallRef::initialize() throw()
{
	if (!_call)
		return;
	++(_call->references);
}

void FunctionCallRef::destruct()
{
	if (_call)
	{
		--(_call->references);
		if  (_call->references == 0)
			delete _call;
	}
}

void FunctionStack::_push(const FunctionCallRef& call) throw()
{
	if (call.call()->_caller != _top)
		return;
	_top = call;
}

void FunctionStack::_pop(const FunctionCallRef& call)
{
	if (_top != call || !_top)
		return;
	_top = _top.call()->_caller;
}

} // trace
