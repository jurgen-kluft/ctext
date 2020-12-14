#include "xbase/x_allocator.h"
#include "xbase/x_runes.h"
#include "xtext/x_parser.h"
#include "xunittest/xunittest.h"

using namespace xcore;

extern xcore::xalloc* gTestAllocator;

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
        }

		UNITTEST_TEST(test_parse_2)
		{
		}
	}
}
UNITTEST_SUITE_END
