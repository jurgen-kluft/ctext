#include "ccore/c_target.h"
#include "cbase/c_allocator.h"
#include "cbase/c_buffer.h"
#include "cbase/c_context.h"
#include "ccore/c_debug.h"
#include "cbase/c_runes.h"

#include "ctext/c_text_stream.h"

namespace ncore
{
    text_stream_t::text_stream_t(istream_t* stream, encoding e) : m_stream(stream), m_stream_len(0), m_stream_pos(0), m_buffer_data(nullptr), m_buffer_data0(nullptr), m_buffer_size(0), m_buffer_text()
    {
        m_buffer_cap = 4096; // Should be somewhere like "average line length" * 10
        m_buffer_text.set_type((u8)e);
    }

    static u32 find_eol(crunes_t& text)
    {
        u32     cursor = 0;
        uchar32 c      = text.read(cursor);
        while (c != cEOL && c != cEOF && c != cEOS)
        {
            c = text.read(cursor);
        }
        return cursor;
    }

    bool text_stream_t::grabLine(crunes_t& line)
    {
        line.m_ascii = m_buffer_text.m_ascii;
        line.m_str   = m_buffer_text.m_str;
        line.m_end   = m_buffer_text.m_end;
        line.m_eos   = m_buffer_text.m_eos;

        u32 const chars = find_eol(line);
        if (chars > 0)
        {
            m_buffer_text.m_str += chars;
            line.m_end = m_buffer_text.m_str;
            return true;
        }

        // Something going wrong
        line.m_str = m_buffer_text.m_eos;
        line.m_end = m_buffer_text.m_eos;
        line.m_eos = m_buffer_text.m_eos;
        return false;
    }

    bool text_stream_t::readLine(crunes_t& line)
    {
        if (!grabLine(line))
        {
            if (m_buffer_data == nullptr && m_buffer_data0 == nullptr)
            {
                if (m_stream_pos == 0)
                {
                    if (m_stream->canView())
                    {
                        s64 read              = m_stream->view(m_buffer_data0, m_buffer_cap);
                        m_buffer_text.m_ascii = (ascii::pcrune)m_buffer_data0;
                        m_buffer_text.m_str   = 0;
                        m_buffer_text.m_eos   = m_buffer_cap;
                        m_buffer_text.m_end   = (u32)read;
                        m_stream_pos          = read;
                        m_stream_len          = m_stream->getLength();
                    }
                    else
                    {
                        m_buffer_data = (u8*)context_t::system_alloc()->allocate(m_buffer_cap, sizeof(void*));
                        m_buffer_size = 0;
                        m_stream_pos  = 0;
                        m_stream_len  = m_stream->getLength();
                        m_buffer_size = (u32)m_stream->read(m_buffer_data, m_buffer_cap);
                        m_stream_pos += m_buffer_size;
                    }
                }
            }
            else
            {
                // Could not find EOL or EOF or there is no text left to scan
                if (m_stream->canView())
                {
                    s64 const rest = m_buffer_text.m_end - m_buffer_text.m_str;
                    m_stream_pos   = m_stream->getPos();
                    m_stream_pos -= rest;
                    m_stream->setPos(m_stream_pos);
                    s64 read = m_stream->view(m_buffer_data0, m_buffer_cap);
                    if (read == 0)
                    {
                        m_buffer_text = crunes_t();
                        m_stream_pos  = m_stream_len;
                        return false;
                    }
                    else
                    {
                        m_buffer_size         = 0;
                        m_buffer_text.m_ascii = (ascii::pcrune)m_buffer_data0;
                        m_buffer_text.m_str   = 0;
                        m_buffer_text.m_end   = (u32)read;
                        m_buffer_text.m_eos   = (u32)read;
                    }
                }
                else
                {
                    // Currently we are at the following position in our text buffer

                    // Move the 'rest' to the beginning of our buffer and join it with new data
                    ascii::pcrune src = m_buffer_text.m_ascii + m_buffer_text.m_str;
                    ascii::pcrune end = m_buffer_text.m_ascii + m_buffer_text.m_end;
                    u8*           dst = (u8*)m_buffer_data;
                    while (src < end)
                        *dst++ = *src++;

                    m_buffer_size               = (u32)(dst - m_buffer_data);
                    s64       read_request_size = m_buffer_cap - m_buffer_size;
                    s64 const read_actual_size  = m_stream->read(dst, read_request_size);
                    if (read_actual_size >= 0)
                    {
                        m_buffer_size += (u32)read_actual_size;
                        m_stream_pos += read_actual_size;
                    }
                    else
                    {
                        // an error occured
                        m_stream_pos = m_stream_len;
                    }

                    // Adjust our m_buffer_text
                    m_buffer_text.m_ascii = (ascii::pcrune)m_buffer_data;
                    m_buffer_text.m_eos   = m_buffer_size;
                    m_buffer_text.m_str   = 0;
                    m_buffer_text.m_end   = m_buffer_size;
                }
            }

            if (!grabLine(line))
                return false;
        }

        return true;
    }

    bool text_stream_t::vcanSeek() const { return false; }
    bool text_stream_t::vcanRead() const { return m_stream->canRead(); }
    bool text_stream_t::vcanWrite() const { return m_stream->canWrite(); }
    bool text_stream_t::vcanView() const { return m_stream->canView(); }
    void text_stream_t::vflush() { m_stream->flush(); }

    void text_stream_t::vclose()
    {
        if (m_buffer_data != nullptr)
        {
            context_t::system_alloc()->deallocate(m_buffer_data);
        }
        m_buffer_cap   = 0;
        m_buffer_data  = nullptr;
        m_buffer_data0 = nullptr;
        m_buffer_size  = 0;
        m_stream_pos   = 0;
        m_stream_len   = 0;
        m_buffer_text  = crunes_t();

        m_stream->close();
    }

    u64  text_stream_t::vgetLength() const { return m_stream->getLength(); }
    void text_stream_t::vsetLength(u64 length) { m_stream->setLength(length); }

    s64 text_stream_t::vsetPos(s64 pos)
    {
        if (canSeek())
            return m_stream->setPos(pos);
        return -1;
    }

    s64 text_stream_t::vgetPos() const { return m_stream->getPos(); }
    s64 text_stream_t::vread(u8* buffer, s64 count) { return m_stream->read(buffer, count); }
    s64 text_stream_t::vview(u8 const*& buffer, s64 count) { return m_stream->view(buffer, count); }
    s64 text_stream_t::vwrite(const u8* buffer, s64 count) { return m_stream->write(buffer, count); }

} // namespace ncore
