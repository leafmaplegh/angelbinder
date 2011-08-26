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

#ifdef AB_USE_EVENTSYSTEM
#include <libobs/events.h>
#endif

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

#ifdef AB_TRANSLATE_TYPE
#define AB_MESSAGE_INVOKE(i,m)				this->messages().invoke(i,m);
#define AB_MESSAGE_INVOKE_STATIC(s,i,m)		(s)->messages().invoke(i,m);
#else
#define AB_MESSAGE_INVOKE(i,m)				if(this->messages() != NULL) this->messages()(i,m);
#define AB_MESSAGE_INVOKE_STATIC(s,i,m)		if((s)->messages() != NULL) (s)->messages()(i,m);
#endif

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
		return TypeString<T>::value() + "& inout";
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
		return TypeString<T>::value() + "& in";
	}
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

	/// Stores the pointer to the function
	void* _func;

public:
	FunctionClass(std::string ret, std::string name, void* func);

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
#ifdef AB_USE_EVENTSYSTEM
	typedef Event_v2<Script*, std::string> MessageCallback;
#else
	typedef void(*MessageCallback)(Script*, std::string);
#endif

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

public:
	///
	/// Adds a new exported entry to the queue
	///
	C& operator()(T entry)
	{
		this->_entries.push(entry);
		return (C&)(*this);
	}

	///
	/// Adds a new exported entry to the queue
	///
	C& def(T entry)
	{
		this->_entries.push(entry);
		return (C&)(*this);
	}

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

	///
	/// Called when registering is needed.
	///
	virtual void finish(Script& instance);

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
	static Exporter Exporter::Export( Script& script )
	{
		Exporter exporter(script);
		return exporter;
	}

	///
	/// BaseExporter wrapper
	///
	static FunctionExporter Exporter::Functions()
	{
		FunctionExporter exporter;
		return exporter;
	}

	///
	/// Exports the functions
	///
	void operator[](FunctionExporter& funcs)
	{
		funcs.finish(this->_script);
	}

};

///
/// Function class wrappers
///
template<typename R>
static FunctionClass Function(std::string name, R(*func)())
{
	FunctionClass function(Type<R>::toString(), name, func);
	return function;
}

template<typename R, typename A1>
static FunctionClass Function(std::string name, R(*func)(A1))
{
	FunctionClass function(Type<R>::toString(), name, func);
	AB_PUSH_ARG(A1); 
	return function;
}

template<typename R, typename A1, typename A2>
static FunctionClass Function(std::string name, R(*func)(A1, A2))
{
	FunctionClass function(Type<R>::toString(), name, func);
	AB_PUSH_ARG(A1); AB_PUSH_ARG(A2); 
	return function;
}

template<typename R, typename A1, typename A2, typename A3>
static FunctionClass Function(std::string name, R(*func)(A1, A2, A3))
{
	FunctionClass function(Type<R>::toString(), name, func);
	AB_PUSH_ARG(A1); AB_PUSH_ARG(A2); AB_PUSH_ARG(A3); 
	return function;
}

template<typename R, typename A1, typename A2, typename A3, typename A4>
static FunctionClass Function(std::string name, R(*func)(A1, A2, A3, A4))
{
	FunctionClass function(Type<R>::toString(), name, func);
	AB_PUSH_ARG(A1); AB_PUSH_ARG(A2); AB_PUSH_ARG(A3); AB_PUSH_ARG(A4); 
	return function;
}

template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5>
static FunctionClass Function(std::string name, R(*func)(A1, A2, A3, A4, A5))
{
	FunctionClass function(Type<R>::toString(), name, func);
	AB_PUSH_ARG(A1); AB_PUSH_ARG(A2); AB_PUSH_ARG(A3); AB_PUSH_ARG(A4); AB_PUSH_ARG(A5); 
	return function;
}

template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
static FunctionClass Function(std::string name, R(*func)(A1, A2, A3, A4, A5, A6))
{
	FunctionClass function(Type<R>::toString(), name, func);
	AB_PUSH_ARG(A1); AB_PUSH_ARG(A2); AB_PUSH_ARG(A3); AB_PUSH_ARG(A4); AB_PUSH_ARG(A5); AB_PUSH_ARG(A6); 
	return function;
}

template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
static FunctionClass Function(std::string name, R(*func)(A1, A2, A3, A4, A5, A6, A7))
{
	FunctionClass function(Type<R>::toString(), name, func);
	AB_PUSH_ARG(A1); AB_PUSH_ARG(A2); AB_PUSH_ARG(A3); AB_PUSH_ARG(A4); AB_PUSH_ARG(A5); AB_PUSH_ARG(A6); AB_PUSH_ARG(A7); 
	return function;
}

template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
static FunctionClass Function(std::string name, R(*func)(A1, A2, A3, A4, A5, A6, A7, A8))
{
	FunctionClass function(Type<R>::toString(), name, func);
	AB_PUSH_ARG(A1); AB_PUSH_ARG(A2); AB_PUSH_ARG(A3); AB_PUSH_ARG(A4); AB_PUSH_ARG(A5); AB_PUSH_ARG(A6); AB_PUSH_ARG(A7); AB_PUSH_ARG(A8); 
	return function;
}

template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
static FunctionClass Function(std::string name, R(*func)(A1, A2, A3, A4, A5, A6, A7, A8, A9))
{
	FunctionClass function(Type<R>::toString(), name, func);
	AB_PUSH_ARG(A1); AB_PUSH_ARG(A2); AB_PUSH_ARG(A3); AB_PUSH_ARG(A4); AB_PUSH_ARG(A5); AB_PUSH_ARG(A6); AB_PUSH_ARG(A7); AB_PUSH_ARG(A8); AB_PUSH_ARG(A9); 
	return function;
}

template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10>
static FunctionClass Function(std::string name, R(*func)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10))
{
	FunctionClass function(Type<R>::toString(), name, func);
	AB_PUSH_ARG(A1); AB_PUSH_ARG(A2); AB_PUSH_ARG(A3); AB_PUSH_ARG(A4); AB_PUSH_ARG(A5); AB_PUSH_ARG(A6); AB_PUSH_ARG(A7); AB_PUSH_ARG(A8); AB_PUSH_ARG(A9); AB_PUSH_ARG(A10); 
	return function;
}

template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11>
static FunctionClass Function(std::string name, R(*func)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11))
{
	FunctionClass function(Type<R>::toString(), name, func);
	AB_PUSH_ARG(A1); AB_PUSH_ARG(A2); AB_PUSH_ARG(A3); AB_PUSH_ARG(A4); AB_PUSH_ARG(A5); AB_PUSH_ARG(A6); AB_PUSH_ARG(A7); AB_PUSH_ARG(A8); AB_PUSH_ARG(A9); AB_PUSH_ARG(A10); AB_PUSH_ARG(A11); 
	return function;
}

template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12>
static FunctionClass Function(std::string name, R(*func)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12))
{
	FunctionClass function(Type<R>::toString(), name, func);
	AB_PUSH_ARG(A1); AB_PUSH_ARG(A2); AB_PUSH_ARG(A3); AB_PUSH_ARG(A4); AB_PUSH_ARG(A5); AB_PUSH_ARG(A6); AB_PUSH_ARG(A7); AB_PUSH_ARG(A8); AB_PUSH_ARG(A9); AB_PUSH_ARG(A10); AB_PUSH_ARG(A11); AB_PUSH_ARG(A12); 
	return function;
}

template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12, typename A13>
static FunctionClass Function(std::string name, R(*func)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13))
{
	FunctionClass function(Type<R>::toString(), name, func);
	AB_PUSH_ARG(A1); AB_PUSH_ARG(A2); AB_PUSH_ARG(A3); AB_PUSH_ARG(A4); AB_PUSH_ARG(A5); AB_PUSH_ARG(A6); AB_PUSH_ARG(A7); AB_PUSH_ARG(A8); AB_PUSH_ARG(A9); AB_PUSH_ARG(A10); AB_PUSH_ARG(A11); AB_PUSH_ARG(A12); AB_PUSH_ARG(A13); 
	return function;
}

template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12, typename A13, typename A14>
static FunctionClass Function(std::string name, R(*func)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14))
{
	FunctionClass function(Type<R>::toString(), name, func);
	AB_PUSH_ARG(A1); AB_PUSH_ARG(A2); AB_PUSH_ARG(A3); AB_PUSH_ARG(A4); AB_PUSH_ARG(A5); AB_PUSH_ARG(A6); AB_PUSH_ARG(A7); AB_PUSH_ARG(A8); AB_PUSH_ARG(A9); AB_PUSH_ARG(A10); AB_PUSH_ARG(A11); AB_PUSH_ARG(A12); AB_PUSH_ARG(A13); AB_PUSH_ARG(A14); 
	return function;
}

AB_END_NAMESPACE