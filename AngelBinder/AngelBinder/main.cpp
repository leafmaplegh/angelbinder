// AngelBindTest.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <scriptstdstring/scriptstdstring.h>

#include <iostream>
#include <string>
using namespace std;

///
/// include the header.
///
#include <AngelBinder.h>

#ifdef AS_USE_NAMESPACE
using namespace AngelScript;
using namespace AngelBinder;
#endif

///
/// Exported function/object prototypes.
///

class MyClass
{
private:
	int test;

public:
	MyClass()
		: test(0)
	{
	}

	MyClass(int val)
		: test(val)
	{
	}

	~MyClass()
	{
	}

	void method1()
	{
		this->test = 0;
	}

	int method2()
	{
		return rand();
	}

	void method3(int x)
	{
		this->test = x;
	}

	int method4(int y)
	{
		return y * this->test;
	}

};

int				g_var1;
unsigned int	g_var2;
short			g_var3;
std::string		g_var4;

int sum(int a, int b)
{
	return a + b;
}

int subtract(int a, int b)
{
	return a - b;
}

int multiply(int a, int b)
{
	return a / b;
}

int divide(int a, int b)
{
	return a / b;
}

void log(std::string message)
{
	cout << "[Script Log] " << message << endl;
}

//
// Translates all the types you need to be different 
// from its name, that is contained in a namespace, or for structs.
// 
// if I define, MyClass this way, it doesn't need to be translated:
// 
// class MyClass 
// {
//   ...
// };
// 
// but if I define it this way, it must be translated:
//
// namespace MyNamespace
// {
//   class MyClass
//   {
//     ...
//   };
// }
//
// with AB_TRANSLATE_TYPE(MyNamespace::MyClass, "MyClass")
//
// structs also needs to be translated, or you'll get excpetions 
// when exporting them.
// 
// typedef struct {
//   int x;
// } MyStruct;
//
// AB_TRANSLATE_TYPE(MyStruct, "MyStruct")
//

AB_TRANSLATE_TYPE(MyClass, "myclass")
AB_TRANSLATE_TYPE(std::string, "string")

///
/// Message event handler
///

void onScriptMessage(Engine* script, std::string message)
{
	cout << "[Script] " << message << endl;
}

///
/// Usage Example
///

int main(int argc, char* argv[])
{

	///
	/// Declares the engine
	///

	Engine engine;

	///
	/// You'll be able to access the basic AngelScript engine and builder
	///

	RegisterStdString(engine.asEngine());

	///
	/// Setup the message outputs if you want to
	///

	engine.onMessage() = &onScriptMessage;

	///
	/// Declares the module instance
	///

	Module* module = engine.CreateModule("MyModule");

	///
	/// Exports your global variables to the script
	///

	Exporter::Export(module)
	[
		Exporter::Variables()
			.def("g_int", &g_var1)
			.def("g_uint", &g_var2)
			.def("g_short", &g_var3)
			.def("g_string", &g_var4)
	];

	///
	/// Exports your global functions to the script
	///

	Exporter::Export(module)
	[
		Exporter::Functions()
			.def("sum", &sum) 
			.def("subtract", &subtract)
			.def("multiply", &multiply)
			.def("divide", &divide)
	];

	///
	/// Exports your global classes to the script
	///
	Exporter::Export(module)
	[
		Exporter::Class<MyClass>(/* INITIAL FLAGS HERE */)
			.ctor()
			.ctor<int>()			
			.dtor()
			.method("method1", &MyClass::method1)
			.method("method2", &MyClass::method2)
			.method("method3", &MyClass::method3)
			.method("method4", &MyClass::method4)
	];

	///
	/// Compiles a file
	///

	module->compile("AngelBinder.as");

	///  ---------- Planned "TODO" list Below ------------- 

	return 0;

}

///
/// Implementations
///


