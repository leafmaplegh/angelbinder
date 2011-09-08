// AngelBindTest.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <iostream>
#include "tests.h"

int pause();

int main(int argc, char* argv[])
{
	TEST_INIT();
	TEST_CALL(functions);
	return pause();
}

int pause()
{
	std::cin.clear();
	std::cout << std::endl << "Press enter to continue...\n";
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	return 0;
}