#include "xtext/x_parser.h"

namespace xcore
{
    enum echartype
    {
        NONE  = -1,
        ASCII = 0,
        UTF8  = 1,
        UTF16 = 2,
        UTF32 = 4
    };

    charreader::charreader() : m_type(ASCII) {}

    charreader::charreader(const char* str) : m_type(ASCII)
    {
        if (str != nullptr)
        {
            m_runes._ascii = ascii::crunes(str);
        }
    }

    charreader::charreader(utf32::pcrune str) : m_type(UTF32)
    {
        if (str != nullptr)
        {
            m_runes._utf32 = utf32::crunes(str);
        }
    }

    charreader::charreader(const charreader& t) : m_type(t.m_type) { m_runes._ascii = t.m_runes._ascii; }

    s64 charreader::size() const
    {
        switch (m_type)
        {
            case ASCII: return m_runes._ascii.is_empty() ? 0 : m_runes._ascii.size();
            case UTF32: return m_runes._utf32.is_empty() ? 0 : m_runes._utf32.size();
        }
        return 0;
    }

    void charreader::reset() { m_runes._ascii.reset(); }

    bool charreader::valid() const
    {
        switch (m_type)
        {
            case ASCII: return m_runes._ascii.is_valid();
            case UTF32: return m_runes._utf32.is_valid();
        }
        return false;
    }

    uchar32 charreader::peek() const
    {
        switch (m_type)
        {
            case ASCII: return m_runes._ascii.is_empty() ? '\0' : *m_runes._ascii.m_cur;
            case UTF32: return m_runes._utf32.is_empty() ? '\0' : *m_runes._utf32.m_cur;
        }
        return '\0';
    }

    uchar32 charreader::read()
    {
        switch (m_type)
        {
            case ASCII: return m_runes._ascii.is_empty() ? '\0' : *m_runes._ascii.m_cur++;
            case UTF32: return m_runes._utf32.is_empty() ? '\0' : *m_runes._utf32.m_cur++;
        }
        return '\0';
    }

    void charreader::skip()
    {
        switch (m_type)
        {
            case ASCII:
                if (!m_runes._ascii.is_empty())
                    m_runes._ascii.m_cur++;
                break;
            case UTF32:
                if (!m_runes._utf32.is_empty())
                    m_runes._utf32.m_cur++;
                break;
        }
    }

    void charreader::select(charreader const& from, charreader const& until)
    {
        m_runes._ascii.m_str = from.m_runes._ascii.m_cur;
        m_runes._ascii.m_cur = m_runes._ascii.m_str;
        m_runes._ascii.m_end = until.m_runes._ascii.m_cur;
    }

    charreader& charreader::operator=(const charreader& t)
    {
        m_runes._ascii = t.m_runes._ascii;
        return *this;
    }

    bool charreader::operator<(const charreader& t) const { return m_runes._ascii.m_cur < t.m_runes._ascii.m_cur; }
    bool charreader::operator>(const charreader& t) const { return m_runes._ascii.m_cur > t.m_runes._ascii.m_cur; }
    bool charreader::operator<=(const charreader& t) const { return m_runes._ascii.m_cur <= t.m_runes._ascii.m_cur; }
    bool charreader::operator>=(const charreader& t) const { return m_runes._ascii.m_cur >= t.m_runes._ascii.m_cur; }
    bool charreader::operator==(const charreader& t) const
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

    bool charreader::operator!=(const charreader& t) const
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

    stringreader::stringreader() : m_str() {}
    stringreader::stringreader(const char* str) : m_str(str) {}
    stringreader::stringreader(const stringreader& chars) : m_str(chars.m_str) {}
    stringreader::stringreader(const stringreader& begin, const stringreader& until) { m_str.select(begin.m_str, until.m_str); }

    s64     stringreader::Size() const { return m_str.size(); }
    void    stringreader::Reset() { m_str.reset(); }
    uchar32 stringreader::Read() { return m_str.read(); }
    uchar32 stringreader::Peek() const { return m_str.peek(); }
    void    stringreader::Select(const stringreader& begin, const stringreader& cursor) { m_str.select(begin.m_str, cursor.m_str); }
    bool    stringreader::Valid() const { return m_str.valid(); }
    void    stringreader::Skip() { m_str.skip(); }
    void    stringreader::Write(StringWriter& writer) {}

    bool stringreader::operator<(const stringreader&) const { return false; }
    bool stringreader::operator>(const stringreader&) const { return false; }
    bool stringreader::operator<=(const stringreader&) const { return false; }
    bool stringreader::operator>=(const stringreader&) const { return false; }
    bool stringreader::operator==(const stringreader& r) const
    {
        if (Size() == r.Size())
        {
            return m_str == r.m_str;
        }
        return false;
    }

    bool stringreader::operator!=(const stringreader&) const { return false; }

    stringreader& stringreader::operator=(const stringreader& r)
    {
        m_str = r.m_str;
        return *this;
    }

    namespace parser
    {
        filters::Any          filters::ANY;
        filters::Alphabet     filters::ALPHABET;
        filters::AlphaNumeric filters::ALPHANUMERIC;
        filters::Digit        filters::DIGIT;
        filters::Decimal      filters::DECIMAL;
        filters::EndOfLine    filters::EOL;
        filters::EndOfText    filters::EOT;
        filters::Hex          filters::HEX;
        filters::WhiteSpace   filters::WHITESPACE;
        filters::Word         filters::WORD;
        utils::Email          utils::EMAIL;
        utils::Host           utils::HOST;
        utils::IPv4           utils::IPV4;
        utils::Phone          utils::PHONE;
        utils::ServerAddress  utils::SERVERADDRESS;
        utils::Uri            utils::URI;

        namespace manipulators
        {
            bool Not::Check(stringreader& _reader)
            {
                stringreader start = _reader;
                if (!m_tokenizer_a.Check(start))
                {
                    _reader.Skip();
                    return true;
                }
                return false;
            }

            bool Or::Check(stringreader& _reader)
            {
                stringreader start = _reader;
                if (!m_tokenizer_a.Check(start))
                {
                    if (!m_tokenizer_b.Check(start))
                    {
                        return false;
                    }
                }
                _reader = start;
                return true;
            }

            bool And::Check(stringreader& _reader)
            {
                stringreader start1, start2;

                start1 = _reader;
                if (!m_tokenizer_a.Check(start1))
                    return false;

                start2 = _reader;
                if (!m_tokenizer_b.Check(start2))
                    return false;

                start1 = ((start2 < start1) ? start2 : start1);

                _reader = start1;
                return true;
            }

            bool Sequence::Check(stringreader& _reader)
            {
                stringreader start = _reader;

                if (!m_tokenizer_a.Check(start))
                    return false;

                if (!m_tokenizer_b.Check(start))
                    return false;

                _reader = start;
                return true;
            }

            bool Within::Check(stringreader& _reader)
            {
                stringreader start = _reader;

                u64 i = 0;
                for (; i < m_max; i++)
                {
                    if (!m_tokenizer_a.Check(start))
                        break;
                }

                if (i >= m_min && i <= m_max)
                {
                    _reader = start;
                    return true;
                }

                return false;
            }

            bool Times::Check(stringreader& _reader) { return Within(m_max, m_max, m_tokenizer_a).Check(_reader); }
            bool OneOrMore::Check(stringreader& _reader) { return Within(1, -1, m_tokenizer_a).Check(_reader); }
            bool ZeroOrOne::Check(stringreader& _reader) { return Within(0, 1, m_tokenizer_a).Check(_reader); }
            bool While::Check(stringreader& _reader) { return Within(0, -1, m_tokenizer_a).Check(_reader); }
            bool Until::Check(stringreader& _reader) { return (While(Not(m_tokenizer_a))).Check(_reader); }

            bool Extract::Check(stringreader& _reader)
            {
                stringreader start = _reader;

                bool result = m_tokenizer_a.Check(start);
                m_selection = stringreader(start, _reader);

                _reader = start;
                return result;
            }

            bool ReturnToCallback::Check(stringreader& _reader)
            {
                stringreader start  = _reader;
                bool         result = m_tokenizer_a.Check(start);

                stringreader output = stringreader(start, _reader);
                m_cb(output);

                _reader = start;
                return result;
            }

            bool Enclosed::Check(stringreader& _reader) { return (filters::Exact(m_open) + m_tokenizer_a + filters::Exact(m_close)).Check(_reader); }
        } // namespace manipulators

        namespace filters
        {
            bool Any::Check(stringreader& _reader)
            {
                if (!_reader.Valid())
                    return false;
                _reader.Skip();
                return true;
            }

            bool In::Check(stringreader& _reader)
            {
                if (!_reader.Valid())
                    return false;

                uchar32 const s = _reader.Peek();
                m_input.Reset();
                while (m_input.Valid())
                {
                    uchar32 const c = m_input.Read();
                    if (c == s)
                    {
                        _reader.Skip();
                        return true;
                    }
                }

                return false;
            }

            bool Between::Check(stringreader& _reader)
            {
                uchar32 c = _reader.Peek();
                if (c >= m_lower && c <= m_upper)
                {
                    _reader.Skip();
                    return true;
                }
                return false;
            }

            bool Alphabet::Check(stringreader& _reader) { return (Between(('a'), ('z')).Check(_reader) | Between(('A'), ('Z')).Check(_reader)); }
            bool Digit::Check(stringreader& _reader) { return Between(('0'), ('9')).Check(_reader); }
            bool Hex::Check(stringreader& _reader) { return (DIGIT | Between(('a'), ('f')) | Between(('A'), ('F'))).Check(_reader); }
            bool AlphaNumeric::Check(stringreader& _reader) { return (ALPHABET | DIGIT).Check(_reader); }

            bool Exact::Check(stringreader& _reader)
            {
                stringreader streamcursor = _reader;
                stringreader inputcursor  = m_input;
                while (inputcursor.Valid())
                {
                    uchar32 a = streamcursor.Peek();
                    uchar32 b = inputcursor.Peek();
                    if (a != b)
                        streamcursor.Skip();
                    inputcursor.Skip();
                }
                _reader = streamcursor;
                return true;
            }

            bool Like::Check(stringreader& _reader)
            {
                stringreader streamcursor = _reader;
                stringreader inputcursor  = m_input;
                while (inputcursor.Valid())
                {
                    uchar32 a = streamcursor.Peek();
                    uchar32 b = inputcursor.Peek();
                    if (utf32::to_lower(a) != utf32::to_lower(b))
                        return false;
                    streamcursor.Skip();
                    inputcursor.Skip();
                }
                _reader = streamcursor;
                return true;
            }

            bool WhiteSpace::Check(stringreader& _reader) { return In((" \t\n\r")).Check(_reader); }

            bool Is::Check(stringreader& _reader)
            {
                if (_reader.Peek() == _Letter)
                {
                    _reader.Skip();
                    return true;
                }
                return false;
            }

            bool Decimal::Check(stringreader& _reader) { return manipulators::OneOrMore(DIGIT).Check(_reader); }
            bool Word::Check(stringreader& _reader) { return manipulators::OneOrMore(ALPHABET).Check(_reader); }
            bool EndOfText::Check(stringreader& _reader) { return (_reader.Peek() == ('\0')); }

#if defined(PLATFORM_PC)
            bool EndOfLine::Check(stringreader& _reader) { return Exact("\r\n").Check(_reader); }
#else
            bool EndOfLine::Check(stringreader& _reader) { return Exact("\n").Check(_reader); }
#endif

            bool Integer::Check(stringreader& _reader)
            {
                s32          value       = 0;
                stringreader input       = _reader;
                uchar32      c           = input.Peek();
                bool         is_negative = c == '-';
                if (is_negative)
                    input.Skip();
                while (input.Valid())
                {
                    c = input.Peek();
                    if (!utf32::is_digit(c))
                        break;
                    value = (value * 10) + utf32::to_digit(c);
                }
                if (input == _reader)
                    return false;
                if (is_negative)
                    value = -value;
                if (value >= m_min && value <= m_max)
                {
                    _reader = input;
                    return true;
                }
                return false;
            }

            bool Float::Check(stringreader& _reader)
            {
                f32          value       = 0.0f;
                stringreader input       = _reader;
                uchar32      c           = input.Peek();
                bool         is_negative = c == '-';
                if (is_negative)
                    input.Skip();
                while (input.Valid())
                {
                    c = input.Peek();
                    if (!utf32::is_digit(c))
                        break;
                    value = (value * 10) + utf32::to_digit(c);
                }
                if (c == '.')
                {
                    input.Skip();
                    f32 mantissa = 10.0f;
                    while (input.Valid())
                    {
                        c = input.Peek();
                        if (!utf32::is_digit(c))
                            break;
                        value = value + f32(utf32::to_digit(c)) / mantissa;
                        mantissa *= 10.0f;
                    }
                }
                if (input == _reader)
                    return false;
                if (is_negative)
                    value = -value;
                if (value >= m_min && value <= m_max)
                {
                    _reader = input;
                    return true;
                }
                return false;
            }
        } // namespace filters

        namespace utils
        {
            using namespace manipulators;
            using namespace filters;

            bool IPv4::Check(stringreader& _reader) { return (3 * ((Within(1, 3, DIGIT) & Integer(255)) + Is(('.'))) + (Within(1, 3, DIGIT) & filters::Integer(255))).Check(_reader); }

            bool Host::Check(stringreader& _reader)
            {
                return (IPV4 |
                        (OneOrMore(ALPHANUMERIC) + ZeroOrMore(Is(('-')) + OneOrMore(ALPHANUMERIC)) + ZeroOrMore(Is(('.')) + OneOrMore(ALPHANUMERIC) + ZeroOrMore(Is(('-')) + OneOrMore(ALPHANUMERIC)))))
                    .Check(_reader);
            }

            bool Email::Check(stringreader& _reader)
            {
                return (OneOrMore(ALPHANUMERIC | In(("!#$%&'*+/=?^_`{|}~-"))) + ZeroOrMore(Is('.') + (ALPHANUMERIC | In(("!#$%&'*+/=?^_`{|}~-")))) + Is('@') + HOST).Check(_reader);
            }

            bool Phone::Check(stringreader& _reader)
            {
                return (ZeroOrMore(Is(('+'))) + (ZeroOrMore(Is(('(')) + OneOrMore(DIGIT) + Is((')'))) + ZeroOrMore(WHITESPACE)) + OneOrMore(DIGIT) + ZeroOrMore(In((" -")) + OneOrMore(DIGIT)))
                    .Check(_reader);
            }

            bool ServerAddress::Check(stringreader& _reader) { return (HOST + ZeroOrOne(Is((':')) + Integer(1, 65535))).Check(_reader); }

            bool Uri::Check(stringreader& _reader)
            {
                return (OneOrMore(ALPHANUMERIC) + Is((':')) + (OneOrMore(ALPHANUMERIC | In(("!#$%&'*+/=?^_`{|}~-"))) + ZeroOrMore(Is('.') + (ALPHANUMERIC | In(("!#$%&'*+/=?^_`{|}~-"))))) + Is(('@')) +
                        SERVERADDRESS)
                    .Check(_reader);
            }
        } // namespace utils
    }     // namespace parser

    using namespace xcore::parser;

    /*********************************************************************************/
    stringprocessor::stringprocessor() {}
    stringprocessor::stringprocessor(stringreader const& str)
    {
        m_string = str;
        m_cursor = str;
    }

    bool stringprocessor::Parse(tokenizer& tok)
    {
        stringreader start  = m_cursor;
        bool         result = tok.Check(m_cursor);
        if (result)
            m_lastTokenized = stringreader(start, m_cursor);
        return result;
    }

    bool stringprocessor::Validate(tokenizer& tok) { return (tok + filters::EndOfText()).Check(m_cursor); }

    stringreader stringprocessor::Search(tokenizer& tok)
    {
        bool result = (manipulators::Until(filters::EOT | tok)).Check(m_cursor);
        if (result && m_cursor.Valid())
        {
            stringreader start = m_cursor;
            if ((tok).Check(m_cursor))
            {
                return stringreader(start, m_cursor);
            }
        }
        return stringreader();
    }

    bool stringprocessor::IsEOT() { return (m_cursor.Peek() == ('\0')); }

    void stringprocessor::Reset()
    {
        m_lastTokenized = stringreader();
        m_cursor        = m_string;
    }
} // namespace xcore
