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

#ifndef STACKTRACE_H_INCLUDED
#define STACKTRACE_H_INCLUDED

#include <QString>

#if defined(DEBUG) && defined(NDEBUG)
#error conflicting macros DEBUG and NDEBUG defined
#endif

#include <iostream>

namespace trace
{

	class RefcountedFunctionCall;

	class FunctionCallRef
	{
	public:
		FunctionCallRef(const RefcountedFunctionCall* call_ = 0) throw()
		: _call(call_)
		{
			initialize();
		}
		FunctionCallRef(const FunctionCallRef& other) throw()
		: _call(other._call)
		{
			initialize();
		}
		FunctionCallRef& operator=(const FunctionCallRef& rhs)
		{
			destruct();
			_call = rhs._call;
			initialize();
			return *this;
		}
		virtual ~FunctionCallRef()
		{
			destruct();
		}
		const RefcountedFunctionCall* call() const throw() { return _call; }
		bool operator==(const FunctionCallRef& other) const throw() { return other._call == _call; }
		bool operator!=(const FunctionCallRef& other) const throw() { return other._call != _call; }
		operator bool() const throw() { return _call ? true : false; }
	private:
		void initialize() throw();
		void destruct();
		const RefcountedFunctionCall* _call;
	};

	class RefcountedFunctionCall
	{
		friend class FunctionCallRef;
	public:
		RefcountedFunctionCall(QString namespaceName,
			QString className,
			QString functionName,
			QString arguments,
			FunctionCallRef caller);

		QString toString() const;

		QString _namespaceName;
		QString _className;
		QString _functionName;
		QString _arguments;
		FunctionCallRef _caller;

	private:
		mutable size_t references;
	};

	class FunctionStack
	{
	public:
		static void push(const FunctionCallRef& call) throw() { stack._push(call); }
		static void pop(const FunctionCallRef& call) { stack._pop(call); }
		static FunctionCallRef top() throw() { return stack._top; }
	private:
		void _push(const FunctionCallRef& call) throw();
		void _pop(const FunctionCallRef& call);
		FunctionCallRef _top;
		static FunctionStack stack;
	};

	class FunctionCall : public FunctionCallRef
	{
	public:
		FunctionCall(const QString& namespaceName,
			const QString& className,
			const QString& functionName,
			const QString& arguments)
		: FunctionCallRef(
			new RefcountedFunctionCall(
				namespaceName,
				className,
				functionName,
				arguments,
				FunctionStack::top()))
		{
			FunctionStack::push(*this);
#ifndef NDEBUG
			if (namespaceName.length())
				std::cout << "::" << namespaceName.toStdString();
			if (className.length())
				std::cout << "::" << className.toStdString();
			std::cout << "::" << functionName.toStdString()
				<< '(' << arguments.toStdString() << ')' << std::endl;
#endif
		}
		virtual ~FunctionCall()
		{
			FunctionStack::pop(*this);
		}
	private:
		void * operator new(size_t size) throw() { ((void)size); return 0; } // Force stack-only
	};

}

#ifndef NDEBUG
#define FNTRACE(ns, cn, fn, args) ::trace::FunctionCall __tracer(( ns ), ( cn ), ( fn ), ( args ))
#else
#define FNTRACE(ns, cn, fn, args) ((void)0)
#endif

#endif //STACKTRACE_H_INCLUDED
