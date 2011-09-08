#include "stdafx.h"
#include <fstream>
#include "tests.h"

void scriptOutputInit()
{
	std::ofstream f("AngelBinder.log", std::ios::trunc);
	f << "Starting AngelBinder Tests" << std::endl;
	f << "-----------------------------------------" << std::endl << std::endl;
	f.close();
}

void scriptOutput(Engine* engine, std::string message)
{
	std::cout << message << std::endl;
	std::ofstream f("AngelBinder.log", std::ios::app);
	if(f) {
		f << message << std::endl;
	}
}