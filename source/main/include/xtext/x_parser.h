#ifndef __XTEXT_XPARSER_H__
#define __XTEXT_XPARSER_H__
#include "xbase/x_target.h"
#ifdef USE_PRAGMA_ONCE
#pragma once
#endif

#include "xbase/x_runes.h"
#include "xtext/x_text.h"

namespace xcore
{
    namespace xparser
    {
        class tokenizer_t
        {
        public:
            /*!
             * @fn 	bool Check(xtext::reader_t&, xtext::reader_t::cursor_t&)
             * Check if the string pointed by cursor is complying to this parsing rule
             * @param[in/out] cursor  A cursor to the parsing string, after successful
             * parsing, the cursor value should move to the last character as far as the
             * proper parsing ends
             * @return  Return @a true if the parsing succeeds. Otherwise it returns false
             *
             */
            virtual bool Check(xtext::reader_t&, xtext::reader_t::cursor_t&) = 0;
        };

        class tokenizer_1_t : public tokenizer_t
        {
        public:
            tokenizer_1_t(tokenizer_t& toka) : m_tokenizer_a(toka) {}

        protected:
            tokenizer_t& m_tokenizer_a;
        };
        class tokenizer_2_t : public tokenizer_t
        {
        public:
            tokenizer_2_t(tokenizer_t& toka, tokenizer_t& tokb) : m_tokenizer_a(toka), m_tokenizer_b(tokb) {}

        protected:
            tokenizer_t& m_tokenizer_a;
            tokenizer_t& m_tokenizer_b;
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
         * @namespace 	xmanipulators
         * @brief 		the namespace that contains controlling lexical and
         * relational patterns parsing/recognition elements
         * @see 		xfilters, xfilters::xutils
         */
        namespace xmanipulators
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
            class Not : public tokenizer_1_t
            {
            public:
                inline Not(tokenizer_t& toka) : tokenizer_1_t(toka) {}
                virtual bool Check(xtext::reader_t&, xtext::reader_t::cursor_t&);
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
            class Or : public tokenizer_2_t
            {
            public:
                inline Or(tokenizer_t& toka, tokenizer_t& tokb) : tokenizer_2_t(toka, tokb) {}
                virtual bool Check(xtext::reader_t&, xtext::reader_t::cursor_t&);
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
            class And : public tokenizer_2_t
            {
            public:
                And(tokenizer_t& toka, tokenizer_t& tokb) : tokenizer_2_t(toka, tokb) {}
                virtual bool Check(xtext::reader_t&, xtext::reader_t::cursor_t&);
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
            class Sequence : public tokenizer_2_t
            {
            public:
                inline Sequence(tokenizer_t& toka, tokenizer_t& tokb) : tokenizer_2_t(toka, tokb) {}
                virtual bool Check(xtext::reader_t&, xtext::reader_t::cursor_t&);
            };
            class Sequence3 : public tokenizer_2_t
            {
                tokenizer_t& m_tokenizer_c;
            public:
                inline Sequence3(tokenizer_t& toka, tokenizer_t& tokb, tokenizer_t& tokc) : tokenizer_2_t(toka, tokb), m_tokenizer_c(tokc) {}
                virtual bool Check(xtext::reader_t&, xtext::reader_t::cursor_t&);
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
             * @see 	Times, While, Until, OneOrMore, ZeroOrOne, xfilters::Exact
             * 		, Sequence
             */
            class Within : public tokenizer_1_t
            {
                u64 m_min, m_max;

            public:
                Within(u64 min, u64 max, tokenizer_t& toka) : tokenizer_1_t(toka), m_min(min), m_max(max) {}
                Within(u64 max, tokenizer_t& toka) : tokenizer_1_t(toka), m_min(0), m_max(max) {}
                Within(tokenizer_t& toka) : tokenizer_1_t(toka), m_min(0), m_max(0xffffffffffffffffUL) {}
                virtual bool Check(xtext::reader_t&, xtext::reader_t::cursor_t&);
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
             * @see 	 While, Until, OneOrMore, ZeroOrOne, xfilters::Exact, Sequence
             */
            class Times : public tokenizer_1_t
            {
                s32 m_max;

            public:
                inline Times(s32 max, tokenizer_t& toka) : tokenizer_1_t(toka), m_max(max) {}
                virtual bool Check(xtext::reader_t&, xtext::reader_t::cursor_t&);
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
             * @see 	Times, While, Until, ZeroOrOne, xfilters::Exact, Sequence
             */
            class OneOrMore : public tokenizer_1_t
            {
            public:
                inline OneOrMore(tokenizer_t& toka) : tokenizer_1_t(toka) {}
                virtual bool Check(xtext::reader_t&, xtext::reader_t::cursor_t&);
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
             * @see 	Times, While, Until, ZeroOrOne, xfilters::Exact
             * 		, Sequence
             */
            class ZeroOrOne : public tokenizer_1_t
            {
            public:
                inline ZeroOrOne(tokenizer_t& toka) : tokenizer_1_t(toka) {}
                virtual bool Check(xtext::reader_t&, xtext::reader_t::cursor_t&);
            };
            typedef ZeroOrOne Optional;
            typedef ZeroOrOne _0Or1;

            /*!
             * @class While
             * @brief	it checks if the contained pattern is exist or not .. both cases
             *are acceptable.. it is like checking for optional item multiple times in
             *repititive pattern in contrast to ZeroOrOne
             *			 ..but if it is existing it must comply the rule
             *specified , It is identical to using "Within(0,-1,...)"
             *
             * @b Example:
             * @code{.cpp}
             * 		Is('A') + While(Is('B')) + Is('C') ; // "AC","ABC"and "ABBC" are
             *OK , "ADC" fails
             * @endcode
             * @see 	Times, While, Until, ZeroOrOne, xfilters::Exact
             * 		, Sequence
             */
            class While : public tokenizer_1_t
            {
            public:
                inline While(tokenizer_t& toka) : tokenizer_1_t(toka) {}
                virtual bool Check(xtext::reader_t&, xtext::reader_t::cursor_t&);
            };
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
             * @see 	Times, While, Until, ZeroOrOne, xfilters::Exact
             * 		, Sequence
             */
            class Until : public tokenizer_1_t
            {
            public:
                inline Until(tokenizer_t& toka) : tokenizer_1_t(toka) {}
                virtual bool Check(xtext::reader_t&, xtext::reader_t::cursor_t&);
            };

            /*!
             * @class Extract
             * @brief	It is the extracting utility .. it returns the matched pattern
             * to the supplied reader
             *
             * @b Example:
             * @code{.cpp}
             * 		xtext::reader_t selection;
             * 		Extract(selection, Is('C')) ; // if "C" .. selection = "C"
             * @endcode
             */
            class Extract : public tokenizer_1_t
            {
                xtext::reader_t& m_selection;

            public:
                inline Extract(xtext::reader_t& m1, tokenizer_t& toka) : tokenizer_1_t(toka), m_selection(m1) {}
                virtual bool Check(xtext::reader_t&, xtext::reader_t::cursor_t&);
            };

            /*!
             * @class ReturnToCallback
             * @brief	It is similar to Extract.. but it has more delicate usage than
             * Extract like extract to list or arrays for repetitive tokens supplied
             * callback instead of using buffers
             *
             *
             * @b Example:
             * @code{.cpp}
             * 		void callback(stringreader_t& _token)
             * 		{
             * 			// Process the token
             * 		}
             * 		ReturnToCallback(callback,Is('C')) ; //
             * @endcode
             */
            typedef void (*CallBack)(xtext::reader_t&, xtext::reader_t::cursor_t&);
            class ReturnToCallback : public tokenizer_1_t
            {
                CallBack m_cb;

            public:
                inline ReturnToCallback(CallBack cb, tokenizer_t& toka) : tokenizer_1_t(toka), m_cb(cb) {}
                virtual bool Check(xtext::reader_t&, xtext::reader_t::cursor_t&);
            };

            /*!
             * @class Enclosed
             * @brief	It is pattern which matches the specified parsing element
             * between defined type of brackets
             *
             * @b Example:
             * @code{.cpp}
             * 		Enclosed("<",">",Is('C')) ; // "<C>" OK ... "C" "<C" "C>" "[C]" fails
             * @endcode
             */
            class Enclosed : public tokenizer_1_t
            {
                xtext::reader_t m_open;
                xtext::reader_t m_close;

            public:
                inline Enclosed(xtext::reader_t open, xtext::reader_t close, tokenizer_t& toka) : tokenizer_1_t(toka), m_open(open), m_close(close) {}
                virtual bool Check(xtext::reader_t&, xtext::reader_t::cursor_t&);
            };

        } // namespace xmanipulators

        namespace xfilters
        {
            /*!
             * @class Any
             * @brief	matching element that matches any character (even null
             *          terminator) it is used in skipping characters and go forward ...
             * 			it simply increments the pointer and return true always
             *
             * @b Example:
             * @code{.cpp}
             * 		Any(); // "C" " " "#" OK ... never fails
             * @endcode
             */
            class Any : public tokenizer_t
            {
            public:
                Any() {}
                virtual bool Check(xtext::reader_t&, xtext::reader_t::cursor_t&);
            };
            extern Any sAny;

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
            class In : public tokenizer_t
            {
                xtext::reader_t m_input;

            public:
                In() {}
                In(xtext::reader_t input) : m_input(input) {}
                virtual bool Check(xtext::reader_t&, xtext::reader_t::cursor_t&);
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
            class Between : public tokenizer_t
            {
                uchar32 m_lower;
                uchar32 m_upper;

            public:
                Between() : m_lower('a'), m_upper('z') {}
                Between(uchar32 lower, uchar32 upper) : m_lower(lower), m_upper(upper) {}
                virtual bool Check(xtext::reader_t&, xtext::reader_t::cursor_t&);
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
            class Alphabet : public tokenizer_t
            {
                Between m_lower_case;
                Between m_upper_case;

            public:
                Alphabet() : m_lower_case('a', 'z'), m_upper_case('A', 'Z') {}
                virtual bool Check(xtext::reader_t&, xtext::reader_t::cursor_t&);
            };
            extern Alphabet sAlphabet;

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
            class Digit : public tokenizer_t
            {
                Between m_digit; //@TODO; make static
            public:
                Digit() : m_digit('0', '9') {}
                virtual bool Check(xtext::reader_t&, xtext::reader_t::cursor_t&);
            };
            extern Digit sDigit;

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
            class Hex : public tokenizer_t
            {
                Digit   m_digit;
                Between m_lower_case;
                Between m_upper_case;

            public:
                Hex() : m_lower_case('a', 'f'), m_upper_case('A', 'F') {}
                virtual bool Check(xtext::reader_t&, xtext::reader_t::cursor_t&);
            };
            extern Hex sHex;

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
            class AlphaNumeric : public tokenizer_t
            {
            public:
                AlphaNumeric() {}
                virtual bool Check(xtext::reader_t&, xtext::reader_t::cursor_t&);
            };
            extern AlphaNumeric sAlphaNumeric;

            /*!
             * @class Exact
             * @brief	Checks if the character unnder processing is following
             * 			the specified characters sequence in case sensitive
             * profile
             * 			.. it is identical to using
             * 			Is('...') + Is('...') + Is('...') + ...
             *
             * @b Example:
             * @code{.cpp}
             * 		Exact("ABC") ; // "ABC" is OK ... "abc" or anything else fails
             * @endcode
             */
            class Exact : public tokenizer_t
            {
                xtext::reader_t m_input;

            public:
                Exact() {}
                Exact(xtext::reader_t input) : m_input(input) {}
                virtual bool Check(xtext::reader_t&, xtext::reader_t::cursor_t&);
            };

            /*!
             * @class Like
             * @brief	Checks if the character unnder processing is following
             * 			the specified characters sequence in case insensitive
             * profile
             * 			..
             *
             * @b Example:
             * @code{.cpp}
             * 		Like("ABC") ; // "ABC" and "abc" are OK ... otherwise fails
             * @endcode
             */
            class Like : public tokenizer_t
            {
                xtext::reader_t           m_input;
                xtext::reader_t::cursor_t m_from;
                xtext::reader_t::cursor_t m_to;

            public:
                Like() {}
                Like(xtext::reader_t input) : m_input(input) {}
                Like(xtext::reader_t input, xtext::reader_t::cursor_t to) : m_input(input), m_to(to) {}
                Like(xtext::reader_t input, xtext::reader_t::cursor_t from, xtext::reader_t::cursor_t to) : m_input(input), m_from(from), m_to(to) {}
                virtual bool Check(xtext::reader_t&, xtext::reader_t::cursor_t&);
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
            class WhiteSpace : public tokenizer_t
            {
                In m_whitespace;

            public:
                WhiteSpace() : m_whitespace(" \t\n\r") {}
                virtual bool Check(xtext::reader_t&, xtext::reader_t::cursor_t&);
            };
            extern WhiteSpace sWhitespace;

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
            class Is : public tokenizer_t
            {
                uchar32 m_char;

            public:
                Is() : m_char(' ') {}
                Is(uchar c) : m_char(c) {}
                virtual bool Check(xtext::reader_t&, xtext::reader_t::cursor_t&);
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
            class Decimal : public tokenizer_t
            {
            public:
                Decimal() {}
                virtual bool Check(xtext::reader_t&, xtext::reader_t::cursor_t&);
            };
            extern Decimal sDecimal;

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
            class Word : public tokenizer_t
            {
            public:
                Word() {}
                virtual bool Check(xtext::reader_t&, xtext::reader_t::cursor_t&);
            };
            extern Word sWord;

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
            class EndOfText : public tokenizer_t
            {
            public:
                EndOfText() {}
                virtual bool Check(xtext::reader_t&, xtext::reader_t::cursor_t&);
            };
            extern EndOfText sEOT;

            /*!
             * @class EndOfLine
             * @brief	Checks if the character under processing terminates the line
             * 			...it is identical to using Exact("\\r\\n") on Windows and
             *          Is('\\n') on Mac/Linux
             *
             */
            class EndOfLine : public tokenizer_t
            {
            public:
                EndOfLine() {}
                virtual bool Check(xtext::reader_t&, xtext::reader_t::cursor_t&);
            };
            extern EndOfLine sEOL;

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
            class Integer : public tokenizer_t
            {
                s64 m_min;
                s64 m_max;

            public:
                Integer() : m_min(0), m_max(0x7fffffffffffffffL) {}
                Integer(s64 max) : m_min(0), m_max(max) {}
                Integer(s64 min, s64 max) : m_min(min), m_max(max) {}
                virtual bool Check(xtext::reader_t&, xtext::reader_t::cursor_t&);
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
            class Float : public tokenizer_t
            {
                f32 m_min;
                f32 m_max;

            public:
                Float() : m_min(0.0f), m_max(3.402823e+38f) {}
                Float(f32 max) : m_min(0.0f), m_max(max) {}
                Float(f32 min, f32 max) : m_min(min), m_max(max) {}
                virtual bool Check(xtext::reader_t&, xtext::reader_t::cursor_t&);
            };

            // namespace Date

        } // namespace xfilters

        /*!
         * @namespace 	xutils
         * @brief 		more comprehensive matching pattern elements that are
         * used usually in several development parsing/searching tasks
         */
        namespace xutils
        {
            /*!
             * @struct Range
             * @brief	Range
             *
             */
            typedef struct Range
            {
                s32 m_min, m_max;
                Range(s32 min, s32 max) : m_min(min), m_max(max) {}
            } R;

            /*!
             * @class IPv4
             * @brief	check if the token under processing is in IP address V4 format
             * 			(aaa.bbb.ccc.ddd)
             *
             */
            class IPv4 : public tokenizer_t
            {
                xmanipulators::Within   m_d3;
                xfilters::Integer       m_b8;
                xmanipulators::And      m_sub;
                xfilters::Is            m_dot;
                xmanipulators::Sequence m_bad;
                xmanipulators::Times    m_domain;
                xmanipulators::Sequence m_ipv4;

            public:
                IPv4() : m_d3(1, 3, xfilters::sDigit), m_b8(255), m_sub(m_d3, m_b8), m_dot('.'), m_bad(m_sub, m_dot), m_domain(3, m_bad), m_ipv4(m_domain, m_sub) {}
                virtual bool Check(xtext::reader_t&, xtext::reader_t::cursor_t&);
            };
            extern IPv4 sIPv4;

            /*!
             * @class Host
             * @brief	check if the token under processing is a host descriptive
             * 			(server.domain.com) (127.0.0.1)
             *
             */
            class Host : public tokenizer_t
            {
            public:
                virtual bool Check(xtext::reader_t&, xtext::reader_t::cursor_t&);
            };
            extern Host sHost;

            /*!
             * @class Email
             * @brief	check if the token under processing is a email address
             * 			(abc@host.com)
             *
             */
            class Email : public tokenizer_t
            {
            public:
                virtual bool Check(xtext::reader_t&, xtext::reader_t::cursor_t&);
            };
            extern Email sEmail;

            /*!
             * @class Phone
             * @brief	check if the token under processing is a email address
             * 			(+(123)555 443 2 22)
             *
             */
            class Phone : public tokenizer_t
            {
            public:
                virtual bool Check(xtext::reader_t&, xtext::reader_t::cursor_t&);
            };
            extern Phone sPhone;

            /*!
             * @class ServerAddress
             * @brief	check if the token under processing is server plus port
             * 			(host@domain.com:12345)
             *
             */
            class ServerAddress : public tokenizer_t
            {
            public:
                virtual bool Check(xtext::reader_t&, xtext::reader_t::cursor_t&);
            };
            extern ServerAddress sServerAddress;

            /*!
             * @class URI
             * @brief	check if the token under processing is URI address
             * 			(proto:abc@host.domain.com:4444)
             *
             */
            class Uri : public tokenizer_t
            {
            public:
                virtual bool Check(xtext::reader_t&, xtext::reader_t::cursor_t&);
            };
            extern Uri sURI;
        } // namespace xutils
    }     // namespace xparser

    class stringprocessor_t
    {
    private:
        xtext::reader_t           m_string;
        xtext::reader_t::cursor_t m_cursor;
        xtext::reader_t::cursor_t m_lastTokenized;

    public:
        stringprocessor_t();
        stringprocessor_t(xtext::reader_t const& str);
        stringprocessor_t(xtext::reader_t const& str, xtext::reader_t::cursor_t cursor);

        bool            parse(xparser::tokenizer_t&);
        bool            validate(xparser::tokenizer_t&);
        xtext::reader_t search(xparser::tokenizer_t&);
        bool            isEOT();
        void            reset();
    };

} // namespace xcore

#endif