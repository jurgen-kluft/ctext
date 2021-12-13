#include "xbase/x_allocator.h"
#include "xbase/x_runes.h"
#include "xtext/x_parser.h"
#include "xunittest/xunittest.h"

using namespace xcore;

extern xcore::alloc_t* gTestAllocator;

UNITTEST_SUITE_BEGIN(test_parser)
{
	UNITTEST_FIXTURE(main)
	{
		UNITTEST_FIXTURE_SETUP() 
		{
		}
		UNITTEST_FIXTURE_TEARDOWN() 
		{
		}

		UNITTEST_TEST(test_parse_ip)
		{
			runes_reader_t reader("10.0.8.9");
			combparser::utils::IPv4 ip;
            CHECK_TRUE(ip.Check(reader));
        }

		UNITTEST_TEST(test_search_email)
		{
			runes_reader_t reader("john.doe@gmail.com");
			combparser::utils::Email email;
            CHECK_TRUE(email.Check(reader));
		}
	}
}
UNITTEST_SUITE_END
