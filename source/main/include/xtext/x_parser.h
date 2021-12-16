#ifndef __XTEXT_XPARSER_H__
#define __XTEXT_XPARSER_H__
#include "xbase/x_target.h"
#ifdef USE_PRAGMA_ONCE
#pragma once
#endif

#include "xbase/x_runes.h"

namespace xcore
{
    namespace combparser
    {
        class tokenizer_t
        {
        public:
            /*!
             * @fn 	bool Check(runes_reader_t&)
             * Check if the string pointed by cursor is complying to this parsing rule
             * @param[in/out] cursor  A cursor to the parsing string, after successful
             * parsing, the cursor value should move to the last character as far as the
             * proper parsing ends
             * @return  Return @a true if the parsing succeeds. Otherwise it returns false
             *
             */
            virtual bool Check(runes_reader_t&) = 0;
        };

        class tokenizer_1_t : public tokenizer_t
        {
        public:
            tokenizer_1_t(tokenizer_t& toka)
                : m_tokenizer_a(toka)
            {
            }

        protected:
            tokenizer_t& m_tokenizer_a;
        };
        class tokenizer_2_t : public tokenizer_t
        {
        public:
            tokenizer_2_t(tokenizer_t& toka, tokenizer_t& tokb)
                : m_tokenizer_a(toka)
                , m_tokenizer_b(tokb)
            {
            }

        protected:
            tokenizer_t& m_tokenizer_a;
            tokenizer_t& m_tokenizer_b;
        };

        namespace manipulators
        {
            class Not : public tokenizer_1_t
            {
            public:
                inline Not(tokenizer_t& toka)
                    : tokenizer_1_t(toka)
                {
                }
                virtual bool Check(runes_reader_t&);
            };

            class Or : public tokenizer_2_t
            {
            public:
                inline Or(tokenizer_t& toka, tokenizer_t& tokb)
                    : tokenizer_2_t(toka, tokb)
                {
                }
                virtual bool Check(runes_reader_t&);
            };

            class And : public tokenizer_2_t
            {
            public:
                And(tokenizer_t& toka, tokenizer_t& tokb)
                    : tokenizer_2_t(toka, tokb)
                {
                }
                virtual bool Check(runes_reader_t&);
            };

            class Sequence : public tokenizer_2_t
            {
            public:
                inline Sequence(tokenizer_t& toka, tokenizer_t& tokb)
                    : tokenizer_2_t(toka, tokb)
                {
                }
                virtual bool Check(runes_reader_t&);
            };
            class Sequence3 : public tokenizer_2_t
            {
                tokenizer_t& m_tokenizer_c;

            public:
                inline Sequence3(tokenizer_t& toka, tokenizer_t& tokb, tokenizer_t& tokc)
                    : tokenizer_2_t(toka, tokb)
                    , m_tokenizer_c(tokc)
                {
                }
                virtual bool Check(runes_reader_t&);
            };

            class Within : public tokenizer_1_t
            {
                u64 m_min, m_max;

            public:
                Within(u64 min, u64 max, tokenizer_t& toka)
                    : tokenizer_1_t(toka)
                    , m_min(min)
                    , m_max(max)
                {
                }
                Within(u64 max, tokenizer_t& toka)
                    : tokenizer_1_t(toka)
                    , m_min(0)
                    , m_max(max)
                {
                }
                Within(tokenizer_t& toka)
                    : tokenizer_1_t(toka)
                    , m_min(0)
                    , m_max(0xffffffffffffffffUL)
                {
                }
                virtual bool Check(runes_reader_t&);
            };

            class Times : public tokenizer_1_t
            {
                s32 m_max;

            public:
                inline Times(s32 max, tokenizer_t& toka)
                    : tokenizer_1_t(toka)
                    , m_max(max)
                {
                }
                virtual bool Check(runes_reader_t&);
            };

            class OneOrMore : public tokenizer_1_t
            {
            public:
                inline OneOrMore(tokenizer_t& toka)
                    : tokenizer_1_t(toka)
                {
                }
                virtual bool Check(runes_reader_t&);
            };

            class ZeroOrOne : public tokenizer_1_t
            {
            public:
                inline ZeroOrOne(tokenizer_t& toka)
                    : tokenizer_1_t(toka)
                {
                }
                virtual bool Check(runes_reader_t&);
            };
            typedef ZeroOrOne Optional;
            typedef ZeroOrOne _0Or1;

            class While : public tokenizer_1_t
            {
            public:
                inline While(tokenizer_t& toka)
                    : tokenizer_1_t(toka)
                {
                }
                virtual bool Check(runes_reader_t&);
            };
            typedef While ZeroOrMore;

            class Until : public tokenizer_1_t
            {
            public:
                inline Until(tokenizer_t& toka)
                    : tokenizer_1_t(toka)
                {
                }
                virtual bool Check(runes_reader_t&);
            };

            class Extract : public tokenizer_1_t
            {
                runes_reader_t& m_selection;

            public:
                inline Extract(runes_reader_t& m1, tokenizer_t& toka)
                    : tokenizer_1_t(toka)
                    , m_selection(m1)
                {
                }
                virtual bool Check(runes_reader_t&);
            };

            typedef void (*CallBack)(runes_reader_t&, crunes_t::ptr_t&);
            class ReturnToCallback : public tokenizer_1_t
            {
                CallBack m_cb;

            public:
                inline ReturnToCallback(CallBack cb, tokenizer_t& toka)
                    : tokenizer_1_t(toka)
                    , m_cb(cb)
                {
                }
                virtual bool Check(runes_reader_t&);
            };

            class Enclosed : public tokenizer_1_t
            {
                runes_reader_t m_open;
                runes_reader_t m_close;

            public:
                inline Enclosed(runes_reader_t open, runes_reader_t close, tokenizer_t& toka)
                    : tokenizer_1_t(toka)
                    , m_open(open)
                    , m_close(close)
                {
                }
                virtual bool Check(runes_reader_t&);
            };

        } // namespace manipulators

        namespace filters
        {
            class Any : public tokenizer_t
            {
            public:
                Any() {}
                virtual bool Check(runes_reader_t&);
            };
            extern Any sAny;

            class In : public tokenizer_t
            {
                runes_reader_t m_input;

            public:
                In() {}
				In(const char* str, u32 len) : m_input(str,str+len) {}
                In(runes_reader_t input)
                    : m_input(input)
                {
                }
                virtual bool Check(runes_reader_t&);
            };

            class Between : public tokenizer_t
            {
                uchar32 m_lower;
                uchar32 m_upper;    // upper is inclusive

            public:
                Between()
                    : m_lower('a')
                    , m_upper('z')
                {
                }
                Between(uchar32 lower, uchar32 upper)
                    : m_lower(lower)
                    , m_upper(upper)
                {
                }
                virtual bool Check(runes_reader_t&);
            };

            // TODO:class SmallLetter;
            // TODO:class CapitalLetter;
            // TODO:Class Letter;
            // TODO:Class Punctuation

            class Alphabet : public tokenizer_t
            {
                Between m_lower_case;
                Between m_upper_case;

            public:
                Alphabet()
                    : m_lower_case('a', 'z')
                    , m_upper_case('A', 'Z')
                {
                }
                virtual bool Check(runes_reader_t&);
            };
            extern Alphabet sAlphabet;

            class Digit : public tokenizer_t
            {
                Between m_digit;

            public:
                Digit()
                    : m_digit('0', '9')
                {
                }
                virtual bool Check(runes_reader_t&);
            };
            extern Digit sDigit;

            class Hex : public tokenizer_t
            {
                Digit   m_digit;
                Between m_lower_case;
                Between m_upper_case;

            public:
                Hex()
                    : m_lower_case('a', 'f')
                    , m_upper_case('A', 'F')
                {
                }
                virtual bool Check(runes_reader_t&);
            };
            extern Hex sHex;

            class AlphaNumeric : public tokenizer_t
            {
            public:
                AlphaNumeric() {}
                virtual bool Check(runes_reader_t&);
            };
            extern AlphaNumeric sAlphaNumeric;

            class Exact : public tokenizer_t
            {
                runes_reader_t m_input;

            public:
                Exact() {}
                Exact(const char* str, u32 len ) : m_input(str, str + len) {}
                Exact(runes_reader_t input)
                    : m_input(input)
                {
                }
                virtual bool Check(runes_reader_t&);
            };

            class Like : public tokenizer_t
            {
                runes_reader_t  m_input;
                crunes_t::ptr_t m_from;
                crunes_t::ptr_t m_to;

            public:
                Like() {}
                Like(runes_reader_t input)
                    : m_input(input)
                {
                }
                Like(runes_reader_t input, crunes_t::ptr_t to)
                    : m_input(input)
                    , m_to(to)
                {
                }
                Like(runes_reader_t input, crunes_t::ptr_t from, crunes_t::ptr_t to)
                    : m_input(input)
                    , m_from(from)
                    , m_to(to)
                {
                }
                virtual bool Check(runes_reader_t&);
            };

            class WhiteSpace : public tokenizer_t
            {
                In m_whitespace;

            public:
                WhiteSpace() : m_whitespace(" \t\n\r", 4)
                {
                }
                virtual bool Check(runes_reader_t&);
            };
            extern WhiteSpace sWhitespace;

            class Is : public tokenizer_t
            {
                uchar32 m_char;

            public:
                Is()
                    : m_char(' ')
                {
                }
                Is(uchar32 c)
                    : m_char(c)
                {
                }
                virtual bool Check(runes_reader_t&);
            };

            class Decimal : public tokenizer_t
            {
            public:
                Decimal() {}
                virtual bool Check(runes_reader_t&);
            };
            extern Decimal sDecimal;

            class Word : public tokenizer_t
            {
            public:
                Word() {}
                virtual bool Check(runes_reader_t&);
            };
            extern Word sWord;

            class EndOfText : public tokenizer_t
            {
            public:
                EndOfText() {}
                virtual bool Check(runes_reader_t&);
            };
            extern EndOfText sEOT;

            class EndOfLine : public tokenizer_t
            {
            public:
                EndOfLine() {}
                virtual bool Check(runes_reader_t&);
            };
            extern EndOfLine sEOL;

            class Integer : public tokenizer_t
            {
                s64 m_min;
                s64 m_max;

            public:
                Integer()
                    : m_min(0)
                    , m_max(0x7fffffffffffffffL)
                {
                }
                Integer(s64 max)
                    : m_min(0)
                    , m_max(max)
                {
                }
                Integer(s64 min, s64 max)
                    : m_min(min)
                    , m_max(max)
                {
                }
                virtual bool Check(runes_reader_t&);
            };

            class Float : public tokenizer_t
            {
                f32 m_min;
                f32 m_max;

            public:
                Float()
                    : m_min(0.0f)
                    , m_max(3.402823e+38f)
                {
                }
                Float(f32 max)
                    : m_min(0.0f)
                    , m_max(max)
                {
                }
                Float(f32 min, f32 max)
                    : m_min(min)
                    , m_max(max)
                {
                }
                virtual bool Check(runes_reader_t&);
            };

            // namespace Date

        } // namespace filters

        namespace utils
        {
            class IPv4 : public tokenizer_t
            {
                manipulators::Within   m_d3;
                filters::Integer       m_b8;
                manipulators::And      m_sub;
                filters::Is            m_dot;
                manipulators::Sequence m_bad;
                manipulators::Times    m_domain;
                manipulators::Sequence m_ipv4;

            public:
                IPv4()
                    : m_d3(1, 3, filters::sDigit)
                    , m_b8(255)
                    , m_sub(m_d3, m_b8)
                    , m_dot('.')
                    , m_bad(m_sub, m_dot)
                    , m_domain(3, m_bad)
                    , m_ipv4(m_domain, m_sub)
                {
                }
                virtual bool Check(runes_reader_t&);
            };
            extern IPv4 sIPv4;

            class Host : public tokenizer_t
            {
            public:
                virtual bool Check(runes_reader_t&);
            };
            extern Host sHost;

            class Email : public tokenizer_t
            {
            public:
                virtual bool Check(runes_reader_t&);
            };
            extern Email sEmail;

            class ServerAddress : public tokenizer_t
            {
            public:
                virtual bool Check(runes_reader_t&);
            };
            extern ServerAddress sServerAddress;

            class Uri : public tokenizer_t
            {
            public:
                virtual bool Check(runes_reader_t&);
            };
            extern Uri sURI;
        } // namespace utils
    }     // namespace combparser


} // namespace xcore

#endif