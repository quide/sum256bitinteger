#include <unittest++/UnitTest++.h>
#include "../sum256/integer256.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// To add a test, simply put the following code in the a .cpp file of your choice:
//
// =================================
// Simple Test
// =================================
//
//  TEST(YourTestName)
//  {
//  }
//
// The TEST macro contains enough machinery to turn this slightly odd-looking syntax into legal C++, and automatically register the test in a global list. 
// This test list forms the basis of what is executed by RunAllTests().
//
// If you want to re-use a set of test data for more than one test, or provide setup/teardown for tests, 
// you can use the TEST_FIXTURE macro instead. The macro requires that you pass it a class name that it will instantiate, so any setup and teardown code should be in its constructor and destructor.
//
//  struct SomeFixture
//  {
//    SomeFixture() { /* some setup */ }
//    ~SomeFixture() { /* some teardown */ }
//
//    int testData;
//  };
// 
//  TEST_FIXTURE(SomeFixture, YourTestName)
//  {
//    int temp = testData;
//  }
//
// =================================
// Test Suites
// =================================
// 
// Tests can be grouped into suites, using the SUITE macro. A suite serves as a namespace for test names, so that the same test name can be used in two difference contexts.
//
//  SUITE(YourSuiteName)
//  {
//    TEST(YourTestName)
//    {
//    }
//
//    TEST(YourOtherTestName)
//    {
//    }
//  }
//
// This will place the tests into a C++ namespace called YourSuiteName, and make the suite name available to UnitTest++. 
// RunAllTests() can be called for a specific suite name, so you can use this to build named groups of tests to be run together.
// Note how members of the fixture are used as if they are a part of the test, since the macro-generated test class derives from the provided fixture class.
//
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct fixture
{
	ceiia::integer256 full_dig1;
	ceiia::integer256 full_dig2;
	ceiia::integer256 result;
	unsigned int base;
	
	fixture() : base(16) { /* some setup */ }
	~fixture() { /* some teardown */ }
};

TEST_FIXTURE(fixture, sum256_fff_fff)
{
	full_dig1.init_from_string("fff", base);
	full_dig2.init_from_string("fff", base);
	result = full_dig1 + full_dig2;
	
	CHECK_EQUAL("0000000000000000000000000000000000000000000000000000000000001ffe", result.print());
}

TEST_FIXTURE(fixture, sum256_0_0)
{
	full_dig1.init_from_string("00000000", base);
	full_dig2.init_from_string("00000000", base);
	result = full_dig1 + full_dig2;
	
	CHECK_EQUAL("0000000000000000000000000000000000000000000000000000000000000000", result.print());
}

TEST_FIXTURE(fixture, sum256_123_123)
{
	full_dig1.init_from_string("123", base);
	full_dig2.init_from_string("123", base);
	result = full_dig1 + full_dig2;
	
	CHECK_EQUAL("0000000000000000000000000000000000000000000000000000000000000246", result.print());
}

TEST_FIXTURE(fixture, sum256_1_2)
{
	full_dig1.init_from_string("1", base);
	full_dig2.init_from_string("2", base);
	result = full_dig1 + full_dig2;
	
	CHECK_EQUAL("0000000000000000000000000000000000000000000000000000000000000003", result.print());
}

TEST_FIXTURE(fixture, sum256_1_2_v2)
{
	full_dig1.init_from_string("01", base);
	full_dig2.init_from_string("02", base);
	result = full_dig1 + full_dig2;
	
	CHECK_EQUAL("0000000000000000000000000000000000000000000000000000000000000003", result.print());
}

TEST_FIXTURE(fixture, sum256_1_2_v3)
{
	full_dig1.init_from_string("000000000000000000000000000000001", base);
	full_dig2.init_from_string("000000000000000000000000000000002", base);
	result = full_dig1 + full_dig2;
	
	CHECK_EQUAL("0000000000000000000000000000000000000000000000000000000000000003", result.print());
}

TEST_FIXTURE(fixture, sum256_fff_fff_v2)
{
	full_dig1.init_from_string("0000000000000000000000000000000000000000000000000000000000000fff", base);
	full_dig2.init_from_string("0000000000000000000000000000000000000000000000000000000000000fff", base);
	result = full_dig1 + full_dig2;
	
	CHECK_EQUAL("0000000000000000000000000000000000000000000000000000000000001ffe", result.print());
}

TEST_FIXTURE(fixture, sum256_int1_too_big)
{
	full_dig1.init_from_string("10000000000000000000000000000000000000000000000000000000000000fff", base);
	full_dig2.init_from_string("0000000000000000000000000000000000000000000000000000000000000fff", base);
	result = full_dig1 + full_dig2;
	
	CHECK_EQUAL("0000000000000000000000000000000000000000000000000000000000000000 Invalid value!", result.print());
}

TEST_FIXTURE(fixture, sum256_max_values)
{
	full_dig1.init_from_string("ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff", base);
	full_dig2.init_from_string("ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff", base);
	result = full_dig1 + full_dig2;
	
	CHECK_EQUAL("fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffe (overflown)", result.print());
}

TEST_FIXTURE(fixture, sum256_max_value_0)
{
	full_dig1.init_from_string("ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff", base);
	full_dig2.init_from_string("0", base);
	result = full_dig1 + full_dig2;
	
	CHECK_EQUAL("ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff", result.print());
}

TEST_FIXTURE(fixture, sum256_0_max_value)
{
	full_dig1.init_from_string("0", base);
	full_dig2.init_from_string("ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff", base);
	result = full_dig1 + full_dig2;
	
	CHECK_EQUAL("ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff", result.print());
}

TEST_FIXTURE(fixture, sum256_max_value_fff)
{
	full_dig1.init_from_string("ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff", base);
	full_dig2.init_from_string("fff", base);
	result = full_dig1 + full_dig2;
	
	CHECK_EQUAL("0000000000000000000000000000000000000000000000000000000000000ffe (overflown)", result.print());
}

TEST_FIXTURE(fixture, sum256_max_value_1)
{
	full_dig1.init_from_string("ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff", base);
	full_dig2.init_from_string("1", base);
	result = full_dig1 + full_dig2;
	
	CHECK_EQUAL("0000000000000000000000000000000000000000000000000000000000000000 (overflown)", result.print());
}

TEST_FIXTURE(fixture, sum256_reach_max_value)
{
	full_dig1.init_from_string("fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffe", base);
	full_dig2.init_from_string("1", base);
	result = full_dig1 + full_dig2;
	
	CHECK_EQUAL("ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff", result.print());
}

TEST_FIXTURE(fixture, sum256_invalid_digits)
{
	full_dig1.init_from_string("invalid_digits", base);
	full_dig2.init_from_string("1234", base);
	result = full_dig1 + full_dig2;
	
	CHECK_EQUAL("0000000000000000000000000000000000000000000000000000000000000000 Invalid value!", result.print());
}

TEST_FIXTURE(fixture, sum256_softer_invalid_digits)
{
	full_dig1.init_from_string("fgf", base);
	full_dig2.init_from_string("1234", base);
	result = full_dig1 + full_dig2;
	
	CHECK_EQUAL("0000000000000000000000000000000000000000000000000000000000000000 Invalid value!", result.print());
}

TEST_FIXTURE(fixture, sum256_uppercase_and_sequence)
{
	full_dig1.init_from_string("0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF", base);
	full_dig2.init_from_string("0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF", base);
	result = full_dig1 + full_dig2;
	
	CHECK_EQUAL("02468acf13579bde02468acf13579bde02468acf13579bde02468acf13579bde", result.print());
}

TEST_FIXTURE(fixture, sum256_run_lots_of_times)
{
	for(int i = 0;  i<100000;  ++i)
	{
		full_dig1.init_from_string("0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF", base);
		full_dig2.init_from_string("0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF", base);
		result = full_dig1 + full_dig2;
	
		CHECK_EQUAL("02468acf13579bde02468acf13579bde02468acf13579bde02468acf13579bde", result.print());
	}
}
  
 
// run all tests
int main(int argc, char **argv)
{
	return UnitTest::RunAllTests();
}


