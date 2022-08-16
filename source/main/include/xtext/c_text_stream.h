#ifndef __XTEXT_STREAM_H__
#define __XTEXT_STREAM_H__
#include "cbase/c_target.h"
#ifdef USE_PRAGMA_ONCE
#pragma once
#endif

#include "cbase/c_runes.h"
#include "cbase/c_stream.h"

namespace ncore
{
    class crunes_t;

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

        void close() { vclose(); }

    protected:
        istream_t*      m_stream;
        s64             m_stream_pos;
        u64             m_stream_len;
        u8*             m_buffer_data;
        u8 const*       m_buffer_data0;
        s64             m_buffer_cap;
        s64             m_buffer_size;
        crunes_t        m_buffer_text;
        crunes_t::ptr_t m_buffer_text_cursor;

        bool determineLine(crunes_t& line);

        virtual bool vcanSeek() const;
        virtual bool vcanRead() const;
        virtual bool vcanWrite() const;
        virtual bool vcanZeroCopy() const;
        virtual void vflush();
        virtual void vclose();
        virtual u64  vgetLength() const;
        virtual void vsetLength(u64 length);
        virtual s64  vsetPos(s64 pos);
        virtual s64  vgetPos() const;
        virtual s64  vread(u8* buffer, s64 count);
        virtual s64  vread0(u8 const*& buffer, s64 count);
        virtual s64  vwrite(const u8* buffer, s64 count);
    };

} // namespace ncore

#endif // __XTEXT_STREAM_H__