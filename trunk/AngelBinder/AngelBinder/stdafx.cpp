// stdafx.cpp : source file that includes just the standard includes
// AngelBinder.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

#ifdef _DEBUG
#pragma comment(lib, "angelscriptd.lib")
#pragma comment(lib, "addonsd.lib")
#else
#pragma comment(lib, "angelscript.lib")
#pragma comment(lib, "addons.lib")
#endif

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file
