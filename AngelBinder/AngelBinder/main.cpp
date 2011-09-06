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
/// Example class
///
class Character
{
private:
	std::string _name;
	int _life;

public:
	int _mana;

public:
	Character()
		: _name("Unnamed"), _life(100)
	{
	}

	~Character()
	{
	}

	std::string getName() const
	{
		return this->_name;
	}

	void setName(std::string val)
	{
		this->_name = val;
	}

	void say(std::string& message)
	{
		std::cout << this->_name << ": " << message << std::endl;
	}

	void hit()
	{
		this->_life -= rand() % 10;
		std::cout << "[" << this->_name << ": Damn it!!!!" << std::endl;
	}

	void heal()
	{
		this->_life += rand() % 10;
		std::cout << "[" << this->_name << "] Ahh, that feels good!" << std::endl;
	}

	void kill()
	{
		std::cout << this->_name << " has been murdered." << std::endl;
	}

	bool isDead()
	{
		return this->_life <= 0;
	}

};

AB_TRANSLATE_TYPE(Character, "Character")
AB_TRANSLATE_TYPE(std::string, "string")

///
/// Message event handler
///

void onScriptMessage(Engine* script, std::string message);

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

	///
	/// Exports your global functions to the script
	///

	///
	/// Exports your global classes to the script
	///
	Exporter::Export(module)
	[
		Exporter::Class<Character>()
			.ctor()
			.dtor()
			.member("test", &Character::_mana)
			.property("name", &Character::getName, &Character::setName)
			.method("hit", &Character::hit)
			.method("heal", &Character::heal)
			.method("isdead", &Character::isDead)
			.method("kill", &Character::kill)
			.method("say", &Character::say)
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

void onScriptMessage(Engine* script, std::string message)
{
	cout << "[Script] " << message << endl;
}

void log(std::string message)
{
	cout << "[Script Log] " << message << endl;
}