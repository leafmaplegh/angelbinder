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

#define AB_PUSH_ARG(v,x)	v.parameters().push_back(Type<x>::toString());

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
	bool compile(std::string file);

private:
	///
	/// Raised when script machine writes a message
	///
	static void onScriptMessage( const asSMessageInfo *msg, void *param );

};

///
/// VariableExporter parser
///
class VariableClass
{
private:
	/// Type of the variable
	std::string _type;

	/// Name of the variable
	std::string _name;

	/// Address of the variable
	void* _address;

public:
	///
	/// Constructs a variable class
	///
	VariableClass(std::string type, std::string name, void* address);

	///
	/// Returns the name of this variable
	///
	std::string name();

	///
	/// Returns the type of this variable
	///
	std::string type();

	///
	/// Decomposes the exported variable into an equivalent string
	///
	std::string decompose();

	///
	/// Returns the address of the variable
	///
	void* address();

};

///
/// Base exporter
///
class BaseExporter
{
public:
	virtual void finish(Script& script) = 0;
};

///
/// Global variable exporter
///
class VariableExporter : public BaseExporter
{

	/// Friend exporter class
	friend class Exporter;

protected:
	/// Stores all exported entries
	std::queue<VariableClass> _entries;

protected:
	///
	/// Variable exporter 
	///
	VariableExporter();

	///
	/// Called when registering is needed.
	///
	virtual void finish(Script& instance);

public:
	template<typename T>
	VariableExporter& def(std::string name, T* address)
	{
		VariableClass var(Type<T>::toString(), name, address);
		this->_entries.push(var);
		return *this;
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
	///
	/// FunctionClass constructor
	///
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
/// FunctionClass export container
///
class FunctionExporter : public BaseExporter
{

	/// Friend exporter class
	friend class Exporter;

protected:
	/// Stores all exported entries
	std::queue<FunctionClass> _entries;

protected:
	///
	/// Function exporter
	///
	FunctionExporter();

	///
	/// Called when registering is needed.
	///
	virtual void finish(Script& instance);

public:
	///
	/// Templated function exporters
	///

	template<typename R>
	FunctionExporter& def(std::string name, R(__stdcall *func)())
	{
		FunctionClass fc(Type<R>::toString(), name, CallStdcall, func);
		this->_entries.push(fc);
		return *this;
	}

	template<typename R>
	FunctionExporter& def(std::string name, R(__cdecl *func)())
	{
		FunctionClass fc(Type<R>::toString(), name, CallCdecl, func);
		this->_entries.push(fc);
		return *this;
	}

	template<typename R, typename A1>
	FunctionExporter& def(std::string name, R(__stdcall *func)(A1))
	{
		FunctionClass fc(Type<R>::toString(), name, CallStdcall, func);
		AB_PUSH_ARG(fc, A1); 
		this->_entries.push(fc);
		return *this;
	}

	template<typename R, typename A1>
	FunctionExporter& def(std::string name, R(__cdecl *func)(A1))
	{
		FunctionClass fc(Type<R>::toString(), name, CallCdecl, func);
		AB_PUSH_ARG(fc, A1); 
		this->_entries.push(fc);
		return *this;
	}

	template<typename R, typename A1, typename A2>
	FunctionExporter& def(std::string name, R(__stdcall *func)(A1, A2))
	{
		FunctionClass fc(Type<R>::toString(), name, CallStdcall, func);
		AB_PUSH_ARG(fc, A1); AB_PUSH_ARG(fc, A2); 
		this->_entries.push(fc);
		return *this;
	}

	template<typename R, typename A1, typename A2>
	FunctionExporter& def(std::string name, R(__cdecl *func)(A1, A2))
	{
		FunctionClass fc(Type<R>::toString(), name, CallCdecl, func);
		AB_PUSH_ARG(fc, A1); AB_PUSH_ARG(fc, A2); 
		this->_entries.push(fc);
		return *this;
	}

	template<typename R, typename A1, typename A2, typename A3>
	FunctionExporter& def(std::string name, R(__stdcall *func)(A1, A2, A3))
	{
		FunctionClass fc(Type<R>::toString(), name, CallStdcall, func);
		AB_PUSH_ARG(fc, A1); AB_PUSH_ARG(fc, A2); AB_PUSH_ARG(fc, A3); 
		this->_entries.push(fc);
		return *this;
	}

	template<typename R, typename A1, typename A2, typename A3>
	FunctionExporter& def(std::string name, R(__cdecl *func)(A1, A2, A3))
	{
		FunctionClass fc(Type<R>::toString(), name, CallCdecl, func);
		AB_PUSH_ARG(fc, A1); AB_PUSH_ARG(fc, A2); AB_PUSH_ARG(fc, A3); 
		this->_entries.push(fc);
		return *this;
	}

	template<typename R, typename A1, typename A2, typename A3, typename A4>
	FunctionExporter& def(std::string name, R(__stdcall *func)(A1, A2, A3, A4))
	{
		FunctionClass fc(Type<R>::toString(), name, CallStdcall, func);
		AB_PUSH_ARG(fc, A1); AB_PUSH_ARG(fc, A2); AB_PUSH_ARG(fc, A3); AB_PUSH_ARG(fc, A4); 
		this->_entries.push(fc);
		return *this;
	}

	template<typename R, typename A1, typename A2, typename A3, typename A4>
	FunctionExporter& def(std::string name, R(__cdecl *func)(A1, A2, A3, A4))
	{
		FunctionClass fc(Type<R>::toString(), name, CallCdecl, func);
		AB_PUSH_ARG(fc, A1); AB_PUSH_ARG(fc, A2); AB_PUSH_ARG(fc, A3); AB_PUSH_ARG(fc, A4); 
		this->_entries.push(fc);
		return *this;
	}

	template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5>
	FunctionExporter& def(std::string name, R(__stdcall *func)(A1, A2, A3, A4, A5))
	{
		FunctionClass fc(Type<R>::toString(), name, CallStdcall, func);
		AB_PUSH_ARG(fc, A1); AB_PUSH_ARG(fc, A2); AB_PUSH_ARG(fc, A3); AB_PUSH_ARG(fc, A4); AB_PUSH_ARG(fc, A5); 
		this->_entries.push(fc);
		return *this;
	}

	template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5>
	FunctionExporter& def(std::string name, R(__cdecl *func)(A1, A2, A3, A4, A5))
	{
		FunctionClass fc(Type<R>::toString(), name, CallCdecl, func);
		AB_PUSH_ARG(fc, A1); AB_PUSH_ARG(fc, A2); AB_PUSH_ARG(fc, A3); AB_PUSH_ARG(fc, A4); AB_PUSH_ARG(fc, A5); 
		this->_entries.push(fc);
		return *this;
	}

	template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
	FunctionExporter& def(std::string name, R(__stdcall *func)(A1, A2, A3, A4, A5, A6))
	{
		FunctionClass fc(Type<R>::toString(), name, CallStdcall, func);
		AB_PUSH_ARG(fc, A1); AB_PUSH_ARG(fc, A2); AB_PUSH_ARG(fc, A3); AB_PUSH_ARG(fc, A4); AB_PUSH_ARG(fc, A5); AB_PUSH_ARG(fc, A6); 
		this->_entries.push(fc);
		return *this;
	}

	template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
	FunctionExporter& def(std::string name, R(__cdecl *func)(A1, A2, A3, A4, A5, A6))
	{
		FunctionClass fc(Type<R>::toString(), name, CallCdecl, func);
		AB_PUSH_ARG(fc, A1); AB_PUSH_ARG(fc, A2); AB_PUSH_ARG(fc, A3); AB_PUSH_ARG(fc, A4); AB_PUSH_ARG(fc, A5); AB_PUSH_ARG(fc, A6); 
		this->_entries.push(fc);
		return *this;
	}

	template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
	FunctionExporter& def(std::string name, R(__stdcall *func)(A1, A2, A3, A4, A5, A6, A7))
	{
		FunctionClass fc(Type<R>::toString(), name, CallStdcall, func);
		AB_PUSH_ARG(fc, A1); AB_PUSH_ARG(fc, A2); AB_PUSH_ARG(fc, A3); AB_PUSH_ARG(fc, A4); AB_PUSH_ARG(fc, A5); AB_PUSH_ARG(fc, A6); AB_PUSH_ARG(fc, A7); 
		this->_entries.push(fc);
		return *this;
	}

	template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
	FunctionExporter& def(std::string name, R(__cdecl *func)(A1, A2, A3, A4, A5, A6, A7))
	{
		FunctionClass fc(Type<R>::toString(), name, CallCdecl, func);
		AB_PUSH_ARG(fc, A1); AB_PUSH_ARG(fc, A2); AB_PUSH_ARG(fc, A3); AB_PUSH_ARG(fc, A4); AB_PUSH_ARG(fc, A5); AB_PUSH_ARG(fc, A6); AB_PUSH_ARG(fc, A7); 
		this->_entries.push(fc);
		return *this;
	}

	template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
	FunctionExporter& def(std::string name, R(__stdcall *func)(A1, A2, A3, A4, A5, A6, A7, A8))
	{
		FunctionClass fc(Type<R>::toString(), name, CallStdcall, func);
		AB_PUSH_ARG(fc, A1); AB_PUSH_ARG(fc, A2); AB_PUSH_ARG(fc, A3); AB_PUSH_ARG(fc, A4); AB_PUSH_ARG(fc, A5); AB_PUSH_ARG(fc, A6); AB_PUSH_ARG(fc, A7); AB_PUSH_ARG(fc, A8); 
		this->_entries.push(fc);
		return *this;
	}

	template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
	FunctionExporter& def(std::string name, R(__cdecl *func)(A1, A2, A3, A4, A5, A6, A7, A8))
	{
		FunctionClass fc(Type<R>::toString(), name, CallCdecl, func);
		AB_PUSH_ARG(fc, A1); AB_PUSH_ARG(fc, A2); AB_PUSH_ARG(fc, A3); AB_PUSH_ARG(fc, A4); AB_PUSH_ARG(fc, A5); AB_PUSH_ARG(fc, A6); AB_PUSH_ARG(fc, A7); AB_PUSH_ARG(fc, A8); 
		this->_entries.push(fc);
		return *this;
	}

	template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
	FunctionExporter& def(std::string name, R(__stdcall *func)(A1, A2, A3, A4, A5, A6, A7, A8, A9))
	{
		FunctionClass fc(Type<R>::toString(), name, CallStdcall, func);
		AB_PUSH_ARG(fc, A1); AB_PUSH_ARG(fc, A2); AB_PUSH_ARG(fc, A3); AB_PUSH_ARG(fc, A4); AB_PUSH_ARG(fc, A5); AB_PUSH_ARG(fc, A6); AB_PUSH_ARG(fc, A7); AB_PUSH_ARG(fc, A8); AB_PUSH_ARG(fc, A9); 
		this->_entries.push(fc);
		return *this;
	}

	template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
	FunctionExporter& def(std::string name, R(__cdecl *func)(A1, A2, A3, A4, A5, A6, A7, A8, A9))
	{
		FunctionClass fc(Type<R>::toString(), name, CallCdecl, func);
		AB_PUSH_ARG(fc, A1); AB_PUSH_ARG(fc, A2); AB_PUSH_ARG(fc, A3); AB_PUSH_ARG(fc, A4); AB_PUSH_ARG(fc, A5); AB_PUSH_ARG(fc, A6); AB_PUSH_ARG(fc, A7); AB_PUSH_ARG(fc, A8); AB_PUSH_ARG(fc, A9); 
		this->_entries.push(fc);
		return *this;
	}

	template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10>
	FunctionExporter& def(std::string name, R(__stdcall *func)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10))
	{
		FunctionClass fc(Type<R>::toString(), name, CallStdcall, func);
		AB_PUSH_ARG(fc, A1); AB_PUSH_ARG(fc, A2); AB_PUSH_ARG(fc, A3); AB_PUSH_ARG(fc, A4); AB_PUSH_ARG(fc, A5); AB_PUSH_ARG(fc, A6); AB_PUSH_ARG(fc, A7); AB_PUSH_ARG(fc, A8); AB_PUSH_ARG(fc, A9); AB_PUSH_ARG(fc, A10); 
		this->_entries.push(fc);
		return *this;
	}

	template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10>
	FunctionExporter& def(std::string name, R(__cdecl *func)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10))
	{
		FunctionClass fc(Type<R>::toString(), name, CallCdecl, func);
		AB_PUSH_ARG(fc, A1); AB_PUSH_ARG(fc, A2); AB_PUSH_ARG(fc, A3); AB_PUSH_ARG(fc, A4); AB_PUSH_ARG(fc, A5); AB_PUSH_ARG(fc, A6); AB_PUSH_ARG(fc, A7); AB_PUSH_ARG(fc, A8); AB_PUSH_ARG(fc, A9); AB_PUSH_ARG(fc, A10); 
		this->_entries.push(fc);
		return *this;
	}

	template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11>
	FunctionExporter& def(std::string name, R(__stdcall *func)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11))
	{
		FunctionClass fc(Type<R>::toString(), name, CallStdcall, func);
		AB_PUSH_ARG(fc, A1); AB_PUSH_ARG(fc, A2); AB_PUSH_ARG(fc, A3); AB_PUSH_ARG(fc, A4); AB_PUSH_ARG(fc, A5); AB_PUSH_ARG(fc, A6); AB_PUSH_ARG(fc, A7); AB_PUSH_ARG(fc, A8); AB_PUSH_ARG(fc, A9); AB_PUSH_ARG(fc, A10); AB_PUSH_ARG(fc, A11); 
		this->_entries.push(fc);
		return *this;
	}

	template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11>
	FunctionExporter& def(std::string name, R(__cdecl *func)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11))
	{
		FunctionClass fc(Type<R>::toString(), name, CallCdecl, func);
		AB_PUSH_ARG(fc, A1); AB_PUSH_ARG(fc, A2); AB_PUSH_ARG(fc, A3); AB_PUSH_ARG(fc, A4); AB_PUSH_ARG(fc, A5); AB_PUSH_ARG(fc, A6); AB_PUSH_ARG(fc, A7); AB_PUSH_ARG(fc, A8); AB_PUSH_ARG(fc, A9); AB_PUSH_ARG(fc, A10); AB_PUSH_ARG(fc, A11); 
		this->_entries.push(fc);
		return *this;
	}

	template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12>
	FunctionExporter& def(std::string name, R(__stdcall *func)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12))
	{
		FunctionClass fc(Type<R>::toString(), name, CallStdcall, func);
		AB_PUSH_ARG(fc, A1); AB_PUSH_ARG(fc, A2); AB_PUSH_ARG(fc, A3); AB_PUSH_ARG(fc, A4); AB_PUSH_ARG(fc, A5); AB_PUSH_ARG(fc, A6); AB_PUSH_ARG(fc, A7); AB_PUSH_ARG(fc, A8); AB_PUSH_ARG(fc, A9); AB_PUSH_ARG(fc, A10); AB_PUSH_ARG(fc, A11); AB_PUSH_ARG(fc, A12); 
		this->_entries.push(fc);
		return *this;
	}

	template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12>
	FunctionExporter& def(std::string name, R(__cdecl *func)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12))
	{
		FunctionClass fc(Type<R>::toString(), name, CallCdecl, func);
		AB_PUSH_ARG(fc, A1); AB_PUSH_ARG(fc, A2); AB_PUSH_ARG(fc, A3); AB_PUSH_ARG(fc, A4); AB_PUSH_ARG(fc, A5); AB_PUSH_ARG(fc, A6); AB_PUSH_ARG(fc, A7); AB_PUSH_ARG(fc, A8); AB_PUSH_ARG(fc, A9); AB_PUSH_ARG(fc, A10); AB_PUSH_ARG(fc, A11); AB_PUSH_ARG(fc, A12); 
		this->_entries.push(fc);
		return *this;
	}

	template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12, typename A13>
	FunctionExporter& def(std::string name, R(__stdcall *func)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13))
	{
		FunctionClass fc(Type<R>::toString(), name, CallStdcall, func);
		AB_PUSH_ARG(fc, A1); AB_PUSH_ARG(fc, A2); AB_PUSH_ARG(fc, A3); AB_PUSH_ARG(fc, A4); AB_PUSH_ARG(fc, A5); AB_PUSH_ARG(fc, A6); AB_PUSH_ARG(fc, A7); AB_PUSH_ARG(fc, A8); AB_PUSH_ARG(fc, A9); AB_PUSH_ARG(fc, A10); AB_PUSH_ARG(fc, A11); AB_PUSH_ARG(fc, A12); AB_PUSH_ARG(fc, A13); 
		this->_entries.push(fc);
		return *this;
	}

	template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12, typename A13>
	FunctionExporter& def(std::string name, R(__cdecl *func)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13))
	{
		FunctionClass fc(Type<R>::toString(), name, CallCdecl, func);
		AB_PUSH_ARG(fc, A1); AB_PUSH_ARG(fc, A2); AB_PUSH_ARG(fc, A3); AB_PUSH_ARG(fc, A4); AB_PUSH_ARG(fc, A5); AB_PUSH_ARG(fc, A6); AB_PUSH_ARG(fc, A7); AB_PUSH_ARG(fc, A8); AB_PUSH_ARG(fc, A9); AB_PUSH_ARG(fc, A10); AB_PUSH_ARG(fc, A11); AB_PUSH_ARG(fc, A12); AB_PUSH_ARG(fc, A13); 
		this->_entries.push(fc);
		return *this;
	}

	template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12, typename A13, typename A14>
	FunctionExporter& def(std::string name, R(__stdcall *func)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14))
	{
		FunctionClass fc(Type<R>::toString(), name, CallStdcall, func);
		AB_PUSH_ARG(fc, A1); AB_PUSH_ARG(fc, A2); AB_PUSH_ARG(fc, A3); AB_PUSH_ARG(fc, A4); AB_PUSH_ARG(fc, A5); AB_PUSH_ARG(fc, A6); AB_PUSH_ARG(fc, A7); AB_PUSH_ARG(fc, A8); AB_PUSH_ARG(fc, A9); AB_PUSH_ARG(fc, A10); AB_PUSH_ARG(fc, A11); AB_PUSH_ARG(fc, A12); AB_PUSH_ARG(fc, A13); AB_PUSH_ARG(fc, A14); 
		this->_entries.push(fc);
		return *this;
	}

	template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12, typename A13, typename A14>
	FunctionExporter& def(std::string name, R(__cdecl *func)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14))
	{
		FunctionClass fc(Type<R>::toString(), name, CallCdecl, func);
		AB_PUSH_ARG(fc, A1); AB_PUSH_ARG(fc, A2); AB_PUSH_ARG(fc, A3); AB_PUSH_ARG(fc, A4); AB_PUSH_ARG(fc, A5); AB_PUSH_ARG(fc, A6); AB_PUSH_ARG(fc, A7); AB_PUSH_ARG(fc, A8); AB_PUSH_ARG(fc, A9); AB_PUSH_ARG(fc, A10); AB_PUSH_ARG(fc, A11); AB_PUSH_ARG(fc, A12); AB_PUSH_ARG(fc, A13); AB_PUSH_ARG(fc, A14); 
		this->_entries.push(fc);
		return *this;
	}

};

///
/// Dummy constructors / destructors
///
void DummyConstructor(void* memory);
void DummyDestructor(void* memory);

///
/// Class destructor wrapper
///

template<typename T>
void DestructorWrapper(void* memory)
{
	((T*)memory)->~T();
}

///
/// Class constructor helper
///

template<typename T>
void ConstructorWrapper(void* memory)
{
	new (memory) T();
}

template<typename T, typename A1>
void ConstructorWrapper(A1 a1, void* memory)
{
	new (memory) T(a1);
}

template<typename T, typename A1, typename A2>
void ConstructorWrapper(A1 a1, A2 a2, void* memory)
{
	new (memory) T(a1, a2);
}

template<typename T, typename A1, typename A2, typename A3>
void ConstructorWrapper(A1 a1, A2 a2, A3 a3, void* memory)
{
	new (memory) T(a1, a2, a3);
}

template<typename T, typename A1, typename A2, typename A3, typename A4>
void ConstructorWrapper(A1 a1, A2 a2, A3 a3, A4 a4, void* memory)
{
	new (memory) T(a1, a2, a3, a4);
}

template<typename T, typename A1, typename A2, typename A3, typename A4, typename A5>
void ConstructorWrapper(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, void* memory)
{
	new (memory) T(a1, a2, a3, a4, a5);
}

template<typename T, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
void ConstructorWrapper(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, void* memory)
{
	new (memory) T(a1, a2, a3, a4, a5, a6);
}

template<typename T, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
void ConstructorWrapper(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, void* memory)
{
	new (memory) T(a1, a2, a3, a4, a5, a6, a7);
}

template<typename T, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
void ConstructorWrapper(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, void* memory)
{
	new (memory) T(a1, a2, a3, a4, a5, a6, a7, a8);
}

template<typename T, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
void ConstructorWrapper(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, void* memory)
{
	new (memory) T(a1, a2, a3, a4, a5, a6, a7, a8, a9);
}

template<typename T, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10>
void ConstructorWrapper(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10, void* memory)
{
	new (memory) T(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10);
}

template<typename T, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11>
void ConstructorWrapper(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10, A11 a11, void* memory)
{
	new (memory) T(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11);
}

template<typename T, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12>
void ConstructorWrapper(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10, A11 a11, A12 a12, void* memory)
{
	new (memory) T(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12);
}

template<typename T, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12, typename A13>
void ConstructorWrapper(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10, A11 a11, A12 a12, A13 a13, void* memory)
{
	new (memory) T(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13);
}

template<typename T, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12, typename A13, typename A14>
void ConstructorWrapper(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10, A11 a11, A12 a12, A13 a13, A14 a14, void* memory)
{
	new (memory) T(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14);
}

///
/// Class constructors
///
class ConstructorClass
{
private:
	/// Stores a vector of arguments
	std::vector<std::string> _params;

	/// Stores the pointer to the function
	void* _func;

public:
	///
	/// FunctionClass constructor
	///
	ConstructorClass(void* func)
		: _func(func)
	{
	}

	///
	/// Returns the parameters
	///
	std::vector<std::string>& parameters()
	{
		return this->_params;
	}

	///
	/// Returns the pointer to the function
	///
	void* address()
	{
		return this->_func;
	}

	///
	/// Decomposes the constructor into an equivalent string
	///
	std::string decompose()
	{
		std::stringstream stream;
		stream << "void f(";
		for(size_t i = 0; i < this->_params.size(); i++) 
		{
			stream << this->_params[i];
			if(i != this->_params.size() - 1) 
			{
				stream << ", ";
			}
		}
		stream << ")";
		return stream.str();
	}

};

///
/// Class exporter
///
template<typename T>
class ClassExporter
	: public BaseExporter
{

	/// Friend exporter class
	friend class Exporter;

protected:
	/// Stores all exported constructors
	std::queue<ConstructorClass> _ctors;

	/// Stores all assignment operators
	// std::queue<AssignClass> _assigns;

	/// Stores all copy constructors
	// void* _copy;

	/// Stores the destructor pointer
	void* _dtor;

	/// Stores the dummy constructor pointer
	void* _ctor;

	/// Initial flags
	int _flags;

protected:
	///
	/// Function exporter
	///
	ClassExporter( int flags ) 
		: _flags(flags | asOBJ_VALUE | asOBJ_APP_CLASS), _dtor(NULL), _ctor(NULL)
	{
	}

	///
	/// Called when registering is needed.
	///
	virtual void finish(Script& instance)
	{

		int flags = this->_flags;
		flags |= this->_ctors.empty() && this->_ctor == NULL ? 0 : asOBJ_APP_CLASS_CONSTRUCTOR;
		flags |= this->_dtor == NULL ? 0 : asOBJ_APP_CLASS_DESTRUCTOR;
		// flags |= this->_assigns.empty() ? 0 : asOBJ_APP_CLASS_ASSIGNMENT;
		// flags |= this->_copy == NULL ? 0 : asOBJ_APP_CLASS_COPY_CONSTRUCTOR;

		std::string name = Type<T>::toString();
		AB_MESSAGE_INVOKE_STATIC(&instance, &instance, "Registering type '" + name + "'");
		int r = instance.engine().RegisterObjectType(Type<T>::toString().c_str(), sizeof(T), flags);
		AB_SCRIPT_ASSERT_STATIC(r >= 0, std::string("Could not register type '" + name + "'").c_str(), AB_THROW, &instance);

		if(this->_ctor != NULL && !this->_ctors.empty())
		{
			AB_SCRIPT_ASSERT_STATIC(false, std::string("Can't register dummy constructor if there's other constructors.").c_str(), AB_THROW, &instance);
		}

		while(!this->_ctors.empty())
		{
			ConstructorClass memb = this->_ctors.front();
			std::string decomp = memb.decompose();
			AB_MESSAGE_INVOKE_STATIC(&instance, &instance, "Registering constructor for '" + name + "' as '" + decomp + "'");
			r = instance.engine().RegisterObjectBehaviour(name.c_str(), asBEHAVE_CONSTRUCT, decomp.c_str(), asFUNCTION(memb.address()), asCALL_CDECL_OBJLAST);
			AB_SCRIPT_ASSERT_STATIC(r >= 0, std::string("Error registering constructor for type '" + name + "'").c_str(), AB_THROW, &instance);
			this->_ctors.pop();
		}

		if(this->_ctor != NULL)
		{
			AB_MESSAGE_INVOKE_STATIC(&instance, &instance, "Registering dummy constructor for '" + name + "' as 'void f()'");
			int r = instance.engine().RegisterObjectBehaviour(name.c_str(), asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(this->_ctor), asCALL_CDECL_OBJLAST);
			AB_SCRIPT_ASSERT_STATIC(r >= 0, std::string("Can't register constructor for type '" + name + "'").c_str(), AB_THROW, &instance);
		}

		if(this->_dtor != NULL)
		{
			AB_MESSAGE_INVOKE_STATIC(&instance, &instance, "Registering destructor for '" + name + "' as 'void f()'");
			r = instance.engine().RegisterObjectBehaviour(name.c_str(), asBEHAVE_DESTRUCT, "void f()", asFUNCTION(this->_dtor), asCALL_CDECL_OBJLAST);
			AB_SCRIPT_ASSERT_STATIC(r >= 0, std::string("Can't register destructor for type '" + name + "'").c_str(), AB_THROW, &instance);
		}

	}

public:
	///
	/// Constructors
	///
	ClassExporter& ctor()
	{
		ConstructorClass cc(static_cast<void(*)(void*)>(&ConstructorWrapper<T>));
		this->_ctors.push(cc);
		return *this;
	}

	template<typename A1>
	ClassExporter& ctor()
	{
		ConstructorClass cc(static_cast<void(*)(A1, void*)>(&ConstructorWrapper<T, A1>));
		AB_PUSH_ARG(cc, A1);         
		this->_ctors.push(cc);
		return *this;
	}

	template<typename A1, typename A2>
	ClassExporter& ctor()
	{
		ConstructorClass cc(static_cast<void(*)(A1, A2, void*)>(&ConstructorWrapper<T, A1, A2>));
		AB_PUSH_ARG(cc, A1); AB_PUSH_ARG(cc, A2);         
		this->_ctors.push(cc);
		return *this;
	}

	template<typename A1, typename A2, typename A3>
	ClassExporter& ctor()
	{
		ConstructorClass cc(static_cast<void(*)(A1, A2, A3, void*)>(&ConstructorWrapper<T, A1, A2, A3>));
		AB_PUSH_ARG(cc, A1); AB_PUSH_ARG(cc, A2); AB_PUSH_ARG(cc, A3);         
		this->_ctors.push(cc);
		return *this;
	}

	template<typename A1, typename A2, typename A3, typename A4>
	ClassExporter& ctor()
	{
		ConstructorClass cc(static_cast<void(*)(A1, A2, A3, A4, void*)>(&ConstructorWrapper<T, A1, A2, A3, A4>));
		AB_PUSH_ARG(cc, A1); AB_PUSH_ARG(cc, A2); AB_PUSH_ARG(cc, A3); AB_PUSH_ARG(cc, A4);         
		this->_ctors.push(cc);
		return *this;
	}

	template<typename A1, typename A2, typename A3, typename A4, typename A5>
	ClassExporter& ctor()
	{
		ConstructorClass cc(static_cast<void(*)(A1, A2, A3, A4, A5, void*)>(&ConstructorWrapper<T, A1, A2, A3, A4, A5>));
		AB_PUSH_ARG(cc, A1); AB_PUSH_ARG(cc, A2); AB_PUSH_ARG(cc, A3); AB_PUSH_ARG(cc, A4); AB_PUSH_ARG(cc, A5);         
		this->_ctors.push(cc);
		return *this;
	}

	template<typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
	ClassExporter& ctor()
	{
		ConstructorClass cc(static_cast<void(*)(A1, A2, A3, A4, A5, A6, void*)>(&ConstructorWrapper<T, A1, A2, A3, A4, A5, A6>));
		AB_PUSH_ARG(cc, A1); AB_PUSH_ARG(cc, A2); AB_PUSH_ARG(cc, A3); AB_PUSH_ARG(cc, A4); AB_PUSH_ARG(cc, A5); AB_PUSH_ARG(cc, A6);         
		this->_ctors.push(cc);
		return *this;
	}

	template<typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
	ClassExporter& ctor()
	{
		ConstructorClass cc(static_cast<void(*)(A1, A2, A3, A4, A5, A6, A7, void*)>(&ConstructorWrapper<T, A1, A2, A3, A4, A5, A6, A7>));
		AB_PUSH_ARG(cc, A1); AB_PUSH_ARG(cc, A2); AB_PUSH_ARG(cc, A3); AB_PUSH_ARG(cc, A4); AB_PUSH_ARG(cc, A5); AB_PUSH_ARG(cc, A6); AB_PUSH_ARG(cc, A7);         
		this->_ctors.push(cc);
		return *this;
	}

	template<typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
	ClassExporter& ctor()
	{
		ConstructorClass cc(static_cast<void(*)(A1, A2, A3, A4, A5, A6, A7, A8, void*)>(&ConstructorWrapper<T, A1, A2, A3, A4, A5, A6, A7, A8>));
		AB_PUSH_ARG(cc, A1); AB_PUSH_ARG(cc, A2); AB_PUSH_ARG(cc, A3); AB_PUSH_ARG(cc, A4); AB_PUSH_ARG(cc, A5); AB_PUSH_ARG(cc, A6); AB_PUSH_ARG(cc, A7); AB_PUSH_ARG(cc, A8);         
		this->_ctors.push(cc);
		return *this;
	}

	template<typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
	ClassExporter& ctor()
	{
		ConstructorClass cc(static_cast<void(*)(A1, A2, A3, A4, A5, A6, A7, A8, A9, void*)>(&ConstructorWrapper<T, A1, A2, A3, A4, A5, A6, A7, A8, A9>));
		AB_PUSH_ARG(cc, A1); AB_PUSH_ARG(cc, A2); AB_PUSH_ARG(cc, A3); AB_PUSH_ARG(cc, A4); AB_PUSH_ARG(cc, A5); AB_PUSH_ARG(cc, A6); AB_PUSH_ARG(cc, A7); AB_PUSH_ARG(cc, A8); AB_PUSH_ARG(cc, A9);         
		this->_ctors.push(cc);
		return *this;
	}

	template<typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10>
	ClassExporter& ctor()
	{
		ConstructorClass cc(static_cast<void(*)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, void*)>(&ConstructorWrapper<T, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>));
		AB_PUSH_ARG(cc, A1); AB_PUSH_ARG(cc, A2); AB_PUSH_ARG(cc, A3); AB_PUSH_ARG(cc, A4); AB_PUSH_ARG(cc, A5); AB_PUSH_ARG(cc, A6); AB_PUSH_ARG(cc, A7); AB_PUSH_ARG(cc, A8); AB_PUSH_ARG(cc, A9); AB_PUSH_ARG(cc, A10);         
		this->_ctors.push(cc);
		return *this;
	}

	template<typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11>
	ClassExporter& ctor()
	{
		ConstructorClass cc(static_cast<void(*)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, void*)>(&ConstructorWrapper<T, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11>));
		AB_PUSH_ARG(cc, A1); AB_PUSH_ARG(cc, A2); AB_PUSH_ARG(cc, A3); AB_PUSH_ARG(cc, A4); AB_PUSH_ARG(cc, A5); AB_PUSH_ARG(cc, A6); AB_PUSH_ARG(cc, A7); AB_PUSH_ARG(cc, A8); AB_PUSH_ARG(cc, A9); AB_PUSH_ARG(cc, A10); AB_PUSH_ARG(cc, A11);         
		this->_ctors.push(cc);
		return *this;
	}

	template<typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12>
	ClassExporter& ctor()
	{
		ConstructorClass cc(static_cast<void(*)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, void*)>(&ConstructorWrapper<T, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12>));
		AB_PUSH_ARG(cc, A1); AB_PUSH_ARG(cc, A2); AB_PUSH_ARG(cc, A3); AB_PUSH_ARG(cc, A4); AB_PUSH_ARG(cc, A5); AB_PUSH_ARG(cc, A6); AB_PUSH_ARG(cc, A7); AB_PUSH_ARG(cc, A8); AB_PUSH_ARG(cc, A9); AB_PUSH_ARG(cc, A10); AB_PUSH_ARG(cc, A11); AB_PUSH_ARG(cc, A12);         
		this->_ctors.push(cc);
		return *this;
	}

	template<typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12, typename A13>
	ClassExporter& ctor()
	{
		ConstructorClass cc(static_cast<void(*)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, void*)>(&ConstructorWrapper<T, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13>));
		AB_PUSH_ARG(cc, A1); AB_PUSH_ARG(cc, A2); AB_PUSH_ARG(cc, A3); AB_PUSH_ARG(cc, A4); AB_PUSH_ARG(cc, A5); AB_PUSH_ARG(cc, A6); AB_PUSH_ARG(cc, A7); AB_PUSH_ARG(cc, A8); AB_PUSH_ARG(cc, A9); AB_PUSH_ARG(cc, A10); AB_PUSH_ARG(cc, A11); AB_PUSH_ARG(cc, A12); AB_PUSH_ARG(cc, A13);         
		this->_ctors.push(cc);
		return *this;
	}

	template<typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12, typename A13, typename A14>
	ClassExporter& ctor()
	{
		ConstructorClass cc(static_cast<void(*)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, void*)>(&ConstructorWrapper<T, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14>));
		AB_PUSH_ARG(cc, A1); AB_PUSH_ARG(cc, A2); AB_PUSH_ARG(cc, A3); AB_PUSH_ARG(cc, A4); AB_PUSH_ARG(cc, A5); AB_PUSH_ARG(cc, A6); AB_PUSH_ARG(cc, A7); AB_PUSH_ARG(cc, A8); AB_PUSH_ARG(cc, A9); AB_PUSH_ARG(cc, A10); AB_PUSH_ARG(cc, A11); AB_PUSH_ARG(cc, A12); AB_PUSH_ARG(cc, A13); AB_PUSH_ARG(cc, A14);         
		this->_ctors.push(cc);
		return *this;
	}

	///
	/// Destructor
	///
	ClassExporter& dtor()
	{
		this->_dtor = &DestructorWrapper<T>;
		return *this;
	}

	///
	/// Dummy destructor
	///
	ClassExporter& dtor_dummy()
	{
		this->_dtor = &DummyDestructor;
		return *this;
	}

	///
	/// Dummy constructor
	///
	ClassExporter& ctor_dummy()
	{
		this->_ctor = &DummyConstructor;
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
	/// VariableExporter wrapper
	///
	static VariableExporter Variables();

	///
	/// ClassExporter wrapper
	///
	template<typename T>
	static ClassExporter<T> Class(int flags = 0)
	{
		ClassExporter<T> exporter(flags);
		return exporter;
	};

	///
	/// Exports the functions
	///
	template<typename T>
	void operator[] (T& exp)
	{
		BaseExporter* exporter = dynamic_cast<BaseExporter*>(&exp);
		if(exporter != NULL)
		{
			exporter->finish(this->_script);
		}
	}

};

AB_END_NAMESPACE
