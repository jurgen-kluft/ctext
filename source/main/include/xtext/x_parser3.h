#ifndef __XTEXT_XPARSER3_H__
#define __XTEXT_XPARSER3_H__
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

            
            auto rule2 = p.Sequence(
                p.Digest(p.cWHITESPACE)->
                p.Until(p.Is('='), p.Any())->
                p.Digest(p.cWHITESPACE)->
                p.Extract(&var,
                    p.Unsigned32()
                )
            );

            runes_reader_t text("This is an integer = 512 in text to be parsed");
            bool valid = p.Parse(text);

            CHECK_EQUAL(true, valid);
            CHECK_EQUAL(512, value);

            alloc->deallocate(buffer);
        */
        static const u32 cWHITESPACE = 0x01;
        static const u32 cALPHABET   = 0x02;
        static const u32 cNUMERIC    = 0x04;
        static const u32 cIGNORECASE = 0x08;
        static const u32 cZeroOrMore = 0x10;

        struct parser_t
        {
            // Call parser_t when starting a new code block. 
            // You will not be able to write any additional commands to this block when it has been
            // handed to another code block as input. From that moment on this code-block will be locked.
            struct code_t
            {
                code_t* Extract(va_r_t * var, code_t* lhs);
                code_t* Not(code_t* lhs);
                code_t* Or(code_t* lhs, code_t* rhs);
                code_t* And(code_t* lhs, code_t* rhs);
                code_t* Sequence(code_t* lhs);
                code_t* Within(s32 _min = 0, s32 _max = 0x7fffffff);
                code_t* Times(s32 _count);
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
                code_t* WhiteSpace(u8 flags=cZeroOrMore);
                code_t* Is(uchar32 _c);
                code_t* Word();
                code_t* EndOfText();
                code_t* EndOfLine();
                code_t* EOL();
                code_t* Unsigned32(u32 _min = 0, u32 _max = 0xffffffff);
                code_t* Unsigned64(u64 _min = 0, u64 _max = 0xffffffffffffffffUL);
                code_t* Integer32(s32 _min = 0, s32 _max = 0x7fffffff);
                code_t* Integer64(s64 _min = 0, s64 _max = 0x7fffffffffffffffL);
                code_t* Float32(f32 _min = 0.0f, f32 _max = 3.402823e+38f);
                code_t* Float64(f64 _min = 0.0, f64 _max = 3.402823e+38f);
                code_t* Email();
                code_t* IPv4();
                code_t* Host();
                code_t* Date();
                code_t* Time();
                code_t* Phone();
                code_t* ServerAddress();
                code_t* URI();
            };

            parser_t(buffer_t buffer);

            bool Parse(runes_reader_t&);

            code_t* Extract(va_r_t * var, code_t* lhs);
            code_t* Not(code_t* lhs);
            code_t* Or(code_t* lhs, code_t* rhs);
            code_t* And(code_t* lhs, code_t* rhs);
            code_t* Sequence(code_t* lhs);
            code_t* Within(s32 _min = 0, s32 _max = 0x7fffffff);
            code_t* Times(s32 _count);
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
            code_t* WhiteSpace(u8 flags=cZeroOrMore);
            code_t* Is(uchar32 _c);
            code_t* Word();
            code_t* EndOfText();
            code_t* EndOfLine();
            code_t* EOL();
            code_t* Unsigned32(u32 _min = 0, u32 _max = 0xffffffff);
            code_t* Unsigned64(u64 _min = 0, u64 _max = 0xffffffffffffffffUL);
            code_t* Integer32(s32 _min = 0, s32 _max = 0x7fffffff);
            code_t* Integer64(s64 _min = 0, s64 _max = 0x7fffffffffffffffL);
            code_t* Float32(f32 _min = 0.0f, f32 _max = 3.402823e+38f);
            code_t* Float64(f64 _min = 0.0, f64 _max = 3.402823e+38f);
            code_t* Email();
            code_t* IPv4();
            code_t* Host();
            code_t* Date();
            code_t* Time();
            code_t* Phone();
            code_t* ServerAddress();
            code_t* URI();

            buffer_t m_buffer;
        };

    } // namespace xparser
} // namespace xcore

#endif