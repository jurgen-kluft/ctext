#ifndef __XTEXT_XPARSER_H__
#define __XTEXT_XPARSER_H__
#include "xbase/x_target.h"
#ifdef USE_PRAGMA_ONCE
#pragma once
#endif

#include "xbase/x_runes.h"

namespace xcore
{
    class charwriter
    {
    public:
        charwriter();
        charwriter(char* str, char* end);
        charwriter(utf32::prune str, utf32::prune end);
        charwriter(const charwriter&);

        void reset();
        bool valid() const;
        void write(uchar32 c);

        bool operator<(const charwriter&) const;
        bool operator>(const charwriter&) const;
        bool operator<=(const charwriter&) const;
        bool operator>=(const charwriter&) const;
        bool operator==(const charwriter& t) const;
        bool operator!=(const charwriter& t) const;

        union ptr {
            void*        _any;
            char*        _ascii;
            utf8::rune*  _utf8;
            utf16::rune* _utf16;
            utf32::rune* _utf32;
        };

        s32 m_type;
        ptr m_str;
        ptr m_cur;
        ptr m_end;
    };

    class charreader
    {
    public:
        charreader();
        charreader(const char* str);
        charreader(utf32::pcrune str);
        charreader(const charreader&);

        s64     size() const;
        void    reset();
        bool    valid() const;
        uchar32 peek() const;
        uchar32 read();
        void    skip();
        void    select(charreader const& from, charreader const& until);

        charreader& operator=(const charreader&);
        bool        operator<(const charreader&) const;
        bool        operator>(const charreader&) const;
        bool        operator<=(const charreader&) const;
        bool        operator>=(const charreader&) const;
        bool        operator==(const charreader& t) const;
        bool        operator!=(const charreader& t) const;

        union crunes {
            inline crunes()
                : _ascii()
            {
            }
            ascii::crunes _ascii;
            utf8::crunes  _utf8;
            utf16::crunes _utf16;
            utf32::crunes _utf32;
        };
        crunes m_runes;
        s32    m_type;
    };

    class StringWriter
    {
    };

    class StringReader
    {
        charreader m_str;

    public:
        StringReader();
        StringReader(const char* str);
        StringReader(const StringReader& chars);
        StringReader(const StringReader& begin, const StringReader& until);

        s64  Size() const;
        void Reset();

        uchar32 Read();
        uchar32 Peek() const;

        void Select(const StringReader& begin, const StringReader& cursor);
        bool Valid() const;
        void Skip();

        void Write(StringWriter& writer);

        bool operator<(const StringReader&) const;
        bool operator>(const StringReader&) const;
        bool operator<=(const StringReader&) const;
        bool operator>=(const StringReader&) const;
        bool operator==(const StringReader&) const;
        bool operator!=(const StringReader&) const;

        StringReader& operator=(const StringReader&);
    };

    namespace xparser
    {
        class TokenizerInterface
        {
        public:
            /*!
             * @fn 	bool Check(StringReader&)
             * Check if the string pointed by cursor is complying to this parsing rule
             * @param[in/out] cursor  A cursor to the parsing string, after successful
             * parsing, the cursor value should move to the last character as far as the
             * proper parsing ends
             * @return 				 Rerturn @a true if the parsing
             * succeeds. Otherwise it returns false
             *
             */
            virtual bool Check(StringReader&) = 0;
        };

        /*!
         * @class RulesSet
         * @brief a generic container for all types of parsing elements ..
         * 		  it is recommended to use it as named rule template which can
         * be used several times
         * @code{.cpp}
         * 		RuleSet rs = Sequence(Is('A'),Is('B'));
         * @endcode
         */

        /*!
         * @namespace 	Manipulators
         * @brief 		the namespace that contains controlling lexical and
         * relational patterns parsing/recognition elements
         * @see 		Filters, Filters::Utils
         */
        namespace Manipulators
        {
            /*!
             * @class Not
             * @brief it is a negating effect parsing element. it turns the parsing process
             * 		 result of the contained element to the opposite ...
             * @note  You can use (!) operator for the same functionality
             * @warning 	Do not use nested @a Not instancess as it will not adjust the
             * 			cursor position properly
             *
             * @code{.cpp}
             * 		Not(Is('A')); 	// A,C,D,E ..etc => TRUE
             * 		!Is('A'); 		// the same effect
             * @endcode
             * @see And, Or, Not
             */
            class Not : public TokenizerInterface
            {
            private:
                TokenizerInterface& _Tokenizer;
                /*!
                 * @fn 	Not(TokenizerInterface& tok)
                 * Constructor to Not class
                 * @param[in] tok  contained parsing rule element
                 */
            public:
                /*!
                 * @fn 	Not(TokenizerInterface& tok)
                 * Constructor to Not
                 * @param[in] tok  contained parsing rule element
                 */
                Not(TokenizerInterface& tok)
                    : _Tokenizer(tok)
                {
                }
                virtual bool Check(StringReader&);
            };

            /*!
             * @class Or
             * @brief it is accepts two parsing elements and performs @a OR boolean
             * 			operation between them
             * 	is accepts two parsing elements and performs @a OR boolean
             * 	operation between them.the conditions has short circut effect
             * 	it will return for the first applicable condition and ignores
             * 	the rest of them, So be carful to use the most general form
             * 	later as the last condition after the most specified one
             * 	consider the example
             * @code{.cpp}
             * 		Or(Exact("A"),Exact("ABC")); // Wrong: it always returns in "A"
             * 		Or(Exact("ABC"),Exact("A")); // Correct
             * @endcode
             *
             *
             * @note  You can use (|) operator for the same functionality
             *
             * @b Example:
             * @code{.cpp}
             * 		Or(Is('A'),Is('B')); // "A" and "B" are Ok , "C" fails
             * 		Is('A') |  Is('B'); // the same functionality
             * @endcode
             * @see And, Or, Not
             */
            class Or : public TokenizerInterface
            {
            private:
                TokenizerInterface& _TokenizerA;
                TokenizerInterface& _TokenizerB;

            public:
                /*!
                 * @fn 	Or(TokenizerInterface& tok1, TokenizerInterface& tok2)
                 * Constructor to Or class
                 * @param[in] tok1  the first contained parsing rule element
                 * @param[in] tok2  the second contained parsing rule element
                 */
                Or(TokenizerInterface& tok1, TokenizerInterface& tok2)
                    : _TokenizerA(tok1)
                    , _TokenizerB(tok2)
                {
                }
                virtual bool Check(StringReader&);
            };

            /*!
             * @class And
             * @brief accepts two parsing elements and performs @a AND boolean
             * 	      operation between them,
             * 	accepts two parsing elements and performs @a AND boolean
             * 	operation between them, the conditions has short circut effect
             * 	it will return for upon the first unapplicable condition and ignores
             * 	the rest .. this pattern is not intended to be widely used.. and it has
             * 	a perticular case as excluding pattern ,ex:
             * @code{.cpp}
             * 		And(Alphanumeric(),Not(Number()));		// "A" ,"B" are Ok ,
             * "7" fails
             * 		And(Within(1,2,Digit()),Integer(55)) 	// "11" and "32" Ok ,
             * "57" and "122" Fails
             * @endcode
             *
             * @note  You can use (&) operator as a short hand for the same functionality
             *
             * @b Example:
             * @code{.cpp}
             * 		And(Alphanumeric(),Number()); // "1" and "2" are Ok , "A" fails
             * 		Alphanumeric() & Number(); // the same functionality
             * @endcode
             * @see And, Or, Not
             */
            class And : public TokenizerInterface
            {
            private:
                TokenizerInterface& _TokenizerA;
                TokenizerInterface& _TokenizerB;

            public:
                /*!
                 * @fn 	And(TokenizerInterface& tok1, TokenizerInterface& tok2)
                 * Constructor to And class
                 * @param[in] tok1  the first contained parsing rule element
                 * @param[in] tok2  the second contained parsing rule element
                 */
                And(TokenizerInterface& tok1, TokenizerInterface& tok2)
                    : _TokenizerA(tok1)
                    , _TokenizerB(tok2)
                {
                }

                virtual bool Check(StringReader&);
            };
            /*!
             * @class Sequence
             * @brief it accepts two parsing elements and execute them sequentialy in order
             * 			(ex. pattern1 then pattern2), if the first one fails the whole
             * sequence fails immediately .. this is common pattern to use
             *
             * @note  You can use (+) addition operator as a short hand for the same
             * functionality
             *
             * @b Example:
             * @code{.cpp}
             * 		Sequence(Is('A'),Is('B')); // "AB" is OK , "A" or "B" fails
             * 		Is('A') + Is('B'); // the same functionality
             * @endcode
             * @see Exact, Within
             */

            class Sequence : public TokenizerInterface
            {
            private:
                TokenizerInterface& _TokenizerA;
                TokenizerInterface& _TokenizerB;

            public:
                /*!
                 * @fn 	Sequence(TokenizerInterface& tok1, TokenizerInterface& tok2)
                 * Constructor to Sequence class
                 * @param[in] tok1  the first contained parsing rule element
                 * @param[in] tok2  the next contained parsing rule element
                 */
                Sequence(TokenizerInterface& tok1, TokenizerInterface& tok2)
                    : _TokenizerA(tok1)
                    , _TokenizerB(tok2)
                {
                }
                virtual bool Check(StringReader&);
            };

            /*!
             * @class Within
             * @brief it checks if the contained pattern is repeated number of times between
             * 		minimum and maximum limits , If maximum value equals -1 so the wont
             * be any upper limit (theoritically infinite repititions)
             *
             * @note  In repitition funtions, there is not cheking on end of text
             * 		 make sure that the rules is properly arranged to not overflow
             * the string limits
             *
             * @b Example:
             * @code{.cpp}
             * 		Within(2,4,Is('A')); // "AA","AAA","AAAA" are OK , "","A" or
             * "AAAAA" fail
             * @endcode
             * @see 	Times, While, Until, OneOrMore, ZeroOrOne, Filters::Exact
             * 		, Sequence
             */
            class Within : public TokenizerInterface
            {
            private:
                u64                 _Min;
                u64                 _Max;
                TokenizerInterface& _Tokenizer;

            public:
                /*!
                 * @fn 	Within(u64 min, u64 max, TokenizerInterface& tok)
                 * Constructor to Sequence class
                 * @param[in] min  Minimum number of repitition
                 * @param[in] max  Maximum number of repitition
                 * @param[in] tok  the next contained parsing rule element
                 */
                Within(u64 min, u64 max, TokenizerInterface& tok)
                    : _Min(min)
                    , _Max(max)
                    , _Tokenizer(tok)
                {
                }
                /*!
                 * @fn 	Within( u64 max, TokenizerInterface& tok)
                 * Constructor to Sequence class
                 * @param[in] max  Maximum number of repitition
                 * @param[in] tok  the next contained parsing rule element
                 */
                Within(u64 max, TokenizerInterface& tok)
                    : _Min(0)
                    , _Max(max)
                    , _Tokenizer(tok)
                {
                }
                virtual bool Check(StringReader&);
            };

            /*!
             * @class Times
             * @brief it checks if the contained pattern is repeated in exactly specified
             * number of times If the number equals -1 so there will not be any upper limit
             * (theoritically infinite repititions).. It is identical to using
             * "Within(number,number,...)"
             *
             * @note  In repitition funtions, there is not cheking on end of text
             * 		 make sure that the rules is properly arranged to not overflow
             * the string limits
             *
             * @b Example:
             * @code{.cpp}
             * 		Within(2,4,Is('A')); // "AA","AAA","AAAA" are OK , "","A" or
             * "AAAAA" fail
             * @endcode
             * @see 	 While, Until, OneOrMore, ZeroOrOne, Filters::Exact, Sequence
             */
            class Times : public TokenizerInterface
            {
            private:
                s32                 _Max;
                TokenizerInterface& _Tokenizer;

            public:
                /*!
                 * @fn 	Times( s32 max, TokenizerInterface& tok)
                 * Constructor to Sequence class
                 * @param[in] _Max  number of repitition
                 * @param[in] tok  the next contained parsing rule element
                 */
                Times(s32 max, TokenizerInterface& tok)
                    : _Max(max)
                    , _Tokenizer(tok)
                {
                }
                virtual bool Check(StringReader&);
            };

            /*!
             * @class OneOrMore
             * @brief	it checks if the contained pattern is repeated at least once ...
             *			It is identical to using "Within(1,-1,...)"
             *
             * @note  In repitition funtions, there is not cheking on end of text
             * 		 make sure that the rules is properly arranged to not overflow
             *the string limits
             *
             * @b Example:
             * @code{.cpp}
             * 		OneOrMore(Is('A')); // "A","AA","AAA"  are OK , "" or "B" fail
             * @endcode
             * @see 	Times, While, Until, ZeroOrOne, Filters::Exact, Sequence
             */
            class OneOrMore : public TokenizerInterface
            {
            private:
                TokenizerInterface& _Tokenizer;

            public:
                /*!
                 * @fn 	OneOrMore(TokenizerInterface& tok)
                 * Constructor to OneOrMore class
                 * @param[in] tok  the contained parsing rule element
                 */
                OneOrMore(TokenizerInterface& tok)
                    : _Tokenizer(tok)
                {
                }
                virtual bool Check(StringReader&);
            };

            /*!
             * @class ZeroOrOne
             * @brief	it checks if the contained pattern is exist or not .. both cases
             *are acceptable.. it is like checking for optional item ..but if it is existing
             *it must compley the rule specified , It is identical to using
             *"Within(0,1,...)"
             *
             * @b Example:
             * @code{.cpp}
             * 		Is('A') + ZeroOrOne(Is('B')) + Is('C') ; // "AC","ABC" are OK ,
             *"ADC" fails
             * @endcode
             * @see 	Times, While, Until, ZeroOrOne, Filters::Exact
             * 		, Sequence
             */
            class ZeroOrOne : public TokenizerInterface
            {
            private:
                TokenizerInterface& _Tokenizer;

            public:
                /*!
                 * @fn 	ZeroOrOne(TokenizerInterface& tok)
                 * Constructor to ZeroOrOne class
                 * @param[in] tok  the contained parsing rule element
                 */
                ZeroOrOne(TokenizerInterface& tok)
                    : _Tokenizer(tok)
                {
                }
                virtual bool Check(StringReader&);
            };

            /*!
             * @class Optional
             * @brief	see ZeroOrOne
             */
            typedef ZeroOrOne Optional;
            /*!
             * @class _0Or1
             * @brief	see ZeroOrOne
             */
            typedef ZeroOrOne _0Or1;

            /*!
             * @class While
             * @brief	it checks if the contained pattern is exist or not .. both cases
             *are acceptable.. it is like checking for optional item multiple times in
             *repititive pattern in contrast to ZeroOrOne
             *			 ..but if it is existing it must compley the rule
             *specified , It is identical to using "Within(0,-1,...)"
             *
             * @b Example:
             * @code{.cpp}
             * 		Is('A') + While(Is('B')) + Is('C') ; // "AC","ABC"and "ABBC" are
             *OK , "ADC" fails
             * @endcode
             * @see 	Times, While, Until, ZeroOrOne, Filters::Exact
             * 		, Sequence
             */
            class While : public TokenizerInterface
            {
            private:
                /*!
                 * @fn 	While(TokenizerInterface& tok)
                 * Constructor to While class
                 * @param[in] tok  the contained parsing rule element
                 */
                TokenizerInterface& _Tokenizer;

            public:
                While(TokenizerInterface& tok)
                    : _Tokenizer(tok)
                {
                }
                virtual bool Check(StringReader&);
            };

            /*!
             * @class Until
             * @brief	it is the main searching utility ..it skips the string until
             * 			the contained pattern found , It is identical
             * 			to using "While(Not(...))"  or "Within(0,-1,...)"
             *
             * @b Example:
             * @code{.cpp}
             * 		Until(Is('C')) + Is('C') ; // "AC","AAC"and "ABBC" are OK , "" "D"
             * fails
             * @endcode
             * @see 	Times, While, Until, ZeroOrOne, Filters::Exact
             * 		, Sequence
             */
            class Until : public TokenizerInterface
            {
            private:
                TokenizerInterface& _Tokenizer;

            public:
                /*!
                 * @fn 	Until(TokenizerInterface& tok)
                 * Constructor to Ultil class
                 * @param[in] tok  the contained parsing rule element
                 */
                Until(TokenizerInterface& tok)
                    : _Tokenizer(tok)
                {
                }
                virtual bool Check(StringReader&);
            };

            /*!
             * @class Extract
             * @brief	It is the extracting utility .. it returns the matched pattern
             * to the supplied buffer ...
             *
             * @b Example:
             * @code{.cpp}
             * 		char buffer[1024];
             * 		Extract(buffer,Is('C')) ; // if "C" .. buffer = "C"
             * @endcode
             */
            class Extract : public TokenizerInterface
            {
            private:
                TokenizerInterface& _Tokenizer;
                StringReader&       _Selection;

            public:
                /*!
                 * @fn 	Extract(TokenizerInterface& tok)
                 * Constructor to Extract class
                 * @param[out] str  the returned string
                 * @param[in] tok  the contained parsing rule element
                 */
                Extract(StringReader& selection, TokenizerInterface& tok)
                    : _Selection(selection)
                    , _Tokenizer(tok)
                {
                }
                virtual bool Check(StringReader&);
            };

            /*!
             * @class ReturnToCallback
             * @brief	It is similar to Extract.. but it has more delicate usage than
             * Extract like extract to list or arrays for repititive tokens supplied
             * callback instead of using buffers
             *
             *
             * @b Example:
             * @code{.cpp}
             * 		void callback(StringReader& _token)
             * 		{
             * 			// Process the token
             * 		}
             * 		ReturnToCallback(callback,Is('C')) ; //
             * @endcode
             */
            class ReturnToCallback : public TokenizerInterface
            {
            public:
                typedef void (*CallBack)(StringReader&);

            private:
                TokenizerInterface& _Tokenizer;
                CallBack            cb;

            public:
                /*!
                 * @fn 	ReturnToCallback(CallBack cb, const TokenizerInterface&
                 * _Tokenizer) Constructor to Extract class
                 * @param[in] cb  the called callback for returning
                 * @param[in] _Tokenizer  the contained parsing rule element
                 */
                ReturnToCallback(CallBack cb, TokenizerInterface& tok)
                    : cb(cb)
                    , _Tokenizer(tok)
                {
                }
                virtual bool Check(StringReader&);
            };

            typedef While ZeroOrMore;

            /*!
             * @class Enclosed
             * @brief	It is pattern which matches the specified parsing element
             * between defined type of brackets
             *
             * @b Example:
             * @code{.cpp}
             * 		Enclosed("<",">",Is('C')) ; // "<C>" OK ... "C" "<C" "C>" "[C]"
             * fails
             * @endcode
             */
            class Enclosed : public TokenizerInterface
            {
                TokenizerInterface& _Tokenizer;
                StringReader        m_open;
                StringReader        m_close;

            public:
                /*!
                 * @fn 	Enclosed(Char* open, Char* close, const TokenizerInterface& tok)
                 * Constructor to Extract class
                 * @param[in] open the opening bracket
                 * @param[in] close the closing bracket
                 * @param[in] token the contained parsing rule element
                 */
                Enclosed(const char* open, const char* close, TokenizerInterface& token)
                    : _Tokenizer(token)
                    , m_open(open)
                    , m_close(close)
                {
                }
                virtual bool Check(StringReader&);
            };

        } // namespace Manipulators

        namespace Filters
        {
            /*!
             * @class Any
             * @brief	matching element that matches any character (even null
             * terminator) it is used in skipping characters and go forward ... it simply
             * 			increment the pointer and return true always
             *
             * @b Example:
             * @code{.cpp}
             * 		Any(); // "C" " " "#" OK ... never fails
             * @endcode
             */
            class Any : public TokenizerInterface
            {
            public:
                Any() {}
                virtual bool Check(StringReader&);
            };

            extern Any ANY;

            /*!
             * @class In
             * @brief	Checks if the character unnder processing is existing in
             * 			a defined set ... it is identical to use
             * 			Is('...')|Is('...')|Is('...')
             *
             * @b Example:
             * @code{.cpp}
             * 		In("ABC"); // "A" , "B" or "C" OK ... "D" fails
             * @endcode
             */
            class In : public TokenizerInterface
            {
            private:
                StringReader _Input;

            public:
                In(const char* str)
                    : _Input(str)
                {
                }
                virtual bool Check(StringReader&);
            };

            /*!
             * @class Between
             * @brief	Checks if the character unnder processing is existing between
             * 			a defined range.. it can be represented by relation
             * 			min <= a <= max (<= : lesser than or equal)..minimum must be
             * lesser than maximum otherwise it will always return false... if min and max
             * are equal it is better to user Is()
             *
             * @b Example:
             * @code{.cpp}
             * 		Between('A','C'); // "A" , "B" or "C" OK ... "D" fails
             * @endcode
             */
            class Between : public TokenizerInterface
            {
                uchar32 _Lower, _Upper;

            public:
                Between(uchar32 a, uchar32 b)
                    : _Lower(a)
                    , _Upper(b)
                {
                }
                virtual bool Check(StringReader&);
            };
            // TODO:class SmallLetter;
            // TODO:class CapitalLetter;
            // TODO:Class Letter;
            // TODO:Class Punctuation
            /*!
             * @class Alphabet
             * @brief	Checks if the character unnder processing is an alphabet letter
             * 			(small or capital) .. it can be represented by relation
             * 			A <= a <= Z (<= : lesser than or equal)..
             *
             * @b Example:
             * @code{.cpp}
             * 		Alphabet() ; // "A" , "B" or "C" are OK ... "9" and "#" fail
             * @endcode
             */
            class Alphabet : public TokenizerInterface
            {

            public:
                Alphabet() {}
                virtual bool Check(StringReader&);
            };

            extern Alphabet ALPHABET;

            /*!
             * @class Digit
             * @brief	Checks if the character unnder processing is an numeric digit
             * 			 .. it can be represented by relation
             * 			0 <= a <= 9 (<= : lesser than or equal)..
             *
             * @b Example:
             * @code{.cpp}
             * 		Digit() ; // "1" , "2" or "7" are OK ... "A" and "#" fail
             * @endcode
             */
            class Digit : public TokenizerInterface
            {

            public:
                Digit() {}
                virtual bool Check(StringReader&);
            };

            extern Digit DIGIT;
            // Class Octal
            /*!
             * @class Hex
             * @brief	Checks if the character unnder processing is an hex digit
             * 			from 0 to f .. it can be represented by relation
             *
             * @b Example:
             * @code{.cpp}
             * 		Hex() ; // "1" , "A" or "7" are OK ... "G" and "#" fail
             * @endcode
             */
            class Hex : public TokenizerInterface
            {

            public:
                Hex() {}
                virtual bool Check(StringReader&);
            };

            extern Hex HEX;
            /*!
             * @class AlphaNumeric
             * @brief	Checks if the character unnder processing is an alphabet letter
             * 			or numeric digit  .. it is identical to using
             * 			Alphabet() | Digit()
             *
             * @b Example:
             * @code{.cpp}
             * 		AlphaNumeric() ; // "A" , "2" or "0" are OK ... " " and "#" fail
             * @endcode
             */
            class AlphaNumeric : public TokenizerInterface
            {
            public:
                AlphaNumeric() {}
                virtual bool Check(StringReader&);
            };

            extern AlphaNumeric ALPHANUMERIC;

            /*!
             * @class Exact
             * @brief	Checks if the character unnder processing is following
             * 			the specified charcters sequence in case sensitive
             * profile
             * 			.. it is identical to using
             * 			Is('...') + Is('...') + Is('...') + ...
             *
             * @b Example:
             * @code{.cpp}
             * 		Exact("ABC") ; // "ABC" is OK ... "abc" or anything else fails
             * @endcode
             */
            class Exact : public TokenizerInterface
            {
                StringReader _Input;

            public:
                Exact(const char* str)
                    : _Input(str)
                {
                }
                Exact(StringReader const& str)
                    : _Input(str)
                {
                }

                virtual bool Check(StringReader&);
            };

            /*!
             * @class Like
             * @brief	Checks if the character unnder processing is following
             * 			the specified charcters sequence in case insensitive
             * profile
             * 			..
             *
             * @b Example:
             * @code{.cpp}
             * 		Like("ABC") ; // "ABC" and "abc" are OK ... otherwise fails
             * @endcode
             */
            class Like : public TokenizerInterface
            {
                StringReader _Input;

            public:
                Like(const char* str)
                    : _Input(str)
                {
                }
                virtual bool Check(StringReader&);
            };
            /*!
             * @class WhiteSpace
             * @brief	Checks if the character unnder processing is a whitespace,tab
             * 			or newline.. it is identical to using
             * 			In(" \t\n\r");
             *
             * @b Example:
             * @code{.cpp}
             * 		WhiteSpace () ; // " " , "\n" or "\t" are OK ... "A" and "#"
             * fail
             * @endcode
             */
            class WhiteSpace : public TokenizerInterface
            {
            public:
                WhiteSpace() {}
                virtual bool Check(StringReader&);
            };
            extern WhiteSpace WHITESPACE;

            /*!
             * @class Is
             * @brief	Checks if the character under processing is equal to the
             * specified character.. it is the most basic for of matching pattern
             *
             * @b Example:
             * @code{.cpp}
             * 		Is('A') ; // "A" are OK ... "a" or anything else fails
             * @endcode
             */
            class Is : public TokenizerInterface
            {
                uchar32 _Letter;

            public:
                Is(const uchar32 c)
                    : _Letter(c)
                {
                }
                // Is(const Char* s) :letter(s[0]) {}
                virtual bool Check(StringReader&);
            };

            /*!
             * @class Decimal
             * @brief	Checks if the token unnder processing is following
             * 			the numeric pattern forming like a positive integer
             * number.. it is identical to using OneOrMore(Digit())
             * 			..
             *
             * @b Example:
             * @code{.cpp}
             * 		Decimal() ; // "1" and "12" are OK ... "A" "-1" fails
             * @endcode
             */
            class Decimal : public TokenizerInterface
            {

            public:
                Decimal() {}
                virtual bool Check(StringReader&);
            };
            extern Decimal DECIMAL;

            /*!
             * @class Word
             * @brief	Checks if the token unnder processing is following
             * 			the aphabetic sequence.. it is identical to using
             * 			OneOrMore(Alphabet())
             * 			..
             *
             * @b Example:
             * @code{.cpp}
             * 		Word() ; // "1" and "12" are OK ... "A" "-1" fails
             * @endcode
             */
            class Word : public TokenizerInterface
            {

            public:
                Word() {}
                virtual bool Check(StringReader&);
            };
            extern Word WORD;

            /*!
             * @class EndOfText
             * @brief	Checks if the character under processing terminates the text
             * 			in contrast to other patterns ...this matching pattern does
             * not increment string pointer.. it is not identical to using Is('\0')
             *
             * @b Example:
             * @code{.cpp}
             * 		EndOfText() ; // "" are OK ... otherwise fails
             * 		EndOfText() + EndOfText(); // "" are OK ... does not pass the
             * end
             * @endcode
             */
            class EndOfText : public TokenizerInterface
            {
            public:
                EndOfText() {}
                virtual bool Check(StringReader&);
            };
            extern EndOfText EOT;

            /*!
             * @class EndOfLine
             * @brief	Checks if the character under processing terminates the line
             * 			...it is identical to using Exact("\\r\\n") on Windows and
             *          Is('\\n') on Mac/Linux
             *
             */
            class EndOfLine : public TokenizerInterface
            {
            public:
                EndOfLine() {}
                virtual bool Check(StringReader&);
            };
            extern EndOfLine EOL;

            /*!
             * @class Integer
             * @brief	this is more intellegent matching pattern ...
             * 			Checks if the token under processing is integer number
             * 			within the specified number range
             * 			...it is not like using Decimal()
             *
             * @b Example:
             * @code{.cpp}
             * 		Integer(-1,15) ; // "0" "-1" "12" are OK ... "-2" or "16" fail
             * @endcode
             */

            class Integer : public TokenizerInterface
            {
                s32 _Min, _Max;

            public:
                Integer(s32 min, s32 max)
                    : _Min(min)
                    , _Max(max)
                {
                }
                Integer(s32 max)
                    : _Min(0)
                    , _Max(max)
                {
                }
                Integer()
                    : _Min(0)
                    , _Max(0x7fffffff)
                {
                }
                virtual bool Check(StringReader&);
            };

            /*!
             * @class Float
             * @brief	this is more intellegent matching pattern ...
             * 			Checks if the token under processing is a floating point
             * number within the specified number range
             * 			...it is not like using Decimal()
             *
             * @b Example:
             * @code{.cpp}
             * 		Float(-1.2f,7.24f) ; // "0" "-1.0" "5.447567" are OK ... "-2.0"
             * or "16.3" fail
             * @endcode
             */
            class Float : public TokenizerInterface
            {
                f32 _Min, _Max;

            public:
                Float(f32 min, f32 max)
                    : _Min(min)
                    , _Max(max)
                {
                }
                Float(f32 max)
                    : _Min(0.0f)
                    , _Max(max)
                {
                }
                Float()
                    : _Min(0.0f)
                    , _Max(3.402823e+38f)
                {
                }
                virtual bool Check(StringReader&);
            };
            // namespace Date

        } // namespace Filters
        /*!
         * @namespace 	Utils
         * @brief 		more comprehensive matching pattern elements that are
         * used usually in several development parsing/searching tasks
         */
        namespace Utils
        {
            /*!
             * @struct Range
             * @brief	Range
             *
             */
            typedef struct Range
            {
                s32 Minimum;
                s32 Maximum;
                Range(s32 min, s32 max)
                    : Minimum(min)
                    , Maximum(max)
                {
                }
            } R;

            /*!
             * @class IPv4
             * @brief	check if the token under processing is in IP address V4 format
             * 			(aaa.bbb.ccc.ddd)
             *
             */
            class IPv4 : public TokenizerInterface
            {
            public:
                virtual bool Check(StringReader&);
            };
            extern IPv4 IPV4;

            /*!
             * @class Host
             * @brief	check if the token under processing is a host descriptive
             * 			(server.domain.com) (127.0.0.1)
             *
             */
            class Host : public TokenizerInterface
            {
            public:
                virtual bool Check(StringReader&);
            };

            extern Host HOST;

            /*!
             * @class Email
             * @brief	check if the token under processing is a email address
             * 			(abc@host.com)
             *
             */
            class Email : public TokenizerInterface
            {
            public:
                virtual bool Check(StringReader&);
            };
            extern Email EMAIL;

            /*!
             * @class Phone
             * @brief	check if the token under processing is a email address
             * 			(+(123)555 443 2 22)
             *
             */
            class Phone : public TokenizerInterface
            {
            public:
                virtual bool Check(StringReader&);
            };
            extern Phone PHONE;

            /*!
             * @class ServerAddress
             * @brief	check if the token under processing is server plus port
             * 			(host@domain.com:12345)
             *
             */
            class ServerAddress : public TokenizerInterface
            {
            public:
                virtual bool Check(StringReader&);
            };
            extern ServerAddress SERVERADDRESS;

            /*!
             * @class URI
             * @brief	check if the token under processing is URI address
             * 			(proto:abc@host.domain.com:4444)
             *
             */
            class Uri : public TokenizerInterface
            {
            public:
                virtual bool Check(StringReader&);
            };
            extern Uri URI;

        } // namespace Utils

    } // namespace xparser

    class StringProcessor
    {
    private:
        StringReader _String;
        StringReader _Cursor;
        StringReader _LastTokenized;

    public:
        StringProcessor();
        StringProcessor(StringReader const& str);

        bool         Parse(xparser::TokenizerInterface&);
        bool         Validate(xparser::TokenizerInterface&);
        StringReader Search(xparser::TokenizerInterface&);
        bool         IsEOT();
        void         Reset();
    };

    inline xcore::xparser::Manipulators::Times operator*(xcore::xparser::TokenizerInterface& tok, s32 times) { return xcore::xparser::Manipulators::Times(times, tok); }

    inline xcore::xparser::Manipulators::Times operator*(s32 times, xcore::xparser::TokenizerInterface& tok) { return xcore::xparser::Manipulators::Times(times, tok); }

    inline xcore::xparser::Manipulators::Within operator*(xcore::xparser::TokenizerInterface& tok, const xcore::xparser::Utils::Range& range)
    {
        return xcore::xparser::Manipulators::Within(range.Minimum, range.Maximum, tok);
    }

    inline xcore::xparser::Manipulators::Within operator*(xcore::xparser::Utils::Range& range, xcore::xparser::TokenizerInterface& tok)
    {
        return xcore::xparser::Manipulators::Within(range.Minimum, range.Maximum, tok);
    }

    inline xcore::xparser::Manipulators::Or operator|(xcore::xparser::TokenizerInterface& t1, xcore::xparser::TokenizerInterface& t2)
    {
        return xcore::xparser::Manipulators::Or(t1, t2);
    }

    inline xcore::xparser::Manipulators::And operator&(xcore::xparser::TokenizerInterface& t1, xcore::xparser::TokenizerInterface& t2)
    {
        return xcore::xparser::Manipulators::And(t1, t2);
    }

    inline xcore::xparser::Manipulators::Sequence operator+(xcore::xparser::TokenizerInterface& t1, xcore::xparser::TokenizerInterface& t2)
    {
        return xcore::xparser::Manipulators::Sequence(t1, t2);
    }

    inline xcore::xparser::Manipulators::Not operator!(xcore::xparser::TokenizerInterface& tok) { return xcore::xparser::Manipulators::Not(tok); }

} // namespace xcore

#endif