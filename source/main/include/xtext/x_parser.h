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
    // Even for console, we can pass this charreader_t and literally use
    // any string format, from ascii, utf16 ....
    //

    namespace xtext
    {
        class reader_t;
        class reader_t::cursor_t;

        class writer_t
        {
        public:
            struct cursor_t
            {
                union runeptr
                {
                    inline runeptr() : _ascii(nullptr) {}
                    ascii::prune m_ascii;
                    utf8::prune  m_utf8;
                    utf16::prune m_utf16;
                    utf32::prune m_utf32;
                };

                cursor_t& operator=(const cursor_t&);
                bool operator<(const cursor_t&) const;
                bool operator>(const cursor_t&) const;
                bool operator<=(const cursor_t&) const;
                bool operator>=(const cursor_t&) const;
                bool operator==(const cursor_t&) const;
                bool operator!=(const cursor_t&) const;

                runeptr m_cursor;
            };

            writer_t();
            writer_t(char* str, char* end);
            writer_t(utf32::prune str, utf32::prune end);
            writer_t(ascii::runes const& str);
            writer_t(utf8::runes const& str);
            writer_t(utf16::runes const& str);
            writer_t(utf32::runes const& str);
            writer_t(const writer_t&);

            cursor_t get_cursor() const;

            void reset(cursor_t&) const;
            bool valid(cursor_t&) const;
            void write(cursor_t&, uchar32 c);
            void write(reader_t const& c, reader_t::cursor_t&);

        private:
            union runes
            {
                inline runes() : _ascii() {}
                ascii::runes _ascii;
                utf8::runes  _utf8;
                utf16::runes _utf16;
                utf32::runes _utf32;
            };
            runes m_runes;
            s32   m_type;
        };

        class reader_t
        {
        public:
            struct cursor_t
            {
                union runeptr
                {
                    inline runeptr() : _ascii(nullptr) {}
                    ascii::pcrune m_ascii;
                    utf8::pcrune  m_utf8;
                    utf16::pcrune m_utf16;
                    utf32::pcrune m_utf32;
                };

                cursor_t& operator=(const cursor_t&);
                bool    operator<(const cursor_t&) const;
                bool    operator>(const cursor_t&) const;
                bool    operator<=(const cursor_t&) const;
                bool    operator>=(const cursor_t&) const;
                bool    operator==(const cursor_t&) const;
                bool    operator!=(const cursor_t&) const;

                runeptr m_cursor;
            };

            reader_t();
            reader_t(const char* str);
            reader_t(utf32::pcrune str);
            reader_t(ascii::crunes const& str);
            reader_t(utf8::crunes const& str);
            reader_t(utf16::crunes const& str);
            reader_t(utf32::crunes const& str);
            reader_t(const writer_t&);
            reader_t(const reader_t&);

            cursor_t get_cursor() const;

            s64     size() const;
            s64     size(cursor_t const&) const;
            void    reset(cursor_t const&);
            bool    valid(cursor_t const&) const;
            uchar32 peek(cursor_t const&) const;
            uchar32 read(cursor_t const&);
            void    skip(cursor_t const&);
            void    select(cursor_t const& from, cursor_t const& until);

        private:
            union crunes
            {
                inline crunes() : _ascii() {}
                ascii::crunes _ascii;
                utf8::crunes  _utf8;
                utf16::crunes _utf16;
                utf32::crunes _utf32;
            };
            crunes m_runes;
            s32    m_type;
        };
    }

    namespace parser
    {
        class tokenizer
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

        class tokenizer_1 : public tokenizer
        {
            tokenizer& m_tokenizer_a;

        public:
            tokenizer_1(tokenizer& toka) : m_tokenizer_a(toka) {}
        };
        class tokenizer_2 : public tokenizer_1
        {
            tokenizer& m_tokenizer_b;

        public:
            tokenizer_1(tokenizer& toka, tokenizer& tokb) : tokenizer_1(toka), m_tokenizer_b(tokb) {}
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
         * @namespace 	manipulators
         * @brief 		the namespace that contains controlling lexical and
         * relational patterns parsing/recognition elements
         * @see 		filters, filters::utils
         */
        namespace manipulators
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
            class Not : public tokenizer_1
            {
            public:
                inline Not(tokenizer& toka) : tokenizer_1(toka) {}
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
            class Or : public tokenizer_2
            {
            public:
                inline Or(tokenizer& toka, tokenizer& tokb) : tokenizer_2(toka, tokb) {}
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
            class And : public tokenizer_2
            {
            public:
                inline And(tokenizer& toka, tokenizer& tokb) : tokenizer_2(toka, tokb) {}
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
            class Sequence : public tokenizer_2
            {
            public:
                inline Sequence(tokenizer& toka, tokenizer& tokb) : tokenizer_2(toka, tokb) {}
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
             * @see 	Times, While, Until, OneOrMore, ZeroOrOne, filters::Exact
             * 		, Sequence
             */
            class Within : public tokenizer_1
            {
                u64 m_min, m_max;

            public:
                inline Within(u64 min, u64 max, tokenizer& toka) : tokenizer_1(toka), m_min(min), m_max(max) {}
                inline Within(u64 max, tokenizer& toka) : tokenizer_1(toka), m_min(0), m_max(max) {}
                inline Within(tokenizer& toka) : tokenizer_1(toka), m_min(0), m_max(0xffffffffffffffffUL) {}
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
             * @see 	 While, Until, OneOrMore, ZeroOrOne, filters::Exact, Sequence
             */
            class Times : public tokenizer_1
            {
                s32 m_max;

            public:
                inline Times(s32 max, tokenizer& toka) : tokenizer_1(toka), m_max(max) {}
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
             * @see 	Times, While, Until, ZeroOrOne, filters::Exact, Sequence
             */
            class OneOrMore : public tokenizer_1
            {
            public:
                inline OneOrMore(tokenizer& toka) : tokenizer_1(toka) {}
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
             * @see 	Times, While, Until, ZeroOrOne, filters::Exact
             * 		, Sequence
             */
            class ZeroOrOne : public tokenizer_1
            {
            public:
                inline ZeroOrOne(tokenizer& toka) : tokenizer_1(toka) {}
                virtual bool Check(xtext::reader_t&, xtext::reader_t::cursor_t&);
            };
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
             * @see 	Times, While, Until, ZeroOrOne, filters::Exact
             * 		, Sequence
             */
            class While : public tokenizer_1
            {
            public:
                inline While(tokenizer& toka) : tokenizer_1(toka) {}
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
             * @see 	Times, While, Until, ZeroOrOne, filters::Exact
             * 		, Sequence
             */
            class Until : public tokenizer_1
            {
            public:
                inline Until(tokenizer& toka) : tokenizer_1(toka) {}
                virtual bool Check(xtext::reader_t&, xtext::reader_t::cursor_t&);
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
            class Extract : public tokenizer_1
            {
                stringreader_t& _Selection;

            public:
                inline Extract(stringreader_t& m1, tokenizer& toka) : tokenizer_1(toka), _Selection(m1) {}
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
            typedef void (*CallBack)(stringreader_t&);
            class ReturnToCallback : public tokenizer_1
            {
                CallBack m_cb;

            public:
                inline ReturnToCallback(Callback cb, tokenizer& toka) : tokenizer_1(toka), m_cb(cb) {}
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
            class Enclosed : public tokenizer_1
            {
                stringreader_t m_open;
                stringreader_t m_close;

            public:
                inline Enclosed(tokenizer& toka) : tokenizer_1(toka), m_cb(cb) {}
                virtual bool Check(xtext::reader_t&, xtext::reader_t::cursor_t&);
            };

        } // namespace manipulators

        namespace filters
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
            class Any : public interface
            {
            public:
                Any() {}
                virtual bool Check(xtext::reader_t&, xtext::reader_t::cursor_t&);
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
            class In : public interface
            {
                stringreader_t m_input;

            public:
                In() {}
                In(stringreader_t input) : m_input(input) {}
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
            class Between : public interface
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
            class Alphabet : public interface
            {
            public:
                Alphabet() {}
                virtual bool Check(xtext::reader_t&, xtext::reader_t::cursor_t&);
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
            class Digit : public interface
            {
            public:
                Digit() {}
                virtual bool Check(xtext::reader_t&, xtext::reader_t::cursor_t&);
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
            class Hex : public interface
            {
            public:
                Hex() {}
                virtual bool Check(xtext::reader_t&, xtext::reader_t::cursor_t&);
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
            class AlphaNumeric : public interface
            {
            public:
                Hex() {}
                virtual bool Check(xtext::reader_t&, xtext::reader_t::cursor_t&);
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
            class Exact : public interface
            {
                stringreader_t m_input;

            public:
                Exact() {}
                Exact(stringreader_t input) : m_input(input) {}
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
            class Like : public interface
            {
                stringreader_t m_input;

            public:
                Like() {}
                Like(stringreader_t input) : m_input(input) {}
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
            class WhiteSpace : public interface
            {
            public:
                WhiteSpace() {}
                virtual bool Check(xtext::reader_t&, xtext::reader_t::cursor_t&);
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
            class Is : public interface
            {
                uchar32 m_letter;

            public:
                Is() : m_letter(' ') {}
                Is(uchar letter) : m_letter(letter) {}
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
            class Decimal : public interface
            {
            public:
                Decimal() {}
                virtual bool Check(xtext::reader_t&, xtext::reader_t::cursor_t&);
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
            class Word : public interface
            {
            public:
                Word() {}
                virtual bool Check(xtext::reader_t&, xtext::reader_t::cursor_t&);
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
            class EndOfText : public interface
            {
            public:
                EndOfText() {}
                virtual bool Check(xtext::reader_t&, xtext::reader_t::cursor_t&);
            };
            extern EndOfText EOT;

            /*!
             * @class EndOfLine
             * @brief	Checks if the character under processing terminates the line
             * 			...it is identical to using Exact("\\r\\n") on Windows and
             *          Is('\\n') on Mac/Linux
             *
             */
            class EndOfLine : public interface
            {
            public:
                EndOfLine() {}
                virtual bool Check(xtext::reader_t&, xtext::reader_t::cursor_t&);
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
            class Integer : public interface
            {
                s32 m_min;
                s32 m_max;

            public:
                Integer() : m_min(0), max(0x7fffffff) {}
                Integer(s32 max) : m_min(0), max(max) {}
                Integer(s32 min, s32 max) : m_min(min), max(max) {}
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
            class Float : public interface
            {
                f32 m_min;
                f32 m_max;

            public:
                Float() : m_min(0.0f), max(3.402823e+38f) {}
                Float(f32 max) : m_min(0.0f), max(max) {}
                Float(f32 min, f32 max) : m_min(min), max(max) {}
                virtual bool Check(xtext::reader_t&, xtext::reader_t::cursor_t&);
            };

            // namespace Date

        } // namespace filters

        /*!
         * @namespace 	utils
         * @brief 		more comprehensive matching pattern elements that are
         * used usually in several development parsing/searching tasks
         */
        namespace utils
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
            class IPv4 : public interface
            {
            public:
                virtual bool Check(xtext::reader_t&, xtext::reader_t::cursor_t&);
            };
            extern IPv4 IPV4;

            /*!
             * @class Host
             * @brief	check if the token under processing is a host descriptive
             * 			(server.domain.com) (127.0.0.1)
             *
             */
            class Host : public interface
            {
            public:
                virtual bool Check(xtext::reader_t&, xtext::reader_t::cursor_t&);
            };
            extern Host HOST;

            /*!
             * @class Email
             * @brief	check if the token under processing is a email address
             * 			(abc@host.com)
             *
             */
            class Email : public interface
            {
            public:
                virtual bool Check(xtext::reader_t&, xtext::reader_t::cursor_t&);
            };
            extern Email EMAIL;

            /*!
             * @class Phone
             * @brief	check if the token under processing is a email address
             * 			(+(123)555 443 2 22)
             *
             */
            class Phone : public interface
            {
            public:
                virtual bool Check(xtext::reader_t&, xtext::reader_t::cursor_t&);
            };
            extern Phone PHONE;

            /*!
             * @class ServerAddress
             * @brief	check if the token under processing is server plus port
             * 			(host@domain.com:12345)
             *
             */
            class ServerAddress : public interface
            {
            public:
                virtual bool Check(xtext::reader_t&, xtext::reader_t::cursor_t&);
            };
            extern ServerAddress SERVERADDRESS;

            /*!
             * @class URI
             * @brief	check if the token under processing is URI address
             * 			(proto:abc@host.domain.com:4444)
             *
             */
            class Uri : public interface
            {
            public:
                virtual bool Check(xtext::reader_t&, xtext::reader_t::cursor_t&);
            };
            extern Uri URI;

        } // namespace utils

    } // namespace parser

    class stringprocessor
    {
    private:
        stringreader_t m_string;
        stringreader_t m_cursor;
        stringreader_t m_lastTokenized;

    public:
        stringprocessor();
        stringprocessor(stringreader_t const& str);

        bool         Parse(tokenizer::tokenizer&);
        bool         Validate(tokenizer::tokenizer&);
        stringreader_t Search(tokenizer::tokenizer&);
        bool         IsEOT();
        void         Reset();
    };

    inline xcore::tokenizer::manipulators::Times  operator*(xcore::tokenizer::tokenizer& tok, s32 times) { return xcore::tokenizer::manipulators::Times(times, tok); }
    inline xcore::tokenizer::manipulators::Times  operator*(s32 times, xcore::tokenizer::tokenizer& tok) { return xcore::tokenizer::manipulators::Times(times, tok); }
    inline xcore::tokenizer::manipulators::Within operator*(xcore::tokenizer::tokenizer& tok, const xcore::tokenizer::utils::Range& range)
    {
        return xcore::tokenizer::manipulators::Within(range._Min, range._Max, tok);
    }
    inline xcore::tokenizer::manipulators::Within operator*(xcore::tokenizer::utils::Range& range, xcore::tokenizer::tokenizer& tok)
    {
        return xcore::tokenizer::manipulators::Within(range._Min, range._Max, tok);
    }

    inline xcore::tokenizer::manipulators::Or       operator|(xcore::tokenizer::tokenizer& t1, xcore::tokenizer::tokenizer& t2) { return xcore::tokenizer::manipulators::Or(t1, t2); }
    inline xcore::tokenizer::manipulators::And      operator&(xcore::tokenizer::tokenizer& t1, xcore::tokenizer::tokenizer& t2) { return xcore::tokenizer::manipulators::And(t1, t2); }
    inline xcore::tokenizer::manipulators::Sequence operator+(xcore::tokenizer::tokenizer& t1, xcore::tokenizer::tokenizer& t2) { return xcore::tokenizer::manipulators::Sequence(t1, t2); }
    inline xcore::tokenizer::manipulators::Not      operator!(xcore::tokenizer::tokenizer& tok) { return xcore::tokenizer::manipulators::Not(tok); }

} // namespace xcore

#endif