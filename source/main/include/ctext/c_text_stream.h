#ifndef __CTEXT_STREAM_H__
#define __CTEXT_STREAM_H__
#include "ccore/c_target.h"
#ifdef USE_PRAGMA_ONCE
#    pragma once
#endif

#include "cbase/c_runes.h"
#include "ccore/c_stream.h"

namespace ncore
{
    struct crunes_t;

    class text_stream_t : protected istream_t
    {
    public:
        enum encoding
        {
            encoding_ascii = ascii::TYPE,
            encoding_utf8  = utf8::TYPE,
            encoding_utf16 = utf16::TYPE,
            encoding_utf32 = utf32::TYPE
        };
        text_stream_t(istream_t* stream, encoding e = encoding_utf8);

        bool readText(crunes_t& line, s64 length);
        bool readLine(crunes_t& line);

        void close() { v_close(); }

    protected:
        istream_t* m_stream;
        s64        m_stream_pos;
        u64        m_stream_len;
        u8*        m_buffer_data;
        u8 const*  m_buffer_data0;
        u32        m_buffer_cap;
        u32        m_buffer_size;
        crunes_t   m_buffer_text;

        bool grabLine(crunes_t& line);

        virtual bool v_canSeek() const;
        virtual bool v_canRead() const;
        virtual bool v_canWrite() const;
        virtual bool v_canView() const;
        virtual void v_flush();
        virtual void v_close();
        virtual u64  v_getLength() const;
        virtual void v_setLength(u64 length);
        virtual s64  v_setPos(s64 pos);
        virtual s64  v_getPos() const;
        virtual s64  v_read(u8* buffer, s64 count);
        virtual s64  v_view(u8 const*& buffer, s64 count);
        virtual s64  v_write(const u8* buffer, s64 count);
    };

} // namespace ncore

#endif // __CTEXT_STREAM_H__
