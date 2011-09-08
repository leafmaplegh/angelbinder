#include "stdafx.h"
#include "tests.h"

#ifdef AS_USE_NAMESPACE
using namespace AngelBinder
#endif

#define TEST_FLOATS(var,value,eps) (fabs((var) - (value)) < eps)

TEST_START(functions)

	std::string decomposition = Function<int(float,bool)>::decompose("function");

	Function<int(int,int)> sumi = module->getFunction<int(int,int)>("sum");
	Function<float(float,float)> sumf = module->getFunction<float(float,float)>("sum");
	Function<int(int,int)> sub = module->getFunction<int(int,int)>("sub");
	Function<int(int,int)> div = module->getFunction<int(int,int)>("div");
	Function<int(int,int)> mul = module->getFunction<int(int,int)>("mul");
	Function<int(int,int)> mod = module->getFunction<int(int,int)>("mod");
	Function<int(int,int)> pow = module->getFunction<int(int,int)>("pow");

	int sumint_res = sumi(13, 17);
	TEST_ASSERT(sumint_res == 30, "Sum function failed.");

	float sumfloat_res = sumf(13.5f, 17.8f);
	TEST_ASSERT(TEST_FLOATS(sumfloat_res, 31.3f, 0.01f), "Sum function failed.");

	int sub_res = sub(30, 17);
	TEST_ASSERT(sub_res == 13, "Sub function failed.");

	int div_res = div(15, 5);
	TEST_ASSERT(div_res == 3, "Div function failed.");

	int mul_res = mul(15, 5);
	TEST_ASSERT(mul_res == 75, "Mul function failed.");

	int mod_res = mod(15, 4);
	TEST_ASSERT(mod_res == 3, "Mod function failed.");

	int pow_res = pow(7, 3);
	TEST_ASSERT(pow_res == 343, "Pow function failed.");

TEST_END