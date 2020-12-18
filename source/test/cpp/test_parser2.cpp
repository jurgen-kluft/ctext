#include "xbase/x_allocator.h"
#include "xbase/x_runes.h"
#include "xtext/x_parser2.h"
#include "xunittest/xunittest.h"

using namespace xcore;

namespace xcore
{
	extern alloc_t* gTestAllocator;
	namespace xparser
	{
		extern void use_case_parser2();
	}
}

UNITTEST_SUITE_BEGIN(test_parser2)
{
	UNITTEST_FIXTURE(main)
	{
		UNITTEST_FIXTURE_SETUP() 
		{
		}
		UNITTEST_FIXTURE_TEARDOWN() 
		{
		}

		UNITTEST_TEST(test_parse_1)
		{
			xparser::use_case_parser2();
		}

	}
}
UNITTEST_SUITE_END
