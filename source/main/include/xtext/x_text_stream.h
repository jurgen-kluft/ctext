#ifndef __XTEXT_STREAM_H__
#define __XTEXT_STREAM_H__
#include "xbase/x_target.h"
#ifdef USE_PRAGMA_ONCE
#pragma once
#endif

#include "xbase/x_runes.h"
#include "xbase/x_stream.h"

namespace xcore
{
    class crunes_t;

    class text_stream_t : public istream_t
    {
    public:
        text_stream_t(istream_t* stream);
        
        bool    readText(crunes_t& line, s64 length);
        bool    readLine(crunes_t& line);

    protected:
        istream_t* m_stream;
        xbyte* m_buffer_data;
        u32 m_buffer_size;
        runes_t m_buffer_text;

		virtual bool vcanSeek() const;
		virtual bool vcanRead() const;
		virtual bool vcanWrite() const;
		virtual void vflush();
		virtual void vclose();
		virtual u64  vgetLength() const;
		virtual void vsetLength(u64 length);
		virtual s64  vsetPos(s64 pos);
		virtual s64  vgetPos() const;
		virtual s64 vread(xbyte* buffer, s64 count);
		virtual s64 vwrite(const xbyte* buffer, s64 count);
    }

} // namespace xcore

#endif  // __XTEXT_STREAM_H__