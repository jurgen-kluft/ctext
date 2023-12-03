#include "cbase/c_runes.h"
#include "cbase/c_runes.h"
#include "ctext/c_parser.h"

namespace ncore
{
    namespace combparser
    {
        filters::Any          filters::sAny;
        filters::Alphabet     filters::sAlphabet;
        filters::AlphaNumeric filters::sAlphaNumeric;
        filters::Digit        filters::sDigit;
        filters::Decimal      filters::sDecimal;
        filters::EndOfLine    filters::sEOL;
        filters::EndOfText    filters::sEOT;
        filters::Hex          filters::sHex;
        filters::WhiteSpace   filters::sWhitespace;
        filters::Word         filters::sWord;
        utils::Email          utils::sEmail;
        utils::Host           utils::sHost;
        utils::IPv4           utils::sIPv4;
        utils::ServerAddress  utils::sServerAddress;
        utils::Uri            utils::sURI;

        namespace manipulators
        {
            bool Not::Check(runes_reader_t& _reader)
            {
                u32 start = _reader.get_cursor();
                if (!m_tokenizer_a.Check(_reader))
                {
                    return true;
                }
                _reader.set_cursor(start);
                return false;
            }

            bool Or::Check(runes_reader_t& _reader)
            {
                u32 start = _reader.get_cursor();
                if (!m_tokenizer_a.Check(_reader))
                {
                    _reader.set_cursor(start);
                    if (!m_tokenizer_b.Check(_reader))
                    {
                        _reader.set_cursor(start);
                        return false;
                    }
                }
                return true;
            }

            bool And::Check(runes_reader_t& _reader)
            {
                u32 start = _reader.get_cursor();
                if (!m_tokenizer_a.Check(_reader))
                {
                    _reader.set_cursor(start);
                    return false;
                }
                u32 c1 = _reader.get_cursor();

                _reader.set_cursor(start);
                if (!m_tokenizer_b.Check(_reader))
                {
                    _reader.set_cursor(start);
                    return false;
                }
                u32 c2 = _reader.get_cursor();

                u32 c = ((c2 < c1) ? c2 : c1);
                _reader.set_cursor(c);
                return true;
            }

            bool Sequence::Check(runes_reader_t& _reader)
            {
                u32 start = _reader.get_cursor();

                if (m_tokenizer_a.Check(_reader))
                {
                    if (m_tokenizer_b.Check(_reader))
                    {
                        return true;
                    }
                }
                _reader.set_cursor(start);
                return false;
            }

            bool Sequence3::Check(runes_reader_t& _reader)
            {
                u32 start = _reader.get_cursor();
                if (m_tokenizer_a.Check(_reader))
                {
                    if (m_tokenizer_b.Check(_reader))
                    {
                        if (m_tokenizer_c.Check(_reader))
                        {
                            return true;
                        }
                    }
                }
                _reader.set_cursor(start);
                return false;
            }

            bool Within::Check(runes_reader_t& _reader)
            {
                u32 start = _reader.get_cursor();

                u64 i = 0;
                for (; i < m_max; i++)
                {
                    if (!m_tokenizer_a.Check(_reader))
                        break;
                }

                if (i >= m_min && i <= m_max)
                {
                    return true;
                }

                _reader.set_cursor(start);
                return false;
            }

            bool Times::Check(runes_reader_t& _reader) { return Within(m_max, m_max, m_tokenizer_a).Check(_reader); }
            bool OneOrMore::Check(runes_reader_t& _reader) { return Within(1, -1, m_tokenizer_a).Check(_reader); }
            bool ZeroOrOne::Check(runes_reader_t& _reader) { return Within(0, 1, m_tokenizer_a).Check(_reader); }
            bool While::Check(runes_reader_t& _reader) { return Within(0, -1, m_tokenizer_a).Check(_reader); }
            bool Until::Check(runes_reader_t& _reader)
            {
                Not n(m_tokenizer_a);
                return (While(n)).Check(_reader);
            }

            bool Extract::Check(runes_reader_t& _reader)
            {
                u32 start  = _reader.get_cursor();
                bool            result = m_tokenizer_a.Check(_reader);
                if (result)
                {
                    u32 end = _reader.get_cursor();
                    m_selection         = _reader.select(start, end);
                    return result;
                }
                _reader.set_cursor(start);
                return result;
            }

            bool ReturnToCallback::Check(runes_reader_t& _reader)
            {
                u32 start  = _reader.get_cursor();
                bool            result = m_tokenizer_a.Check(_reader);
                if (result)
                {
                    m_cb(_reader, start);
                }
                _reader.set_cursor(start);
                return result;
            }

            bool Enclosed::Check(runes_reader_t& _reader)
            {
                filters::Exact open(m_open);
                filters::Exact close(m_close);
                Sequence       a(open, m_tokenizer_a);
                Sequence       b(a, close);
                return b.Check(_reader);
            }
        } // namespace manipulators

        namespace filters
        {
            bool Any::Check(runes_reader_t& _reader)
            {
                if (!_reader.valid())
                    return false;
                _reader.skip();
                return true;
            }

            bool In::Check(runes_reader_t& _reader)
            {
                if (!_reader.valid())
                    return false;

                m_input.reset();

                uchar32 const s = _reader.peek();
                while (m_input.valid())
                {
                    uchar32 const c = m_input.read();
                    if (c == s)
                    {
                        _reader.skip();
                        return true;
                    }
                }
                return false;
            }

            bool Between::Check(runes_reader_t& _reader)
            {
                uchar32 c = _reader.peek();
                if (c >= m_lower && c <= m_upper)
                {
                    _reader.skip();
                    return true;
                }
                return false;
            }

            bool Alphabet::Check(runes_reader_t& _reader) { return (m_lower_case.Check(_reader) || m_upper_case.Check(_reader)); }
            bool Digit::Check(runes_reader_t& _reader) { return m_digit.Check(_reader); }
            bool Hex::Check(runes_reader_t& _reader)
            {
                if (!m_digit.Check(_reader))
                {
                    if (!m_lower_case.Check(_reader))
                    {
                        if (!m_upper_case.Check(_reader))
                        {
                            return false;
                        }
                    }
                }
                return true;
            }

            bool AlphaNumeric::Check(runes_reader_t& _reader)
            {
                manipulators::Or r(sAlphabet, sDigit);
                return r.Check(_reader);
            }

            bool Exact::Check(runes_reader_t& _reader)
            {
                m_input.reset();

                u32 start = _reader.get_cursor();
                while (m_input.valid())
                {
                    uchar32 a = _reader.peek();
                    uchar32 b = m_input.peek();
                    if (a != b)
                    {
                        _reader.set_cursor(start);
                        return false;
                    }
                    m_input.skip();
                    _reader.skip();
                }
                return true;
            }

            bool Like::Check(runes_reader_t& _reader)
            {
                m_input.reset();

                u32 start = _reader.get_cursor();
                while (m_input.valid())
                {
                    uchar32 a = _reader.peek();
                    uchar32 b = m_input.peek();
                    if (a != b && (to_lower(a) != to_lower(b)))
                    {
                        _reader.set_cursor(start);
                        return false;
                    }
                    m_input.skip();
                    _reader.skip();
                }
                return true;
            }

            bool WhiteSpace::Check(runes_reader_t& _reader) { return m_whitespace.Check(_reader); }

            bool Is::Check(runes_reader_t& _reader)
            {
                if (_reader.peek() == m_char)
                {
                    _reader.skip();
                    return true;
                }
                return false;
            }

            bool Decimal::Check(runes_reader_t& _reader) { return manipulators::OneOrMore(sDigit).Check(_reader); }
            bool Word::Check(runes_reader_t& _reader) { return manipulators::OneOrMore(sAlphabet).Check(_reader); }
            bool EndOfText::Check(runes_reader_t& _reader) { return (_reader.peek() == ('\0')); }

#if defined(PLATFORM_PC)
            bool EndOfLine::Check(runes_reader_t& _reader) { return Exact("\r\n", 2).Check(_reader); }
#else
            bool EndOfLine::Check(runes_reader_t& _reader) { return Exact("\n", 1).Check(_reader); }
#endif

            bool Integer::Check(runes_reader_t& _reader)
            {
                s64             value       = 0;
                u32 start       = _reader.get_cursor();
                uchar32         c           = _reader.peek();
                bool            is_negative = (c == '-');
                if (is_negative)
                    _reader.skip();
                while (_reader.valid())
                {
                    c = _reader.peek();
                    if (!is_digit(c))
                        break;
                    value = (value * 10) + to_digit(c);
                    _reader.skip();
                }
                if (start == _reader.get_cursor())
                    return false;
                if (is_negative)
                    value = -value;
                if (value >= m_min && value <= m_max)
                {
                    return true;
                }
                _reader.set_cursor(start);
                return false;
            }

            bool Float::Check(runes_reader_t& _reader)
            {
                f32             value       = 0.0f;
                u32 start       = _reader.get_cursor();
                uchar32         c           = _reader.peek();
                bool            is_negative = c == '-';
                if (is_negative)
                    _reader.skip();
                while (_reader.valid())
                {
                    c = _reader.peek();
                    if (!is_digit(c))
                        break;
                    value = (value * 10) + to_digit(c);
                }
                if (c == '.')
                {
                    _reader.skip();
                    f32 mantissa = 10.0f;
                    while (_reader.valid())
                    {
                        c = _reader.peek();
                        if (!is_digit(c))
                            break;
                        value = value + f32(to_digit(c)) / mantissa;
                        mantissa *= 10.0f;
                    }
                }
                if (start == _reader.get_cursor())
                    return false;
                if (is_negative)
                    value = -value;
                if (value >= m_min && value <= m_max)
                {
                    return true;
                }
                _reader.set_cursor(start);
                return false;
            }
        } // namespace filters

        namespace utils
        {
            using namespace manipulators;
            using namespace filters;

            bool IPv4::Check(runes_reader_t& _reader) { return m_ipv4.Check(_reader); }

            bool Host::Check(runes_reader_t& _reader)
            {
                if (sIPv4.Check(_reader))
                {
                    return true;
                }
                OneOrMore  oom_an(sAlphaNumeric);
                Is         dash('-');
                Is         dot('.');
                Sequence   dash_oom_an(dash, oom_an);
                Sequence   dot_oom_an(dot, oom_an);
                ZeroOrMore zom_dash_oom_an(dash_oom_an);
                Sequence   dot_oom_an_zom_dash_oom_an(dot_oom_an, zom_dash_oom_an);
                ZeroOrMore host_tail(dot_oom_an_zom_dash_oom_an);
                Sequence   host_head(oom_an, host_tail);
                Sequence   host(host_head, host_tail);
                return host.Check(_reader);
            }

            static const char* sValidEmailUriChars = "!#$%&'*+/=?^_`{|}~-";

            bool Email::Check(runes_reader_t& _reader)
            {
                In         validchars(sValidEmailUriChars, 19);
                Or         valid(sAlphaNumeric, validchars);
                OneOrMore  oom_valid(valid);
                Is         dot('.');
                Sequence   dot_valid(dot, oom_valid);
                ZeroOrMore zom_dot_valid(dot_valid);
                Is         mt('@');
                Sequence   a(oom_valid, zom_dot_valid);
                Sequence   b(mt, sHost);
                Sequence   email(a, b);
                return email.Check(_reader);
            }

            bool ServerAddress::Check(runes_reader_t& _reader) { return false; }

            bool Uri::Check(runes_reader_t& _reader) { return false; }

        } // namespace utils
    }     // namespace combparser

} // namespace ncore
