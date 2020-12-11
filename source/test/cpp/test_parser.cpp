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
			stringreader reader("+(123)555 443 2 22");
			stringprocessor processor(reader);
			CHECK_TRUE(processor.Parse(parser::utils::Phone()));
		}

		UNITTEST_TEST(test_search_phone_number)
		{
			stringreader reader("Name: John Doe, Phone: +(123)555 443 2 22, Occupation: Software Engineer");
			stringprocessor processor(reader);
			stringreader selection = processor.Search(parser::utils::Phone());
			stringreader phonenr("+(123)555 443 2 22");
			CHECK_TRUE(phonenr == selection);
		}

	}
}
UNITTEST_SUITE_END
