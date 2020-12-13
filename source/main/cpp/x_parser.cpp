#include "xtext/x_parser.h"

namespace xcore
{
    namespace xtext
    {
        enum echartype
        {
            NONE = -1,
            ASCII = 0,
            UTF8 = 1,
            UTF16 = 2,
            UTF32 = 4
        };

        reader_t::reader_t() : m_type(ASCII) {}

        reader_t::reader_t(const char *str) : m_type(ASCII)
        {
            if (str != nullptr)
            {
                m_runes.m_ascii = ascii::crunes(str);
                m_begin.m_cursor.m_ascii = m_runes.m_ascii.m_str;
                m_end.m_cursor.m_ascii = m_runes.m_ascii.m_end;
            }
        }

        reader_t::reader_t(utf32::pcrune str) : m_type(UTF32)
        {
            if (str != nullptr)
            {
                m_runes.m_utf32 = utf32::crunes(str);
                m_begin.m_cursor.m_utf32 = m_runes.m_utf32.m_str;
                m_end.m_cursor.m_utf32 = m_runes.m_utf32.m_end;
            }
        }

        reader_t::reader_t(const reader_t &t) : m_type(t.m_type), m_begin(t.m_begin), m_end(t.m_end) { m_runes.m_ascii = t.m_runes.m_ascii; }

        reader_t::reader_t(const reader_t &t, const cursor_t &begin, const cursor_t &end) : m_type(t.m_type), m_begin(begin), m_end(end)
        {
            m_runes.m_ascii = t.m_runes.m_ascii;
            //@TODO: validate begin and end
        }

        reader_t::cursor_t reader_t::get_cursor() const { return m_begin; }

        s64 reader_t::size() const
        {
            switch (m_type)
            {
            case ASCII:
                return m_runes.m_ascii.is_empty() ? 0 : m_runes.m_ascii.size();
            case UTF32:
                return m_runes.m_utf32.is_empty() ? 0 : m_runes.m_utf32.size();
            }
            return 0;
        }

        void reader_t::reset(cursor_t &cursor) const { cursor = m_begin; }

        bool reader_t::valid(cursor_t const &cursor) const { return cursor >= m_begin && cursor < m_end; }

        uchar32 reader_t::peek(cursor_t const &cursor) const
        {
            uchar32 c = '\0';
            if (valid(cursor))
            {
                switch (m_type)
                {
                case ASCII:
                    c = *cursor.m_cursor.m_ascii;
                    break;
                case UTF32:
                    c = *cursor.m_cursor.m_utf32;
                    break;
                }
            }
            return c;
        }

        uchar32 reader_t::read(cursor_t &cursor) const
        {
            uchar32 c = '\0';
            if (valid(cursor))
            {
                c = peek(cursor);
                skip(cursor);
            }
            return c;
        }

        void reader_t::skip(cursor_t &cursor) const
        {
            if (valid(cursor))
            {
                switch (m_type)
                {
                case ASCII:
                    cursor.m_cursor.m_ascii++;
                    break;
                case UTF32:
                    cursor.m_cursor.m_utf32++;
                    break;
                }
            }
        }

        reader_t reader_t::select(cursor_t const &from, cursor_t const &to) const
        {
            reader_t reader;
            reader.m_type = reader.m_type;
            reader.m_runes.m_ascii = m_runes.m_ascii;
            //@todo: reader.m_begin
            //@todo: reader.m_end
            if (valid(from))
            {
            }

            return reader;
        }

        reader_t &reader_t::operator=(const reader_t &other)
        {
            m_type = other.m_type;
            m_runes.m_ascii = other.m_runes.m_ascii;
            m_begin = other.m_begin;
            m_end = other.m_end;
            return *this;
        }

        bool reader_t::operator==(const reader_t &other) const { return other.m_type == m_type && other.m_runes.m_ascii == m_runes.m_ascii && other.m_begin == m_begin && other.m_end == m_end; }
        bool reader_t::operator!=(const reader_t &other) const { return other.m_type != m_type || other.m_runes.m_ascii != m_runes.m_ascii || other.m_begin != m_begin || other.m_end != m_end; }

        reader_t::cursor_t &reader_t::cursor_t::operator=(const reader_t::cursor_t &t)
        {
            m_cursor = t.m_cursor;
            return *this;
        }

        bool reader_t::cursor_t::operator<(const reader_t::cursor_t &t) const { return m_cursor.m_ascii < t.m_cursor.m_ascii; }
        bool reader_t::cursor_t::operator>(const reader_t::cursor_t &t) const { return m_cursor.m_ascii > t.m_cursor.m_ascii; }
        bool reader_t::cursor_t::operator<=(const reader_t::cursor_t &t) const { return m_cursor.m_ascii <= t.m_cursor.m_ascii; }
        bool reader_t::cursor_t::operator>=(const reader_t::cursor_t &t) const { return m_cursor.m_ascii >= t.m_cursor.m_ascii; }
        bool reader_t::cursor_t::operator==(const reader_t::cursor_t &t) const { return m_cursor.m_ascii == t.m_cursor.m_ascii; }
        bool reader_t::cursor_t::operator!=(const reader_t::cursor_t &t) const { return m_cursor.m_ascii != t.m_cursor.m_ascii; }
    } // namespace xtext

    namespace xparser
    {
        xfilters::Any xfilters::sAny;
        xfilters::Alphabet xfilters::sAlphabet;
        xfilters::AlphaNumeric xfilters::sAlphaNumeric;
        xfilters::Digit xfilters::sDigit;
        xfilters::Decimal xfilters::sDecimal;
        xfilters::EndOfLine xfilters::sEOL;
        xfilters::EndOfText xfilters::sEOT;
        xfilters::Hex xfilters::sHex;
        xfilters::WhiteSpace xfilters::sWhitespace;
        xfilters::Word xfilters::sWord;
        xutils::Email xutils::sEmail;
        xutils::Host xutils::sHost;
        xutils::IPv4 xutils::sIPv4;
        xutils::Phone xutils::sPhone;
        xutils::ServerAddress xutils::sServerAddress;
        xutils::Uri xutils::sURI;

        namespace xmanipulators
        {
            bool Not::Check(xtext::reader_t &_reader, xtext::reader_t::cursor_t &_cursor)
            {
                xtext::reader_t::cursor_t start = _cursor;
                if (!m_tokenizer_a.Check(_reader, start))
                {
                    _cursor = start;
                    return true;
                }
                return false;
            }

            bool Or::Check(xtext::reader_t &_reader, xtext::reader_t::cursor_t &_cursor)
            {
                xtext::reader_t::cursor_t start = _cursor;
                if (!m_tokenizer_a.Check(_reader, start))
                {
                    if (!m_tokenizer_b.Check(_reader, start))
                    {
                        return false;
                    }
                }
                _cursor = start;
                return true;
            }

            bool And::Check(xtext::reader_t &_reader, xtext::reader_t::cursor_t &_cursor)
            {
                xtext::reader_t::cursor_t start1 = _cursor;
                if (!m_tokenizer_a.Check(_reader, start1))
                    return false;

                xtext::reader_t::cursor_t start2 = _cursor;
                if (!m_tokenizer_b.Check(_reader, start2))
                    return false;

                start1 = ((start2 < start1) ? start2 : start1);

                _cursor = start1;
                return true;
            }

            bool Sequence::Check(xtext::reader_t &_reader, xtext::reader_t::cursor_t &_cursor)
            {
                xtext::reader_t::cursor_t start = _cursor;

                if (!m_tokenizer_a.Check(_reader, start))
                    return false;

                if (!m_tokenizer_b.Check(_reader, start))
                    return false;

                _cursor = start;
                return true;
            }

            bool Sequence3::Check(xtext::reader_t &_reader, xtext::reader_t::cursor_t &_cursor)
            {
                xtext::reader_t::cursor_t start = _cursor;

                if (!m_tokenizer_a.Check(_reader, start))
                    return false;
                if (!m_tokenizer_b.Check(_reader, start))
                    return false;
                if (!m_tokenizer_c.Check(_reader, start))
                    return false;

                _cursor = start;
                return true;
            }

            bool Within::Check(xtext::reader_t &_reader, xtext::reader_t::cursor_t &_cursor)
            {
                xtext::reader_t::cursor_t start = _cursor;

                u64 i = 0;
                for (; i < m_max; i++)
                {
                    if (!m_tokenizer_a.Check(_reader, start))
                        break;
                }

                if (i >= m_min && i <= m_max)
                {
                    _cursor = start;
                    return true;
                }

                return false;
            }

            bool Times::Check(xtext::reader_t &_reader, xtext::reader_t::cursor_t &_cursor) { return Within(m_max, m_max, m_tokenizer_a).Check(_reader, _cursor); }
            bool OneOrMore::Check(xtext::reader_t &_reader, xtext::reader_t::cursor_t &_cursor) { return Within(1, -1, m_tokenizer_a).Check(_reader, _cursor); }
            bool ZeroOrOne::Check(xtext::reader_t &_reader, xtext::reader_t::cursor_t &_cursor) { return Within(0, 1, m_tokenizer_a).Check(_reader, _cursor); }
            bool While::Check(xtext::reader_t &_reader, xtext::reader_t::cursor_t &_cursor) { return Within(0, -1, m_tokenizer_a).Check(_reader, _cursor); }
            bool Until::Check(xtext::reader_t &_reader, xtext::reader_t::cursor_t &_cursor)
            {
                Not n(m_tokenizer_a);
                return (While(n)).Check(_reader, _cursor);
            }

            bool Extract::Check(xtext::reader_t &_reader, xtext::reader_t::cursor_t &_cursor)
            {
                xtext::reader_t::cursor_t end = _cursor;

                bool result = m_tokenizer_a.Check(_reader, end);
                m_selection = _reader.select(_cursor, end);

                _cursor = end;
                return result;
            }

            bool ReturnToCallback::Check(xtext::reader_t &_reader, xtext::reader_t::cursor_t &_cursor)
            {
                xtext::reader_t::cursor_t start = _cursor;
                bool result = m_tokenizer_a.Check(_reader, start);

                m_cb(_reader, start);

                _cursor = start;
                return result;
            }

            bool Enclosed::Check(xtext::reader_t &_reader, xtext::reader_t::cursor_t &_cursor)
            {
                xfilters::Exact open(m_open);
                xfilters::Exact close(m_close);
                Sequence a(open, m_tokenizer_a);
                Sequence b(a, close);
                return b.Check(_reader, _cursor);
            }
        } // namespace xmanipulators

        namespace xfilters
        {
            bool Any::Check(xtext::reader_t &_reader, xtext::reader_t::cursor_t &_cursor)
            {
                if (!_reader.valid(_cursor))
                    return false;
                _reader.skip(_cursor);
                return true;
            }

            bool In::Check(xtext::reader_t &_reader, xtext::reader_t::cursor_t &_cursor)
            {
                if (!_reader.valid(_cursor))
                    return false;

                uchar32 const s = _reader.peek(_cursor);
                m_input.reset(_cursor);
                while (m_input.valid(_cursor))
                {
                    uchar32 const c = m_input.read(_cursor);
                    if (c == s)
                    {
                        _reader.skip(_cursor);
                        return true;
                    }
                }

                return false;
            }

            bool Between::Check(xtext::reader_t &_reader, xtext::reader_t::cursor_t &_cursor)
            {
                uchar32 c = _reader.peek(_cursor);
                if (c >= m_lower && c < m_upper)
                {
                    _reader.skip(_cursor);
                    return true;
                }
                return false;
            }

            bool Alphabet::Check(xtext::reader_t &_reader, xtext::reader_t::cursor_t &_cursor) { return (m_lower_case.Check(_reader, _cursor) | m_upper_case.Check(_reader, _cursor)); }
            bool Digit::Check(xtext::reader_t &_reader, xtext::reader_t::cursor_t &_cursor) { return m_digit.Check(_reader, _cursor); }
            bool Hex::Check(xtext::reader_t &_reader, xtext::reader_t::cursor_t &_cursor)
            {
                xtext::reader_t::cursor_t start = _cursor;
                if (!m_digit.Check(_reader, start))
                {
                    if (!m_lower_case.Check(_reader, start))
                    {
                        if (!m_upper_case.Check(_reader, start))
                        {
                            return false;
                        }
                    }
                }
                return true;
            }
            bool AlphaNumeric::Check(xtext::reader_t &_reader, xtext::reader_t::cursor_t &_cursor) { return (sAlphabet | sDigit).Check(_reader, _cursor); }

            bool Exact::Check(xtext::reader_t &_reader, xtext::reader_t::cursor_t &_cursor)
            {
                xtext::reader_t::cursor_t rcursor = _cursor;
                xtext::reader_t::cursor_t icursor = m_input.get_cursor();
                while (m_input.valid(icursor))
                {
                    uchar32 a = _reader.peek(rcursor);
                    uchar32 b = m_input.peek(icursor);
                    if (a != b)
                        return false;
                    m_input.skip(icursor);
                    _reader.skip(rcursor);
                }
                _cursor = rcursor;
                return true;
            }

            bool Like::Check(xtext::reader_t &_reader, xtext::reader_t::cursor_t &_cursor)
            {
                xtext::reader_t::cursor_t rcursor = _cursor;
                xtext::reader_t::cursor_t icursor = m_input.get_cursor();
                while (m_input.valid(icursor))
                {
                    uchar32 a = _reader.peek(rcursor);
                    uchar32 b = m_input.peek(icursor);
                    if (a != b && (utf32::to_lower(a) != utf32::to_lower(b)))
                        return false;
                    m_input.skip(icursor);
                    _reader.skip(rcursor);
                }
                _cursor = rcursor;
                return true;
            }

            bool WhiteSpace::Check(xtext::reader_t &_reader, xtext::reader_t::cursor_t &_cursor) { return m_whitespace.Check(_reader, _cursor); }

            bool Is::Check(xtext::reader_t &_reader, xtext::reader_t::cursor_t &_cursor)
            {
                if (_reader.peek(_cursor) == m_char)
                {
                    _reader.skip(_cursor);
                    return true;
                }
                return false;
            }

            bool Decimal::Check(xtext::reader_t &_reader, xtext::reader_t::cursor_t &_cursor) { return xmanipulators::OneOrMore(sDigit).Check(_reader, _cursor); }
            bool Word::Check(xtext::reader_t &_reader, xtext::reader_t::cursor_t &_cursor) { return xmanipulators::OneOrMore(sAlphabet).Check(_reader, _cursor); }
            bool EndOfText::Check(xtext::reader_t &_reader, xtext::reader_t::cursor_t &_cursor) { return (_reader.peek(_cursor) == ('\0')); }

#if defined(PLATFORM_PC)
            bool EndOfLine::Check(xtext::reader_t &_reader, xtext::reader_t::cursor_t &_cursor)
            {
                return Exact("\r\n").Check(_reader, _cursor);
            }
#else
            bool EndOfLine::Check(xtext::reader_t &_reader, xtext::reader_t::cursor_t &_cursor)
            {
                return Exact("\n").Check(_reader, _cursor);
            }
#endif

            bool Integer::Check(xtext::reader_t &_reader, xtext::reader_t::cursor_t &_cursor)
            {
                s64 value = 0;
                xtext::reader_t::cursor_t cursor = _cursor;
                uchar32 c = _reader.peek(cursor);
                bool is_negative = (c == '-');
                if (is_negative)
                    _reader.skip(cursor);
                while (_reader.valid(cursor))
                {
                    c = _reader.peek(cursor);
                    if (!utf32::is_digit(c))
                        break;
                    value = (value * 10) + utf32::to_digit(c);
                }
                if (cursor == _cursor)
                    return false;
                if (is_negative)
                    value = -value;
                if (value >= m_min && value <= m_max)
                {
                    _cursor = cursor;
                    return true;
                }
                return false;
            }

            bool Float::Check(xtext::reader_t &_reader, xtext::reader_t::cursor_t &_cursor)
            {
                f32 value = 0.0f;
                xtext::reader_t::cursor_t cursor = _cursor;
                uchar32 c = _reader.peek(cursor);
                bool is_negative = c == '-';
                if (is_negative)
                    _reader.skip(cursor);
                while (_reader.valid(cursor))
                {
                    c = _reader.peek(cursor);
                    if (!utf32::is_digit(c))
                        break;
                    value = (value * 10) + utf32::to_digit(c);
                }
                if (c == '.')
                {
                    _reader.skip(cursor);
                    f32 mantissa = 10.0f;
                    while (_reader.valid(cursor))
                    {
                        c = _reader.peek(cursor);
                        if (!utf32::is_digit(c))
                            break;
                        value = value + f32(utf32::to_digit(c)) / mantissa;
                        mantissa *= 10.0f;
                    }
                }
                if (cursor == _cursor)
                    return false;
                if (is_negative)
                    value = -value;
                if (value >= m_min && value <= m_max)
                {
                    _cursor = cursor;
                    return true;
                }
                return false;
            }
        } // namespace xfilters

        namespace xutils
        {
            using namespace xmanipulators;
            using namespace xfilters;

            bool IPv4::Check(xtext::reader_t &_reader, xtext::reader_t::cursor_t &_cursor) { return m_ipv4.Check(_reader, _cursor); }

            bool Host::Check(xtext::reader_t &_reader, xtext::reader_t::cursor_t &_cursor)
            {
                if (sIPv4.Check(_reader, _cursor))
                {
                    return true;
                }
                OneOrMore oom_an(sAlphaNumeric);
                Is dash('-');
                Is dot('.');
                Sequence dash_oom_an(dash, oom_an);
                Sequence dot_oom_an(dot, oom_an);
                ZeroOrMore zom_dash_oom_an(dash_oom_an);
                Sequence dot_oom_an_zom_dash_oom_an(dot_oom_an, zom_dash_oom_an);
                ZeroOrMore host_tail(dot_oom_an_zom_dash_oom_an);
                Sequence host_head(oom_an, host_tail);
                Sequence host(host_head, host_tail);
                return host.Check(_reader, _cursor);
            }

            static const char *sValidEmailUriChars = "!#$%&'*+/=?^_`{|}~-";

            bool Email::Check(xtext::reader_t &_reader, xtext::reader_t::cursor_t &_cursor)
            {
                In validchars(sValidEmailUriChars);
                Or valid(sAlphaNumeric, validchars);
                OneOrMore oom_valid(valid);
                Is dot('.');
                Sequence dot_valid(dot, valid);
                ZeroOrMore zom_dot_valid(dot_valid);
                Is mt('@');
                Sequence a(oom_valid, zom_dot_valid);
                Sequence b(mt, sHost);
                Sequence email(a, b);
                return email.Check(_reader, _cursor);
            }
            bool Phone::Check(xtext::reader_t &_reader, xtext::reader_t::cursor_t &_cursor)
            {
                /*
                ZeroOrMore zom_plus(Is('+'));
                Is         open('(');
                Is         close('(');
                OneOrMore  oomdigit(sDIGIT);
                Sequence3  open_oomdigit_close(open, oomdigit, close);
                ZeroOrMore zom_open_oomdigit_close(open_oomdigit_close);
                ZeroOrMore zom_whitespace(sWHITESPACE);
                In         spaceordash(" -");
                Sequence   spaceordash_oomdigit(spaceordash, oomdigit);
                ZeroOrMore zom_spaceordash_oomdigit(spaceordash_oomdigit);

                Sequence zom_open_oomdigit_close_whitespace(zom_open_oomdigit_close, zom_whitespace);
                
                ZeroOrMore(Is('+')) + 
                (
                    ZeroOrMore(Is('(') + OneOrMore(DIGIT) + Is(')')) +
                    ZeroOrMore(WHITESPACE)
                    ) + 
                    OneOrMore(DIGIT) + 
                    ZeroOrMore(In(" -") + OneOrMore(DIGIT))
*/
                //return (ZeroOrMore(Is(('+'))) + (ZeroOrMore(Is('(') + OneOrMore(DIGIT) + Is(')')) + ZeroOrMore(WHITESPACE)) + OneOrMore(DIGIT) + ZeroOrMore(In(" -") + OneOrMore(DIGIT)))
                //    .Check(_reader, _cursor);
            }

            /*

                        bool ServerAddress::Check(xtext::reader_t& _reader, xtext::reader_t::cursor_t& _cursor)
                        {
                            return (HOST + ZeroOrOne(Is(':') + Integer(1, 65535))).Check(_reader, _cursor);
                        }

                        bool Uri::Check(xtext::reader_t& _reader, xtext::reader_t::cursor_t& _cursor)
                        {
                            return (OneOrMore(ALPHANUMERIC) + Is(':') + (OneOrMore(ALPHANUMERIC | In(sValidEmailUriChars)) + ZeroOrMore(Is('.') + (ALPHANUMERIC | In(sValidEmailUriChars))))
               + Is('@') + SERVERADDRESS) .Check(_reader, _cursor);
                        }
             */
        } // namespace xutils
    }     // namespace xparser

    namespace parser_design
    {
        class machine_t
        {
            machine_t &PushOpcode(int opcode);
            enum
            {
                ePop,
                eNot,
                eOr,
                eAnd,
                eSequence,
                eSequence3,
                eWithin,
                eTimes,
                eOneOrMore,
                eZeroOrOne,
                eWhile,
                eUntil,
                eExtract,
                eReturnToCallback,
                eEnclosed,
                eAny,
                eIn,
                eBetween,
                eAlphabet,
                eDigit,
                eHex,
                eAlphaNumeric,
                eExact,
                eLike,
                eWhiteSpace,
                eIs,
                eDecimal,
                eWord,
                eEndOfText,
                eEndOfLine,
                eInteger,
                eFloat,
                eIPv4,
                eHost,
                eEmail,
                ePhone,
                eServerAddress,
                eUri
            };

        public:
            bool execute(xtext::reader_t const &reader, xtext::reader_t::cursor_t &cursor);

            machine_t &Pop();
            machine_t &Not();
            machine_t &Or();
            machine_t &And();
            machine_t &Sequence();
            machine_t &Within(s32 _min = 0, s32 _max = 0x7fffffff);
            machine_t &Times(s32 _count);
            machine_t &OneOrMore();
            machine_t &ZeroOrOne();
            machine_t &While();
            machine_t &Until();
            machine_t &Extract();
            machine_t &Enclosed(uchar32 _open, u32char _close);
            machine_t &Any();
            machine_t &In(xtext::reader const &_chars);
            machine_t &Between(uchar32 _from, u32char _until);
            machine_t &Alphabet();
            machine_t &Digit();
            machine_t &Hex();
            machine_t &AlphaNumeric();
            machine_t &Exact(xtext::reader const &_text); // Case-Sensitive
            machine_t &Like(xtext::reader const &_text);  // Case-Insensitive
            machine_t &WhiteSpace();
            machine_t &Is(uchar32 _c);
            machine_t &Word();
            machine_t &EndOfText();
            machine_t &EndOfLine();
            machine_t &Unsigned32(u32 _min = 0, u32 _max = 0xffffffff);
            machine_t &Unsigned64(u64 _min = 0, u64 _max = 0xffffffffffffffffUL);
            machine_t &Integer32(s32 _min = 0, s32 _max = 0x7fffffff);
            machine_t &Integer64(s64 _min = 0, s64 _max = 0x7fffffffffffffffL);
            machine_t &Float32(f32 _min = 0.0f, f32 _max = 3.402823e+38f);
            machine_t &Float64(f64 _min = 0.0, f64 _max = 3.402823e+38f);

            machine_t &Email();
            machine_t &Host();

            //machine_t &Date();
            //machine_t &Time();
            //machine_t &IPv4();
            //machine_t &Phone();
            //machine_t &ServerAddress();
            //machine_t &URI();
        };

        static void use_case()
        {
            static const char *sValidEmailUriChars = "!#$%&'*+/=?^_`{|}~-";

            // Example: This is the run that can validate an email addresse
            // Comment: Pretty and readable :-)
            // Thoughts: We can even precompile certain runs to be reused

            // Q: How to extract certain parts?

            // clang-format off
            machine_t p;
            p.Or();
                p.AlphaNumeric();
                p.In(sValidEmailUriChars)
            p.Pop();

            machine_t m;
            m.Sequence();
                m.Extract();        // e.g. john.doe
                    m.OneOrMore();
                        m.Run(p);
                    m.Pop();
                    m.ZeroOrMore();
                        m.Sequence();
                            m.Or();
                                m.Is('.');
                                m.Is('_');
                            m.Pop();
                            m.Run(p);
                        m.Pop();
                    m.Pop();
                m.Pop();

                m.Is('@');
                m.Extract();        // e.g. hotmail.com
                    m.Host();
                m.Pop();
            m.Pop();

            // clang-format on

            xtext::reader_t reader;
            xtext::reader_t::cursor_t cursor;
            m.execute(reader, cursor);
        }

    } // namespace parser_design

    using namespace xcore::xparser;

    stringprocessor_t::stringprocessor_t() {}
    stringprocessor_t::stringprocessor_t(xtext::reader_t const &str)
    {
        m_string = str;
        m_cursor = str.get_cursor();
    }
    stringprocessor_t::stringprocessor_t(xtext::reader_t const &str, xtext::reader_t::cursor_t cursor)
    {
        m_string = str;
        m_cursor = cursor;
    }

    bool stringprocessor_t::parse(xparser::tokenizer_t &tok)
    {
        xtext::reader_t::cursor_t start = m_cursor;
        bool result = tok.Check(m_string, start);
        if (result)
        {
            m_lastTokenized = m_cursor;
            m_cursor = start;
        }
        return result;
    }

    bool stringprocessor_t::validate(xparser::tokenizer_t &tok)
    {
        xmanipulators::Sequence validate(tok, xfilters::sEOT);
        return validate.Check(m_string, m_cursor);
    }

    xtext::reader_t stringprocessor_t::search(tokenizer_t &tok)
    {
        xmanipulators::Or search(xfilters::sEOT, tok);
        bool result = xmanipulators::Until(search).Check(m_string, m_cursor);
        if (result && m_string.valid(m_cursor))
        {
            xtext::reader_t::cursor_t start = m_cursor;
            if (tok.Check(m_string, m_cursor))
            {
                return xtext::reader_t(m_string, start, m_cursor);
            }
        }
        return xtext::reader_t();
    }

    bool stringprocessor_t::isEOT() { return (m_string.peek(m_cursor) == ('\0')); }

    void stringprocessor_t::reset()
    {
        m_lastTokenized = xtext::reader_t::cursor_t();
        m_cursor = m_string.get_cursor();
    }
} // namespace xcore
