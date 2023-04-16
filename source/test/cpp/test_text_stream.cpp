#include "cbase/c_allocator.h"
#include "cbase/c_stream.h"
#include "cbase/c_runes.h"
#include "ctext/c_text_stream.h"
#include "cunittest/cunittest.h"

extern unsigned char   read_text_txt[];
extern unsigned int    read_text_txt_len;

namespace ncore
{
    class mem_stream : public istream_t
    {
        u8 const* m_buffer;
        uint_t      m_size;
        uint_t      m_cursor;

    public:
        mem_stream(u8 const* data, uint_t length) : m_buffer(data), m_size(length), m_cursor(0) {}

    protected:
        virtual bool vcanSeek() const { return true; }
        virtual bool vcanRead() const { return true; }
        virtual bool vcanWrite() const { return false; }
        virtual bool vcanZeroCopy() const { return true; }
        virtual void vflush() {}
        virtual void vclose() {}
        virtual u64  vgetLength() const { return m_size; }
        virtual void vsetLength(u64 length) {}
        virtual s64  vsetPos(s64 pos)
        {
            m_cursor = pos;
            return m_cursor;
        }
        virtual s64 vgetPos() const { return m_cursor; }
        virtual s64 vread(u8* buffer, s64 count)
        {
            s64 i = 0;
            while (i < count && m_cursor < m_size)
            {
                buffer[i] = m_buffer[m_cursor++];
            }
            return i;
        }

        virtual s64 vread0(u8 const*& buffer, s64 count)
        {
            if (m_cursor < m_size)
            {
                buffer = m_buffer + m_cursor;
                if ((m_cursor + count) > m_size)
                {
                    count = m_size - m_cursor;
                }
                m_cursor += count;
            }
            else
            {
                buffer = nullptr;
                count  = 0;
            }
            return count;
        }

        virtual s64 vwrite(const u8* buffer, s64 count) { return -1; }
    };
} // namespace ncore

using namespace ncore;

UNITTEST_SUITE_BEGIN(test_text_stream)
{
    UNITTEST_FIXTURE(main)
    {
        UNITTEST_FIXTURE_SETUP() {}
        UNITTEST_FIXTURE_TEARDOWN() {}

        UNITTEST_TEST(read_from_memory_stream)
        {
            mem_stream    memtext(read_text_txt, read_text_txt_len);
            text_stream_t text(&memtext, text_stream_t::encoding_ascii);

            crunes_t thisstr("this ");
            crunes_t line;
            while (text.readLine(line))
            {
                CHECK_TRUE(starts_with(line, thisstr));
            }

            text.close();
            memtext.close();
        }
    }
}
UNITTEST_SUITE_END
