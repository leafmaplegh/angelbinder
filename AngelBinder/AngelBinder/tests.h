#pragma once

#include <angelscript.h>
#include <AngelBinder.h>
#include <scriptstdstring/scriptstdstring.h>
#include <iostream>
#include <string>

///
/// Type translator
///
AB_TRANSLATE_TYPE(std::string, "string")

#define TEST_ASSERT(cond, message) \
	if(!(cond)) \
		throw std::exception(message);

#define TEST_DECL(name) \
	bool test_##name(std::string testname = #name, std::string testfile = ".\\Scripts\\" #name ".as");

#define TEST_START(name) \
	bool test_##name(std::string testname, std::string testfile) \
	{ \
		Engine engine(&scriptOutput); \
		TEST_LOG("Testing '" + testname + "' ...\n"); \
		try \
		{ \
			RegisterStdString(engine.asEngine()); \
			Module* module = engine.createModule("module_" #name); \
			module->compile(testfile);

#define TEST_END \
			TEST_LOG("Test SUCCEED\n"); \
			TEST_LOG("------------------------------------------------\n\n"); \
			return true; \
		} \
		catch(std::exception& ex) \
		{ \
			TEST_LOG("\nTest FAILED"); \
			TEST_LOG("Reason: " + std::string(ex.what())); \
			TEST_LOG("------------------------------------------------\n\n"); \
			return false; \
		} \
		return true; \
	}

#define TEST_LOG(message)	scriptOutput(&engine, std::string(message));
#define TEST_CALL(name) test_##name()
#define TEST_INIT()	scriptOutputInit()

///
/// TESTS
///
TEST_DECL(functions)

void scriptOutputInit();
void scriptOutput(AB_NAMESPACE_QUALIFIER Engine* engine, std::string message);