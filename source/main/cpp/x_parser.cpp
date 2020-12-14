#include "xbase/x_runes.h"
#include "xbase/x_runes.h"
#include "xtext/x_parser.h"

namespace xcore
{
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
				if (result)
				{
					m_selection = _reader.select(_cursor, end);
					_cursor = end;
				}
                return result;
            }

            bool ReturnToCallback::Check(xtext::reader_t &_reader, xtext::reader_t::cursor_t &_cursor)
            {
                xtext::reader_t::cursor_t start = _cursor;
                bool result = m_tokenizer_a.Check(_reader, start);
				if (result)
				{
					m_cb(_reader, start);
					_cursor = start;
				}
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

                xtext::reader_t::cursor_t inputcursor = m_input.get_cursor();
                uchar32 const s = _reader.peek(_cursor);
                while (m_input.valid(inputcursor))
                {
                    uchar32 const c = m_input.read(inputcursor);
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

            bool AlphaNumeric::Check(xtext::reader_t &_reader, xtext::reader_t::cursor_t &_cursor) 
            {
                xmanipulators::Or r(sAlphabet, sDigit);
                return r.Check(_reader, _cursor);
            }

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
                    _reader.skip(cursor);
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
                Is         plus('+');
                ZeroOrMore zom_plus(plus);
                Is         open('(');
                Is         close('(');
                OneOrMore  oomdigit(xfilters::sDigit);
                Sequence3  open_oomdigit_close(open, oomdigit, close);
                ZeroOrMore zom_open_oomdigit_close(open_oomdigit_close);
                ZeroOrMore zom_whitespace(sWhitespace);
                In         spaceordash(" -");
                Sequence   spaceordash_oomdigit(spaceordash, oomdigit);
                ZeroOrMore zom_spaceordash_oomdigit(spaceordash_oomdigit);

                Sequence zom_open_oomdigit_close_whitespace(zom_open_oomdigit_close, zom_whitespace);
                return zom_open_oomdigit_close_whitespace.Check(_reader, _cursor);
            }

            bool ServerAddress::Check(xtext::reader_t& _reader, xtext::reader_t::cursor_t& _cursor)
            {
				return false;
            }

            bool Uri::Check(xtext::reader_t& _reader, xtext::reader_t::cursor_t& _cursor)
            {
				return false;
            }

        } // namespace xutils
    }     // namespace xparser

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
