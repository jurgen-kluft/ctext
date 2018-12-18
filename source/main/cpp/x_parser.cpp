#include "xtext/x_parser.h"
#include <stdlib.h>

using namespace xcore::xparser::Manipulators;
using namespace xcore::xparser::Filters;
using namespace xcore::xparser::Utils;

namespace xcore
{
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

#if defined(PLATFORM_PC)
#define END_OF_LINE ("\r\n")
#pragma warning(disable : 4251)
#else
#define END_OF_LINE ("\n")
#endif

        namespace Manipulators
        {
            bool Not::Check(Characters& _Stream)
            {
                Characters _Start = _Stream;
                if (!_Tokenizer.Check(_Start))
                {
                    _Stream.Skip();
                    return true;
                }
                return false;
            }

            bool Or::Check(Characters& _Stream)
            {
                Characters _Start = _Stream;
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

            bool And::Check(Characters& _Stream)
            {
                Characters _Start1, _Start2;

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

            bool Sequence::Check(Characters& _Stream)
            {
                Characters _Start = _Stream;

                if (!_TokenizerA.Check(_Start))
                    return false;

                if (!_TokenizerB.Check(_Start))
                    return false;

                _Stream = _Start;
                return true;
            }

            bool Within::Check(Characters& _Stream)
            {
                Characters _Start = _Stream;

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

            bool Times::Check(Characters& _Stream) { return Within(_Max, _Max, _Tokenizer).Check(_Stream); }

            bool OneOrMore::Check(Characters& _Stream) { return Within(1, -1, _Tokenizer).Check(_Stream); }

            bool ZeroOrOne::Check(Characters& _Stream) { return Within(0, 1, _Tokenizer).Check(_Stream); }

            bool While::Check(Characters& _Stream) { return Within(0, -1, _Tokenizer).Check(_Stream); }

            bool Until::Check(Characters& _Stream) { return (While(Not(_Tokenizer))).Check(_Stream); }

            bool Extract::Check(Characters& _Stream)
            {
                Characters _Start = _Stream;

                bool _Result = _Tokenizer.Check(_Start);
                //*_Input             = 0;
                // STRCAT(_Input, *_Stream, (_Start - *_Stream));
                Characters     output = Characters(_Start, _Stream);
                CharactersOut* writer = _Writer;
                output.Write(*writer);

                _Stream = _Start;
                return _Result;
            }

            bool ReturnToCallback::Check(Characters& _Stream)
            {
                Characters _Start  = _Stream;
                bool       _Result = _Tokenizer.Check(_Start);

                Characters output = Characters(_Start, _Stream);
                cb(output);

                _Stream = _Start;
                return _Result;
            }

            bool Enclosed::Check(Characters& _Stream) { return (Filters::Exact(m_open) + _Tokenizer + Filters::Exact(m_close)).Check(_Stream); }
        } // namespace Manipulators

        namespace Filters
        {
            bool Any::Check(Characters& _Stream)
            {
                if (!_Stream.Valid())
                    return false;
                _Stream.Skip();
                return true;
            }

            bool In::Check(Characters& _Stream)
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

            bool Between::Check(Characters& _Stream)
            {
                uchar32 c = _Stream.Peek();
                if (c >= _Lower && c <= _Upper)
                {
                    _Stream.Skip();
                    return true;
                }
                return false;
            }

            bool Alphabet::Check(Characters& _Stream) { return (Between(('a'), ('z')).Check(_Stream) | Between(('A'), ('Z')).Check(_Stream)); }

            bool Digit::Check(Characters& _Stream) { return Between(('0'), ('9')).Check(_Stream); }

            bool Hex::Check(Characters& _Stream) { return (DIGIT | Between(('a'), ('f')) | Between(('A'), ('F'))).Check(_Stream); }

            bool AlphaNumeric::Check(Characters& _Stream) { return (ALPHABET | DIGIT).Check(_Stream); }

            bool Exact::Check(Characters& _Stream)
            {
                Characters _StreamCursor = _Stream;
                Characters _InputCursor  = _Input;
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

            bool Like::Check(Characters& _Stream)
            {
                Characters _StreamCursor = _Stream;
                Characters _InputCursor  = _Input;
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

            bool WhiteSpace::Check(Characters& _Stream) { return In((" \t\n\r")).Check(_Stream); }

            bool Is::Check(Characters& _Stream)
            {
                if (_Stream.Peek() == _Letter)
                {
                    _Stream.Skip();
                    return true;
                }
                return false;
            }

            bool Decimal::Check(Characters& _Stream) { return OneOrMore(DIGIT).Check(_Stream); }

            bool Word::Check(Characters& _Stream) { return OneOrMore(ALPHABET).Check(_Stream); }

            bool EndOfText::Check(Characters& _Stream) { return (_Stream.Peek() == ('\0')); }

            bool EndOfLine::Check(Characters& _Stream) { return Exact(END_OF_LINE).Check(_Stream); }

            bool Integer::Check(Characters& _Stream)
            {
                s32        value       = 0;
                Characters input       = _Stream;
                uchar32    c           = input.Peek();
                bool       is_negative = c == '-';
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

            bool Float::Check(Characters& _Stream)
            {
                f32        value       = 0.0f;
                Characters input       = _Stream;
                uchar32    c           = input.Peek();
                bool       is_negative = c == '-';
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
            bool IPv4::Check(Characters& _Stream) { return (3 * ((Within(1, 3, DIGIT) & Integer(255)) + Is(('.'))) + (Within(1, 3, DIGIT) & Filters::Integer(255))).Check(_Stream); }

            bool Host::Check(Characters& _Stream)
            {
                return (IPV4 |
                        (OneOrMore(ALPHANUMERIC) + ZeroOrMore(Is(('-')) + OneOrMore(ALPHANUMERIC)) + ZeroOrMore(Is(('.')) + OneOrMore(ALPHANUMERIC) + ZeroOrMore(Is(('-')) + OneOrMore(ALPHANUMERIC)))))
                    .Check(_Stream);
            }

            bool Email::Check(Characters& _Stream)
            {
                return (OneOrMore(ALPHANUMERIC | In(("!#$%&'*+/=?^_`{|}~-"))) + ZeroOrMore(Is('.') + (ALPHANUMERIC | In(("!#$%&'*+/=?^_`{|}~-")))) + Is('@') + HOST).Check(_Stream);
            }

            bool Phone::Check(Characters& _Stream)
            {
                return (ZeroOrMore(Is(('+'))) + (ZeroOrMore(Is(('(')) + OneOrMore(DIGIT) + Is((')'))) + ZeroOrMore(WHITESPACE)) + OneOrMore(DIGIT) + ZeroOrMore(In((" -")) + OneOrMore(DIGIT)))
                    .Check(_Stream);
            }

            bool ServerAddress::Check(Characters& _Stream) { return (HOST + ZeroOrOne(Is((':')) + Integer(1, 65535))).Check(_Stream); }

            bool Uri::Check(Characters& _Stream)
            {
                return (OneOrMore(ALPHANUMERIC) + Is((':')) + (OneOrMore(ALPHANUMERIC | In(("!#$%&'*+/=?^_`{|}~-"))) + ZeroOrMore(Is('.') + (ALPHANUMERIC | In(("!#$%&'*+/=?^_`{|}~-"))))) + Is(('@')) +
                        SERVERADDRESS)
                    .Check(_Stream);
            }
        } // namespace Utils
    }     // namespace xparser

    using namespace xcore::xparser;

    /*********************************************************************************/
    StringProcessor::StringProcessor() {}

    StringProcessor::StringProcessor(Characters const& str)
    {
        _String = str;
        _Cursor = str;
    }

    bool StringProcessor::Parse(TokenizerInterface& tok)
    {
        Characters _Start  = _Cursor;
        bool       _Result = tok.Check(_Cursor);
        if (_Result)
            _LastTokenized = Characters(_Start, _Cursor);
        return _Result;
    }

    bool StringProcessor::Validate(TokenizerInterface& tok) { return (tok + EndOfText()).Check(_Cursor); }

    Characters StringProcessor::Search(TokenizerInterface& tok)
    {
        bool _Result = (Until(EOT | tok)).Check(_Cursor);
        if (_Result && _Cursor.Valid())
        {
            Characters _Start = _Cursor;
            (tok).Check(_Cursor);
            return _Start;
        }
        return Characters();
    }

    bool StringProcessor::IsEOT() { return (_Cursor.Peek() == ('\0')); }

    void StringProcessor::Reset()
    {
        _LastTokenized = Characters();
        _Cursor        = _String;
    }
} // namespace xcore
