#include "xbase/x_runes.h"

namespace xcore
{
    enum
    {
        NONE,
        ASCII,
        UTF8,
        UTF16,
        UTF32
    };

    class StringReader
    {
        union crunes {
            inline crunes()
                : _ascii()
            {
            }
            ascii::crunes  _ascii;
            utf8::crunes   _utf8;
            utf16::crunes  _utf16;
            utf32::crunes  _utf32;
        };
        crunes  m_runes;
        s32     m_type;

    public:
        StringReader();
        StringReader(const char* str);
        StringReader(utf32::pcrune str);
        StringReader(const StringReader& chars);

        bool valid() const;
        void reset();

        uchar32 read();
        uchar32 peek() const;
        void    skip();

        void select(StringReader const& from, StringReader const& until);

        bool operator<(const StringReader&) const;
        bool operator>(const StringReader&) const;
        bool operator<=(const StringReader&) const;
        bool operator>=(const StringReader&) const;
        bool operator==(const StringReader&) const;
        bool operator!=(const StringReader&) const;

        StringReader& operator=(const StringReader&);
    };

    StringReader::StringReader()
        : m_type(NONE)
    {
    }

    StringReader::StringReader(const char* str)
        : m_type(ASCII)
    {
        if (str != nullptr)
        {
            m_runes._ascii = ascii::crunes(str);
        }
    }

    StringReader::StringReader(utf32::pcrune str)
        : m_type(UTF32)
    {
        if (str != nullptr)
        {
            m_runes._utf32 = utf32::crunes(str);
        }
    }

    StringReader::StringReader(const StringReader& t)
        : m_type(t.m_type)
    {
        m_runes._ascii = t.m_runes._ascii;
    }

    bool StringReader::valid() const
    {
        switch (m_type)
        {
            case ASCII: return m_runes._ascii.is_valid();
            case UTF32: return m_runes._utf32.is_valid();
        }
        return false;
    }

    void StringReader::reset() { m_runes._ascii.reset(); }

    uchar32 StringReader::read()
    {
        switch (m_type)
        {
            case ASCII:
                if (m_runes._ascii.is_valid())
                    return *m_runes._ascii.m_cur++;
            case UTF32:
                if (m_runes._utf32.is_valid())
                    return *m_runes._utf32.m_cur++;
        }
        return '\0';
    }

    uchar32 StringReader::peek() const
    {
        switch (m_type)
        {
            case ASCII:
                if (m_runes._ascii.is_valid())
                    return *m_runes._ascii.m_cur;
            case UTF32:
                if (m_runes._utf32.is_valid())
                    return *m_runes._utf32.m_cur;
        }
        return '\0';
    }

    void StringReader::skip()
    {
        switch (m_type)
        {
            case ASCII:
                if (m_runes._ascii.is_valid())
                    m_runes._ascii.m_cur++;
                break;
            case UTF32:
                if (m_runes._utf32.is_valid())
                    m_runes._utf32.m_cur++;
                break;
        }
    }

    void StringReader::select(StringReader const& from, StringReader const& until)
    {
        m_runes._ascii.m_str = from.m_runes._ascii.m_cur;
        m_runes._ascii.m_cur = m_runes._ascii.m_str;
        m_runes._ascii.m_end = until.m_runes._ascii.m_cur;
    }

    StringReader& StringReader::operator=(const StringReader& t)
    {
        m_runes._ascii = t.m_runes._ascii;
        return *this;
    }

    bool StringReader::operator<(const StringReader& t) const { return m_runes._ascii.m_cur < t.m_runes._ascii.m_cur; }
    bool StringReader::operator>(const StringReader& t) const { return m_runes._ascii.m_cur > t.m_runes._ascii.m_cur; }
    bool StringReader::operator<=(const StringReader& t) const { return m_runes._ascii.m_cur <= t.m_runes._ascii.m_cur; }
    bool StringReader::operator>=(const StringReader& t) const { return m_runes._ascii.m_cur >= t.m_runes._ascii.m_cur; }
    bool StringReader::operator==(const StringReader& t) const
    {
        if (t.m_type == m_type)
        {
            if (t.m_runes._ascii.m_str == m_runes._ascii.m_str && t.m_runes._ascii.m_end == m_runes._ascii.m_end)
                return true;

            // Character by character comparison
            switch (m_type)
            {
                case ASCII: return ascii::compare(m_runes._ascii, t.m_runes._ascii) == 0;
                case UTF32: return utf32::compare(m_runes._utf32, t.m_runes._utf32) == 0;
            }
        }
        return false;
    }

    bool StringReader::operator!=(const StringReader& t) const
    {
        if (t.m_type == m_type)
        {
            if (t.m_runes._ascii.m_str == m_runes._ascii.m_str && t.m_runes._ascii.m_end == m_runes._ascii.m_end)
                return false;

            // Character by character comparison
            switch (m_type)
            {
                case ASCII: return ascii::compare(m_runes._ascii, t.m_runes._ascii) != 0;
                case UTF32: return utf32::compare(m_runes._utf32, t.m_runes._utf32) != 0;
            }
        }
        return true;
    }

    class StringWriter
    {
        union runes {
            inline runes()
                : _ascii()
            {
            }
            ascii::runes _ascii;
            utf8::runes  _utf8;
            utf16::runes _utf16;
            utf32::runes _utf32;
        };
        runes m_runes;
        s32   m_type;

    public:
        StringWriter();
        StringWriter(char* str, char* end);
        StringWriter(utf32::rune* str, utf32::rune* end);
        StringWriter(const StringWriter& chars);

        bool valid() const;
        void reset();

        bool write(uchar32 c);

        bool operator<(const StringWriter&) const;
        bool operator>(const StringWriter&) const;
        bool operator<=(const StringWriter&) const;
        bool operator>=(const StringWriter&) const;
        bool operator==(const StringWriter&) const;
        bool operator!=(const StringWriter&) const;

        StringWriter& operator=(const StringWriter&);
    };

    StringWriter::StringWriter()
        : m_type(NONE)
    {
    }

    StringWriter::StringWriter(char* str, char* end)
        : m_type(ASCII)
    {
        if (str != nullptr)
        {
            m_runes._ascii = ascii::runes(str, str, end);
        }
    }

    StringWriter::StringWriter(utf32::rune* str, utf32::rune* end)
        : m_type(UTF32)
    {
        if (str != nullptr)
        {
            m_runes._utf32 = utf32::runes(str, str, end);
        }
    }

    StringWriter::StringWriter(const StringWriter& t)
        : m_type(t.m_type)
    {
        m_runes._ascii = t.m_runes._ascii;
    }

    bool StringWriter::valid() const
    {
        switch (m_type)
        {
            case ASCII: return m_runes._ascii.is_valid();
            case UTF32: return m_runes._utf32.is_valid();
        }
        return false;
    }

    void StringWriter::reset() 
	{
		m_runes._ascii.reset(); 
        switch (m_type)
        {
            case ASCII:
                if (m_runes._ascii.is_valid())
					*m_runes._ascii.m_end = '\0';
                break;
            case UTF32:
                if (m_runes._utf32.is_valid())
					*m_runes._utf32.m_end = '\0';
                break;
        }
	}

    bool StringWriter::write(uchar32 c)
    {
        switch (m_type)
        {
            case ASCII:
                if (m_runes._ascii.is_valid())
                {
                    *m_runes._ascii.m_end++ = c;
					*m_runes._ascii.m_end = '\0';
                    return true;
                }
                break;
            case UTF32:
                if (m_runes._utf32.is_valid())
                {
                    *m_runes._utf32.m_end++ = c;
					*m_runes._utf32.m_end = '\0';
                    return true;
                }
                break;
        }
        return false;
    }

    StringWriter& StringWriter::operator=(const StringWriter& t)
    {
        m_runes._ascii = t.m_runes._ascii;
        return *this;
    }

    bool StringWriter::operator<(const StringWriter& t) const { return m_runes._ascii.m_end < t.m_runes._ascii.m_end; }
    bool StringWriter::operator>(const StringWriter& t) const { return m_runes._ascii.m_end > t.m_runes._ascii.m_end; }
    bool StringWriter::operator<=(const StringWriter& t) const { return m_runes._ascii.m_end <= t.m_runes._ascii.m_end; }
    bool StringWriter::operator>=(const StringWriter& t) const { return m_runes._ascii.m_end >= t.m_runes._ascii.m_end; }
    bool StringWriter::operator==(const StringWriter& t) const
    {
        if (t.m_type == m_type)
        {
            if (t.m_runes._ascii.m_str == m_runes._ascii.m_str && t.m_runes._ascii.m_end == m_runes._ascii.m_end)
                return true;

            // Character by character comparison
            switch (m_type)
            {
                case ASCII: return ascii::compare(m_runes._ascii, t.m_runes._ascii) == 0;
                case UTF32: return utf32::compare(m_runes._utf32, t.m_runes._utf32) == 0;
            }
        }
        return false;
    }

    bool StringWriter::operator!=(const StringWriter& t) const
    {
        if (t.m_type == m_type)
        {
            if (t.m_runes._ascii.m_str == m_runes._ascii.m_str && t.m_runes._ascii.m_end == m_runes._ascii.m_end)
                return false;

            // Character by character comparison
            switch (m_type)
            {
                case ASCII: return ascii::compare(m_runes._ascii, t.m_runes._ascii) != 0;
                case UTF32: return utf32::compare(m_runes._utf32, t.m_runes._utf32) != 0;
            }
        }
        return true;
    }

} // namespace xcore
