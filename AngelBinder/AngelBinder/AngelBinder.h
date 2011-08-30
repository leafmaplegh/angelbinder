//
//     Copyright © 2011 - João Francisco Biondo Trinca
//          a.k.a WoLfulus <wolfulus@gmail.com>
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include <assert.h>
#include <queue>
#include <hash_map>
#include <sstream>
#include <string>

#include <angelscript.h>
#include <scriptbuilder.h>

/// 
/// Define the binder to not use namespaces
/// #define AB_NO_NAMESPACE
/// 
/// Define the binder to use libobs library (http://code.google.com/p/libobs/)
/// #define AB_USE_EVENTSYSTEM
/// 

#ifndef AB_NO_NAMESPACE
#define AB_BEGIN_NAMESPACE		namespace AngelBinder {
#define AB_END_NAMESPACE		}
#define AB_NAMESPACE			AngelBinder::
#else
#define AB_BEGIN_NAMESPACE
#define AB_END_NAMESPACE
#define AB_NAMESPACE
#endif

///
/// Namespace start
///
AB_BEGIN_NAMESPACE

///
/// Macro definitions
///
#define AB_THROW		true
#define AB_NOTHROW		false

#define AB_MESSAGE_INVOKE(i,m)				if(this->messages() != NULL) this->messages()(i,m);
#define AB_MESSAGE_INVOKE_STATIC(s,i,m)		if((s)->messages() != NULL) (s)->messages()(i,m);

#define AB_SCRIPT_ASSERT(x,m,t) { \
		if(!(x)) { \
			if((t)) { \
				throw std::exception(m); \
			} else { \
				AB_MESSAGE_INVOKE(this, ##m); \
			} \
		} \
	}

#define AB_SCRIPT_ASSERT_STATIC(x,m,t,i) { \
	if(!(x)) { \
		if((t)) { \
			throw std::exception(m); \
		} else { \
			AB_MESSAGE_INVOKE_STATIC(##i, ##i, ##m); \
		} \
	} \
}

#define AB_PUSH_ARG(x)	function.parameters().push_back(Type<x>::toString());

///
/// Type name string conversion
///
template<typename T>
struct TypeString
{
	///
	/// Retrieves the typeid of T, and then get its name.
	///
	static std::string value()
	{
		return std::string(typeid(T).name());
	}
};

///
/// Macro to help declaring built-in types.
///
#define AB_TRANSLATE_TYPE(t,n) \
	template<> \
	struct AB_NAMESPACE TypeString<t> \
	{ \
		static std::string value() \
		{ \
			return n; \
		} \
	}; 

///
/// Built-in type definition
///

AB_TRANSLATE_TYPE(char, "int8")
AB_TRANSLATE_TYPE(unsigned char, "uint8")
AB_TRANSLATE_TYPE(short, "int16")
AB_TRANSLATE_TYPE(unsigned short, "uint16")
AB_TRANSLATE_TYPE(int, "int")
AB_TRANSLATE_TYPE(unsigned int, "uint")
AB_TRANSLATE_TYPE(long long, "int64")
AB_TRANSLATE_TYPE(unsigned long long, "uint64")
AB_TRANSLATE_TYPE(float, "float")
AB_TRANSLATE_TYPE(double, "double")
AB_TRANSLATE_TYPE(bool, "bool")

///
/// Base type class used to convert template into strings.
///
template<typename T>
struct Type
{
	static std::string toString()
	{
		return TypeString<T>::value();
	}
};

///
/// Pointers
///
template<typename T>
struct Type<T*>
{
	static std::string toString()
	{
		return TypeString<T>::value() + "@";
	}
};

///
/// Constant types
///
template<typename T>
struct Type<const T>
{
	static std::string toString()
	{
		return "const " + TypeString<T>::value();
	}
};

///
/// Constant pointer types
///
template<typename T>
struct Type<const T*>
{
	static std::string toString()
	{
		return "const " + TypeString<T>::value() + "@";
	}
};

///
/// Reference types
///
template<typename T>
struct Type<T&>
{
	static std::string toString()
	{
		return TypeString<T>::value() + "&inout";
	}
};

///
/// Constant reference types
///
template<typename T>
struct Type<const T&>
{
	static std::string toString()
	{
		return TypeString<T>::value() + "&in";
	}
};

///
/// Calling conventions
///
enum CallingConvention
{
	CallStdcall = asCALL_STDCALL,
	CallCdecl = asCALL_CDECL
	/* TODO: Add more? */
};

///
/// FunctionClass parser
///
class FunctionClass
{
private:
	/// Stores the return type string
	std::string _ret;

	/// Stores the function name
	std::string _name;

	/// Stores a vector of arguments
	std::vector<std::string> _params;

	/// Calling convention
	CallingConvention _conv;

	/// Stores the pointer to the function
	void* _func;

public:
	FunctionClass(std::string ret, std::string name, CallingConvention conv, void* func);

	///
	/// Returns the function name
	///
	std::string name();

	///
	/// Returns the parameters
	///
	std::vector<std::string>& parameters();

	///
	/// Returns the pointer to the function
	///
	void* address();

	///
	/// Decomposes the function into an equivalent string
	///
	std::string decompose();

	///
	/// Gets the function calling convention
	///
	CallingConvention convention();

};

///
/// AngelScript types
///
typedef	AS_NAMESPACE_QUALIFIER asIScriptEngine	Engine;
typedef AS_NAMESPACE_QUALIFIER CScriptBuilder	Builder;

///
/// Script Engine
///
class Script
{
public:
	///
	/// Script engine's message callback
	///
	typedef void(*MessageCallback)(Script*, std::string);

private:
	///
	/// Script Engine Instance
	///
	Engine* _engine;

	///
	/// Script Engine Builder
	///
	Builder* _builder;

	///
	/// Script Module Name
	///
	std::string _module;

	///
	/// FunctionClass caching
	///
	std::hash_map<std::string, int> _functions;

	///
	/// Message callbacks
	///
	MessageCallback _messages;

	///
	/// Startup stuff
	///
	void initialize();
	void uninitialize();

public:
	///
	/// Constructors
	///
	Script();
	Script(std::string name);
	~Script(void);

	///
	/// Retrieves the engine instance
	///
	Engine& engine();

	///
	/// Retrieves the script builder addon
	///
	Builder& builder();

	///
	/// Message callbacks
	///
	MessageCallback& messages();

	///
	/// Script compiler
	///
	bool compileFile(std::string file);

	///
	/// Script text Ccompiler
	///
	bool compileScript(std::string script);

private:
	///
	/// Raised when script machine writes a message
	///
	static void onScriptMessage( const asSMessageInfo *msg, void *param );

};

///
/// Generic exporter
///
template<typename C, typename T>
class BaseExporter
{	
	/// Friend class to call 
	friend class Exporter;

protected:
	/// Stores all exported entries
	std::queue<T> _entries;

	/// Method that must be implemented by the exporter
	virtual void finish(Script& instance) = 0;

};

///
/// FunctionClass export container
///
class FunctionExporter 
	: public BaseExporter<FunctionExporter, FunctionClass>
{

	/// Friend exporter class
	friend class Exporter;

protected:
	///
	/// Function exporter
	///
	FunctionExporter();

public:

	///
	/// Templated function exporters
	///

	template<typename R>
	FunctionExporter& def(std::string name, R(__stdcall *func)())
	{
		FunctionClass function(Type<R>::toString(), name, CallStdcall, func);

		this->_entries.push(function);
		return *this;
	}

	template<typename R>
	FunctionExporter& def(std::string name, R(__cdecl *func)())
	{
		FunctionClass function(Type<R>::toString(), name, CallCdecl, func);

		this->_entries.push(function);
		return *this;
	}

	template<typename R, typename A1>
	FunctionExporter& def(std::string name, R(__stdcall *func)(A1))
	{
		FunctionClass function(Type<R>::toString(), name, CallStdcall, func);
		AB_PUSH_ARG(A1); 
		this->_entries.push(function);
		return *this;
	}

	template<typename R, typename A1>
	FunctionExporter& def(std::string name, R(__cdecl *func)(A1))
	{
		FunctionClass function(Type<R>::toString(), name, CallCdecl, func);
		AB_PUSH_ARG(A1); 
		this->_entries.push(function);
		return *this;
	}

	template<typename R, typename A1, typename A2>
	FunctionExporter& def(std::string name, R(__stdcall *func)(A1, A2))
	{
		FunctionClass function(Type<R>::toString(), name, CallStdcall, func);
		AB_PUSH_ARG(A1); AB_PUSH_ARG(A2); 
		this->_entries.push(function);
		return *this;
	}

	template<typename R, typename A1, typename A2>
	FunctionExporter& def(std::string name, R(__cdecl *func)(A1, A2))
	{
		FunctionClass function(Type<R>::toString(), name, CallCdecl, func);
		AB_PUSH_ARG(A1); AB_PUSH_ARG(A2); 
		this->_entries.push(function);
		return *this;
	}

	template<typename R, typename A1, typename A2, typename A3>
	FunctionExporter& def(std::string name, R(__stdcall *func)(A1, A2, A3))
	{
		FunctionClass function(Type<R>::toString(), name, CallStdcall, func);
		AB_PUSH_ARG(A1); AB_PUSH_ARG(A2); AB_PUSH_ARG(A3); 
		this->_entries.push(function);
		return *this;
	}

	template<typename R, typename A1, typename A2, typename A3>
	FunctionExporter& def(std::string name, R(__cdecl *func)(A1, A2, A3))
	{
		FunctionClass function(Type<R>::toString(), name, CallCdecl, func);
		AB_PUSH_ARG(A1); AB_PUSH_ARG(A2); AB_PUSH_ARG(A3); 
		this->_entries.push(function);
		return *this;
	}

	template<typename R, typename A1, typename A2, typename A3, typename A4>
	FunctionExporter& def(std::string name, R(__stdcall *func)(A1, A2, A3, A4))
	{
		FunctionClass function(Type<R>::toString(), name, CallStdcall, func);
		AB_PUSH_ARG(A1); AB_PUSH_ARG(A2); AB_PUSH_ARG(A3); AB_PUSH_ARG(A4); 
		this->_entries.push(function);
		return *this;
	}

	template<typename R, typename A1, typename A2, typename A3, typename A4>
	FunctionExporter& def(std::string name, R(__cdecl *func)(A1, A2, A3, A4))
	{
		FunctionClass function(Type<R>::toString(), name, CallCdecl, func);
		AB_PUSH_ARG(A1); AB_PUSH_ARG(A2); AB_PUSH_ARG(A3); AB_PUSH_ARG(A4); 
		this->_entries.push(function);
		return *this;
	}

	template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5>
	FunctionExporter& def(std::string name, R(__stdcall *func)(A1, A2, A3, A4, A5))
	{
		FunctionClass function(Type<R>::toString(), name, CallStdcall, func);
		AB_PUSH_ARG(A1); AB_PUSH_ARG(A2); AB_PUSH_ARG(A3); AB_PUSH_ARG(A4); AB_PUSH_ARG(A5); 
		this->_entries.push(function);
		return *this;
	}

	template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5>
	FunctionExporter& def(std::string name, R(__cdecl *func)(A1, A2, A3, A4, A5))
	{
		FunctionClass function(Type<R>::toString(), name, CallCdecl, func);
		AB_PUSH_ARG(A1); AB_PUSH_ARG(A2); AB_PUSH_ARG(A3); AB_PUSH_ARG(A4); AB_PUSH_ARG(A5); 
		this->_entries.push(function);
		return *this;
	}

	template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
	FunctionExporter& def(std::string name, R(__stdcall *func)(A1, A2, A3, A4, A5, A6))
	{
		FunctionClass function(Type<R>::toString(), name, CallStdcall, func);
		AB_PUSH_ARG(A1); AB_PUSH_ARG(A2); AB_PUSH_ARG(A3); AB_PUSH_ARG(A4); AB_PUSH_ARG(A5); AB_PUSH_ARG(A6); 
		this->_entries.push(function);
		return *this;
	}

	template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
	FunctionExporter& def(std::string name, R(__cdecl *func)(A1, A2, A3, A4, A5, A6))
	{
		FunctionClass function(Type<R>::toString(), name, CallCdecl, func);
		AB_PUSH_ARG(A1); AB_PUSH_ARG(A2); AB_PUSH_ARG(A3); AB_PUSH_ARG(A4); AB_PUSH_ARG(A5); AB_PUSH_ARG(A6); 
		this->_entries.push(function);
		return *this;
	}

	template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
	FunctionExporter& def(std::string name, R(__stdcall *func)(A1, A2, A3, A4, A5, A6, A7))
	{
		FunctionClass function(Type<R>::toString(), name, CallStdcall, func);
		AB_PUSH_ARG(A1); AB_PUSH_ARG(A2); AB_PUSH_ARG(A3); AB_PUSH_ARG(A4); AB_PUSH_ARG(A5); AB_PUSH_ARG(A6); AB_PUSH_ARG(A7); 
		this->_entries.push(function);
		return *this;
	}

	template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
	FunctionExporter& def(std::string name, R(__cdecl *func)(A1, A2, A3, A4, A5, A6, A7))
	{
		FunctionClass function(Type<R>::toString(), name, CallCdecl, func);
		AB_PUSH_ARG(A1); AB_PUSH_ARG(A2); AB_PUSH_ARG(A3); AB_PUSH_ARG(A4); AB_PUSH_ARG(A5); AB_PUSH_ARG(A6); AB_PUSH_ARG(A7); 
		this->_entries.push(function);
		return *this;
	}

	template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
	FunctionExporter& def(std::string name, R(__stdcall *func)(A1, A2, A3, A4, A5, A6, A7, A8))
	{
		FunctionClass function(Type<R>::toString(), name, CallStdcall, func);
		AB_PUSH_ARG(A1); AB_PUSH_ARG(A2); AB_PUSH_ARG(A3); AB_PUSH_ARG(A4); AB_PUSH_ARG(A5); AB_PUSH_ARG(A6); AB_PUSH_ARG(A7); AB_PUSH_ARG(A8); 
		this->_entries.push(function);
		return *this;
	}

	template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
	FunctionExporter& def(std::string name, R(__cdecl *func)(A1, A2, A3, A4, A5, A6, A7, A8))
	{
		FunctionClass function(Type<R>::toString(), name, CallCdecl, func);
		AB_PUSH_ARG(A1); AB_PUSH_ARG(A2); AB_PUSH_ARG(A3); AB_PUSH_ARG(A4); AB_PUSH_ARG(A5); AB_PUSH_ARG(A6); AB_PUSH_ARG(A7); AB_PUSH_ARG(A8); 
		this->_entries.push(function);
		return *this;
	}

	template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
	FunctionExporter& def(std::string name, R(__stdcall *func)(A1, A2, A3, A4, A5, A6, A7, A8, A9))
	{
		FunctionClass function(Type<R>::toString(), name, CallStdcall, func);
		AB_PUSH_ARG(A1); AB_PUSH_ARG(A2); AB_PUSH_ARG(A3); AB_PUSH_ARG(A4); AB_PUSH_ARG(A5); AB_PUSH_ARG(A6); AB_PUSH_ARG(A7); AB_PUSH_ARG(A8); AB_PUSH_ARG(A9); 
		this->_entries.push(function);
		return *this;
	}

	template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
	FunctionExporter& def(std::string name, R(__cdecl *func)(A1, A2, A3, A4, A5, A6, A7, A8, A9))
	{
		FunctionClass function(Type<R>::toString(), name, CallCdecl, func);
		AB_PUSH_ARG(A1); AB_PUSH_ARG(A2); AB_PUSH_ARG(A3); AB_PUSH_ARG(A4); AB_PUSH_ARG(A5); AB_PUSH_ARG(A6); AB_PUSH_ARG(A7); AB_PUSH_ARG(A8); AB_PUSH_ARG(A9); 
		this->_entries.push(function);
		return *this;
	}

	template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10>
	FunctionExporter& def(std::string name, R(__stdcall *func)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10))
	{
		FunctionClass function(Type<R>::toString(), name, CallStdcall, func);
		AB_PUSH_ARG(A1); AB_PUSH_ARG(A2); AB_PUSH_ARG(A3); AB_PUSH_ARG(A4); AB_PUSH_ARG(A5); AB_PUSH_ARG(A6); AB_PUSH_ARG(A7); AB_PUSH_ARG(A8); AB_PUSH_ARG(A9); AB_PUSH_ARG(A10); 
		this->_entries.push(function);
		return *this;
	}

	template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10>
	FunctionExporter& def(std::string name, R(__cdecl *func)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10))
	{
		FunctionClass function(Type<R>::toString(), name, CallCdecl, func);
		AB_PUSH_ARG(A1); AB_PUSH_ARG(A2); AB_PUSH_ARG(A3); AB_PUSH_ARG(A4); AB_PUSH_ARG(A5); AB_PUSH_ARG(A6); AB_PUSH_ARG(A7); AB_PUSH_ARG(A8); AB_PUSH_ARG(A9); AB_PUSH_ARG(A10); 
		this->_entries.push(function);
		return *this;
	}

	template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11>
	FunctionExporter& def(std::string name, R(__stdcall *func)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11))
	{
		FunctionClass function(Type<R>::toString(), name, CallStdcall, func);
		AB_PUSH_ARG(A1); AB_PUSH_ARG(A2); AB_PUSH_ARG(A3); AB_PUSH_ARG(A4); AB_PUSH_ARG(A5); AB_PUSH_ARG(A6); AB_PUSH_ARG(A7); AB_PUSH_ARG(A8); AB_PUSH_ARG(A9); AB_PUSH_ARG(A10); AB_PUSH_ARG(A11); 
		this->_entries.push(function);
		return *this;
	}

	template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11>
	FunctionExporter& def(std::string name, R(__cdecl *func)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11))
	{
		FunctionClass function(Type<R>::toString(), name, CallCdecl, func);
		AB_PUSH_ARG(A1); AB_PUSH_ARG(A2); AB_PUSH_ARG(A3); AB_PUSH_ARG(A4); AB_PUSH_ARG(A5); AB_PUSH_ARG(A6); AB_PUSH_ARG(A7); AB_PUSH_ARG(A8); AB_PUSH_ARG(A9); AB_PUSH_ARG(A10); AB_PUSH_ARG(A11); 
		this->_entries.push(function);
		return *this;
	}

	template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12>
	FunctionExporter& def(std::string name, R(__stdcall *func)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12))
	{
		FunctionClass function(Type<R>::toString(), name, CallStdcall, func);
		AB_PUSH_ARG(A1); AB_PUSH_ARG(A2); AB_PUSH_ARG(A3); AB_PUSH_ARG(A4); AB_PUSH_ARG(A5); AB_PUSH_ARG(A6); AB_PUSH_ARG(A7); AB_PUSH_ARG(A8); AB_PUSH_ARG(A9); AB_PUSH_ARG(A10); AB_PUSH_ARG(A11); AB_PUSH_ARG(A12); 
		this->_entries.push(function);
		return *this;
	}

	template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12>
	FunctionExporter& def(std::string name, R(__cdecl *func)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12))
	{
		FunctionClass function(Type<R>::toString(), name, CallCdecl, func);
		AB_PUSH_ARG(A1); AB_PUSH_ARG(A2); AB_PUSH_ARG(A3); AB_PUSH_ARG(A4); AB_PUSH_ARG(A5); AB_PUSH_ARG(A6); AB_PUSH_ARG(A7); AB_PUSH_ARG(A8); AB_PUSH_ARG(A9); AB_PUSH_ARG(A10); AB_PUSH_ARG(A11); AB_PUSH_ARG(A12); 
		this->_entries.push(function);
		return *this;
	}

	template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12, typename A13>
	FunctionExporter& def(std::string name, R(__stdcall *func)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13))
	{
		FunctionClass function(Type<R>::toString(), name, CallStdcall, func);
		AB_PUSH_ARG(A1); AB_PUSH_ARG(A2); AB_PUSH_ARG(A3); AB_PUSH_ARG(A4); AB_PUSH_ARG(A5); AB_PUSH_ARG(A6); AB_PUSH_ARG(A7); AB_PUSH_ARG(A8); AB_PUSH_ARG(A9); AB_PUSH_ARG(A10); AB_PUSH_ARG(A11); AB_PUSH_ARG(A12); AB_PUSH_ARG(A13); 
		this->_entries.push(function);
		return *this;
	}

	template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12, typename A13>
	FunctionExporter& def(std::string name, R(__cdecl *func)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13))
	{
		FunctionClass function(Type<R>::toString(), name, CallCdecl, func);
		AB_PUSH_ARG(A1); AB_PUSH_ARG(A2); AB_PUSH_ARG(A3); AB_PUSH_ARG(A4); AB_PUSH_ARG(A5); AB_PUSH_ARG(A6); AB_PUSH_ARG(A7); AB_PUSH_ARG(A8); AB_PUSH_ARG(A9); AB_PUSH_ARG(A10); AB_PUSH_ARG(A11); AB_PUSH_ARG(A12); AB_PUSH_ARG(A13); 
		this->_entries.push(function);
		return *this;
	}

	template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12, typename A13, typename A14>
	FunctionExporter& def(std::string name, R(__stdcall *func)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14))
	{
		FunctionClass function(Type<R>::toString(), name, CallStdcall, func);
		AB_PUSH_ARG(A1); AB_PUSH_ARG(A2); AB_PUSH_ARG(A3); AB_PUSH_ARG(A4); AB_PUSH_ARG(A5); AB_PUSH_ARG(A6); AB_PUSH_ARG(A7); AB_PUSH_ARG(A8); AB_PUSH_ARG(A9); AB_PUSH_ARG(A10); AB_PUSH_ARG(A11); AB_PUSH_ARG(A12); AB_PUSH_ARG(A13); AB_PUSH_ARG(A14); 
		this->_entries.push(function);
		return *this;
	}

	template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12, typename A13, typename A14>
	FunctionExporter& def(std::string name, R(__cdecl *func)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14))
	{
		FunctionClass function(Type<R>::toString(), name, CallCdecl, func);
		AB_PUSH_ARG(A1); AB_PUSH_ARG(A2); AB_PUSH_ARG(A3); AB_PUSH_ARG(A4); AB_PUSH_ARG(A5); AB_PUSH_ARG(A6); AB_PUSH_ARG(A7); AB_PUSH_ARG(A8); AB_PUSH_ARG(A9); AB_PUSH_ARG(A10); AB_PUSH_ARG(A11); AB_PUSH_ARG(A12); AB_PUSH_ARG(A13); AB_PUSH_ARG(A14); 
		this->_entries.push(function);
		return *this;
	}


	///
	/// Called when registering is needed.
	///
	virtual void finish(Script& instance);

};

///
/// StructMember struct
///
typedef struct StructMember_t
{
	/// Name of the member
	std::string name;

	/// Name of the member
	std::string type;

	/// Offset of the member
	int offset;
} 
StructMember, StructMemberPtr;

///
/// StructExporter export container
///
class StructExporter
	: public BaseExporter<StructExporter, StructMember>
{

	/// Friend exporter class
	friend class Exporter;

private:
	/// Type name
	std::string _name;

	/// Struct size
	int _size;

protected:
	///
	/// Struct exporter
	///
	StructExporter(std::string name, int size);

	///
	/// Called when registering is needed.
	///
	virtual void finish(Script& instance);

public:
	///
	/// Registers a member of the struct
	///
	template<typename T, typename S>
	StructExporter& member(std::string name, T S::*offset)
	{
		StructMember m;
		m.name = name;
		m.offset = *(int*)&offset;
		m.type = Type<T>::toString();
		this->_entries.push(m);
		return *this;
	}

};

///
/// Start exporting functions to the script
///
class Exporter
{
private:
	/// Stores the script instance.
	Script& _script;

protected:
	///
	/// Initializes the exporter
	///
	Exporter(Script& script);

public:
	///
	/// Starts an exporter
	///
	static Exporter Export( Script& script );

	///
	/// FunctionExporter wrapper
	///
	static FunctionExporter Functions();

	///
	/// StructExporter
	///
	template<typename T>
	static StructExporter Struct()
	{
		StructExporter instance(Type<T>::toString(), sizeof(T));
		return instance;
	}

	///
	/// Exports the functions
	///
	void operator[] (FunctionExporter& funcs)
	{
		funcs.finish(this->_script);
	}

	///
	/// Exports the functions
	///
	void operator[] (StructExporter& structs)
	{
		structs.finish(this->_script);
	}

};

AB_END_NAMESPACE