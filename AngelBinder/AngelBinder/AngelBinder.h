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
#include <functional>
#include <angelscript.h>
#include <scriptbuilder.h>

/// 
/// Define the binder to not use namespaces
/// #define AB_NO_NAMESPACE
/// 
/// Define the binder to use libobs library (http://code.google.com/p/libobs/)
/// #define AB_USE_EVENTSYSTEM
/// 

#ifdef AS_USE_NAMESPACE
#define AB_BEGIN_NAMESPACE		namespace AngelBinder {
#define AB_END_NAMESPACE		}
#define AB_NAMESPACE_QUALIFIER	AngelBinder::
#else
#define AB_BEGIN_NAMESPACE
#define AB_END_NAMESPACE
#define AB_NAMESPACE_QUALIFIER
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

#define AB_MESSAGE_INVOKE(i,m)				if(this->onMessage() != NULL) this->onMessage()(i,m);
#define AB_MESSAGE_INVOKE_STATIC(s,i,m)		if((s)->onMessage() != NULL) (s)->onMessage()(i,m);

#define AB_SCRIPT_ASSERT(x,m,t,i) { \
	if(!(x)) { \
		if((t)) { \
			throw std::exception(m); \
		} else { \
			AB_MESSAGE_INVOKE_STATIC(##i, ##i, ##m); \
		} \
	} \
}

#define AB_PUSH_ARG(v,x)		v.parameters().push_back(Type<x>::toString());

#define AB_FUNCTION(f)			AS_NAMESPACE_QUALIFIER asFUNCTION(##f)
#define AB_FUNCTIONPR(f,p,r)	AS_NAMESPACE_QUALIFIER asFUNCTIONPR(##f, ##p, ##r)
#define AB_METHOD(c,p,r,ptr)	AS_NAMESPACE_QUALIFIER asSMethodPtr<sizeof(void (c::*)())>::Convert(AS_METHOD_AMBIGUITY_CAST(r (c::*)p)(ptr))

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
	struct AB_NAMESPACE_QUALIFIER TypeString<t> \
	{ \
		static std::string value() \
		{ \
			return n; \
		} \
	}; 

///
/// Macro to help declaring parameter setters.
///
#define AB_PARAMETER_SETTER(t,f) \
	template<> \
	struct ParameterSetter<t> \
	{ \
		void operator()(Context* context, t value) \
		{ \
			context->f(value); \
		} \
	}; 

///
/// Macro to help declaring return value readers.
///
#define AB_RETURN_READER(t,f) \
	template<> \
	struct ReturnReader<t> \
	{ \
		t operator()(Context* context) \
		{ \
			return (t)context->f(); \
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
		return TypeString<T>::value() + "& in";
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
/// AngelScript types
///
typedef	AS_NAMESPACE_QUALIFIER asIScriptEngine	ASEngine;
typedef AS_NAMESPACE_QUALIFIER asIScriptContext ASContext;
typedef AS_NAMESPACE_QUALIFIER asIScriptModule	ASModule;
typedef AS_NAMESPACE_QUALIFIER CScriptBuilder	ASBuilder;

/// Empty declaration to use as pointer into the engine
class Module;

///
/// Script engine
///
class Engine
{
public:
	///
	/// Script engine's message callback
	///
	typedef void(*MessageCallback)(Engine*, std::string);

private:
	///
	/// Script Engine Instance
	///
	ASEngine* _engine;

	///
	/// Script Engine Builder
	///
	ASBuilder* _builder;

	///
	/// Message callbacks
	///
	MessageCallback _messages;

	///
	/// FunctionClass caching
	///
	std::hash_map<std::string, int> _functions;

	///
	/// Modules
	///
	std::hash_map<std::string, Module*> _modules;

	///
	/// Raised when script machine writes a message
	///
	static void __cdecl onScriptMessage( const AS_NAMESPACE_QUALIFIER asSMessageInfo *msg, void *param );

	///
	/// Startup stuff
	///
	void initialize();
	void uninitialize();

public:
	///
	/// Constructors / destructors
	///
	Engine();
	Engine(MessageCallback callback);
	~Engine();

	///
	/// Message callbacks
	///
	MessageCallback& onMessage();

	///
	/// AngelScript stuff
	///
	ASEngine* asEngine();
	ASBuilder* asBuilder();

	///
	/// Creates a new module, or return an existent one.
	///
	Module* createModule(std::string name);

	///
	/// Creates a new module, or return an existent one.
	///
	Module* getModule(std::string name);

};

///
/// Script Function
///
class Context
{
	/// Friend function class 
	template<typename T>
	friend class Function;

private:
	/// Selected function to execute
	int _function;

	/// Count of parameters set to this context
	int _params;

	/// AngelScript's context
	ASContext* _context;

	/// Engine instance
	Engine& _engine;

protected:
	///
	/// Creates a new context
	///
	Context(Engine& engine, int function);

	///
	/// Releases the context
	///
	~Context();

	///
	/// Script exception callback
	///
	void exceptionCallback(asIScriptContext *context);

public:
	///
	/// Executes the context
	///
	void execute();

	///
	/// Parameter setters
	///
	void setAddress(void* value);
	void setObject(void* value);
	void setByte(asBYTE value);
	void setWord(asWORD value);
	void setDWord(unsigned int value);
	void setQWord(unsigned long long value);
	void setFloat(float value);
	void setDouble(double value);

	///
	/// Return readers
	///
	void* readAddress();
	void* readObject();
	asBYTE readByte();
	asWORD readWord();
	asDWORD readDWord();
	asQWORD readQWord();
	float readFloat();
	double readDouble();

};

///
/// Parameter setters
///
template<typename T>
struct ParameterSetter
{
	void operator()(Context* context, T& value)
	{
		context->setObject((void*)&value);
	}
};

template<typename T>
struct ParameterSetter<T*>
{
	void operator()(Context* context, T* value)
	{
		ParameterSetter<T>()(context, *value);
	}
};

template<typename T>
struct ParameterSetter<const T>
{
	void operator()(Context* context, const T value)
	{
		ParameterSetter<T>()(context, (T&)(value));
	}
};

template<typename T>
struct ParameterSetter<const T*>
{
	void operator()(Context* context, const T* value)
	{
		ParameterSetter<T>()(context, (T&)(*value));
	}
};

template<typename T>
struct ParameterSetter<T&>
{
	void operator()(Context* context, T& value)
	{
		ParameterSetter<T>()(context, value);
	}
};

template<typename T>
struct ParameterSetter<const T&>
{
	void operator()(Context* context, const T& value)
	{
		ParameterSetter<T>()(context, value);
	}
};

///
/// Special parameter types.
///
AB_PARAMETER_SETTER(char, setByte);
AB_PARAMETER_SETTER(unsigned char, setByte);
AB_PARAMETER_SETTER(short, setWord);
AB_PARAMETER_SETTER(unsigned short, setWord);
AB_PARAMETER_SETTER(int, setDWord);
AB_PARAMETER_SETTER(unsigned int, setDWord);
AB_PARAMETER_SETTER(long long, setQWord);
AB_PARAMETER_SETTER(unsigned long long, setQWord);
AB_PARAMETER_SETTER(float, setFloat);
AB_PARAMETER_SETTER(double, setDouble);

///
/// Parameter setters
///
template<typename T>
struct ReturnReader
{
	T operator()(Context* context)
	{
		return *static_cast<T*>(context->readObject());
	}
};

template<typename T>
struct ReturnReader<T*>
{
	T* operator()(Context* context)
	{
		return static_cast<T*>(context->readObject());
	}
};

template<typename T>
struct ReturnReader<const T>
{
	const T operator()(Context* context)
	{
		return ReturnReader<T>()(context);
	}
};

template<typename T>
struct ReturnReader<const T*>
{
	const T* operator()(Context* context)
	{
		return ReturnReader<T*>()(context);
	}
};

template<typename T>
struct ReturnReader<T&>
{
	T& operator()(Context* context)
	{
		return ReturnReader<T>()(context);
	}
};

template<typename T>
struct ReturnReader<const T&>
{
	const T& operator()(Context* context)
	{
		return ReturnReader<T>()(context);
	}
};

///
/// Special return types.
///
AB_RETURN_READER(char, readByte);
AB_RETURN_READER(unsigned char, readByte);
AB_RETURN_READER(short, readWord);
AB_RETURN_READER(unsigned short, readWord);
AB_RETURN_READER(int, readDWord);
AB_RETURN_READER(unsigned int, readDWord);
AB_RETURN_READER(long long, readQWord);
AB_RETURN_READER(unsigned long long, readQWord);
AB_RETURN_READER(float, readFloat);
AB_RETURN_READER(double, readDouble);

#define AB_FUNCTION_CONSTRUCTOR() \
		friend class Module; \
	private: \
		int _function; \
		Engine& _engine; \
	protected: \
		Function(Engine& engine, int function) \
			: _engine(engine), _function(function) \
		{ \
		}

///
/// Dummy function class
///
template<typename F>
class Function 
{
	friend class Module;
	static std::string decompose(std::string name)
	{
		return "";
	}
};

///
/// Function definitions
///


template<>
class Function<void()>
{    
	AB_FUNCTION_CONSTRUCTOR();
public:
	void operator()()
	{
		Context ctx(this->_engine, this->_function);

		ctx.execute();
	}
	static std::string decompose(std::string name)
	{
		std::stringstream stream;
		stream << "void " << name << "()";
		return stream.str();
	}
};

template<typename R>
class Function<R()>
{    
	AB_FUNCTION_CONSTRUCTOR();
public:
	R operator()()
	{
		Context ctx(this->_engine, this->_function);

		ctx.execute();
		return ReturnReader<R>()(&ctx);
	}
	static std::string decompose(std::string name)
	{
		std::stringstream stream;
		stream << Type<R>::toString() << " " << name << "()";
		return stream.str();
	}
};

template<typename A1>
class Function<void(A1)>
{    
	AB_FUNCTION_CONSTRUCTOR();
public:
	void operator()(A1 a1)
	{
		Context ctx(this->_engine, this->_function);
		ParameterSetter<A1>()(&ctx, a1); 
		ctx.execute();
	}
	static std::string decompose(std::string name)
	{
		std::stringstream stream;
		stream << "void " << name << "(" << Type<A1>::toString() << ")";
		return stream.str();
	}
};

template<typename R, typename A1>
class Function<R(A1)>
{    
	AB_FUNCTION_CONSTRUCTOR();
public:
	R operator()(A1 a1)
	{
		Context ctx(this->_engine, this->_function);
		ParameterSetter<A1>()(&ctx, a1); 
		ctx.execute();
		return ReturnReader<R>()(&ctx);
	}
	static std::string decompose(std::string name)
	{
		std::stringstream stream;
		stream << Type<R>::toString() << " " << name << "(" << Type<A1>::toString() << ")";
		return stream.str();
	}
};

template<typename A1, typename A2>
class Function<void(A1, A2)>
{    
	AB_FUNCTION_CONSTRUCTOR();
public:
	void operator()(A1 a1, A2 a2)
	{
		Context ctx(this->_engine, this->_function);
		ParameterSetter<A1>()(&ctx, a1); ParameterSetter<A2>()(&ctx, a2); 
		ctx.execute();
	}
	static std::string decompose(std::string name)
	{
		std::stringstream stream;
		stream << "void " << name << "(" << Type<A1>::toString() << ", " << Type<A2>::toString() << ")";
		return stream.str();
	}
};

template<typename R, typename A1, typename A2>
class Function<R(A1, A2)>
{    
	AB_FUNCTION_CONSTRUCTOR();
public:
	R operator()(A1 a1, A2 a2)
	{
		Context ctx(this->_engine, this->_function);
		ParameterSetter<A1>()(&ctx, a1); ParameterSetter<A2>()(&ctx, a2); 
		ctx.execute();
		return ReturnReader<R>()(&ctx);
	}
	static std::string decompose(std::string name)
	{
		std::stringstream stream;
		stream << Type<R>::toString() << " " << name << "(" << Type<A1>::toString() << ", " << Type<A2>::toString() << ")";
		return stream.str();
	}
};

template<typename A1, typename A2, typename A3>
class Function<void(A1, A2, A3)>
{    
	AB_FUNCTION_CONSTRUCTOR();
public:
	void operator()(A1 a1, A2 a2, A3 a3)
	{
		Context ctx(this->_engine, this->_function);
		ParameterSetter<A1>()(&ctx, a1); ParameterSetter<A2>()(&ctx, a2); ParameterSetter<A3>()(&ctx, a3); 
		ctx.execute();
	}
	static std::string decompose(std::string name)
	{
		std::stringstream stream;
		stream << "void " << name << "(" << Type<A1>::toString() << ", " << Type<A2>::toString() << ", " << Type<A3>::toString() << ")";
		return stream.str();
	}
};

template<typename R, typename A1, typename A2, typename A3>
class Function<R(A1, A2, A3)>
{    
	AB_FUNCTION_CONSTRUCTOR();
public:
	R operator()(A1 a1, A2 a2, A3 a3)
	{
		Context ctx(this->_engine, this->_function);
		ParameterSetter<A1>()(&ctx, a1); ParameterSetter<A2>()(&ctx, a2); ParameterSetter<A3>()(&ctx, a3); 
		ctx.execute();
		return ReturnReader<R>()(&ctx);
	}
	static std::string decompose(std::string name)
	{
		std::stringstream stream;
		stream << Type<R>::toString() << " " << name << "(" << Type<A1>::toString() << ", " << Type<A2>::toString() << ", " << Type<A3>::toString() << ")";
		return stream.str();
	}
};

template<typename A1, typename A2, typename A3, typename A4>
class Function<void(A1, A2, A3, A4)>
{    
	AB_FUNCTION_CONSTRUCTOR();
public:
	void operator()(A1 a1, A2 a2, A3 a3, A4 a4)
	{
		Context ctx(this->_engine, this->_function);
		ParameterSetter<A1>()(&ctx, a1); ParameterSetter<A2>()(&ctx, a2); ParameterSetter<A3>()(&ctx, a3); ParameterSetter<A4>()(&ctx, a4); 
		ctx.execute();
	}
	static std::string decompose(std::string name)
	{
		std::stringstream stream;
		stream << "void " << name << "(" << Type<A1>::toString() << ", " << Type<A2>::toString() << ", " << Type<A3>::toString() << ", " << Type<A4>::toString() << ")";
		return stream.str();
	}
};

template<typename R, typename A1, typename A2, typename A3, typename A4>
class Function<R(A1, A2, A3, A4)>
{    
	AB_FUNCTION_CONSTRUCTOR();
public:
	R operator()(A1 a1, A2 a2, A3 a3, A4 a4)
	{
		Context ctx(this->_engine, this->_function);
		ParameterSetter<A1>()(&ctx, a1); ParameterSetter<A2>()(&ctx, a2); ParameterSetter<A3>()(&ctx, a3); ParameterSetter<A4>()(&ctx, a4); 
		ctx.execute();
		return ReturnReader<R>()(&ctx);
	}
	static std::string decompose(std::string name)
	{
		std::stringstream stream;
		stream << Type<R>::toString() << " " << name << "(" << Type<A1>::toString() << ", " << Type<A2>::toString() << ", " << Type<A3>::toString() << ", " << Type<A4>::toString() << ")";
		return stream.str();
	}
};

template<typename A1, typename A2, typename A3, typename A4, typename A5>
class Function<void(A1, A2, A3, A4, A5)>
{    
	AB_FUNCTION_CONSTRUCTOR();
public:
	void operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5)
	{
		Context ctx(this->_engine, this->_function);
		ParameterSetter<A1>()(&ctx, a1); ParameterSetter<A2>()(&ctx, a2); ParameterSetter<A3>()(&ctx, a3); ParameterSetter<A4>()(&ctx, a4); ParameterSetter<A5>()(&ctx, a5); 
		ctx.execute();
	}
	static std::string decompose(std::string name)
	{
		std::stringstream stream;
		stream << "void " << name << "(" << Type<A1>::toString() << ", " << Type<A2>::toString() << ", " << Type<A3>::toString() << ", " << Type<A4>::toString() << ", " << Type<A5>::toString() << ")";
		return stream.str();
	}
};

template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5>
class Function<R(A1, A2, A3, A4, A5)>
{    
	AB_FUNCTION_CONSTRUCTOR();
public:
	R operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5)
	{
		Context ctx(this->_engine, this->_function);
		ParameterSetter<A1>()(&ctx, a1); ParameterSetter<A2>()(&ctx, a2); ParameterSetter<A3>()(&ctx, a3); ParameterSetter<A4>()(&ctx, a4); ParameterSetter<A5>()(&ctx, a5); 
		ctx.execute();
		return ReturnReader<R>()(&ctx);
	}
	static std::string decompose(std::string name)
	{
		std::stringstream stream;
		stream << Type<R>::toString() << " " << name << "(" << Type<A1>::toString() << ", " << Type<A2>::toString() << ", " << Type<A3>::toString() << ", " << Type<A4>::toString() << ", " << Type<A5>::toString() << ")";
		return stream.str();
	}
};

template<typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
class Function<void(A1, A2, A3, A4, A5, A6)>
{    
	AB_FUNCTION_CONSTRUCTOR();
public:
	void operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6)
	{
		Context ctx(this->_engine, this->_function);
		ParameterSetter<A1>()(&ctx, a1); ParameterSetter<A2>()(&ctx, a2); ParameterSetter<A3>()(&ctx, a3); ParameterSetter<A4>()(&ctx, a4); ParameterSetter<A5>()(&ctx, a5); ParameterSetter<A6>()(&ctx, a6); 
		ctx.execute();
	}
	static std::string decompose(std::string name)
	{
		std::stringstream stream;
		stream << "void " << name << "(" << Type<A1>::toString() << ", " << Type<A2>::toString() << ", " << Type<A3>::toString() << ", " << Type<A4>::toString() << ", " << Type<A5>::toString() << ", " << Type<A6>::toString() << ")";
		return stream.str();
	}
};

template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
class Function<R(A1, A2, A3, A4, A5, A6)>
{    
	AB_FUNCTION_CONSTRUCTOR();
public:
	R operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6)
	{
		Context ctx(this->_engine, this->_function);
		ParameterSetter<A1>()(&ctx, a1); ParameterSetter<A2>()(&ctx, a2); ParameterSetter<A3>()(&ctx, a3); ParameterSetter<A4>()(&ctx, a4); ParameterSetter<A5>()(&ctx, a5); ParameterSetter<A6>()(&ctx, a6); 
		ctx.execute();
		return ReturnReader<R>()(&ctx);
	}
	static std::string decompose(std::string name)
	{
		std::stringstream stream;
		stream << Type<R>::toString() << " " << name << "(" << Type<A1>::toString() << ", " << Type<A2>::toString() << ", " << Type<A3>::toString() << ", " << Type<A4>::toString() << ", " << Type<A5>::toString() << ", " << Type<A6>::toString() << ")";
		return stream.str();
	}
};

template<typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
class Function<void(A1, A2, A3, A4, A5, A6, A7)>
{    
	AB_FUNCTION_CONSTRUCTOR();
public:
	void operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7)
	{
		Context ctx(this->_engine, this->_function);
		ParameterSetter<A1>()(&ctx, a1); ParameterSetter<A2>()(&ctx, a2); ParameterSetter<A3>()(&ctx, a3); ParameterSetter<A4>()(&ctx, a4); ParameterSetter<A5>()(&ctx, a5); ParameterSetter<A6>()(&ctx, a6); ParameterSetter<A7>()(&ctx, a7); 
		ctx.execute();
	}
	static std::string decompose(std::string name)
	{
		std::stringstream stream;
		stream << "void " << name << "(" << Type<A1>::toString() << ", " << Type<A2>::toString() << ", " << Type<A3>::toString() << ", " << Type<A4>::toString() << ", " << Type<A5>::toString() << ", " << Type<A6>::toString() << ", " << Type<A7>::toString() << ")";
		return stream.str();
	}
};

template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
class Function<R(A1, A2, A3, A4, A5, A6, A7)>
{    
	AB_FUNCTION_CONSTRUCTOR();
public:
	R operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7)
	{
		Context ctx(this->_engine, this->_function);
		ParameterSetter<A1>()(&ctx, a1); ParameterSetter<A2>()(&ctx, a2); ParameterSetter<A3>()(&ctx, a3); ParameterSetter<A4>()(&ctx, a4); ParameterSetter<A5>()(&ctx, a5); ParameterSetter<A6>()(&ctx, a6); ParameterSetter<A7>()(&ctx, a7); 
		ctx.execute();
		return ReturnReader<R>()(&ctx);
	}
	static std::string decompose(std::string name)
	{
		std::stringstream stream;
		stream << Type<R>::toString() << " " << name << "(" << Type<A1>::toString() << ", " << Type<A2>::toString() << ", " << Type<A3>::toString() << ", " << Type<A4>::toString() << ", " << Type<A5>::toString() << ", " << Type<A6>::toString() << ", " << Type<A7>::toString() << ")";
		return stream.str();
	}
};

template<typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
class Function<void(A1, A2, A3, A4, A5, A6, A7, A8)>
{    
	AB_FUNCTION_CONSTRUCTOR();
public:
	void operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8)
	{
		Context ctx(this->_engine, this->_function);
		ParameterSetter<A1>()(&ctx, a1); ParameterSetter<A2>()(&ctx, a2); ParameterSetter<A3>()(&ctx, a3); ParameterSetter<A4>()(&ctx, a4); ParameterSetter<A5>()(&ctx, a5); ParameterSetter<A6>()(&ctx, a6); ParameterSetter<A7>()(&ctx, a7); ParameterSetter<A8>()(&ctx, a8); 
		ctx.execute();
	}
	static std::string decompose(std::string name)
	{
		std::stringstream stream;
		stream << "void " << name << "(" << Type<A1>::toString() << ", " << Type<A2>::toString() << ", " << Type<A3>::toString() << ", " << Type<A4>::toString() << ", " << Type<A5>::toString() << ", " << Type<A6>::toString() << ", " << Type<A7>::toString() << ", " << Type<A8>::toString() << ")";
		return stream.str();
	}
};

template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
class Function<R(A1, A2, A3, A4, A5, A6, A7, A8)>
{    
	AB_FUNCTION_CONSTRUCTOR();
public:
	R operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8)
	{
		Context ctx(this->_engine, this->_function);
		ParameterSetter<A1>()(&ctx, a1); ParameterSetter<A2>()(&ctx, a2); ParameterSetter<A3>()(&ctx, a3); ParameterSetter<A4>()(&ctx, a4); ParameterSetter<A5>()(&ctx, a5); ParameterSetter<A6>()(&ctx, a6); ParameterSetter<A7>()(&ctx, a7); ParameterSetter<A8>()(&ctx, a8); 
		ctx.execute();
		return ReturnReader<R>()(&ctx);
	}
	static std::string decompose(std::string name)
	{
		std::stringstream stream;
		stream << Type<R>::toString() << " " << name << "(" << Type<A1>::toString() << ", " << Type<A2>::toString() << ", " << Type<A3>::toString() << ", " << Type<A4>::toString() << ", " << Type<A5>::toString() << ", " << Type<A6>::toString() << ", " << Type<A7>::toString() << ", " << Type<A8>::toString() << ")";
		return stream.str();
	}
};

template<typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
class Function<void(A1, A2, A3, A4, A5, A6, A7, A8, A9)>
{    
	AB_FUNCTION_CONSTRUCTOR();
public:
	void operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9)
	{
		Context ctx(this->_engine, this->_function);
		ParameterSetter<A1>()(&ctx, a1); ParameterSetter<A2>()(&ctx, a2); ParameterSetter<A3>()(&ctx, a3); ParameterSetter<A4>()(&ctx, a4); ParameterSetter<A5>()(&ctx, a5); ParameterSetter<A6>()(&ctx, a6); ParameterSetter<A7>()(&ctx, a7); ParameterSetter<A8>()(&ctx, a8); ParameterSetter<A9>()(&ctx, a9); 
		ctx.execute();
	}
	static std::string decompose(std::string name)
	{
		std::stringstream stream;
		stream << "void " << name << "(" << Type<A1>::toString() << ", " << Type<A2>::toString() << ", " << Type<A3>::toString() << ", " << Type<A4>::toString() << ", " << Type<A5>::toString() << ", " << Type<A6>::toString() << ", " << Type<A7>::toString() << ", " << Type<A8>::toString() << ", " << Type<A9>::toString() << ")";
		return stream.str();
	}
};

template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
class Function<R(A1, A2, A3, A4, A5, A6, A7, A8, A9)>
{    
	AB_FUNCTION_CONSTRUCTOR();
public:
	R operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9)
	{
		Context ctx(this->_engine, this->_function);
		ParameterSetter<A1>()(&ctx, a1); ParameterSetter<A2>()(&ctx, a2); ParameterSetter<A3>()(&ctx, a3); ParameterSetter<A4>()(&ctx, a4); ParameterSetter<A5>()(&ctx, a5); ParameterSetter<A6>()(&ctx, a6); ParameterSetter<A7>()(&ctx, a7); ParameterSetter<A8>()(&ctx, a8); ParameterSetter<A9>()(&ctx, a9); 
		ctx.execute();
		return ReturnReader<R>()(&ctx);
	}
	static std::string decompose(std::string name)
	{
		std::stringstream stream;
		stream << Type<R>::toString() << " " << name << "(" << Type<A1>::toString() << ", " << Type<A2>::toString() << ", " << Type<A3>::toString() << ", " << Type<A4>::toString() << ", " << Type<A5>::toString() << ", " << Type<A6>::toString() << ", " << Type<A7>::toString() << ", " << Type<A8>::toString() << ", " << Type<A9>::toString() << ")";
		return stream.str();
	}
};

template<typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10>
class Function<void(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10)>
{    
	AB_FUNCTION_CONSTRUCTOR();
public:
	void operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10)
	{
		Context ctx(this->_engine, this->_function);
		ParameterSetter<A1>()(&ctx, a1); ParameterSetter<A2>()(&ctx, a2); ParameterSetter<A3>()(&ctx, a3); ParameterSetter<A4>()(&ctx, a4); ParameterSetter<A5>()(&ctx, a5); ParameterSetter<A6>()(&ctx, a6); ParameterSetter<A7>()(&ctx, a7); ParameterSetter<A8>()(&ctx, a8); ParameterSetter<A9>()(&ctx, a9); ParameterSetter<A10>()(&ctx, a10); 
		ctx.execute();
	}
	static std::string decompose(std::string name)
	{
		std::stringstream stream;
		stream << "void " << name << "(" << Type<A1>::toString() << ", " << Type<A2>::toString() << ", " << Type<A3>::toString() << ", " << Type<A4>::toString() << ", " << Type<A5>::toString() << ", " << Type<A6>::toString() << ", " << Type<A7>::toString() << ", " << Type<A8>::toString() << ", " << Type<A9>::toString() << ", " << Type<A10>::toString() << ")";
		return stream.str();
	}
};

template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10>
class Function<R(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10)>
{    
	AB_FUNCTION_CONSTRUCTOR();
public:
	R operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10)
	{
		Context ctx(this->_engine, this->_function);
		ParameterSetter<A1>()(&ctx, a1); ParameterSetter<A2>()(&ctx, a2); ParameterSetter<A3>()(&ctx, a3); ParameterSetter<A4>()(&ctx, a4); ParameterSetter<A5>()(&ctx, a5); ParameterSetter<A6>()(&ctx, a6); ParameterSetter<A7>()(&ctx, a7); ParameterSetter<A8>()(&ctx, a8); ParameterSetter<A9>()(&ctx, a9); ParameterSetter<A10>()(&ctx, a10); 
		ctx.execute();
		return ReturnReader<R>()(&ctx);
	}
	static std::string decompose(std::string name)
	{
		std::stringstream stream;
		stream << Type<R>::toString() << " " << name << "(" << Type<A1>::toString() << ", " << Type<A2>::toString() << ", " << Type<A3>::toString() << ", " << Type<A4>::toString() << ", " << Type<A5>::toString() << ", " << Type<A6>::toString() << ", " << Type<A7>::toString() << ", " << Type<A8>::toString() << ", " << Type<A9>::toString() << ", " << Type<A10>::toString() << ")";
		return stream.str();
	}
};

template<typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11>
class Function<void(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11)>
{    
	AB_FUNCTION_CONSTRUCTOR();
public:
	void operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10, A11 a11)
	{
		Context ctx(this->_engine, this->_function);
		ParameterSetter<A1>()(&ctx, a1); ParameterSetter<A2>()(&ctx, a2); ParameterSetter<A3>()(&ctx, a3); ParameterSetter<A4>()(&ctx, a4); ParameterSetter<A5>()(&ctx, a5); ParameterSetter<A6>()(&ctx, a6); ParameterSetter<A7>()(&ctx, a7); ParameterSetter<A8>()(&ctx, a8); ParameterSetter<A9>()(&ctx, a9); ParameterSetter<A10>()(&ctx, a10); ParameterSetter<A11>()(&ctx, a11); 
		ctx.execute();
	}
	static std::string decompose(std::string name)
	{
		std::stringstream stream;
		stream << "void " << name << "(" << Type<A1>::toString() << ", " << Type<A2>::toString() << ", " << Type<A3>::toString() << ", " << Type<A4>::toString() << ", " << Type<A5>::toString() << ", " << Type<A6>::toString() << ", " << Type<A7>::toString() << ", " << Type<A8>::toString() << ", " << Type<A9>::toString() << ", " << Type<A10>::toString() << ", " << Type<A11>::toString() << ")";
		return stream.str();
	}
};

template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11>
class Function<R(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11)>
{    
	AB_FUNCTION_CONSTRUCTOR();
public:
	R operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10, A11 a11)
	{
		Context ctx(this->_engine, this->_function);
		ParameterSetter<A1>()(&ctx, a1); ParameterSetter<A2>()(&ctx, a2); ParameterSetter<A3>()(&ctx, a3); ParameterSetter<A4>()(&ctx, a4); ParameterSetter<A5>()(&ctx, a5); ParameterSetter<A6>()(&ctx, a6); ParameterSetter<A7>()(&ctx, a7); ParameterSetter<A8>()(&ctx, a8); ParameterSetter<A9>()(&ctx, a9); ParameterSetter<A10>()(&ctx, a10); ParameterSetter<A11>()(&ctx, a11); 
		ctx.execute();
		return ReturnReader<R>()(&ctx);
	}
	static std::string decompose(std::string name)
	{
		std::stringstream stream;
		stream << Type<R>::toString() << " " << name << "(" << Type<A1>::toString() << ", " << Type<A2>::toString() << ", " << Type<A3>::toString() << ", " << Type<A4>::toString() << ", " << Type<A5>::toString() << ", " << Type<A6>::toString() << ", " << Type<A7>::toString() << ", " << Type<A8>::toString() << ", " << Type<A9>::toString() << ", " << Type<A10>::toString() << ", " << Type<A11>::toString() << ")";
		return stream.str();
	}
};

template<typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12>
class Function<void(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12)>
{    
	AB_FUNCTION_CONSTRUCTOR();
public:
	void operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10, A11 a11, A12 a12)
	{
		Context ctx(this->_engine, this->_function);
		ParameterSetter<A1>()(&ctx, a1); ParameterSetter<A2>()(&ctx, a2); ParameterSetter<A3>()(&ctx, a3); ParameterSetter<A4>()(&ctx, a4); ParameterSetter<A5>()(&ctx, a5); ParameterSetter<A6>()(&ctx, a6); ParameterSetter<A7>()(&ctx, a7); ParameterSetter<A8>()(&ctx, a8); ParameterSetter<A9>()(&ctx, a9); ParameterSetter<A10>()(&ctx, a10); ParameterSetter<A11>()(&ctx, a11); ParameterSetter<A12>()(&ctx, a12); 
		ctx.execute();
	}
	static std::string decompose(std::string name)
	{
		std::stringstream stream;
		stream << "void " << name << "(" << Type<A1>::toString() << ", " << Type<A2>::toString() << ", " << Type<A3>::toString() << ", " << Type<A4>::toString() << ", " << Type<A5>::toString() << ", " << Type<A6>::toString() << ", " << Type<A7>::toString() << ", " << Type<A8>::toString() << ", " << Type<A9>::toString() << ", " << Type<A10>::toString() << ", " << Type<A11>::toString() << ", " << Type<A12>::toString() << ")";
		return stream.str();
	}
};

template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12>
class Function<R(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12)>
{    
	AB_FUNCTION_CONSTRUCTOR();
public:
	R operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10, A11 a11, A12 a12)
	{
		Context ctx(this->_engine, this->_function);
		ParameterSetter<A1>()(&ctx, a1); ParameterSetter<A2>()(&ctx, a2); ParameterSetter<A3>()(&ctx, a3); ParameterSetter<A4>()(&ctx, a4); ParameterSetter<A5>()(&ctx, a5); ParameterSetter<A6>()(&ctx, a6); ParameterSetter<A7>()(&ctx, a7); ParameterSetter<A8>()(&ctx, a8); ParameterSetter<A9>()(&ctx, a9); ParameterSetter<A10>()(&ctx, a10); ParameterSetter<A11>()(&ctx, a11); ParameterSetter<A12>()(&ctx, a12); 
		ctx.execute();
		return ReturnReader<R>()(&ctx);
	}
	static std::string decompose(std::string name)
	{
		std::stringstream stream;
		stream << Type<R>::toString() << " " << name << "(" << Type<A1>::toString() << ", " << Type<A2>::toString() << ", " << Type<A3>::toString() << ", " << Type<A4>::toString() << ", " << Type<A5>::toString() << ", " << Type<A6>::toString() << ", " << Type<A7>::toString() << ", " << Type<A8>::toString() << ", " << Type<A9>::toString() << ", " << Type<A10>::toString() << ", " << Type<A11>::toString() << ", " << Type<A12>::toString() << ")";
		return stream.str();
	}
};

template<typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12, typename A13>
class Function<void(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13)>
{    
	AB_FUNCTION_CONSTRUCTOR();
public:
	void operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10, A11 a11, A12 a12, A13 a13)
	{
		Context ctx(this->_engine, this->_function);
		ParameterSetter<A1>()(&ctx, a1); ParameterSetter<A2>()(&ctx, a2); ParameterSetter<A3>()(&ctx, a3); ParameterSetter<A4>()(&ctx, a4); ParameterSetter<A5>()(&ctx, a5); ParameterSetter<A6>()(&ctx, a6); ParameterSetter<A7>()(&ctx, a7); ParameterSetter<A8>()(&ctx, a8); ParameterSetter<A9>()(&ctx, a9); ParameterSetter<A10>()(&ctx, a10); ParameterSetter<A11>()(&ctx, a11); ParameterSetter<A12>()(&ctx, a12); ParameterSetter<A13>()(&ctx, a13); 
		ctx.execute();
	}
	static std::string decompose(std::string name)
	{
		std::stringstream stream;
		stream << "void " << name << "(" << Type<A1>::toString() << ", " << Type<A2>::toString() << ", " << Type<A3>::toString() << ", " << Type<A4>::toString() << ", " << Type<A5>::toString() << ", " << Type<A6>::toString() << ", " << Type<A7>::toString() << ", " << Type<A8>::toString() << ", " << Type<A9>::toString() << ", " << Type<A10>::toString() << ", " << Type<A11>::toString() << ", " << Type<A12>::toString() << ", " << Type<A13>::toString() << ")";
		return stream.str();
	}
};

template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12, typename A13>
class Function<R(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13)>
{    
	AB_FUNCTION_CONSTRUCTOR();
public:
	R operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10, A11 a11, A12 a12, A13 a13)
	{
		Context ctx(this->_engine, this->_function);
		ParameterSetter<A1>()(&ctx, a1); ParameterSetter<A2>()(&ctx, a2); ParameterSetter<A3>()(&ctx, a3); ParameterSetter<A4>()(&ctx, a4); ParameterSetter<A5>()(&ctx, a5); ParameterSetter<A6>()(&ctx, a6); ParameterSetter<A7>()(&ctx, a7); ParameterSetter<A8>()(&ctx, a8); ParameterSetter<A9>()(&ctx, a9); ParameterSetter<A10>()(&ctx, a10); ParameterSetter<A11>()(&ctx, a11); ParameterSetter<A12>()(&ctx, a12); ParameterSetter<A13>()(&ctx, a13); 
		ctx.execute();
		return ReturnReader<R>()(&ctx);
	}
	static std::string decompose(std::string name)
	{
		std::stringstream stream;
		stream << Type<R>::toString() << " " << name << "(" << Type<A1>::toString() << ", " << Type<A2>::toString() << ", " << Type<A3>::toString() << ", " << Type<A4>::toString() << ", " << Type<A5>::toString() << ", " << Type<A6>::toString() << ", " << Type<A7>::toString() << ", " << Type<A8>::toString() << ", " << Type<A9>::toString() << ", " << Type<A10>::toString() << ", " << Type<A11>::toString() << ", " << Type<A12>::toString() << ", " << Type<A13>::toString() << ")";
		return stream.str();
	}
};

template<typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12, typename A13, typename A14>
class Function<void(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14)>
{    
	AB_FUNCTION_CONSTRUCTOR();
public:
	void operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10, A11 a11, A12 a12, A13 a13, A14 a14)
	{
		Context ctx(this->_engine, this->_function);
		ParameterSetter<A1>()(&ctx, a1); ParameterSetter<A2>()(&ctx, a2); ParameterSetter<A3>()(&ctx, a3); ParameterSetter<A4>()(&ctx, a4); ParameterSetter<A5>()(&ctx, a5); ParameterSetter<A6>()(&ctx, a6); ParameterSetter<A7>()(&ctx, a7); ParameterSetter<A8>()(&ctx, a8); ParameterSetter<A9>()(&ctx, a9); ParameterSetter<A10>()(&ctx, a10); ParameterSetter<A11>()(&ctx, a11); ParameterSetter<A12>()(&ctx, a12); ParameterSetter<A13>()(&ctx, a13); ParameterSetter<A14>()(&ctx, a14); 
		ctx.execute();
	}
	static std::string decompose(std::string name)
	{
		std::stringstream stream;
		stream << "void " << name << "(" << Type<A1>::toString() << ", " << Type<A2>::toString() << ", " << Type<A3>::toString() << ", " << Type<A4>::toString() << ", " << Type<A5>::toString() << ", " << Type<A6>::toString() << ", " << Type<A7>::toString() << ", " << Type<A8>::toString() << ", " << Type<A9>::toString() << ", " << Type<A10>::toString() << ", " << Type<A11>::toString() << ", " << Type<A12>::toString() << ", " << Type<A13>::toString() << ", " << Type<A14>::toString() << ")";
		return stream.str();
	}
};

template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12, typename A13, typename A14>
class Function<R(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14)>
{    
	AB_FUNCTION_CONSTRUCTOR();
public:
	R operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10, A11 a11, A12 a12, A13 a13, A14 a14)
	{
		Context ctx(this->_engine, this->_function);
		ParameterSetter<A1>()(&ctx, a1); ParameterSetter<A2>()(&ctx, a2); ParameterSetter<A3>()(&ctx, a3); ParameterSetter<A4>()(&ctx, a4); ParameterSetter<A5>()(&ctx, a5); ParameterSetter<A6>()(&ctx, a6); ParameterSetter<A7>()(&ctx, a7); ParameterSetter<A8>()(&ctx, a8); ParameterSetter<A9>()(&ctx, a9); ParameterSetter<A10>()(&ctx, a10); ParameterSetter<A11>()(&ctx, a11); ParameterSetter<A12>()(&ctx, a12); ParameterSetter<A13>()(&ctx, a13); ParameterSetter<A14>()(&ctx, a14); 
		ctx.execute();
		return ReturnReader<R>()(&ctx);
	}
	static std::string decompose(std::string name)
	{
		std::stringstream stream;
		stream << Type<R>::toString() << " " << name << "(" << Type<A1>::toString() << ", " << Type<A2>::toString() << ", " << Type<A3>::toString() << ", " << Type<A4>::toString() << ", " << Type<A5>::toString() << ", " << Type<A6>::toString() << ", " << Type<A7>::toString() << ", " << Type<A8>::toString() << ", " << Type<A9>::toString() << ", " << Type<A10>::toString() << ", " << Type<A11>::toString() << ", " << Type<A12>::toString() << ", " << Type<A13>::toString() << ", " << Type<A14>::toString() << ")";
		return stream.str();
	}
};

///
/// Script Engine
///
class Module
{
private:
	///
	/// Script Module Name
	///
	std::string _name;

	///
	/// AngelBinder engine
	///
	Engine& _engine;

public:
	///
	/// Constructors
	///
	Module(Engine& engine, std::string name);
	~Module(void);

	///
	/// Retrieves the engine instance
	///
	Engine& engine();

	///
	/// Script compiler
	///
	bool compile(std::string file);

	template<typename F>
	Function<F> getFunction(std::string name)
	{
		int funcid = this->getFunctionByDecl(Function<F>::decompose(name));
		AB_SCRIPT_ASSERT(funcid >= 0, "Function could not be found.", AB_THROW, &this->_engine);
		Function<F> func(this->_engine, funcid);
		return func;
	}

private:
	int getFunctionByName(std::string name)
	{
		ASModule* module = this->_engine.asEngine()->GetModule(this->_name.c_str());
		return module->GetFunctionIdByName(name.c_str());
	}

	int getFunctionByDecl(std::string decl)
	{
		ASModule* module = this->_engine.asEngine()->GetModule(this->_name.c_str());
		return module->GetFunctionIdByDecl(decl.c_str());
	}

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
	virtual void finish(Module& script) = 0;
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
	virtual void finish(Module& instance);

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
	CallStdcall = AS_NAMESPACE_QUALIFIER asCALL_STDCALL,
	CallCdecl = AS_NAMESPACE_QUALIFIER asCALL_CDECL
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
	AS_NAMESPACE_QUALIFIER asSFuncPtr _func;

public:
	///
	/// FunctionClass constructor
	///
	FunctionClass(std::string ret, std::string name, CallingConvention conv, AS_NAMESPACE_QUALIFIER asSFuncPtr func);

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
	AS_NAMESPACE_QUALIFIER asSFuncPtr address();

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
	virtual void finish(Module& instance);

public:
	///
	/// Templated function exporters
	///
	template<typename R>
	FunctionExporter& def(std::string name, R(__stdcall *func)())
	{
		FunctionClass fc(Type<R>::toString(), name, CallStdcall, AB_FUNCTION(func));
		this->_entries.push(fc);
		return *this;
	}

	template<typename R>
	FunctionExporter& def(std::string name, R(__cdecl *func)())
	{
		FunctionClass fc(Type<R>::toString(), name, CallCdecl, AB_FUNCTION(func));
		this->_entries.push(fc);
		return *this;
	}

	template<typename R, typename A1>
	FunctionExporter& def(std::string name, R(__stdcall *func)(A1))
	{
		FunctionClass fc(Type<R>::toString(), name, CallStdcall, AB_FUNCTION(func));
		AB_PUSH_ARG(fc, A1); 
		this->_entries.push(fc);
		return *this;
	}

	template<typename R, typename A1>
	FunctionExporter& def(std::string name, R(__cdecl *func)(A1))
	{
		FunctionClass fc(Type<R>::toString(), name, CallCdecl, AB_FUNCTION(func));
		AB_PUSH_ARG(fc, A1); 
		this->_entries.push(fc);
		return *this;
	}

	template<typename R, typename A1, typename A2>
	FunctionExporter& def(std::string name, R(__stdcall *func)(A1, A2))
	{
		FunctionClass fc(Type<R>::toString(), name, CallStdcall, AB_FUNCTION(func));
		AB_PUSH_ARG(fc, A1); AB_PUSH_ARG(fc, A2); 
		this->_entries.push(fc);
		return *this;
	}

	template<typename R, typename A1, typename A2>
	FunctionExporter& def(std::string name, R(__cdecl *func)(A1, A2))
	{
		FunctionClass fc(Type<R>::toString(), name, CallCdecl, AB_FUNCTION(func));
		AB_PUSH_ARG(fc, A1); AB_PUSH_ARG(fc, A2); 
		this->_entries.push(fc);
		return *this;
	}

	template<typename R, typename A1, typename A2, typename A3>
	FunctionExporter& def(std::string name, R(__stdcall *func)(A1, A2, A3))
	{
		FunctionClass fc(Type<R>::toString(), name, CallStdcall, AB_FUNCTION(func));
		AB_PUSH_ARG(fc, A1); AB_PUSH_ARG(fc, A2); AB_PUSH_ARG(fc, A3); 
		this->_entries.push(fc);
		return *this;
	}

	template<typename R, typename A1, typename A2, typename A3>
	FunctionExporter& def(std::string name, R(__cdecl *func)(A1, A2, A3))
	{
		FunctionClass fc(Type<R>::toString(), name, CallCdecl, AB_FUNCTION(func));
		AB_PUSH_ARG(fc, A1); AB_PUSH_ARG(fc, A2); AB_PUSH_ARG(fc, A3); 
		this->_entries.push(fc);
		return *this;
	}

	template<typename R, typename A1, typename A2, typename A3, typename A4>
	FunctionExporter& def(std::string name, R(__stdcall *func)(A1, A2, A3, A4))
	{
		FunctionClass fc(Type<R>::toString(), name, CallStdcall, AB_FUNCTION(func));
		AB_PUSH_ARG(fc, A1); AB_PUSH_ARG(fc, A2); AB_PUSH_ARG(fc, A3); AB_PUSH_ARG(fc, A4); 
		this->_entries.push(fc);
		return *this;
	}

	template<typename R, typename A1, typename A2, typename A3, typename A4>
	FunctionExporter& def(std::string name, R(__cdecl *func)(A1, A2, A3, A4))
	{
		FunctionClass fc(Type<R>::toString(), name, CallCdecl, AB_FUNCTION(func));
		AB_PUSH_ARG(fc, A1); AB_PUSH_ARG(fc, A2); AB_PUSH_ARG(fc, A3); AB_PUSH_ARG(fc, A4); 
		this->_entries.push(fc);
		return *this;
	}

	template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5>
	FunctionExporter& def(std::string name, R(__stdcall *func)(A1, A2, A3, A4, A5))
	{
		FunctionClass fc(Type<R>::toString(), name, CallStdcall, AB_FUNCTION(func));
		AB_PUSH_ARG(fc, A1); AB_PUSH_ARG(fc, A2); AB_PUSH_ARG(fc, A3); AB_PUSH_ARG(fc, A4); AB_PUSH_ARG(fc, A5); 
		this->_entries.push(fc);
		return *this;
	}

	template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5>
	FunctionExporter& def(std::string name, R(__cdecl *func)(A1, A2, A3, A4, A5))
	{
		FunctionClass fc(Type<R>::toString(), name, CallCdecl, AB_FUNCTION(func));
		AB_PUSH_ARG(fc, A1); AB_PUSH_ARG(fc, A2); AB_PUSH_ARG(fc, A3); AB_PUSH_ARG(fc, A4); AB_PUSH_ARG(fc, A5); 
		this->_entries.push(fc);
		return *this;
	}

	template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
	FunctionExporter& def(std::string name, R(__stdcall *func)(A1, A2, A3, A4, A5, A6))
	{
		FunctionClass fc(Type<R>::toString(), name, CallStdcall, AB_FUNCTION(func));
		AB_PUSH_ARG(fc, A1); AB_PUSH_ARG(fc, A2); AB_PUSH_ARG(fc, A3); AB_PUSH_ARG(fc, A4); AB_PUSH_ARG(fc, A5); AB_PUSH_ARG(fc, A6); 
		this->_entries.push(fc);
		return *this;
	}

	template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
	FunctionExporter& def(std::string name, R(__cdecl *func)(A1, A2, A3, A4, A5, A6))
	{
		FunctionClass fc(Type<R>::toString(), name, CallCdecl, AB_FUNCTION(func));
		AB_PUSH_ARG(fc, A1); AB_PUSH_ARG(fc, A2); AB_PUSH_ARG(fc, A3); AB_PUSH_ARG(fc, A4); AB_PUSH_ARG(fc, A5); AB_PUSH_ARG(fc, A6); 
		this->_entries.push(fc);
		return *this;
	}

	template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
	FunctionExporter& def(std::string name, R(__stdcall *func)(A1, A2, A3, A4, A5, A6, A7))
	{
		FunctionClass fc(Type<R>::toString(), name, CallStdcall, AB_FUNCTION(func));
		AB_PUSH_ARG(fc, A1); AB_PUSH_ARG(fc, A2); AB_PUSH_ARG(fc, A3); AB_PUSH_ARG(fc, A4); AB_PUSH_ARG(fc, A5); AB_PUSH_ARG(fc, A6); AB_PUSH_ARG(fc, A7); 
		this->_entries.push(fc);
		return *this;
	}

	template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
	FunctionExporter& def(std::string name, R(__cdecl *func)(A1, A2, A3, A4, A5, A6, A7))
	{
		FunctionClass fc(Type<R>::toString(), name, CallCdecl, AB_FUNCTION(func));
		AB_PUSH_ARG(fc, A1); AB_PUSH_ARG(fc, A2); AB_PUSH_ARG(fc, A3); AB_PUSH_ARG(fc, A4); AB_PUSH_ARG(fc, A5); AB_PUSH_ARG(fc, A6); AB_PUSH_ARG(fc, A7); 
		this->_entries.push(fc);
		return *this;
	}

	template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
	FunctionExporter& def(std::string name, R(__stdcall *func)(A1, A2, A3, A4, A5, A6, A7, A8))
	{
		FunctionClass fc(Type<R>::toString(), name, CallStdcall, AB_FUNCTION(func));
		AB_PUSH_ARG(fc, A1); AB_PUSH_ARG(fc, A2); AB_PUSH_ARG(fc, A3); AB_PUSH_ARG(fc, A4); AB_PUSH_ARG(fc, A5); AB_PUSH_ARG(fc, A6); AB_PUSH_ARG(fc, A7); AB_PUSH_ARG(fc, A8); 
		this->_entries.push(fc);
		return *this;
	}

	template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
	FunctionExporter& def(std::string name, R(__cdecl *func)(A1, A2, A3, A4, A5, A6, A7, A8))
	{
		FunctionClass fc(Type<R>::toString(), name, CallCdecl, AB_FUNCTION(func));
		AB_PUSH_ARG(fc, A1); AB_PUSH_ARG(fc, A2); AB_PUSH_ARG(fc, A3); AB_PUSH_ARG(fc, A4); AB_PUSH_ARG(fc, A5); AB_PUSH_ARG(fc, A6); AB_PUSH_ARG(fc, A7); AB_PUSH_ARG(fc, A8); 
		this->_entries.push(fc);
		return *this;
	}

	template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
	FunctionExporter& def(std::string name, R(__stdcall *func)(A1, A2, A3, A4, A5, A6, A7, A8, A9))
	{
		FunctionClass fc(Type<R>::toString(), name, CallStdcall, AB_FUNCTION(func));
		AB_PUSH_ARG(fc, A1); AB_PUSH_ARG(fc, A2); AB_PUSH_ARG(fc, A3); AB_PUSH_ARG(fc, A4); AB_PUSH_ARG(fc, A5); AB_PUSH_ARG(fc, A6); AB_PUSH_ARG(fc, A7); AB_PUSH_ARG(fc, A8); AB_PUSH_ARG(fc, A9); 
		this->_entries.push(fc);
		return *this;
	}

	template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
	FunctionExporter& def(std::string name, R(__cdecl *func)(A1, A2, A3, A4, A5, A6, A7, A8, A9))
	{
		FunctionClass fc(Type<R>::toString(), name, CallCdecl, AB_FUNCTION(func));
		AB_PUSH_ARG(fc, A1); AB_PUSH_ARG(fc, A2); AB_PUSH_ARG(fc, A3); AB_PUSH_ARG(fc, A4); AB_PUSH_ARG(fc, A5); AB_PUSH_ARG(fc, A6); AB_PUSH_ARG(fc, A7); AB_PUSH_ARG(fc, A8); AB_PUSH_ARG(fc, A9); 
		this->_entries.push(fc);
		return *this;
	}

	template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10>
	FunctionExporter& def(std::string name, R(__stdcall *func)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10))
	{
		FunctionClass fc(Type<R>::toString(), name, CallStdcall, AB_FUNCTION(func));
		AB_PUSH_ARG(fc, A1); AB_PUSH_ARG(fc, A2); AB_PUSH_ARG(fc, A3); AB_PUSH_ARG(fc, A4); AB_PUSH_ARG(fc, A5); AB_PUSH_ARG(fc, A6); AB_PUSH_ARG(fc, A7); AB_PUSH_ARG(fc, A8); AB_PUSH_ARG(fc, A9); AB_PUSH_ARG(fc, A10); 
		this->_entries.push(fc);
		return *this;
	}

	template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10>
	FunctionExporter& def(std::string name, R(__cdecl *func)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10))
	{
		FunctionClass fc(Type<R>::toString(), name, CallCdecl, AB_FUNCTION(func));
		AB_PUSH_ARG(fc, A1); AB_PUSH_ARG(fc, A2); AB_PUSH_ARG(fc, A3); AB_PUSH_ARG(fc, A4); AB_PUSH_ARG(fc, A5); AB_PUSH_ARG(fc, A6); AB_PUSH_ARG(fc, A7); AB_PUSH_ARG(fc, A8); AB_PUSH_ARG(fc, A9); AB_PUSH_ARG(fc, A10); 
		this->_entries.push(fc);
		return *this;
	}

	template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11>
	FunctionExporter& def(std::string name, R(__stdcall *func)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11))
	{
		FunctionClass fc(Type<R>::toString(), name, CallStdcall, AB_FUNCTION(func));
		AB_PUSH_ARG(fc, A1); AB_PUSH_ARG(fc, A2); AB_PUSH_ARG(fc, A3); AB_PUSH_ARG(fc, A4); AB_PUSH_ARG(fc, A5); AB_PUSH_ARG(fc, A6); AB_PUSH_ARG(fc, A7); AB_PUSH_ARG(fc, A8); AB_PUSH_ARG(fc, A9); AB_PUSH_ARG(fc, A10); AB_PUSH_ARG(fc, A11); 
		this->_entries.push(fc);
		return *this;
	}

	template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11>
	FunctionExporter& def(std::string name, R(__cdecl *func)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11))
	{
		FunctionClass fc(Type<R>::toString(), name, CallCdecl, AB_FUNCTION(func));
		AB_PUSH_ARG(fc, A1); AB_PUSH_ARG(fc, A2); AB_PUSH_ARG(fc, A3); AB_PUSH_ARG(fc, A4); AB_PUSH_ARG(fc, A5); AB_PUSH_ARG(fc, A6); AB_PUSH_ARG(fc, A7); AB_PUSH_ARG(fc, A8); AB_PUSH_ARG(fc, A9); AB_PUSH_ARG(fc, A10); AB_PUSH_ARG(fc, A11); 
		this->_entries.push(fc);
		return *this;
	}

	template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12>
	FunctionExporter& def(std::string name, R(__stdcall *func)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12))
	{
		FunctionClass fc(Type<R>::toString(), name, CallStdcall, AB_FUNCTION(func));
		AB_PUSH_ARG(fc, A1); AB_PUSH_ARG(fc, A2); AB_PUSH_ARG(fc, A3); AB_PUSH_ARG(fc, A4); AB_PUSH_ARG(fc, A5); AB_PUSH_ARG(fc, A6); AB_PUSH_ARG(fc, A7); AB_PUSH_ARG(fc, A8); AB_PUSH_ARG(fc, A9); AB_PUSH_ARG(fc, A10); AB_PUSH_ARG(fc, A11); AB_PUSH_ARG(fc, A12); 
		this->_entries.push(fc);
		return *this;
	}

	template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12>
	FunctionExporter& def(std::string name, R(__cdecl *func)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12))
	{
		FunctionClass fc(Type<R>::toString(), name, CallCdecl, AB_FUNCTION(func));
		AB_PUSH_ARG(fc, A1); AB_PUSH_ARG(fc, A2); AB_PUSH_ARG(fc, A3); AB_PUSH_ARG(fc, A4); AB_PUSH_ARG(fc, A5); AB_PUSH_ARG(fc, A6); AB_PUSH_ARG(fc, A7); AB_PUSH_ARG(fc, A8); AB_PUSH_ARG(fc, A9); AB_PUSH_ARG(fc, A10); AB_PUSH_ARG(fc, A11); AB_PUSH_ARG(fc, A12); 
		this->_entries.push(fc);
		return *this;
	}

	template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12, typename A13>
	FunctionExporter& def(std::string name, R(__stdcall *func)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13))
	{
		FunctionClass fc(Type<R>::toString(), name, CallStdcall, AB_FUNCTION(func));
		AB_PUSH_ARG(fc, A1); AB_PUSH_ARG(fc, A2); AB_PUSH_ARG(fc, A3); AB_PUSH_ARG(fc, A4); AB_PUSH_ARG(fc, A5); AB_PUSH_ARG(fc, A6); AB_PUSH_ARG(fc, A7); AB_PUSH_ARG(fc, A8); AB_PUSH_ARG(fc, A9); AB_PUSH_ARG(fc, A10); AB_PUSH_ARG(fc, A11); AB_PUSH_ARG(fc, A12); AB_PUSH_ARG(fc, A13); 
		this->_entries.push(fc);
		return *this;
	}

	template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12, typename A13>
	FunctionExporter& def(std::string name, R(__cdecl *func)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13))
	{
		FunctionClass fc(Type<R>::toString(), name, CallCdecl, AB_FUNCTION(func));
		AB_PUSH_ARG(fc, A1); AB_PUSH_ARG(fc, A2); AB_PUSH_ARG(fc, A3); AB_PUSH_ARG(fc, A4); AB_PUSH_ARG(fc, A5); AB_PUSH_ARG(fc, A6); AB_PUSH_ARG(fc, A7); AB_PUSH_ARG(fc, A8); AB_PUSH_ARG(fc, A9); AB_PUSH_ARG(fc, A10); AB_PUSH_ARG(fc, A11); AB_PUSH_ARG(fc, A12); AB_PUSH_ARG(fc, A13); 
		this->_entries.push(fc);
		return *this;
	}

	template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12, typename A13, typename A14>
	FunctionExporter& def(std::string name, R(__stdcall *func)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14))
	{
		FunctionClass fc(Type<R>::toString(), name, CallStdcall, AB_FUNCTION(func));
		AB_PUSH_ARG(fc, A1); AB_PUSH_ARG(fc, A2); AB_PUSH_ARG(fc, A3); AB_PUSH_ARG(fc, A4); AB_PUSH_ARG(fc, A5); AB_PUSH_ARG(fc, A6); AB_PUSH_ARG(fc, A7); AB_PUSH_ARG(fc, A8); AB_PUSH_ARG(fc, A9); AB_PUSH_ARG(fc, A10); AB_PUSH_ARG(fc, A11); AB_PUSH_ARG(fc, A12); AB_PUSH_ARG(fc, A13); AB_PUSH_ARG(fc, A14); 
		this->_entries.push(fc);
		return *this;
	}

	template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12, typename A13, typename A14>
	FunctionExporter& def(std::string name, R(__cdecl *func)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14))
	{
		FunctionClass fc(Type<R>::toString(), name, CallCdecl, AB_FUNCTION(func));
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
	AS_NAMESPACE_QUALIFIER asSFuncPtr _func;

public:
	///
	/// ConstructorClass constructor
	///
	ConstructorClass(AS_NAMESPACE_QUALIFIER asSFuncPtr func)
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
	AS_NAMESPACE_QUALIFIER asSFuncPtr address()
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
/// Class methods
///
class MethodClass
{
private:
	/// Stores the return value
	std::string _ret;

	/// Stores the name of the method
	std::string _name;

	/// Stores a vector of arguments
	std::vector<std::string> _params;

	/// Stores the function reference
	AS_NAMESPACE_QUALIFIER asSFuncPtr _func;

public:
	///
	/// MethodClass constructor
	///
	MethodClass(std::string name, std::string ret, AS_NAMESPACE_QUALIFIER asSFuncPtr func) 
		: _func(func), _ret(ret), _name(name)
	{
	}

	///
	/// Returns the parameter list
	///
	std::vector<std::string>& parameters()
	{
		return this->_params;
	}

	///
	/// Returns the structure for the method
	///
	AS_NAMESPACE_QUALIFIER asSFuncPtr address()
	{
		return this->_func;
	}

	///
	/// Decomposes the method into an equivalent string
	///
	std::string decompose()
	{
		std::stringstream stream;
		stream << this->_ret << " " << this->_name << "(";
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
/// Class accessors
///
class AccessorClass
{
private:
	/// Stores the name of the method
	std::string _name;

	/// Stores the set function reference
	MethodClass _getfunc;

	/// Stores the get function reference
	MethodClass _setfunc;

public:
	///
	/// AccessorClass constructor
	///
	AccessorClass(std::string name, std::string type, AS_NAMESPACE_QUALIFIER asSFuncPtr getfunc, AS_NAMESPACE_QUALIFIER asSFuncPtr setfunc) 
		: _name(name), _getfunc("get_" + name, type, getfunc), _setfunc("set_" + name, "void", setfunc)
	{
		this->_setfunc.parameters().push_back(type);
	}

	///
	/// Accessor name
	///
	std::string name()
	{
		return this->_name;
	}

	///
	/// Returns the structure for the get method
	///
	MethodClass funcGet()
	{
		return this->_getfunc;
	}

	///
	/// Returns the structure for the set method
	///
	MethodClass funcSet()
	{
		return this->_setfunc;
	}

};

///
/// Class constructors
///
class MemberClass
{
private:
	/// Stores the name of the member
	std::string _name;

	/// Stores the type of the member
	std::string _type;

	/// Stores the offset of the member
	int _offset;

public:
	///
	/// MemberClass constructor
	///
	MemberClass(std::string name, std::string type, int offset) 
		: _name(name), _type(type), _offset(offset)
	{
	}

	///
	/// Returns the member name
	///
	std::string name()
	{
		return this->_name;
	}

	///
	/// Returns the structure for the get method
	///
	std::string type()
	{
		return this->_type;
	}

	///
	/// Returns the structure for the set method
	///
	int offset()
	{
		return this->_offset;
	}

	///
	/// Decomposes the member declaration
	///
	std::string decompose()
	{
		return this->_type + " " + this->_name;
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

	/// Initial flags
	int _flags;

	/// Stores all exported constructors
	std::queue<ConstructorClass> _ctors;

	/// Stores all assignment operators
	// std::queue<AssignClass> _assigns;

	/// Stores all copy constructors
	// asSFuncPtr _copy;

	/// Stores all exported constructors
	std::queue<MethodClass> _methods;

	/// Stores all exported accessors
	std::queue<AccessorClass> _accessors;

	/// Stores all exported accessors
	std::queue<MemberClass> _members;

	/// Stores if the destructor has been set
	bool _dtorset;

	/// Stores if the destructor has been set
	bool _ctorset;

	/// Stores the destructor pointer
	AS_NAMESPACE_QUALIFIER asSFuncPtr _dtor;

	/// Stores the dummy constructor pointer
	AS_NAMESPACE_QUALIFIER asSFuncPtr _ctor;

protected:
	///
	/// Function exporter
	///
	ClassExporter( int flags ) 
		: _flags(flags), _dtorset(false), _ctorset(false)
	{
		asMemClear(&this->_ctor, sizeof(this->_ctor));
		asMemClear(&this->_dtor, sizeof(this->_dtor));
	}

	///
	/// Called when registering is needed.
	///
	virtual void finish(Module& instance)
	{

		int flags = this->_flags;
		flags |= AS_NAMESPACE_QUALIFIER asOBJ_VALUE;
		flags |= AS_NAMESPACE_QUALIFIER asOBJ_APP_CLASS;
		flags |= !this->_ctors.empty() || this->_ctorset ? AS_NAMESPACE_QUALIFIER asOBJ_APP_CLASS_CONSTRUCTOR : 0;
		flags |= this->_dtorset ? AS_NAMESPACE_QUALIFIER asOBJ_APP_CLASS_DESTRUCTOR : 0;
		flags |= std::tr1::is_pod<T>::value == true ? AS_NAMESPACE_QUALIFIER asOBJ_POD : 0;
		// flags |= this->_assigns.empty() ? 0 : asOBJ_APP_CLASS_ASSIGNMENT;
		// flags |= this->_copy == NULL ? 0 : asOBJ_APP_CLASS_COPY_CONSTRUCTOR;

		std::string name = Type<T>::toString();
		AB_MESSAGE_INVOKE_STATIC(&instance.engine(), &instance.engine(), "Registering type '" + name + "'");
		int r = instance.engine().asEngine()->RegisterObjectType(Type<T>::toString().c_str(), sizeof(T), flags);
		AB_SCRIPT_ASSERT(r >= 0, std::string("Could not register type '" + name + "'").c_str(), AB_THROW, &instance.engine());

		if(this->_ctorset && !this->_ctors.empty())
		{
			AB_SCRIPT_ASSERT(false, std::string("Can't register dummy constructor if there's another constructors.").c_str(), AB_THROW, &instance.engine());
		}

		while(!this->_ctors.empty())
		{
			ConstructorClass memb = this->_ctors.front();
			std::string decomp = memb.decompose();
			AB_MESSAGE_INVOKE_STATIC(&instance.engine(), &instance.engine(), "Registering constructor for '" + name + "' as '" + decomp + "'");
			r = instance.engine().asEngine()->RegisterObjectBehaviour(name.c_str(), AS_NAMESPACE_QUALIFIER asBEHAVE_CONSTRUCT, decomp.c_str(), memb.address(), AS_NAMESPACE_QUALIFIER asCALL_CDECL_OBJLAST);
			AB_SCRIPT_ASSERT(r >= 0, std::string("Can't register constructor for type '" + name + "'").c_str(), AB_THROW, &instance.engine());
			this->_ctors.pop();
		}

		if(this->_ctorset)
		{
			AB_MESSAGE_INVOKE_STATIC(&instance.engine(), &instance.engine(), "Registering dummy constructor for '" + name + "' as 'void f()'");
			int r = instance.engine().asEngine()->RegisterObjectBehaviour(name.c_str(), AS_NAMESPACE_QUALIFIER asBEHAVE_CONSTRUCT, "void f()", this->_ctor, AS_NAMESPACE_QUALIFIER asCALL_CDECL_OBJLAST);
			AB_SCRIPT_ASSERT(r >= 0, std::string("Can't register dummy constructor for type '" + name + "'").c_str(), AB_THROW, &instance.engine());
		}

		if(this->_dtorset)
		{
			AB_MESSAGE_INVOKE_STATIC(&instance.engine(), &instance.engine(), "Registering destructor for '" + name + "' as 'void f()'");
			r = instance.engine().asEngine()->RegisterObjectBehaviour(name.c_str(), AS_NAMESPACE_QUALIFIER asBEHAVE_DESTRUCT, "void f()", this->_dtor, AS_NAMESPACE_QUALIFIER asCALL_CDECL_OBJLAST);
			AB_SCRIPT_ASSERT(r >= 0, std::string("Can't register destructor for type '" + name + "'").c_str(), AB_THROW, &instance.engine());
		}

		while(!this->_members.empty())
		{
			MemberClass memb = this->_members.front();
			AB_MESSAGE_INVOKE_STATIC(&instance.engine(), &instance.engine(), "Registering '" + name + "' member as '" + memb.decompose() + "'");

			r = instance.engine().asEngine()->RegisterObjectProperty(name.c_str(), memb.decompose().c_str(), memb.offset());
			AB_SCRIPT_ASSERT(r >= 0, std::string("Can't register member for type '" + name + "'").c_str(), AB_THROW, &instance.engine());

			this->_members.pop();
		}

		while(!this->_accessors.empty())
		{
			AccessorClass memb = this->_accessors.front();
			AB_MESSAGE_INVOKE_STATIC(&instance.engine(), &instance.engine(), "Registering '" + name + "' accessors for property '" + memb.name() + "'");

			MethodClass setf = memb.funcSet();
			r = instance.engine().asEngine()->RegisterObjectMethod(name.c_str(), setf.decompose().c_str(), setf.address(), AS_NAMESPACE_QUALIFIER asCALL_THISCALL);
			AB_SCRIPT_ASSERT(r >= 0, std::string("Can't register accessor's 'set' method for type '" + name + "'").c_str(), AB_THROW, &instance.engine());

			MethodClass getf = memb.funcGet();
			r = instance.engine().asEngine()->RegisterObjectMethod(name.c_str(), getf.decompose().c_str(), getf.address(), AS_NAMESPACE_QUALIFIER asCALL_THISCALL);
			AB_SCRIPT_ASSERT(r >= 0, std::string("Can't register accessor's 'get' method for type '" + name + "'").c_str(), AB_THROW, &instance.engine());

			this->_accessors.pop();
		}

		while(!this->_methods.empty())
		{
			MethodClass memb = this->_methods.front();
			std::string decomp = memb.decompose();
			AB_MESSAGE_INVOKE_STATIC(&instance.engine(), &instance.engine(), "Registering method for '" + name + "' as '" + decomp + "'");
			r = instance.engine().asEngine()->RegisterObjectMethod(name.c_str(), decomp.c_str(), memb.address(), AS_NAMESPACE_QUALIFIER asCALL_THISCALL);
			AB_SCRIPT_ASSERT(r >= 0, std::string("Can't register method for type '" + name + "'").c_str(), AB_THROW, &instance.engine());
			this->_methods.pop();
		}

	}

public:
	///
	/// Constructors
	///
	ClassExporter& ctor()
	{
		ConstructorClass cc(AB_FUNCTIONPR(ConstructorWrapper<T>, (void*), void));
		this->_ctors.push(cc);
		return *this;
	}

	template<typename A1>
	ClassExporter& ctor()
	{
		ConstructorClass cc(AB_FUNCTIONPR((ConstructorWrapper<T, A1>), (A1, void*), void));
		AB_PUSH_ARG(cc, A1);         
		this->_ctors.push(cc);
		return *this;
	}

	template<typename A1, typename A2>
	ClassExporter& ctor()
	{
		ConstructorClass cc(AB_FUNCTIONPR((ConstructorWrapper<T, A1, A2>), (A1, A2, void*), void));
		AB_PUSH_ARG(cc, A1); AB_PUSH_ARG(cc, A2);         
		this->_ctors.push(cc);
		return *this;
	}

	template<typename A1, typename A2, typename A3>
	ClassExporter& ctor()
	{
		ConstructorClass cc(AB_FUNCTIONPR((ConstructorWrapper<T, A1, A2, A3>), (A1, A2, A3, void*), void));
		AB_PUSH_ARG(cc, A1); AB_PUSH_ARG(cc, A2); AB_PUSH_ARG(cc, A3);         
		this->_ctors.push(cc);
		return *this;
	}

	template<typename A1, typename A2, typename A3, typename A4>
	ClassExporter& ctor()
	{
		ConstructorClass cc(AB_FUNCTIONPR((ConstructorWrapper<T, A1, A2, A3, A4>), (A1, A2, A3, A4, void*), void));
		AB_PUSH_ARG(cc, A1); AB_PUSH_ARG(cc, A2); AB_PUSH_ARG(cc, A3); AB_PUSH_ARG(cc, A4);         
		this->_ctors.push(cc);
		return *this;
	}

	template<typename A1, typename A2, typename A3, typename A4, typename A5>
	ClassExporter& ctor()
	{
		ConstructorClass cc(AB_FUNCTIONPR((ConstructorWrapper<T, A1, A2, A3, A4, A5>), (A1, A2, A3, A4, A5, void*), void));
		AB_PUSH_ARG(cc, A1); AB_PUSH_ARG(cc, A2); AB_PUSH_ARG(cc, A3); AB_PUSH_ARG(cc, A4); AB_PUSH_ARG(cc, A5);         
		this->_ctors.push(cc);
		return *this;
	}

	template<typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
	ClassExporter& ctor()
	{
		ConstructorClass cc(AB_FUNCTIONPR((ConstructorWrapper<T, A1, A2, A3, A4, A5, A6>), (A1, A2, A3, A4, A5, A6, void*), void));
		AB_PUSH_ARG(cc, A1); AB_PUSH_ARG(cc, A2); AB_PUSH_ARG(cc, A3); AB_PUSH_ARG(cc, A4); AB_PUSH_ARG(cc, A5); AB_PUSH_ARG(cc, A6);         
		this->_ctors.push(cc);
		return *this;
	}

	template<typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
	ClassExporter& ctor()
	{
		ConstructorClass cc(AB_FUNCTIONPR((ConstructorWrapper<T, A1, A2, A3, A4, A5, A6, A7>), (A1, A2, A3, A4, A5, A6, A7, void*), void));
		AB_PUSH_ARG(cc, A1); AB_PUSH_ARG(cc, A2); AB_PUSH_ARG(cc, A3); AB_PUSH_ARG(cc, A4); AB_PUSH_ARG(cc, A5); AB_PUSH_ARG(cc, A6); AB_PUSH_ARG(cc, A7);         
		this->_ctors.push(cc);
		return *this;
	}

	template<typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
	ClassExporter& ctor()
	{
		ConstructorClass cc(AB_FUNCTIONPR((ConstructorWrapper<T, A1, A2, A3, A4, A5, A6, A7, A8>), (A1, A2, A3, A4, A5, A6, A7, A8, void*), void));
		AB_PUSH_ARG(cc, A1); AB_PUSH_ARG(cc, A2); AB_PUSH_ARG(cc, A3); AB_PUSH_ARG(cc, A4); AB_PUSH_ARG(cc, A5); AB_PUSH_ARG(cc, A6); AB_PUSH_ARG(cc, A7); AB_PUSH_ARG(cc, A8);         
		this->_ctors.push(cc);
		return *this;
	}

	template<typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
	ClassExporter& ctor()
	{
		ConstructorClass cc(AB_FUNCTIONPR((ConstructorWrapper<T, A1, A2, A3, A4, A5, A6, A7, A8, A9>), (A1, A2, A3, A4, A5, A6, A7, A8, A9, void*), void));
		AB_PUSH_ARG(cc, A1); AB_PUSH_ARG(cc, A2); AB_PUSH_ARG(cc, A3); AB_PUSH_ARG(cc, A4); AB_PUSH_ARG(cc, A5); AB_PUSH_ARG(cc, A6); AB_PUSH_ARG(cc, A7); AB_PUSH_ARG(cc, A8); AB_PUSH_ARG(cc, A9);         
		this->_ctors.push(cc);
		return *this;
	}

	template<typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10>
	ClassExporter& ctor()
	{
		ConstructorClass cc(AB_FUNCTIONPR((ConstructorWrapper<T, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>), (A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, void*), void));
		AB_PUSH_ARG(cc, A1); AB_PUSH_ARG(cc, A2); AB_PUSH_ARG(cc, A3); AB_PUSH_ARG(cc, A4); AB_PUSH_ARG(cc, A5); AB_PUSH_ARG(cc, A6); AB_PUSH_ARG(cc, A7); AB_PUSH_ARG(cc, A8); AB_PUSH_ARG(cc, A9); AB_PUSH_ARG(cc, A10);         
		this->_ctors.push(cc);
		return *this;
	}

	template<typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11>
	ClassExporter& ctor()
	{
		ConstructorClass cc(AB_FUNCTIONPR((ConstructorWrapper<T, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11>), (A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, void*), void));
		AB_PUSH_ARG(cc, A1); AB_PUSH_ARG(cc, A2); AB_PUSH_ARG(cc, A3); AB_PUSH_ARG(cc, A4); AB_PUSH_ARG(cc, A5); AB_PUSH_ARG(cc, A6); AB_PUSH_ARG(cc, A7); AB_PUSH_ARG(cc, A8); AB_PUSH_ARG(cc, A9); AB_PUSH_ARG(cc, A10); AB_PUSH_ARG(cc, A11);         
		this->_ctors.push(cc);
		return *this;
	}

	template<typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12>
	ClassExporter& ctor()
	{
		ConstructorClass cc(AB_FUNCTIONPR((ConstructorWrapper<T, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12>), (A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, void*), void));
		AB_PUSH_ARG(cc, A1); AB_PUSH_ARG(cc, A2); AB_PUSH_ARG(cc, A3); AB_PUSH_ARG(cc, A4); AB_PUSH_ARG(cc, A5); AB_PUSH_ARG(cc, A6); AB_PUSH_ARG(cc, A7); AB_PUSH_ARG(cc, A8); AB_PUSH_ARG(cc, A9); AB_PUSH_ARG(cc, A10); AB_PUSH_ARG(cc, A11); AB_PUSH_ARG(cc, A12);         
		this->_ctors.push(cc);
		return *this;
	}

	template<typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12, typename A13>
	ClassExporter& ctor()
	{
		ConstructorClass cc(AB_FUNCTIONPR((ConstructorWrapper<T, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13>), (A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, void*), void));
		AB_PUSH_ARG(cc, A1); AB_PUSH_ARG(cc, A2); AB_PUSH_ARG(cc, A3); AB_PUSH_ARG(cc, A4); AB_PUSH_ARG(cc, A5); AB_PUSH_ARG(cc, A6); AB_PUSH_ARG(cc, A7); AB_PUSH_ARG(cc, A8); AB_PUSH_ARG(cc, A9); AB_PUSH_ARG(cc, A10); AB_PUSH_ARG(cc, A11); AB_PUSH_ARG(cc, A12); AB_PUSH_ARG(cc, A13);         
		this->_ctors.push(cc);
		return *this;
	}

	template<typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12, typename A13, typename A14>
	ClassExporter& ctor()
	{
		ConstructorClass cc(AB_FUNCTIONPR((ConstructorWrapper<T, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14>), (A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, void*), void));
		AB_PUSH_ARG(cc, A1); AB_PUSH_ARG(cc, A2); AB_PUSH_ARG(cc, A3); AB_PUSH_ARG(cc, A4); AB_PUSH_ARG(cc, A5); AB_PUSH_ARG(cc, A6); AB_PUSH_ARG(cc, A7); AB_PUSH_ARG(cc, A8); AB_PUSH_ARG(cc, A9); AB_PUSH_ARG(cc, A10); AB_PUSH_ARG(cc, A11); AB_PUSH_ARG(cc, A12); AB_PUSH_ARG(cc, A13); AB_PUSH_ARG(cc, A14);         
		this->_ctors.push(cc);
		return *this;
	}

	///
	/// Destructor
	///
	ClassExporter& dtor()
	{
		this->_dtor = AB_FUNCTION(&DestructorWrapper<T>);
		this->_dtorset = true;
		return *this;
	}

	///
	/// Dummy destructor
	///
	ClassExporter& dtor_dummy()
	{
		this->_dtor = AB_FUNCTION(&DummyDestructor);
		this->_dtorset = true;
		return *this;
	}

	///
	/// Dummy constructor
	///
	ClassExporter& ctor_dummy()
	{
		this->_ctor = AB_FUNCTION(&DummyConstructor);
		this->_ctorset = true;
		return *this;
	}

	///
	/// Methods
	///

	template<typename R>
	ClassExporter& method(std::string name, R (T::*func)())
	{
		MethodClass mthd(name, Type<R>::toString(), AB_METHOD(T, (), R, func));
		this->_methods.push(mthd);
		return *this;
	}

	template<typename R, typename A1>
	ClassExporter& method(std::string name, R (T::*func)(A1))
	{
		MethodClass mthd(name, Type<R>::toString(), AB_METHOD(T, (A1), R, func));
		AB_PUSH_ARG(mthd, A1);         
		this->_methods.push(mthd);
		return *this;
	}

	template<typename R, typename A1, typename A2>
	ClassExporter& method(std::string name, R (T::*func)(A1, A2))
	{
		MethodClass mthd(name, Type<R>::toString(), AB_METHOD(T, (A1, A2), R, func));
		AB_PUSH_ARG(mthd, A1); AB_PUSH_ARG(mthd, A2);         
		this->_methods.push(mthd);
		return *this;
	}

	template<typename R, typename A1, typename A2, typename A3>
	ClassExporter& method(std::string name, R (T::*func)(A1, A2, A3))
	{
		MethodClass mthd(name, Type<R>::toString(), AB_METHOD(T, (A1, A2, A3), R, func));
		AB_PUSH_ARG(mthd, A1); AB_PUSH_ARG(mthd, A2); AB_PUSH_ARG(mthd, A3);         
		this->_methods.push(mthd);
		return *this;
	}

	template<typename R, typename A1, typename A2, typename A3, typename A4>
	ClassExporter& method(std::string name, R (T::*func)(A1, A2, A3, A4))
	{
		MethodClass mthd(name, Type<R>::toString(), AB_METHOD(T, (A1, A2, A3, A4), R, func));
		AB_PUSH_ARG(mthd, A1); AB_PUSH_ARG(mthd, A2); AB_PUSH_ARG(mthd, A3); AB_PUSH_ARG(mthd, A4);         
		this->_methods.push(mthd);
		return *this;
	}

	template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5>
	ClassExporter& method(std::string name, R (T::*func)(A1, A2, A3, A4, A5))
	{
		MethodClass mthd(name, Type<R>::toString(), AB_METHOD(T, (A1, A2, A3, A4, A5), R, func));
		AB_PUSH_ARG(mthd, A1); AB_PUSH_ARG(mthd, A2); AB_PUSH_ARG(mthd, A3); AB_PUSH_ARG(mthd, A4); AB_PUSH_ARG(mthd, A5);         
		this->_methods.push(mthd);
		return *this;
	}

	template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
	ClassExporter& method(std::string name, R (T::*func)(A1, A2, A3, A4, A5, A6))
	{
		MethodClass mthd(name, Type<R>::toString(), AB_METHOD(T, (A1, A2, A3, A4, A5, A6), R, func));
		AB_PUSH_ARG(mthd, A1); AB_PUSH_ARG(mthd, A2); AB_PUSH_ARG(mthd, A3); AB_PUSH_ARG(mthd, A4); AB_PUSH_ARG(mthd, A5); AB_PUSH_ARG(mthd, A6);         
		this->_methods.push(mthd);
		return *this;
	}

	template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
	ClassExporter& method(std::string name, R (T::*func)(A1, A2, A3, A4, A5, A6, A7))
	{
		MethodClass mthd(name, Type<R>::toString(), AB_METHOD(T, (A1, A2, A3, A4, A5, A6, A7), R, func));
		AB_PUSH_ARG(mthd, A1); AB_PUSH_ARG(mthd, A2); AB_PUSH_ARG(mthd, A3); AB_PUSH_ARG(mthd, A4); AB_PUSH_ARG(mthd, A5); AB_PUSH_ARG(mthd, A6); AB_PUSH_ARG(mthd, A7);         
		this->_methods.push(mthd);
		return *this;
	}

	template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
	ClassExporter& method(std::string name, R (T::*func)(A1, A2, A3, A4, A5, A6, A7, A8))
	{
		MethodClass mthd(name, Type<R>::toString(), AB_METHOD(T, (A1, A2, A3, A4, A5, A6, A7, A8), R, func));
		AB_PUSH_ARG(mthd, A1); AB_PUSH_ARG(mthd, A2); AB_PUSH_ARG(mthd, A3); AB_PUSH_ARG(mthd, A4); AB_PUSH_ARG(mthd, A5); AB_PUSH_ARG(mthd, A6); AB_PUSH_ARG(mthd, A7); AB_PUSH_ARG(mthd, A8);         
		this->_methods.push(mthd);
		return *this;
	}

	template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
	ClassExporter& method(std::string name, R (T::*func)(A1, A2, A3, A4, A5, A6, A7, A8, A9))
	{
		MethodClass mthd(name, Type<R>::toString(), AB_METHOD(T, (A1, A2, A3, A4, A5, A6, A7, A8, A9), R, func));
		AB_PUSH_ARG(mthd, A1); AB_PUSH_ARG(mthd, A2); AB_PUSH_ARG(mthd, A3); AB_PUSH_ARG(mthd, A4); AB_PUSH_ARG(mthd, A5); AB_PUSH_ARG(mthd, A6); AB_PUSH_ARG(mthd, A7); AB_PUSH_ARG(mthd, A8); AB_PUSH_ARG(mthd, A9);         
		this->_methods.push(mthd);
		return *this;
	}

	template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10>
	ClassExporter& method(std::string name, R (T::*func)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10))
	{
		MethodClass mthd(name, Type<R>::toString(), AB_METHOD(T, (A1, A2, A3, A4, A5, A6, A7, A8, A9, A10), R, func));
		AB_PUSH_ARG(mthd, A1); AB_PUSH_ARG(mthd, A2); AB_PUSH_ARG(mthd, A3); AB_PUSH_ARG(mthd, A4); AB_PUSH_ARG(mthd, A5); AB_PUSH_ARG(mthd, A6); AB_PUSH_ARG(mthd, A7); AB_PUSH_ARG(mthd, A8); AB_PUSH_ARG(mthd, A9); AB_PUSH_ARG(mthd, A10);         
		this->_methods.push(mthd);
		return *this;
	}

	template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11>
	ClassExporter& method(std::string name, R (T::*func)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11))
	{
		MethodClass mthd(name, Type<R>::toString(), AB_METHOD(T, (A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11), R, func));
		AB_PUSH_ARG(mthd, A1); AB_PUSH_ARG(mthd, A2); AB_PUSH_ARG(mthd, A3); AB_PUSH_ARG(mthd, A4); AB_PUSH_ARG(mthd, A5); AB_PUSH_ARG(mthd, A6); AB_PUSH_ARG(mthd, A7); AB_PUSH_ARG(mthd, A8); AB_PUSH_ARG(mthd, A9); AB_PUSH_ARG(mthd, A10); AB_PUSH_ARG(mthd, A11);         
		this->_methods.push(mthd);
		return *this;
	}

	template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12>
	ClassExporter& method(std::string name, R (T::*func)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12))
	{
		MethodClass mthd(name, Type<R>::toString(), AB_METHOD(T, (A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12), R, func));
		AB_PUSH_ARG(mthd, A1); AB_PUSH_ARG(mthd, A2); AB_PUSH_ARG(mthd, A3); AB_PUSH_ARG(mthd, A4); AB_PUSH_ARG(mthd, A5); AB_PUSH_ARG(mthd, A6); AB_PUSH_ARG(mthd, A7); AB_PUSH_ARG(mthd, A8); AB_PUSH_ARG(mthd, A9); AB_PUSH_ARG(mthd, A10); AB_PUSH_ARG(mthd, A11); AB_PUSH_ARG(mthd, A12);         
		this->_methods.push(mthd);
		return *this;
	}

	template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12, typename A13>
	ClassExporter& method(std::string name, R (T::*func)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13))
	{
		MethodClass mthd(name, Type<R>::toString(), AB_METHOD(T, (A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13), R, func));
		AB_PUSH_ARG(mthd, A1); AB_PUSH_ARG(mthd, A2); AB_PUSH_ARG(mthd, A3); AB_PUSH_ARG(mthd, A4); AB_PUSH_ARG(mthd, A5); AB_PUSH_ARG(mthd, A6); AB_PUSH_ARG(mthd, A7); AB_PUSH_ARG(mthd, A8); AB_PUSH_ARG(mthd, A9); AB_PUSH_ARG(mthd, A10); AB_PUSH_ARG(mthd, A11); AB_PUSH_ARG(mthd, A12); AB_PUSH_ARG(mthd, A13);         
		this->_methods.push(mthd);
		return *this;
	}

	template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12, typename A13, typename A14>
	ClassExporter& method(std::string name, R (T::*func)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14))
	{
		MethodClass mthd(name, Type<R>::toString(), AB_METHOD(T, (A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14), R, func));
		AB_PUSH_ARG(mthd, A1); AB_PUSH_ARG(mthd, A2); AB_PUSH_ARG(mthd, A3); AB_PUSH_ARG(mthd, A4); AB_PUSH_ARG(mthd, A5); AB_PUSH_ARG(mthd, A6); AB_PUSH_ARG(mthd, A7); AB_PUSH_ARG(mthd, A8); AB_PUSH_ARG(mthd, A9); AB_PUSH_ARG(mthd, A10); AB_PUSH_ARG(mthd, A11); AB_PUSH_ARG(mthd, A12); AB_PUSH_ARG(mthd, A13); AB_PUSH_ARG(mthd, A14);         
		this->_methods.push(mthd);
		return *this;
	}

	///
	/// Accessor functions
	///

	template<typename V>
	ClassExporter& property(std::string name, V (T::*getf)() const, void (T::*setf)(V))
	{
		AccessorClass access(name, Type<V>::toString(), AB_METHOD(T, () const, V, getf), AB_METHOD(T, (V), void, setf));
		this->_accessors.push(access);
		return *this;
	}

	///
	/// Registers a member of the struct
	///
	template<typename V>
	ClassExporter& member(std::string name, V T::*offset)
	{
		MemberClass memb(name, Type<V>::toString(), *(int*)&offset);
		this->_members.push(memb);
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
	Module& _module;

protected:
	///
	/// Initializes the exporter
	///
	Exporter(Module& module);

public:
	///
	/// Starts an exporter
	///
	static Exporter Export( Module* module );

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
			exporter->finish(this->_module);
		}
	}

};

AB_END_NAMESPACE
