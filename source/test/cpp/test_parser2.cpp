#include "xbase/x_allocator.h"
#include "xbase/x_buffer.h"
#include "xbase/x_runes.h"
#include "xtext/x_parser2.h"
#include "xunittest/xunittest.h"

using namespace ncore;

namespace ncore
{
    extern alloc_t* gTestAllocator;
    namespace xparser
    {
        extern void use_case_parser2();
    }
} // namespace ncore

UNITTEST_SUITE_BEGIN(test_parser2)
{
    UNITTEST_FIXTURE(main)
    {
        UNITTEST_FIXTURE_SETUP() {}
        UNITTEST_FIXTURE_TEARDOWN() {}

        UNITTEST_TEST(test_parse_1)
        {
            u8       data[2048];
            buffer_t buffer(2048, data);

            xparser2::parser_t parser(buffer);
            
        }
    }
}
UNITTEST_SUITE_END
