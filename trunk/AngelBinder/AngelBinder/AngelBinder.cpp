//
//     Copyright © 2011 - João Francisco Biondo Trinca
//          a.k.a WoLfulus <wolfulus@gmail.com>
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//

#include <angelbinder.h>

#define AB_INIT_CHECK()		AB_SCRIPT_ASSERT(this->_engine != NULL && this->_builder != NULL, "Script not initialized.", AB_THROW, this)
#define AB_UNINIT_CHECK()	AB_SCRIPT_ASSERT(this->_engine == NULL && this->_builder == NULL, "Script already initialized.", AB_THROW, this)

#ifdef AS_USE_NAMESPACE
using namespace AngelScript;
#endif

AB_BEGIN_NAMESPACE
 
/**
 * Engine
 **/

void Engine::initialize()
{
	AB_UNINIT_CHECK()
	{
		this->_engine = asCreateScriptEngine(ANGELSCRIPT_VERSION);
		AB_SCRIPT_ASSERT(this->_engine != NULL, "Could not create AngelScript instance.", AB_THROW, this);

		this->_builder = new CScriptBuilder();
		AB_SCRIPT_ASSERT(this->_builder != NULL, "Could not create CScriptBuilder instance.", AB_THROW, this);

		this->_engine->SetMessageCallback(asFUNCTION(&Engine::onScriptMessage), this, asCALL_CDECL);
		this->_engine->SetEngineProperty(asEP_SCRIPT_SCANNER, 0);
	}
}

void Engine::uninitialize()
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

void __cdecl Engine::onScriptMessage( const asSMessageInfo *msg, void *param )
{
	Engine* engine = (Engine*)param;
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

Engine::Engine()
	: _builder(NULL), _engine(NULL), _messages(NULL)
{
	this->initialize();
}

Engine::Engine( MessageCallback callback )
	: _builder(NULL), _engine(NULL), _messages(callback)
{
	this->initialize();
}

Engine::~Engine()
{
	this->uninitialize();
}

ASEngine* Engine::asEngine()
{
	return this->_engine;
}

ASBuilder* Engine::asBuilder()
{
	return this->_builder;
}

Engine::MessageCallback& Engine::onMessage()
{
	return this->_messages;
}

Module* Engine::createModule( std::string name )
{
	std::hash_map<std::string, Module*>::iterator it = this->_modules.find(name);
	if(it != this->_modules.end())
	{
		return it->second;
	}
	Module* module = new Module(*this, name);
	if(module == NULL)
	{
		throw std::exception("Couldn't create script module instance.");
	}
	this->_modules[name] = module;
	return module;
}

Module* Engine::getModule( std::string name )
{
	std::hash_map<std::string, Module*>::iterator it = this->_modules.find(name);
	if(it != this->_modules.end())
	{
		return it->second;
	}
	else
	{
		throw std::exception("Couldn't create script module instance.");
	}
}

/**
 * Script Module
 **/

Module::Module(Engine& engine, std::string module)
	: _engine(engine), _name(module)
{
	int r = this->_engine.asBuilder()->StartNewModule(this->_engine.asEngine(), this->_name.c_str());
	AB_SCRIPT_ASSERT(r >= 0, "Failed to create new script module.", AB_THROW, &engine);
}

Module::~Module(void)
{
}

bool Module::compile ( std::string file )
{
	int r = 0;

	r = this->_engine.asBuilder()->AddSectionFromFile(file.c_str()); 
	AB_SCRIPT_ASSERT(r >= 0, std::string("Couldn't add script from file '" + file + "'").c_str(), AB_THROW, &this->_engine);

	r = this->_engine.asBuilder()->BuildModule(); 
	AB_SCRIPT_ASSERT(r >= 0, "Couldn't build script file.", AB_THROW, &this->_engine);

	return true;
}

Engine& Module::engine()
{
	return this->_engine;
}

/**
 * Extras
 **/

Exporter::Exporter( Module& module ) 
	: _module(module)
{
}

Exporter Exporter::Export( Module* module )
{
	Exporter exporter(*module);
	return exporter;
}

FunctionExporter Exporter::Functions()
{
	FunctionExporter exporter;
	return exporter;
}

VariableExporter Exporter::Variables()
{
	VariableExporter exporter;
	return exporter;
}

void FunctionExporter::finish( Module& instance )
{
	while(!this->_entries.empty())
	{
		FunctionClass function = this->_entries.front();
		std::string decomposition = function.decompose();
		AB_MESSAGE_INVOKE_STATIC(&instance.engine(), &instance.engine(), "Registering function '" + function.name() + "' as '" + decomposition + "'");
		int r = instance.engine().asEngine()->RegisterGlobalFunction(decomposition.c_str(), function.address(), function.convention());
		AB_SCRIPT_ASSERT(r >= 0, std::string("Error registering function '" + function.name() + "'").c_str(), AB_THROW, &instance.engine());
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

asSFuncPtr FunctionClass::address()
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

FunctionClass::FunctionClass( std::string ret, std::string name, CallingConvention conv, asSFuncPtr func ) 
	: _name(name), _ret(ret), _func(func), _conv(conv)
{
}

CallingConvention FunctionClass::convention()
{
	return this->_conv;
}

VariableExporter::VariableExporter()
{
}

void VariableExporter::finish( Module& instance )
{
	while(!this->_entries.empty())
	{
		VariableClass memb = this->_entries.front();
		AB_MESSAGE_INVOKE_STATIC(&instance.engine(), &instance.engine(), "Registering global var '" + memb.name() + "' as '" + memb.type() + "'");
		int r = instance.engine().asEngine()->RegisterGlobalProperty(memb.decompose().c_str(), memb.address());
		AB_SCRIPT_ASSERT(r >= 0, std::string("Error registering global var '" + memb.name() + "'").c_str(), AB_THROW, &instance.engine());
		this->_entries.pop();
	}
}

VariableClass::VariableClass( std::string type, std::string name, void* address ) 
	: _type(type), _address(address), _name(name)
{
}

std::string VariableClass::decompose()
{
	std::string ret = this->_type + " " + this->_name;
	return ret;
}

void* VariableClass::address()
{
	return this->_address;
}

std::string VariableClass::name()
{
	return this->_name;
}

std::string VariableClass::type()
{
	return this->_type;
}

void DummyConstructor( void* memory )
{
}

void DummyDestructor( void* memory )
{
}


Context::Context( Engine& engine, int function ) : _engine(engine), _function(function), _params(0)
{
	this->_context = engine.asEngine()->CreateContext();
	AB_SCRIPT_ASSERT(this->_context != NULL, "Context couldn't be created.", AB_THROW, &engine);
	this->_context->SetExceptionCallback(asMETHOD(Context, exceptionCallback), this, asCALL_THISCALL);
	this->_context->Prepare(function);
}

Context::~Context()
{
	this->_context->Release();
}

void Context::exceptionCallback( asIScriptContext *context )
{
	int col;  
	std::stringstream error;
	int row = context->GetExceptionLineNumber(&col);
	error <<  "A script exception occurred: "  << context->GetExceptionString() << " at position (" << row << ',' << col << ')';
	throw std::exception(error.str().c_str());
}

void Context::execute()
{
	int r = this->_context->Execute();
	AB_SCRIPT_ASSERT(r == asEXECUTION_FINISHED, "Error executing call to script.", AB_THROW, &this->_engine);
}

void Context::setAddress( void* value )
{
	this->_context->SetArgAddress(this->_params++, value);
}

void Context::setObject( void* value )
{
	this->_context->SetArgObject(this->_params++, value);
}

void Context::setByte( asBYTE value )
{
	this->_context->SetArgByte(this->_params++, value);
}

void Context::setWord( asWORD value )
{
	this->_context->SetArgWord(this->_params++, value);
}

void Context::setDWord( unsigned int value )
{
	this->_context->SetArgDWord(this->_params++, value);
}

void Context::setQWord( unsigned long long value )
{
	this->_context->SetArgQWord(this->_params++, value);
}

void Context::setFloat( float value )
{
	this->_context->SetArgFloat(this->_params++, value);
}

void Context::setDouble( double value )
{
	this->_context->SetArgDouble(this->_params++, value);
}

void* Context::readAddress()
{
	return this->_context->GetReturnAddress();
}

void* Context::readObject()
{
	return this->_context->GetReturnObject();
}

asBYTE Context::readByte()
{
	return this->_context->GetReturnByte();
}

asWORD Context::readWord()
{
	return this->_context->GetReturnWord();
}

asQWORD Context::readQWord()
{
	return this->_context->GetReturnQWord();
}

asDWORD Context::readDWord()
{
	return this->_context->GetReturnDWord();
}

float Context::readFloat()
{
	return this->_context->GetReturnFloat();
}

double Context::readDouble()
{
	return this->_context->GetReturnDouble();
}

AB_END_NAMESPACE
