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
#define AB_USE_EVENTSYSTEM

///
/// 1st step: include the header.
///
#include <AngelBinder.h>

//
// 2nd step: 
// Translates all the types you need to be different 
// from its name, or that is contained in a namespace.
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

AB_TRANSLATE_TYPE(std::string, "MyClass")

//
// 3rd step:
// "use" the namespace of AngelBinder or you'll need to type alot of 
// "AngelBinder::" before the functions
//

using namespace AngelBinder;

///
/// Exported function prototypes. Implementation in the end of the 
/// file, to keep the example clear.
///

void test1();
void test2();
void test3();

///
/// Usage Example
///

int main(int argc, char* argv[])
{
	
	///
	/// Declares the script engine instance
	///

	Script script("MyModuleName");

	///
	/// You'll be able to use the traditional way to type-register
	///

	RegisterStdString(&script.engine());

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
			.def(Function("test1", &test1)) 
			.def(Function("test2", &test2))
			.def(Function("test3", &test3))
	];

	///  ---------- Planned "TODO" list Below ------------- 

	/*

	Exporter::Export(script)
		[
			Exporter::Struct<MyStruct>()
				.member("member1", MyStruct::member1)
				.member("member1", MyStruct::member1)
				.member("member1", MyStruct::member1)
		];

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

void test1()
{
	cout << "Hello, ";
}

void test2()
{
	cout << "World!";
}

void test3()
{
	cout << "\n";
}