//
//     Copyright © 2011 - João Francisco Biondo Trinca
//          a.k.a WoLfulus <wolfulus@gmail.com>
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//

#include <angelbinder.h>

#define AB_INIT_CHECK()		AB_SCRIPT_ASSERT(this->_engine != NULL && this->_builder != NULL, "Script not initialized.", AB_THROW)
#define AB_UNINIT_CHECK()	AB_SCRIPT_ASSERT(this->_engine == NULL && this->_builder == NULL, "Script already initialized.", AB_THROW)

AB_BEGIN_NAMESPACE
 
/**
 * Scripts
 **/

Script::Script()
	: _engine(NULL),
	_builder(NULL),
	_module("script")
#ifndef AB_USE_EVENTSYSTEM
	, _messages(false)
#endif
{
	this->initialize();
}

Script::Script(std::string module)
	: _engine(NULL),
	  _builder(NULL),
	  _module(module)
#ifndef AB_USE_EVENTSYSTEM
	, _messages(false)
#endif
{
	this->initialize();
}

Script::~Script(void)
{
	this->uninitialize();
}

void Script::initialize()
{
	AB_UNINIT_CHECK()
	{
		this->_engine = asCreateScriptEngine(ANGELSCRIPT_VERSION);
		AB_SCRIPT_ASSERT(this->_engine != NULL, "Could not create AngelScript instance.", AB_THROW);

		this->_builder = new CScriptBuilder();
		AB_SCRIPT_ASSERT(this->_builder != NULL, "Could not create CScriptBuilder instance.", AB_THROW);

		this->_engine->SetMessageCallback(asFUNCTION(&Script::onScriptMessage), this, asCALL_CDECL);
		this->_engine->SetEngineProperty(asEP_SCRIPT_SCANNER, 0);
	}
}

void Script::uninitialize()
{
	this->_functions.clear();
	if(this->_builder != NULL)
	{
		delete this->_builder;
	}
	this->_engine = NULL;
	if(this->_engine != NULL)
	{
		this->_engine->Release();
	}
	this->_engine = NULL;
}

bool Script::compileFile ( std::string file )
{
	AB_INIT_CHECK()
	{
		int r = 0;

		r = this->_builder->StartNewModule(this->_engine, this->_module.c_str()); 
		AB_SCRIPT_ASSERT(r >= 0, "Could not start a new script module.", AB_THROW);

		r = this->_builder->AddSectionFromFile(file.c_str()); 
		AB_SCRIPT_ASSERT(r >= 0, "Could not add script section from file.", AB_THROW);

		r = this->_builder->BuildModule(); 
		AB_SCRIPT_ASSERT(r >= 0, "Could not build script.", AB_THROW);
	}
	return true;
}


bool Script::compileScript ( std::string script )
{
	AB_INIT_CHECK()
	{
		int r = 0;
		r = this->_builder->StartNewModule(this->_engine, this->_module.c_str()); 
		AB_SCRIPT_ASSERT(r >= 0, "Could not start a new script module.", AB_THROW);

		r = this->_builder->AddSectionFromMemory(script.c_str(), this->_module.c_str()); 
		AB_SCRIPT_ASSERT(r >= 0, "Could not add script section from memory.", AB_THROW);

		r = this->_builder->BuildModule(); 
		AB_SCRIPT_ASSERT(r >= 0, "Could not build script.", AB_THROW);
	}
	return true;
}

void Script::onScriptMessage( const asSMessageInfo *msg, void *param )
{
	Script* engine = (Script*)param;
	if(engine != NULL)
	{
		std::stringstream stream;
		switch(msg->type)
		{
		case asMSGTYPE_ERROR:
			stream << "[Script Error] ";
			break;
		case asMSGTYPE_INFORMATION:
			stream << "[Information] ";
			break;
		case asMSGTYPE_WARNING:
			stream << "[Warning] ";
			break;
		}
		stream << "Line (" << msg->row << ", " << msg->col << ") : " << msg->message;
		AB_MESSAGE_INVOKE_STATIC(engine, engine, stream.str().c_str());
	}
}

Engine& Script::engine()
{
	AB_INIT_CHECK()
	{
		return *this->_engine;
	}
}

Builder& Script::builder()
{
	AB_INIT_CHECK()
	{
		return *this->_builder;
	}
}

Script::MessageCallback& Script::messages()
{
	return this->_messages;
}

/**
 * Extras
 **/

Exporter::Exporter( Script& script ) 
	: _script(script)
{
}

AngelBinder::Exporter Exporter::Export( Script& script )
{
	Exporter exporter(script);
	return exporter;
}

AngelBinder::FunctionExporter Exporter::Functions()
{
	FunctionExporter exporter;
	return exporter;
}

void FunctionExporter::finish( Script& instance )
{
	while(!this->_entries.empty())
	{
		FunctionClass function = this->_entries.front();
		std::string decomposition = function.decompose();
		AB_MESSAGE_INVOKE_STATIC(&instance, &instance, "Register function '" + function.name() + "' as '" + decomposition + "'");
		int r = instance.engine().RegisterGlobalFunction(decomposition.c_str(), asFUNCTION(function.address()), function.convention());
		AB_SCRIPT_ASSERT_STATIC(r >= 0, std::string("Error registering function '" + function.name() + "'").c_str(), AB_THROW, &instance);
		this->_entries.pop();
	}
}

FunctionExporter::FunctionExporter()
{
}

/**
 * Function methods
 **/

std::string FunctionClass::decompose()
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

void* FunctionClass::address()
{
	return this->_func;
}

std::vector<std::string>& FunctionClass::parameters()
{
	return this->_params;
}

std::string FunctionClass::name()
{
	return this->_name;
}

FunctionClass::FunctionClass( std::string ret, std::string name, CallingConvention conv, void* func ) 
	: _name(name), _ret(ret), _func(func), _conv(conv)
{
}

AngelBinder::CallingConvention FunctionClass::convention()
{
	return this->_conv;
}

StructExporter::StructExporter( std::string name, int size ) 
	: _name(name), _size(size)
{
}

void StructExporter::finish( Script& instance )
{
	AB_MESSAGE_INVOKE_STATIC(&instance, &instance, "Registering type '" + this->_name + "'");
	int r = instance.engine().RegisterObjectType(this->_name.c_str(), this->_size, asOBJ_VALUE | asOBJ_POD);
	while(!this->_entries.empty())
	{
		StructMember memb = this->_entries.front();
		AB_MESSAGE_INVOKE_STATIC(&instance, &instance, "Registering member '" + this->_name + "::" + memb.name + "' as '" + memb.type + "'");
		r = instance.engine().RegisterObjectProperty(this->_name.c_str(), std::string(memb.type + " " + memb.name).c_str(), memb.offset);
		AB_SCRIPT_ASSERT_STATIC(r >= 0, std::string("Error registering member '" + this->_name + "::" + memb.name + "'").c_str(), AB_THROW, &instance);
		this->_entries.pop();
	}
}

AB_END_NAMESPACE
