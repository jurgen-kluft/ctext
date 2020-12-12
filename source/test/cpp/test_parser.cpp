#include "xbase/x_allocator.h"
#include "xbase/x_runes.h"
#include "xtext/x_parser.h"
#include "xunittest/xunittest.h"

using namespace xcore;

extern xcore::xalloc* gTestAllocator;

UNITTEST_SUITE_BEGIN(test_xparser)
{
	UNITTEST_FIXTURE(main)
	{
		UNITTEST_FIXTURE_SETUP() 
		{
		}
		UNITTEST_FIXTURE_TEARDOWN() 
		{
		}

		UNITTEST_TEST(test_parse_phone_number)
		{
			xtext::reader_t reader("+(123)555 443 2 22");
			stringprocessor_t processor(reader);
            CHECK_TRUE(processor.parse(xparser::xutils::sPhone));
        }

		UNITTEST_TEST(test_search_phone_number)
		{
			xtext::reader_t reader("Name: John Doe, Phone: +(123)555 443 2 22, Occupation: Software Engineer");
			stringprocessor_t processor(reader);
            xtext::reader_t selection = processor.search(xparser::xutils::sPhone);
            xtext::reader_t phonenr("+(123)555 443 2 22");
            CHECK_TRUE(phonenr == selection);
		}
	}
}
UNITTEST_SUITE_END
