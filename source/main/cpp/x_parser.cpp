#include "xbase/x_runes.h"
#include "xbase/x_runes.h"
#include "xtext/x_parser.h"

namespace xcore
{
    namespace xparser
    {
        xfilters::Any          xfilters::sAny;
        xfilters::Alphabet     xfilters::sAlphabet;
        xfilters::AlphaNumeric xfilters::sAlphaNumeric;
        xfilters::Digit        xfilters::sDigit;
        xfilters::Decimal      xfilters::sDecimal;
        xfilters::EndOfLine    xfilters::sEOL;
        xfilters::EndOfText    xfilters::sEOT;
        xfilters::Hex          xfilters::sHex;
        xfilters::WhiteSpace   xfilters::sWhitespace;
        xfilters::Word         xfilters::sWord;
        xutils::Email          xutils::sEmail;
        xutils::Host           xutils::sHost;
        xutils::IPv4           xutils::sIPv4;
        xutils::Phone          xutils::sPhone;
        xutils::ServerAddress  xutils::sServerAddress;
        xutils::Uri            xutils::sURI;

        namespace xmanipulators
        {
            bool Not::Check(runes_reader_t& _reader)
            {
                crunes_t::ptr_t start = _reader.get_cursor();
                if (!m_tokenizer_a.Check(_reader))
                {
                    return true;
                }
                _reader.set_cursor(start);
                return false;
            }

            bool Or::Check(runes_reader_t& _reader)
            {
                crunes_t::ptr_t start = _reader.get_cursor();
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
                crunes_t::ptr_t c1 = _reader.get_cursor();
                if (!m_tokenizer_a.Check(_reader))
                {
                    _reader.set_cursor(c1);
                    return false;
                }
                c1 = _reader.get_cursor();

                crunes_t::ptr_t c2 = _reader.get_cursor();
                if (!m_tokenizer_b.Check(_reader))
                {
                    _reader.set_cursor(c2);
                    return false;
                }
                c2 = _reader.get_cursor();

                crunes_t::ptr_t c = ((c2 < c1) ? c2 : c1);
                _reader.set_cursor(c);
                return true;
            }

            bool Sequence::Check(runes_reader_t& _reader)
            {
                crunes_t::ptr_t start = _reader.get_cursor();

                if (!m_tokenizer_a.Check(_reader))
                {
                    _reader.set_cursor(start);
                    return false;
                }

                if (!m_tokenizer_b.Check(_reader))
                {
                    _reader.set_cursor(start);
                    return false;
                }

                return true;
            }

            bool Sequence3::Check(runes_reader_t& _reader)
            {
                crunes_t::ptr_t start = _reader.get_cursor();
                if (!m_tokenizer_a.Check(_reader))
                {
                    _reader.set_cursor(start);
                    return false;
                }
                if (!m_tokenizer_b.Check(_reader))
                {
                    _reader.set_cursor(start);
                    return false;
                }
                if (!m_tokenizer_c.Check(_reader))
                {
                    _reader.set_cursor(start);
                    return false;
                }

                return true;
            }

            bool Within::Check(runes_reader_t& _reader)
            {
                crunes_t::ptr_t start = _reader.get_cursor();

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
                crunes_t::ptr_t start  = _reader.get_cursor();
                bool            result = m_tokenizer_a.Check(_reader);
                if (result)
                {
                    crunes_t::ptr_t end = _reader.get_cursor();
                    m_selection         = _reader.select(start, end);
                    return result;
                }
                _reader.set_cursor(start);
                return result;
            }

            bool ReturnToCallback::Check(runes_reader_t& _reader)
            {
                crunes_t::ptr_t start  = _reader.get_cursor();
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
                xfilters::Exact open(m_open);
                xfilters::Exact close(m_close);
                Sequence        a(open, m_tokenizer_a);
                Sequence        b(a, close);
                return b.Check(_reader);
            }
        } // namespace xmanipulators

        namespace xfilters
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
                crunes_t::ptr_t inputcursor = m_input.get_cursor();
                uchar32 const   s           = _reader.peek();
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
                if (c >= m_lower && c < m_upper)
                {
                    _reader.skip();
                    return true;
                }
                return false;
            }

            bool Alphabet::Check(runes_reader_t& _reader) { return (m_lower_case.Check(_reader) | m_upper_case.Check(_reader)); }
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
                xmanipulators::Or r(sAlphabet, sDigit);
                return r.Check(_reader);
            }

            bool Exact::Check(runes_reader_t& _reader)
            {
                crunes_t::ptr_t rcursor = _reader.get_cursor();
                crunes_t::ptr_t icursor = m_input.get_cursor();
                while (m_input.valid())
                {
                    uchar32 a = _reader.peek();
                    uchar32 b = m_input.peek();
					if (a != b)
					{
						_reader.set_cursor(rcursor);
						return false;
					}
                    m_input.skip();
                    _reader.skip();
                }
                return true;
            }

            bool Like::Check(runes_reader_t& _reader)
            {
                crunes_t::ptr_t rcursor = _reader.get_cursor();
                crunes_t::ptr_t icursor = m_input.get_cursor();
                while (m_input.valid())
                {
                    uchar32 a = _reader.peek();
                    uchar32 b = m_input.peek();
					if (a != b && (to_lower(a) != to_lower(b)))
					{
						_reader.set_cursor(rcursor);
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

            bool Decimal::Check(runes_reader_t& _reader) { return xmanipulators::OneOrMore(sDigit).Check(_reader); }
            bool Word::Check(runes_reader_t& _reader) { return xmanipulators::OneOrMore(sAlphabet).Check(_reader); }
            bool EndOfText::Check(runes_reader_t& _reader) { return (_reader.peek() == ('\0')); }

#if defined(PLATFORM_PC)
            bool EndOfLine::Check(runes_reader_t& _reader) { return Exact("\r\n", 2).Check(_reader); }
#else
            bool EndOfLine::Check(runes_reader_t& _reader) { return Exact("\n", 1).Check(_reader); }
#endif

            bool Integer::Check(runes_reader_t& _reader)
            {
                s64             value       = 0;
                crunes_t::ptr_t start      = _reader.get_cursor();
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
                crunes_t::ptr_t start      = _reader.get_cursor();
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
        } // namespace xfilters

        namespace xutils
        {
            using namespace xmanipulators;
            using namespace xfilters;

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
                Sequence   dot_valid(dot, valid);
                ZeroOrMore zom_dot_valid(dot_valid);
                Is         mt('@');
                Sequence   a(oom_valid, zom_dot_valid);
                Sequence   b(mt, sHost);
                Sequence   email(a, b);
                return email.Check(_reader);
            }
            bool Phone::Check(runes_reader_t& _reader)
            {
                Is         plus('+');
                ZeroOrMore zom_plus(plus);
                Is         open('(');
                Is         close('(');
                OneOrMore  oomdigit(xfilters::sDigit);
                Sequence3  open_oomdigit_close(open, oomdigit, close);
                ZeroOrMore zom_open_oomdigit_close(open_oomdigit_close);
                ZeroOrMore zom_whitespace(sWhitespace);
                In         spaceordash(" -", 2);
                Sequence   spaceordash_oomdigit(spaceordash, oomdigit);
                ZeroOrMore zom_spaceordash_oomdigit(spaceordash_oomdigit);

                Sequence zom_open_oomdigit_close_whitespace(zom_open_oomdigit_close, zom_whitespace);
                return zom_open_oomdigit_close_whitespace.Check(_reader);
            }

            bool ServerAddress::Check(runes_reader_t& _reader) { return false; }

            bool Uri::Check(runes_reader_t& _reader) { return false; }

        } // namespace xutils
    }     // namespace xparser

    using namespace xcore::xparser;

    stringprocessor_t::stringprocessor_t() {}
    stringprocessor_t::stringprocessor_t(runes_reader_t const& str)
    {
        m_string = str;
        m_cursor = str.get_cursor();
    }
    stringprocessor_t::stringprocessor_t(runes_reader_t const& str, crunes_t::ptr_t cursor)
    {
        m_string = str;
        m_cursor = cursor;
    }

    bool stringprocessor_t::parse(xparser::tokenizer_t& tok)
    {
        crunes_t::ptr_t start  = m_cursor;
        bool            result = tok.Check(m_string);
        if (result)
        {
            m_lastTokenized = m_cursor;
            m_cursor        = start;
        }
        return result;
    }

    bool stringprocessor_t::validate(xparser::tokenizer_t& tok)
    {
        xmanipulators::Sequence validate(tok, xfilters::sEOT);
        return validate.Check(m_string);
    }

    runes_reader_t stringprocessor_t::search(tokenizer_t& tok)
    {
        xmanipulators::Or search(xfilters::sEOT, tok);
        bool              result = xmanipulators::Until(search).Check(m_string);
        if (result && m_string.valid())
        {
            crunes_t::ptr_t start = m_cursor;
            if (tok.Check(m_string))
            {
                return m_string.select(start, m_cursor);
            }
        }
        return runes_reader_t();
    }

    bool stringprocessor_t::isEOT() { return (m_string.peek() == ('\0')); }

    void stringprocessor_t::reset()
    {
        m_lastTokenized = crunes_t::ptr_t();
        m_cursor        = m_string.get_cursor();
    }
} // namespace xcore
