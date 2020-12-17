#include "xbase/x_allocator.h"
#include "xbase/x_runes.h"
#include "xtext/x_parser2.h"
#include "xunittest/xunittest.h"

using namespace xcore;

extern xcore::alloc_t* gTestAllocator;

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
			runez_t<ascii::rune, 128> namestr;
			runes_writer_t            name(namestr);
			runez_t<ascii::rune, 128> domainstr;
			runes_writer_t            domain(domainstr);

			// clang-format off
			machine_t m;
			m.Email();

			// For examples see:
			// - machine_t::Email()
			// - machine_t::IPv4()

			// clang-format on

			runes_reader_t  reader("john.doe@hotmail.com");
			crunes_t::ptr_t cursor;
			bool            result = m.execute(reader, cursor);
		}

	}
}
UNITTEST_SUITE_END
