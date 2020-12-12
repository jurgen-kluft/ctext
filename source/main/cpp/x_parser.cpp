#include "xtext/x_parser.h"

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

        reader_t::reader_t() : m_type(ASCII) {}

        reader_t::reader_t(const char* str) : m_type(ASCII)
        {
            if (str != nullptr)
            {
                m_runes._ascii = ascii::crunes(str);
            }
        }

        reader_t::reader_t(utf32::pcrune str) : m_type(UTF32)
        {
            if (str != nullptr)
            {
                m_runes._utf32 = utf32::crunes(str);
            }
        }

        reader_t::reader_t(const reader_t& t) : m_type(t.m_type) { m_runes._ascii = t.m_runes._ascii; }

        s64 reader_t::size() const
        {
            switch (m_type)
            {
                case ASCII: return m_runes._ascii.is_empty() ? 0 : m_runes._ascii.size();
                case UTF32: return m_runes._utf32.is_empty() ? 0 : m_runes._utf32.size();
            }
            return 0;
        }

        void reader_t::reset() { m_runes._ascii.reset(); }

        bool reader_t::valid() const
        {
            switch (m_type)
            {
                case ASCII: return m_runes._ascii.is_valid();
                case UTF32: return m_runes._utf32.is_valid();
            }
            return false;
        }

        uchar32 reader_t::peek() const
        {
            switch (m_type)
            {
                case ASCII: return m_runes._ascii.is_empty() ? '\0' : *m_runes._ascii.m_cur;
                case UTF32: return m_runes._utf32.is_empty() ? '\0' : *m_runes._utf32.m_cur;
            }
            return '\0';
        }

        uchar32 reader_t::read()
        {
            switch (m_type)
            {
                case ASCII: return m_runes._ascii.is_empty() ? '\0' : *m_runes._ascii.m_cur++;
                case UTF32: return m_runes._utf32.is_empty() ? '\0' : *m_runes._utf32.m_cur++;
            }
            return '\0';
        }

        void reader_t::skip()
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

        void reader_t::select(reader_t const& from, reader_t const& until)
        {
            m_runes._ascii.m_str = from.m_runes._ascii.m_cur;
            m_runes._ascii.m_cur = m_runes._ascii.m_str;
            m_runes._ascii.m_end = until.m_runes._ascii.m_cur;
        }

        reader_t::cursor_t& reader_t::cursor_t::operator=(const reader_t::cursor_t& t)
        {
            m_runes._ascii = t.m_runes._ascii;
            return *this;
        }

        bool reader_t::cursor_t::operator<(const reader_t::cursor_t& t) const { return m_runes._ascii.m_cur < t.m_runes._ascii.m_cur; }
        bool reader_t::cursor_t::operator>(const reader_t::cursor_t& t) const { return m_runes._ascii.m_cur > t.m_runes._ascii.m_cur; }
        bool reader_t::cursor_t::operator<=(const reader_t::cursor_t& t) const { return m_runes._ascii.m_cur <= t.m_runes._ascii.m_cur; }
        bool reader_t::cursor_t::operator>=(const reader_t::cursor_t& t) const { return m_runes._ascii.m_cur >= t.m_runes._ascii.m_cur; }
        bool reader_t::cursor_t::operator==(const reader_t::cursor_t& t) const
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

        bool reader_t::cursor_t::operator!=(const reader_t::cursor_t& t) const
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
            bool Not::Check(xtext::reader_t& _reader, xtext::reader_t::cursor_t& _cursor)
            {
                stringreader start = _reader;
                if (!m_tokenizer_a.Check(start))
                {
                    _reader.Skip();
                    return true;
                }
                return false;
            }

            bool Or::Check(xtext::reader_t& _reader, xtext::reader_t::cursor_t& _cursor)
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

            bool And::Check(xtext::reader_t& _reader, xtext::reader_t::cursor_t& _cursor)
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

            bool Sequence::Check(xtext::reader_t& _reader, xtext::reader_t::cursor_t& _cursor)
            {
                stringreader start = _reader;

                if (!m_tokenizer_a.Check(start))
                    return false;

                if (!m_tokenizer_b.Check(start))
                    return false;

                _reader = start;
                return true;
            }

            bool Within::Check(xtext::reader_t& _reader, xtext::reader_t::cursor_t& _cursor)
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

            bool Times::Check(xtext::reader_t& _reader, xtext::reader_t::cursor_t& _cursor) { return Within(m_max, m_max, m_tokenizer_a).Check(_reader, _cursor); }
            bool OneOrMore::Check(xtext::reader_t& _reader, xtext::reader_t::cursor_t& _cursor) { return Within(1, -1, m_tokenizer_a).Check(_reader, _cursor); }
            bool ZeroOrOne::Check(xtext::reader_t& _reader, xtext::reader_t::cursor_t& _cursor) { return Within(0, 1, m_tokenizer_a).Check(_reader, _cursor); }
            bool While::Check(xtext::reader_t& _reader, xtext::reader_t::cursor_t& _cursor) { return Within(0, -1, m_tokenizer_a).Check(_reader, _cursor); }
            bool Until::Check(xtext::reader_t& _reader, xtext::reader_t::cursor_t& _cursor) { return (While(Not(m_tokenizer_a))).Check(_reader, _cursor); }

            bool Extract::Check(xtext::reader_t& _reader, xtext::reader_t::cursor_t& _cursor)
            {
                stringreader start = _reader;

                bool result = m_tokenizer_a.Check(start);
                m_selection = stringreader(start, _reader);

                _reader = start;
                return result;
            }

            bool ReturnToCallback::Check(xtext::reader_t& _reader, xtext::reader_t::cursor_t& _cursor)
            {
                stringreader start  = _reader;
                bool         result = m_tokenizer_a.Check(start);

                stringreader output = stringreader(start, _reader);
                m_cb(output);

                _reader = start;
                return result;
            }

            bool Enclosed::Check(xtext::reader_t& _reader, xtext::reader_t::cursor_t& _cursor) { return (filters::Exact(m_open) + m_tokenizer_a + filters::Exact(m_close)).Check(_reader, _cursor); }
        } // namespace manipulators

        namespace filters
        {
            bool Any::Check(xtext::reader_t& _reader, xtext::reader_t::cursor_t& _cursor)
            {
                if (!_reader.Valid())
                    return false;
                _reader.Skip();
                return true;
            }

            bool In::Check(xtext::reader_t& _reader, xtext::reader_t::cursor_t& _cursor)
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

            bool Between::Check(xtext::reader_t& _reader, xtext::reader_t::cursor_t& _cursor)
            {
                uchar32 c = _reader.Peek();
                if (c >= m_lower && c <= m_upper)
                {
                    _reader.Skip();
                    return true;
                }
                return false;
            }

            bool Alphabet::Check(xtext::reader_t& _reader, xtext::reader_t::cursor_t& _cursor) { return (Between(('a'), ('z')).Check(_reader, _cursor) | Between(('A'), ('Z')).Check(_reader, _cursor)); }
            bool Digit::Check(xtext::reader_t& _reader, xtext::reader_t::cursor_t& _cursor) { return Between(('0'), ('9')).Check(_reader, _cursor); }
            bool Hex::Check(xtext::reader_t& _reader, xtext::reader_t::cursor_t& _cursor) { return (DIGIT | Between(('a'), ('f')) | Between(('A'), ('F'))).Check(_reader, _cursor); }
            bool AlphaNumeric::Check(xtext::reader_t& _reader, xtext::reader_t::cursor_t& _cursor) { return (ALPHABET | DIGIT).Check(_reader, _cursor); }

            bool Exact::Check(xtext::reader_t& _reader, xtext::reader_t::cursor_t& _cursor)
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

            bool Like::Check(xtext::reader_t& _reader, xtext::reader_t::cursor_t& _cursor)
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

            bool WhiteSpace::Check(xtext::reader_t& _reader, xtext::reader_t::cursor_t& _cursor) { return In((" \t\n\r")).Check(_reader, _cursor); }

            bool Is::Check(xtext::reader_t& _reader, xtext::reader_t::cursor_t& _cursor)
            {
                if (_reader.Peek() == _Letter)
                {
                    _reader.Skip();
                    return true;
                }
                return false;
            }

            bool Decimal::Check(xtext::reader_t& _reader, xtext::reader_t::cursor_t& _cursor) { return manipulators::OneOrMore(DIGIT).Check(_reader, _cursor); }
            bool Word::Check(xtext::reader_t& _reader, xtext::reader_t::cursor_t& _cursor) { return manipulators::OneOrMore(ALPHABET).Check(_reader, _cursor); }
            bool EndOfText::Check(xtext::reader_t& _reader, xtext::reader_t::cursor_t& _cursor) { return (_reader.Peek() == ('\0')); }

#if defined(PLATFORM_PC)
            bool EndOfLine::Check(xtext::reader_t& _reader, xtext::reader_t::cursor_t& _cursor) { return Exact("\r\n").Check(_reader, _cursor); }
#else
            bool EndOfLine::Check(xtext::reader_t& _reader, xtext::reader_t::cursor_t& _cursor) { return Exact("\n").Check(_reader, _cursor); }
#endif

            bool Integer::Check(xtext::reader_t& _reader, xtext::reader_t::cursor_t& _cursor)
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

            bool Float::Check(xtext::reader_t& _reader, xtext::reader_t::cursor_t& _cursor)
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

            bool IPv4::Check(xtext::reader_t& _reader, xtext::reader_t::cursor_t& _cursor) { return (3 * ((Within(1, 3, DIGIT) & Integer(255)) + Is(('.'))) + (Within(1, 3, DIGIT) & filters::Integer(255))).Check(_reader, _cursor); }

            bool Host::Check(xtext::reader_t& _reader, xtext::reader_t::cursor_t& _cursor)
            {
                return (IPV4 |
                        (OneOrMore(ALPHANUMERIC) + ZeroOrMore(Is(('-')) + OneOrMore(ALPHANUMERIC)) + ZeroOrMore(Is(('.')) + OneOrMore(ALPHANUMERIC) + ZeroOrMore(Is(('-')) + OneOrMore(ALPHANUMERIC)))))
                    .Check(_reader, _cursor);
            }

            bool Email::Check(xtext::reader_t& _reader, xtext::reader_t::cursor_t& _cursor)
            {
                return (OneOrMore(ALPHANUMERIC | In(("!#$%&'*+/=?^_`{|}~-"))) + ZeroOrMore(Is('.') + (ALPHANUMERIC | In(("!#$%&'*+/=?^_`{|}~-")))) + Is('@') + HOST).Check(_reader, _cursor);
            }

            bool Phone::Check(xtext::reader_t& _reader, xtext::reader_t::cursor_t& _cursor)
            {
                return (ZeroOrMore(Is(('+'))) + (ZeroOrMore(Is(('(')) + OneOrMore(DIGIT) + Is((')'))) + ZeroOrMore(WHITESPACE)) + OneOrMore(DIGIT) + ZeroOrMore(In((" -")) + OneOrMore(DIGIT)))
                    .Check(_reader, _cursor);
            }

            bool ServerAddress::Check(xtext::reader_t& _reader, xtext::reader_t::cursor_t& _cursor) { return (HOST + ZeroOrOne(Is((':')) + Integer(1, 65535))).Check(_reader, _cursor); }

            bool Uri::Check(xtext::reader_t& _reader, xtext::reader_t::cursor_t& _cursor)
            {
                return (OneOrMore(ALPHANUMERIC) + Is((':')) + (OneOrMore(ALPHANUMERIC | In(("!#$%&'*+/=?^_`{|}~-"))) + ZeroOrMore(Is('.') + (ALPHANUMERIC | In(("!#$%&'*+/=?^_`{|}~-"))))) + Is(('@')) +
                        SERVERADDRESS)
                    .Check(_reader, _cursor);
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
