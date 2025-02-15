#include "cbase/c_allocator.h"
#include "cbase/c_buffer.h"
#include "cbase/c_runes.h"
#include "ctext/c_parser2.h"
#include "cunittest/cunittest.h"

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
            buffer_t buffer(data, data + 2048);
            parser2::parser_t parser(buffer);

        }
    }
}
UNITTEST_SUITE_END
