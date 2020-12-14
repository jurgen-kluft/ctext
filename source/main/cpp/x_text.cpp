#include "xtext/x_text.h"

namespace xcore
{
    namespace xtext
    {
        enum echartype
        {
            NONE  = -1,
            ASCII = 0,
            UTF8  = 1,
            UTF16 = 2,
            UTF32 = 4
        };

        reader_t::reader_t()
            : m_type(ASCII)
        {
        }

        reader_t::reader_t(const char* str)
            : m_type(ASCII)
        {
            if (str != nullptr)
            {
                m_runes.m_ascii          = ascii::crunes(str);
                m_begin.m_cursor.m_ascii = m_runes.m_ascii.m_str;
                m_end.m_cursor.m_ascii   = m_runes.m_ascii.m_end;
            }
        }

        reader_t::reader_t(utf32::pcrune str)
            : m_type(UTF32)
        {
            if (str != nullptr)
            {
                m_runes.m_utf32          = utf32::crunes(str);
                m_begin.m_cursor.m_utf32 = m_runes.m_utf32.m_str;
                m_end.m_cursor.m_utf32   = m_runes.m_utf32.m_end;
            }
        }

        reader_t::reader_t(const reader_t& t)
            : m_type(t.m_type)
            , m_begin(t.m_begin)
            , m_end(t.m_end)
        {
            m_runes.m_ascii = t.m_runes.m_ascii;
        }

        reader_t::reader_t(const reader_t& t, const cursor_t& begin, const cursor_t& end)
            : m_type(t.m_type)
            , m_begin(begin)
            , m_end(end)
        {
            m_runes.m_ascii = t.m_runes.m_ascii;
            //@TODO: validate begin and end
        }

        reader_t::cursor_t reader_t::get_cursor() const { return m_begin; }

        s64 reader_t::size() const
        {
            switch (m_type)
            {
                case ASCII: return m_runes.m_ascii.is_empty() ? 0 : m_runes.m_ascii.size();
                case UTF32: return m_runes.m_utf32.is_empty() ? 0 : m_runes.m_utf32.size();
            }
            return 0;
        }

        void reader_t::reset(cursor_t& cursor) const { cursor = m_begin; }
        bool reader_t::valid(cursor_t const& cursor) const { return cursor >= m_begin && cursor < m_end; }

        uchar32 reader_t::peek(cursor_t const& cursor) const
        {
            uchar32 c = '\0';
            if (valid(cursor))
            {
                switch (m_type)
                {
                    case ASCII: c = *cursor.m_cursor.m_ascii; break;
                    case UTF32: c = *cursor.m_cursor.m_utf32; break;
                }
            }
            return c;
        }

        uchar32 reader_t::read(cursor_t& cursor) const
        {
            uchar32 c = '\0';
            if (valid(cursor))
            {
                c = peek(cursor);
                skip(cursor);
            }
            return c;
        }

        void reader_t::skip(cursor_t& cursor) const
        {
            if (valid(cursor))
            {
                switch (m_type)
                {
                    case ASCII: cursor.m_cursor.m_ascii++; break;
                    case UTF32: cursor.m_cursor.m_utf32++; break;
                }
            }
        }

        reader_t reader_t::select(cursor_t const& from, cursor_t const& to) const
        {
            reader_t reader;
            reader.m_type          = reader.m_type;
            reader.m_runes.m_ascii = m_runes.m_ascii;
			reader.m_begin = m_begin;
			if (valid(from))
			{
				reader.m_begin = from;
			}
			reader.m_end = m_end;
			if (valid(to))
			{
				reader.m_end = to;
			}
            return reader;
        }

        reader_t& reader_t::operator=(const reader_t& other)
        {
            m_type          = other.m_type;
            m_runes.m_ascii = other.m_runes.m_ascii;
            m_begin         = other.m_begin;
            m_end           = other.m_end;
            return *this;
        }

        bool reader_t::operator==(const reader_t& other) const { return other.m_type == m_type && other.m_runes.m_ascii == m_runes.m_ascii && other.m_begin == m_begin && other.m_end == m_end; }
        bool reader_t::operator!=(const reader_t& other) const { return other.m_type != m_type || other.m_runes.m_ascii != m_runes.m_ascii || other.m_begin != m_begin || other.m_end != m_end; }

        reader_t::cursor_t& reader_t::cursor_t::operator=(const reader_t::cursor_t& t)
        {
            m_cursor = t.m_cursor;
            return *this;
        }

        bool reader_t::cursor_t::operator<(const reader_t::cursor_t& t) const { return m_cursor.m_ascii < t.m_cursor.m_ascii; }
        bool reader_t::cursor_t::operator>(const reader_t::cursor_t& t) const { return m_cursor.m_ascii > t.m_cursor.m_ascii; }
        bool reader_t::cursor_t::operator<=(const reader_t::cursor_t& t) const { return m_cursor.m_ascii <= t.m_cursor.m_ascii; }
        bool reader_t::cursor_t::operator>=(const reader_t::cursor_t& t) const { return m_cursor.m_ascii >= t.m_cursor.m_ascii; }
        bool reader_t::cursor_t::operator==(const reader_t::cursor_t& t) const { return m_cursor.m_ascii == t.m_cursor.m_ascii; }
        bool reader_t::cursor_t::operator!=(const reader_t::cursor_t& t) const { return m_cursor.m_ascii != t.m_cursor.m_ascii; }
    } // namespace xtext

} // namespace xcore
