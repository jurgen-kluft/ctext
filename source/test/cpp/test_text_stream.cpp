#include "xbase/x_allocator.h"
#include "xbase/x_stream.h"
#include "xbase/x_runes.h"
#include "xtext/x_text_stream.h"
#include "xunittest/xunittest.h"

extern xcore::alloc_t* gTestAllocator;
extern unsigned char   read_text_txt[];
extern unsigned int    read_text_txt_len;

namespace xcore
{
    class mem_stream : public istream_t
    {
        xbyte const* m_buffer;
        xsize_t      m_size;
        xsize_t      m_cursor;

    public:
        mem_stream(xbyte const* data, xsize_t length) : m_buffer(data), m_size(length), m_cursor(0) {}

    protected:
        virtual bool vcanSeek() const { return true; }
        virtual bool vcanRead() const { return true; }
        virtual bool vcanWrite() const { return false; }
        virtual bool vcanZeroCopy() const { return true; }
        virtual void vflush() {}
        virtual void vclose() {}
        virtual u64  vgetLength() const { return m_size; }
        virtual void vsetLength(u64 length) {}
        virtual s64  vsetPos(s64 pos) { m_cursor = pos; return m_cursor; }
        virtual s64  vgetPos() const { return m_cursor; }
        virtual s64  vread(xbyte* buffer, s64 count)
        {
            s64 i = 0;
            while (i < count && m_cursor < m_size)
            {
                buffer[i] = m_buffer[m_cursor++];
            }
            return i;
        }

        virtual s64 vread0(xbyte const*& buffer, s64 count)
        {
            if (buffer == nullptr)
            {
                buffer = m_buffer;
                if (count > m_size)
                    count = m_size;
                m_cursor = count;
                return count;
            }
            else 
			{
				if (m_cursor < m_size)
				{
					buffer = m_buffer + m_cursor;
					if ((m_cursor + count) > m_size)
					{
						count    = m_size - m_cursor;
					}
					m_cursor += count;
				}
				else
				{
					buffer = nullptr;
					count  = -1;
				}
			}
            return count;
        }

        virtual s64 vwrite(const xbyte* buffer, s64 count) { return -1; }
    };
} // namespace xcore

using namespace xcore;

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

            crunes_t line;
            while (text.readLine(line)) {}

			text.close();
			memtext.close();
        }
    }
}
UNITTEST_SUITE_END
