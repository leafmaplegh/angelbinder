// AngelBindTest.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <scriptstdstring/scriptstdstring.h>

#include <iostream>
#include <string>
using namespace std;

//
// 1st step:
// Configure how you want it.
//

//#define AB_USE_NAMESPACE

///
/// 1st step: include the header.
///
#include <AngelBinder.h>

//
// "use" the namespace of AngelBinder or you'll need to type alot of 
// "AngelBinder::" before the functions
//

using namespace AngelBinder;

///
/// Exported function/object prototypes. Implementation in the end of the 
/// file, to keep the example clear.
///

typedef struct {
	int member1;
	unsigned int member2;
} TestClass;

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

AB_TRANSLATE_TYPE(TestClass, "test")
AB_TRANSLATE_TYPE(std::string, "string")

///
/// Message event handler
///

void onScriptMessage(Script* script, std::string message)
{
	cout << "[Script] " << message << endl;
}

///
/// Usage Example
///

int main(int argc, char* argv[])
{
	
	///
	/// Declares the script engine instance
	///

	Script script("script");

	///
	/// Setup the message outputs if you want to
	///

	script.messages() = &onScriptMessage;

	///
	/// You'll be able to use the traditional way to type-register
	///

	RegisterStdString(&script.engine());

	///
	/// Exports your structs to the script
	///

	Exporter::Export(script)
	[
		Exporter::Struct<TestClass>()
			.member("member1", &TestClass::member1) // registers as int on offset 0
			.member("member2", &TestClass::member2) // registers as unsigned int on offset 4
	];

	///
	/// Exports your functions to the script
	///
	/// alternative syntax
	/// Exporter::Functions()
	///	   (Function("test1", &test1)) 
	///	   (Function("test2", &test2))
	///	   (Function("test3", &test3))
	///

	Exporter::Export(script)
	[
		Exporter::Functions()
			.def("sum", &sum) 
			.def("subtract", &subtract)
			.def("multiply", &multiply)
			.def("divide", &divide)
	];

	///  ---------- Planned "TODO" list Below ------------- 

	/*

	Exporter::Export(script)
	[
		Exporter::Class<MyClass>()
			.constructor()
			.constructor<int>()
			.method("method1", &MyClass::method1)
			.method("method2", &MyClass::method2)
			.property("property1", &MyClass::getProperty1, &MyClass::setProperty1)
			.property("property2", &MyClass::getProperty2, &MyClass::setProperty2)
	];

	Exporter::Export(script)
	[
		Exporter::Globals()
			.def("name", &global_variable1)
			.def("name", &global_variable2)
	];

	*/

	///
	/// Compiles a file
	///

 	script.compileFile("AngelBinder.as");

	return 0;

}

///
/// Implementations
///


