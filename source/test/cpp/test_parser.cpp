#include "cbase/c_allocator.h"
#include "cbase/c_runes.h"
#include "ctext/c_parser.h"
#include "cunittest/xunittest.h"

using namespace ncore;

extern ncore::alloc_t* gTestAllocator;

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
