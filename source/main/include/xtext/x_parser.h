#ifndef __XTEXT_XPARSER_H__
#define __XTEXT_XPARSER_H__
#include "xbase/x_target.h"
#ifdef USE_PRAGMA_ONCE
#pragma once
#endif

#include "xbase/x_runes.h"

namespace xcore
{
    // Move to xbase
    // Actually this can totally replace the runes/crunes everywhere.
    // Even for console, we can pass this charreader and literally use
    // any string format, from ascii, utf16 ....
    //
    class charreader;

    class charwriter
    {
    public:
        charwriter();
        charwriter(char* str, char* end);
        charwriter(utf32::prune str, utf32::prune end);
        charwriter(ascii::runes const& str);
        charwriter(utf8::runes const& str);
        charwriter(utf16::runes const& str);
        charwriter(utf32::runes const& str);
        charwriter(const charwriter&);

        void reset();
        bool valid() const;
        void write(uchar32 c);
        void write(charreader const& c);

        bool operator<(const charwriter&) const;
        bool operator>(const charwriter&) const;
        bool operator<=(const charwriter&) const;
        bool operator>=(const charwriter&) const;
        bool operator==(const charwriter& t) const;
        bool operator!=(const charwriter& t) const;

        union runes
        {
            inline runes()
                : _ascii()
            {
            }
            ascii::runes _ascii;
            utf8::runes  _utf8;
            utf16::runes _utf16;
            utf32::runes _utf32;
        };
        runes m_runes;
        s32   m_type;
    };

    class charreader
    {
    public:
        charreader();
        charreader(const char* str);
        charreader(utf32::pcrune str);
        charreader(ascii::crunes const& str);
        charreader(utf8::crunes const& str);
        charreader(utf16::crunes const& str);
        charreader(utf32::crunes const& str);
        charreader(const charwriter&);
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

        union crunes
        {
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

#include "xtext/private/x_parser_defines.h"

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
            MANIPULATOR1(Not);

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
            MANIPULATOR2(Or);

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
            MANIPULATOR2(And);
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
            MANIPULATOR2(Sequence);

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
            MANIPULATOR_MINMAX(Within, u64, 0, 0xffffffffffffffffUL);

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
            MANIPULATOR11(Times, s32, _Max);

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
            MANIPULATOR1(OneOrMore);

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
            MANIPULATOR1(ZeroOrOne);
            typedef ZeroOrOne Optional;
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
            MANIPULATOR1(While);
            typedef While ZeroOrMore;

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
            MANIPULATOR1(Until);

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
            MANIPULATOR11(Extract, StringReader&, _Selection);

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
            typedef void (*CallBack)(StringReader&);
            MANIPULATOR11(ReturnToCallback, CallBack, cb);

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
            MANIPULATOR12(Enclosed, StringReader, m_open, StringReader, m_close);

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
            FILTER1(Any);
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
            FILTER11(In, StringReader, _Input);

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
            FILTER12(Between, uchar32, _Lower, uchar32, _Upper);

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
            FILTER1(Alphabet);
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
            FILTER1(Digit);
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
            FILTER1(Hex);
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
            FILTER1(AlphaNumeric);
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
            FILTER11(Exact, StringReader, _Input);

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
            FILTER11(Like, StringReader, _Input);

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
            FILTER1(WhiteSpace);
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
            FILTER11(Is, uchar32, _Letter);

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
            FILTER1(Decimal);
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
            FILTER1(Word);
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
            FILTER1(EndOfText);
            extern EndOfText EOT;

            /*!
             * @class EndOfLine
             * @brief	Checks if the character under processing terminates the line
             * 			...it is identical to using Exact("\\r\\n") on Windows and
             *          Is('\\n') on Mac/Linux
             *
             */
            FILTER1(EndOfLine);
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

            FILTER_MINMAX(Integer, s32, 0, 0x7fffffff);
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
            FILTER_MINMAX(Float, f32, 0.0f, 3.402823e+38f);
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
                s32 _Min, _Max;
                Range(s32 min, s32 max)
                    : _Min(min)
                    , _Max(max)
                {
                }
            } R;

            /*!
             * @class IPv4
             * @brief	check if the token under processing is in IP address V4 format
             * 			(aaa.bbb.ccc.ddd)
             *
             */
            UTIL1(IPv4);
            extern IPv4 IPV4;

            /*!
             * @class Host
             * @brief	check if the token under processing is a host descriptive
             * 			(server.domain.com) (127.0.0.1)
             *
             */
            UTIL1(Host);
            extern Host HOST;

            /*!
             * @class Email
             * @brief	check if the token under processing is a email address
             * 			(abc@host.com)
             *
             */
            UTIL1(Email);
            extern Email EMAIL;

            /*!
             * @class Phone
             * @brief	check if the token under processing is a email address
             * 			(+(123)555 443 2 22)
             *
             */
            UTIL1(Phone);
            extern Phone PHONE;

            /*!
             * @class ServerAddress
             * @brief	check if the token under processing is server plus port
             * 			(host@domain.com:12345)
             *
             */
            UTIL1(ServerAddress);
            extern ServerAddress SERVERADDRESS;

            /*!
             * @class URI
             * @brief	check if the token under processing is URI address
             * 			(proto:abc@host.domain.com:4444)
             *
             */
            UTIL1(Uri);
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

    inline xcore::xparser::Manipulators::Times  operator*(xcore::xparser::TokenizerInterface& tok, s32 times) { return xcore::xparser::Manipulators::Times(times, tok); }
    inline xcore::xparser::Manipulators::Times  operator*(s32 times, xcore::xparser::TokenizerInterface& tok) { return xcore::xparser::Manipulators::Times(times, tok); }
    inline xcore::xparser::Manipulators::Within operator*(xcore::xparser::TokenizerInterface& tok, const xcore::xparser::Utils::Range& range)
    {
        return xcore::xparser::Manipulators::Within(range._Min, range._Max, tok);
    }
    inline xcore::xparser::Manipulators::Within operator*(xcore::xparser::Utils::Range& range, xcore::xparser::TokenizerInterface& tok)
    {
        return xcore::xparser::Manipulators::Within(range._Min, range._Max, tok);
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