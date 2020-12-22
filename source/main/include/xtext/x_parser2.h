#ifndef __XTEXT_XPARSER2_H__
#define __XTEXT_XPARSER2_H__
#include "xbase/x_target.h"
#ifdef USE_PRAGMA_ONCE
#pragma once
#endif

#include "xbase/x_buffer.h"
#include "xbase/x_va_list.h"
#include "xbase/x_runes.h"

namespace xcore
{
    namespace xparser
    {
        // Minimal parser interface / API
        /*
            e.g.

            xbyte* buffer = alloc->allocate(1024);
            parser_t p(buffer_t(buffer, 1024));

            s32 value;
            va_r_t integer(&value);

            p.Sequence();
                p.Until();
                    p.Extract(integer);
                        p.Exact("integer");
                    p.Pop();
                p.Pop();
                p.Separator('=');
                p.Extract(integer);
            p.Pop();

            p.Sequence(
                p.Until(p.Is('='),
                        p.ZeroOrMore(p.Whitespace()).
                        p.Extract(integer,
                            p.Unsigned32()
                        )
                ).
                Separator('=').
                Extract(integer)
            );

            struct
            va_r_t r1c;
            auto rule1 = p.Sequence(
                p.Index(idx).Is(':')->
                Digest(p.cWHITESPACE->
                Or(
                    p.Sequence(
                        p.Is('"')->Extract(r1c, p.Until(p.Is('"'), p.Any()))->
                        p.Digest(p.cWHITESPACE)->
                        p.EOL()
                    ),
                    p.Sequence(
                        p.Digest(p.cWHITESPACE)->
                        p.Until(p.Or(p.Is('|'), p.EOL()),
                            p.Sequence(
                                p.Digest(p.cWHITESPACE)->
                                p.Integer32(lvars)->
                                p.Digest(p.cWHITESPACE)
                            )
                        )->
                        p.Or(
                            p.EOL(),
                            p.Sequence(
                                p.Until(p.EOL(),
                                    p.Sequence(
                                        p.Digest(p.cWHITESPACE)->
                                        p.Integer32(rvars)->
                                        p.Digest(p.cWHITESPACE)
                                    )
                                )
                            )
                        )
                    )
                )
            );

            runes_reader_t text("This is an integer = 512 in text to be parsed");
            bool valid = p.Parse(text);

            CHECK_EQUAL(true, valid);
            CHECK_EQUAL(512, value);

            alloc->deallocate(buffer);
        */

        struct parser_t
        {
            static const u32 cWHITESPACE = 1;
            static const u32 cALPHABET   = 2;
            static const u32 cNUMERIC    = 4;
            static const u32 cIGNORECASE = 8;
            struct code_t
            {
                code_t* Extract(va_r_t const& var, code_t* code_block);
                code_t* Not(code_t* lhs);
                code_t* Or(code_t* lhs, code_t* rhs);
                code_t* And(code_t* lhs, code_t* rhs);
                code_t* Sequence(code_t* lhs);
                code_t* Within(s32 _min = 0, s32 _max = 0x7fffffff);
                code_t* Times(s32 _count);
                code_t* Digest(u32 flags = cWHITESPACE);
                code_t* OneOrMore(code_t* code);
                code_t* ZeroOrMore(code_t* code);
                code_t* ZeroOrOne(code_t* code);
                code_t* While(code_t* code);
                code_t* Until(code_t* until, code_t* code);
                code_t* Enclosed(uchar32 _open, uchar32 _close);
                code_t* Any();
                code_t* In(crunes_t const& _chars);
                code_t* Between(uchar32 _from, uchar32 _until);
                code_t* Alphabet();
                code_t* Digit();
                code_t* Hex();
                code_t* AlphaNumeric();
                code_t* Exact(crunes_t const& _text);
                code_t* Like(crunes_t const& _text);
                code_t* WhiteSpace();
                code_t* Is(uchar32 _c);
                code_t* Word();
                code_t* EndOfText();
                code_t* EndOfLine();
                code_t* Unsigned32(u32 _min = 0, u32 _max = 0xffffffff);
                code_t* Unsigned64(u64 _min = 0, u64 _max = 0xffffffffffffffffUL);
                code_t* Integer32(s32 _min = 0, s32 _max = 0x7fffffff);
                code_t* Integer64(s64 _min = 0, s64 _max = 0x7fffffffffffffffL);
                code_t* Float32(f32 _min = 0.0f, f32 _max = 3.402823e+38f);
                code_t* Float64(f64 _min = 0.0, f64 _max = 3.402823e+38f);
                code_t* Email(va_r_t email_name, va_r_t email_domain);
                code_t* IPv4();
                code_t* Host();
                code_t* Date();
                code_t* Time();
                code_t* Phone();
                code_t* ServerAddress();
                code_t* URI();

            private:
                u16       m_size;   // Code Block starts at 'begin' and ends at 'begin + size'
                u16       m_flags;  // State of this object, OPEN or CLOSED
            };

            parser_t(buffer_t buffer);

            bool Parse(runes_reader_t&);

            code_t* Extract(va_r_t const& var, code_t* code_block);
            code_t* Not(code_t* lhs);
            code_t* Or(code_t* lhs, code_t* rhs);
            code_t* And(code_t* lhs, code_t* rhs);
            code_t* Sequence(code_t* lhs);
            code_t* Within(s32 _min = 0, s32 _max = 0x7fffffff);
            code_t* Times(s32 _count);
            code_t* Digest(u32 flags = cWHITESPACE);
            code_t* OneOrMore(code_t* code);
            code_t* ZeroOrMore(code_t* code);
            code_t* ZeroOrOne(code_t* code);
            code_t* While(code_t* code);
            code_t* Until(code_t* until, code_t* code);
            code_t* Enclosed(uchar32 _open, uchar32 _close);
            code_t* Any();
            code_t* In(crunes_t const& _chars);
            code_t* Between(uchar32 _from, uchar32 _until);
            code_t* Alphabet();
            code_t* Digit();
            code_t* Hex();
            code_t* AlphaNumeric();
            code_t* Exact(crunes_t const& _text);
            code_t* Like(crunes_t const& _text);
            code_t* WhiteSpace();
            code_t* Is(uchar32 _c);
            code_t* Word();
            code_t* EndOfText();
            code_t* EndOfLine();
            code_t* Unsigned32(u32 _min = 0, u32 _max = 0xffffffff);
            code_t* Unsigned64(u64 _min = 0, u64 _max = 0xffffffffffffffffUL);
            code_t* Integer32(s32 _min = 0, s32 _max = 0x7fffffff);
            code_t* Integer64(s64 _min = 0, s64 _max = 0x7fffffffffffffffL);
            code_t* Float32(f32 _min = 0.0f, f32 _max = 3.402823e+38f);
            code_t* Float64(f64 _min = 0.0, f64 _max = 3.402823e+38f);
            code_t* Email(va_r_t email_name, va_r_t email_domain);
            code_t* IPv4();
            code_t* Host();
            code_t* Date();
            code_t* Time();
            code_t* Phone();
            code_t* ServerAddress();
            code_t* URI();

            buffer_t         m_buffer;
            binary_writer_t* m_writer;
            code_t           m_main;
        };

    } // namespace xparser
} // namespace xcore

#endif