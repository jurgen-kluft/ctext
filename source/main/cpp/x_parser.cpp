#include "xtext/x_parser.h"
#include <stdlib.h>

using namespace xcore::xparser::Manipulators;
using namespace xcore::xparser::Filters;
using namespace xcore::xparser::Utils;

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

    charreader::charreader()
        : m_type(ASCII)
    {
    }
    charreader::charreader(const char* str)
        : m_type(ASCII)
    {
        if (str != nullptr)
        {
            m_runes._ascii = ascii::crunes(str);
        }
    }
    charreader::charreader(utf32::pcrune str)
        : m_type(UTF32)
    {
        if (str != nullptr)
        {
            m_runes._utf32 = utf32::crunes(str);
        }
    }
    charreader::charreader(const charreader& t)
        : m_type(t.m_type)
    {
        m_runes._ascii = t.m_runes._ascii;
    }

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

    StringReader::StringReader()
        : m_str()
    {
    }

    StringReader::StringReader(const char* str)
        : m_str(str)
    {
    }

    StringReader::StringReader(const StringReader& chars)
        : m_str(chars.m_str)
    {
    }

    StringReader::StringReader(const StringReader& begin, const StringReader& until) { m_str.select(begin.m_str, until.m_str); }

    s64 StringReader::Size() const { return m_str.size(); }

    void StringReader::Reset() { m_str.reset(); }

    uchar32 StringReader::Read() { return m_str.read(); }

    uchar32 StringReader::Peek() const { return m_str.peek(); }

    void StringReader::Select(const StringReader& begin, const StringReader& cursor) { m_str.select(begin.m_str, cursor.m_str); }

    bool StringReader::Valid() const { return m_str.valid(); }

    void StringReader::Skip() { m_str.skip(); }

    void StringReader::Write(StringWriter& writer) {}

    bool StringReader::operator<(const StringReader&) const { return false; }

    bool StringReader::operator>(const StringReader&) const { return false; }

    bool StringReader::operator<=(const StringReader&) const { return false; }

    bool StringReader::operator>=(const StringReader&) const { return false; }

    bool StringReader::operator==(const StringReader& r) const
    {
        if (Size() == r.Size())
        {
            return m_str == r.m_str;
        }
        return false;
    }

    bool StringReader::operator!=(const StringReader&) const { return false; }

    StringReader& StringReader::operator=(const StringReader& r)
    {
        m_str = r.m_str;
        return *this;
    }

    namespace xparser
    {
        Filters::Any          Filters::ANY;
        Filters::Alphabet     Filters::ALPHABET;
        Filters::AlphaNumeric Filters::ALPHANUMERIC;
        Filters::Digit        Filters::DIGIT;
        Filters::Decimal      Filters::DECIMAL;
        Filters::EndOfLine    Filters::EOL;
        Filters::EndOfText    Filters::EOT;
        Filters::Hex          Filters::HEX;
        Filters::WhiteSpace   Filters::WHITESPACE;
        Filters::Word         Filters::WORD;
        Utils::Email          Utils::EMAIL;
        Utils::Host           Utils::HOST;
        Utils::IPv4           Utils::IPV4;
        Utils::Phone          Utils::PHONE;
        Utils::ServerAddress  Utils::SERVERADDRESS;
        Utils::Uri            Utils::URI;

        namespace Manipulators
        {
            bool Not::Check(StringReader& _Stream)
            {
                StringReader _Start = _Stream;
                if (!_Tokenizer.Check(_Start))
                {
                    _Stream.Skip();
                    return true;
                }
                return false;
            }

            bool Or::Check(StringReader& _Stream)
            {
                StringReader _Start = _Stream;
                if (!_TokenizerA.Check(_Start))
                {
                    if (!_TokenizerB.Check(_Start))
                    {
                        return false;
                    }
                }
                _Stream = _Start;
                return true;
            }

            bool And::Check(StringReader& _Stream)
            {
                StringReader _Start1, _Start2;

                _Start1 = _Stream;
                if (!_TokenizerA.Check(_Start1))
                    return false;

                _Start2 = _Stream;
                if (!_TokenizerB.Check(_Start2))
                    return false;

                _Start1 = ((_Start2 < _Start1) ? _Start2 : _Start1);

                _Stream = _Start1;
                return true;
            }

            bool Sequence::Check(StringReader& _Stream)
            {
                StringReader _Start = _Stream;

                if (!_TokenizerA.Check(_Start))
                    return false;

                if (!_TokenizerB.Check(_Start))
                    return false;

                _Stream = _Start;
                return true;
            }

            bool Within::Check(StringReader& _Stream)
            {
                StringReader _Start = _Stream;

                u64 i = 0;
                for (; i < _Max; i++)
                {
                    if (!_Tokenizer.Check(_Start))
                        break;
                }

                if (i >= _Min && i <= _Max)
                {
                    _Stream = _Start;
                    return true;
                }

                return false;
            }

            bool Times::Check(StringReader& _Stream) { return Within(_Max, _Max, _Tokenizer).Check(_Stream); }

            bool OneOrMore::Check(StringReader& _Stream) { return Within(1, -1, _Tokenizer).Check(_Stream); }

            bool ZeroOrOne::Check(StringReader& _Stream) { return Within(0, 1, _Tokenizer).Check(_Stream); }

            bool While::Check(StringReader& _Stream) { return Within(0, -1, _Tokenizer).Check(_Stream); }

            bool Until::Check(StringReader& _Stream) { return (While(Not(_Tokenizer))).Check(_Stream); }

            bool Extract::Check(StringReader& _Stream)
            {
                StringReader _Start = _Stream;

                bool _Result = _Tokenizer.Check(_Start);
                _Selection   = StringReader(_Start, _Stream);

                _Stream = _Start;
                return _Result;
            }

            bool ReturnToCallback::Check(StringReader& _Stream)
            {
                StringReader _Start  = _Stream;
                bool         _Result = _Tokenizer.Check(_Start);

                StringReader output = StringReader(_Start, _Stream);
                cb(output);

                _Stream = _Start;
                return _Result;
            }

            bool Enclosed::Check(StringReader& _Stream) { return (Filters::Exact(m_open) + _Tokenizer + Filters::Exact(m_close)).Check(_Stream); }
        } // namespace Manipulators

        namespace Filters
        {
            bool Any::Check(StringReader& _Stream)
            {
                if (!_Stream.Valid())
                    return false;
                _Stream.Skip();
                return true;
            }

            bool In::Check(StringReader& _Stream)
            {
                if (!_Stream.Valid())
                    return false;

                uchar32 const s = _Stream.Peek();
                _Input.Reset();
                while (_Input.Valid())
                {
                    uchar32 const c = _Input.Read();
                    if (c == s)
                    {
                        _Stream.Skip();
                        return true;
                    }
                }

                return false;
            }

            bool Between::Check(StringReader& _Stream)
            {
                uchar32 c = _Stream.Peek();
                if (c >= _Lower && c <= _Upper)
                {
                    _Stream.Skip();
                    return true;
                }
                return false;
            }

            bool Alphabet::Check(StringReader& _Stream) { return (Between(('a'), ('z')).Check(_Stream) | Between(('A'), ('Z')).Check(_Stream)); }

            bool Digit::Check(StringReader& _Stream) { return Between(('0'), ('9')).Check(_Stream); }

            bool Hex::Check(StringReader& _Stream) { return (DIGIT | Between(('a'), ('f')) | Between(('A'), ('F'))).Check(_Stream); }

            bool AlphaNumeric::Check(StringReader& _Stream) { return (ALPHABET | DIGIT).Check(_Stream); }

            bool Exact::Check(StringReader& _Stream)
            {
                StringReader _StreamCursor = _Stream;
                StringReader _InputCursor  = _Input;
                while (_InputCursor.Valid())
                {
                    uchar32 a = _StreamCursor.Peek();
                    uchar32 b = _InputCursor.Peek();
                    if (a != b)
                        _StreamCursor.Skip();
                    _InputCursor.Skip();
                }
                _Stream = _StreamCursor;
                return true;
            }

            bool Like::Check(StringReader& _Stream)
            {
                StringReader _StreamCursor = _Stream;
                StringReader _InputCursor  = _Input;
                while (_InputCursor.Valid())
                {
                    uchar32 a = _StreamCursor.Peek();
                    uchar32 b = _InputCursor.Peek();
                    if (utf32::to_lower(a) != utf32::to_lower(b))
                        return false;
                    _StreamCursor.Skip();
                    _InputCursor.Skip();
                }
                _Stream = _StreamCursor;
                return true;
            }

            bool WhiteSpace::Check(StringReader& _Stream) { return In((" \t\n\r")).Check(_Stream); }

            bool Is::Check(StringReader& _Stream)
            {
                if (_Stream.Peek() == _Letter)
                {
                    _Stream.Skip();
                    return true;
                }
                return false;
            }

            bool Decimal::Check(StringReader& _Stream) { return OneOrMore(DIGIT).Check(_Stream); }

            bool Word::Check(StringReader& _Stream) { return OneOrMore(ALPHABET).Check(_Stream); }

            bool EndOfText::Check(StringReader& _Stream) { return (_Stream.Peek() == ('\0')); }

#if defined(PLATFORM_PC)
            bool EndOfLine::Check(StringReader& _Stream) { return Exact("\r\n").Check(_Stream); }
#else
            bool EndOfLine::Check(StringReader& _Stream) { return Exact("\n").Check(_Stream); }
#endif

            bool Integer::Check(StringReader& _Stream)
            {
                s32          value       = 0;
                StringReader input       = _Stream;
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
                if (input == _Stream)
                    return false;
                if (is_negative)
                    value = -value;
                if (value >= _Min && value <= _Max)
                {
                    _Stream = input;
                    return true;
                }
                return false;
            }

            bool Float::Check(StringReader& _Stream)
            {
                f32          value       = 0.0f;
                StringReader input       = _Stream;
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
                if (input == _Stream)
                    return false;
                if (is_negative)
                    value = -value;
                if (value >= _Min && value <= _Max)
                {
                    _Stream = input;
                    return true;
                }
                return false;
            }
        } // namespace Filters

        namespace Utils
        {
            bool IPv4::Check(StringReader& _Stream)
            {
                return (3 * ((Within(1, 3, DIGIT) & Integer(255)) + Is(('.'))) + (Within(1, 3, DIGIT) & Filters::Integer(255))).Check(_Stream);
            }

            bool Host::Check(StringReader& _Stream)
            {
                return (IPV4 | (OneOrMore(ALPHANUMERIC) + ZeroOrMore(Is(('-')) + OneOrMore(ALPHANUMERIC)) +
                                ZeroOrMore(Is(('.')) + OneOrMore(ALPHANUMERIC) + ZeroOrMore(Is(('-')) + OneOrMore(ALPHANUMERIC)))))
                    .Check(_Stream);
            }

            bool Email::Check(StringReader& _Stream)
            {
                return (OneOrMore(ALPHANUMERIC | In(("!#$%&'*+/=?^_`{|}~-"))) + ZeroOrMore(Is('.') + (ALPHANUMERIC | In(("!#$%&'*+/=?^_`{|}~-")))) + Is('@') + HOST).Check(_Stream);
            }

            bool Phone::Check(StringReader& _Stream)
            {
                return (ZeroOrMore(Is(('+'))) + (ZeroOrMore(Is(('(')) + OneOrMore(DIGIT) + Is((')'))) + ZeroOrMore(WHITESPACE)) + OneOrMore(DIGIT) +
                        ZeroOrMore(In((" -")) + OneOrMore(DIGIT)))
                    .Check(_Stream);
            }

            bool ServerAddress::Check(StringReader& _Stream) { return (HOST + ZeroOrOne(Is((':')) + Integer(1, 65535))).Check(_Stream); }

            bool Uri::Check(StringReader& _Stream)
            {
                return (OneOrMore(ALPHANUMERIC) + Is((':')) +
                        (OneOrMore(ALPHANUMERIC | In(("!#$%&'*+/=?^_`{|}~-"))) + ZeroOrMore(Is('.') + (ALPHANUMERIC | In(("!#$%&'*+/=?^_`{|}~-"))))) + Is(('@')) + SERVERADDRESS)
                    .Check(_Stream);
            }
        } // namespace Utils
    }     // namespace xparser

    using namespace xcore::xparser;

    /*********************************************************************************/
    StringProcessor::StringProcessor() {}

    StringProcessor::StringProcessor(StringReader const& str)
    {
        _String = str;
        _Cursor = str;
    }

    bool StringProcessor::Parse(TokenizerInterface& tok)
    {
        StringReader _Start  = _Cursor;
        bool         _Result = tok.Check(_Cursor);
        if (_Result)
            _LastTokenized = StringReader(_Start, _Cursor);
        return _Result;
    }

    bool StringProcessor::Validate(TokenizerInterface& tok) { return (tok + EndOfText()).Check(_Cursor); }

    StringReader StringProcessor::Search(TokenizerInterface& tok)
    {
        bool _Result = (Until(EOT | tok)).Check(_Cursor);
        if (_Result && _Cursor.Valid())
        {
            StringReader _Start = _Cursor;
            if ((tok).Check(_Cursor))
            {
                return StringReader(_Start, _Cursor);
            }
        }
        return StringReader();
    }

    bool StringProcessor::IsEOT() { return (_Cursor.Peek() == ('\0')); }

    void StringProcessor::Reset()
    {
        _LastTokenized = StringReader();
        _Cursor        = _String;
    }
} // namespace xcore
