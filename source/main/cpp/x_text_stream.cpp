#include "xbase/x_target.h"
#include "xbase/x_allocator.h"
#include "xbase/x_buffer.h"
#include "xbase/x_context.h"
#include "xbase/x_debug.h"
#include "xbase/x_runes.h"

#include "xtext/x_text_stream.h"

namespace xcore
{
    text_stream_t::text_stream_t(istream_t* stream, encoding e)
        : m_stream(stream)
        , m_stream_len(0)
        , m_stream_pos(0)
        , m_buffer_data(nullptr)
        , m_buffer_size(0)
        , m_buffer_text()
    {
        m_buffer_text.m_type = (s32)e;
    }

    static bool find_eol(runes_t& text)
    {
        uchar32 c = text.read();
        while (c != cEOL && c != cEOF) {
            c = text.read();
        }
        return c==cEOL || c==cEOF;
    }

    bool    text_stream_t::determineLine(crunes_t& line)
    {
        erunes_t chars;
        chars.eol();
        chars.eof();
        if (m_buffer_text.scan(chars))
        {
            // We encountered a special character
            line = m_buffer_text;
            line.m_runes.m_ascii.m_end = m_buffer_text.m_runes.m_ascii.m_str;
            m_buffer_text.skip(chars);
            return true;
        }
        return false;
    }

    bool    text_stream_t::readLine(crunes_t& line)
    {
        if (!determineLine(line))
        {
            if (m_buffer_data == nullptr) {
                m_buffer_cap  = 4096;   // Should be somewhere like "average line length" * 10
                m_buffer_data = (xbyte*)context_t::system_alloc()->allocate(m_buffer_cap, sizeof(void*));
                m_buffer_size = 0;
                m_stream_pos = 0;
                m_stream_len = m_stream->getLength();
                m_buffer_size = m_stream->read(m_buffer_data, m_buffer_cap);
                m_stream_pos += m_buffer_size;
            }
            else
            {
                // Could not find EOL or EOF or there is no text left to scan

                // Is there still any more data in the stream?
                if (m_stream_pos >= m_stream_len)
                    return false;

                // Currently we are at the following position in our text buffer
                ascii::pcrune pos = m_buffer_text.m_runes.m_ascii.m_str;

                // Move the 'rest' to the beginning of our buffer and join it with new data
                xbyte* dst = m_buffer_data;
                while (pos < m_buffer_text.m_runes.m_ascii.m_end)
                {
                    *dst++ = (xbyte)*pos++;
                }
                m_buffer_size = (u32)(dst - m_buffer_data);
                s64 read_request_size = m_buffer_cap - m_buffer_size;
                s64 const read_actual_size = m_stream->read(dst, read_request_size);
                if (read_actual_size > 0)
                {
                    m_stream_pos += read_actual_size;
                    m_buffer_size += read_actual_size;
                }
            }

            // Adjust our m_buffer_text
            m_buffer_text.m_runes.m_ascii.m_bos = (ascii::pcrune)m_buffer_data;
            m_buffer_text.m_runes.m_ascii.m_eos = (ascii::pcrune)m_buffer_data + m_buffer_size;
            m_buffer_text.m_runes.m_ascii.m_str = m_buffer_text.m_runes.m_ascii.m_bos;
            m_buffer_text.m_runes.m_ascii.m_end = m_buffer_text.m_runes.m_ascii.m_eos;

            if (!determineLine(line))
                return false;
        }

        return true;
    }

    bool text_stream_t::vcanSeek() const
    {
        return false;
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
        if (m_buffer_data != nullptr)
        {
            m_buffer_cap  = 0;
            context_t::system_alloc()->deallocate(m_buffer_data);
            m_buffer_data = nullptr;
            m_buffer_size = 0;
            m_stream_pos = 0;
            m_stream_len = 0;
            m_buffer_text = crunes_t();
        }

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
        if (canSeek())
            return m_stream->setPos(pos);
        return -1;
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
