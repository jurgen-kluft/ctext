#include "xbase/x_target.h"
#include "xbase/x_allocator.h"
#include "xbase/x_buffer.h"
#include "xbase/x_context.h"
#include "xbase/x_debug.h"
#include "xbase/x_runes.h"

#include "xtext/x_text_stream.h"

namespace xcore
{
    const uchar32 cEOL = '\n';
    const uchar32 cEOF = 0x05;

    text_stream_t::text_stream_t(istream_t* stream, encoding e)
        : m_stream(stream)
        , m_buffer_data(nullptr)
        , m_buffer_size(0)
        , m_buffer_text()
    {
        m_buffer_text.m_type = (s32)e;
    }

    static bool find_eol(crunes_t& text)
    {
        uchar32 c = text.read();
        while (c != cEOL && c != cEOF) {
            c = text.read();
        }
        return c==cEOL || c==cEOF;
    }

    bool    text_stream_t::readLine(crunes_t& line)
    {
        if (m_buffer_data == nullptr) {
            m_buffer_data = (xbyte*)context_t::system_alloc()->allocate(4096, sizeof(void*));
            m_buffer_size = 4096;
            m_buffer_idx = 0;
            m_buffer_text.m_runes.m_ascii.m_bos = (ascii::prune)m_buffer_data;
            m_buffer_text.m_runes.m_ascii.m_eos = (ascii::prune)m_buffer_data + m_buffer_size;
            m_buffer_text.m_runes.m_ascii.m_str = m_buffer_text.m_runes.m_ascii.m_bos;
            m_buffer_text.m_runes.m_ascii.m_end = m_buffer_text.m_runes.m_ascii.m_bos;
        }
        
        

        return false;
    }

    bool text_stream_t::vcanSeek() const
    {
        return m_stream->canSeek();
    }

    bool text_stream_t::vcanRead() const
    {
        return m_stream->canRead();
    }

    bool text_stream_t::vcanWrite() const
    {
        return m_stream->canWrite();
    }

    void text_stream_t::vflush()
    {
        m_stream->flush();
    }

    void text_stream_t::vclose()
    {
        m_stream->close();
    }

    u64  text_stream_t::vgetLength() const
    {
        return m_stream->getLength();
    }

    void text_stream_t::vsetLength(u64 length)
    {
        m_stream->setLength(length);
    }

    s64  text_stream_t::vsetPos(s64 pos)
    {
        return m_stream->setPos(pos);
    }

    s64  text_stream_t::vgetPos() const
    {
        return m_stream->getPos();
    }

    s64 text_stream_t::vread(xbyte* buffer, s64 count)
    {
        return m_stream->read(buffer, count);
    }

    s64 text_stream_t::vwrite(const xbyte* buffer, s64 count)
    {
        return m_stream->write(buffer, count);
    }


}
