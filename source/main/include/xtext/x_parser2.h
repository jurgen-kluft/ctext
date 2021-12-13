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
    namespace xparser2
    {
        // Minimal parser interface / API
        /*
            e.g.

            xbyte* buffer = alloc->allocate(1024);
            parser_t p(buffer_t(buffer, 1024));

            s32 value;
            va_r_t var(&value);

            p.Sequence();
                p.Until();
                    p.Is('=');
                    p.Extract(&var);
                        p.Unsigned32();
                    p.Pop();
                p.Pop();
            p.Pop();

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
            
            parser_t(buffer_t buffer);

            bool Parse(runes_reader_t&);

            parser_t& Pop();
            parser_t& Extract(va_r_t* var);
            parser_t& Not();
            parser_t& Or();
            parser_t& And();
            parser_t& Sequence();
            parser_t& Within(s32 _min = 0, s32 _max = 0x7fffffff);
            parser_t& Times(s32 _count);
            parser_t& Digest(u32 flags = cWHITESPACE);
            parser_t& OneOrMore();
            parser_t& ZeroOrMore();
            parser_t& ZeroOrOne();
            parser_t& While();
            parser_t& Until();
            parser_t& Enclosed(uchar32 _open, uchar32 _close);
            parser_t& Any();
            parser_t& In(crunes_t const& _chars);
            parser_t& Between(uchar32 _from, uchar32 _until);
            parser_t& Alphabet();
            parser_t& Digit();
            parser_t& Hex();
            parser_t& AlphaNumeric();
            parser_t& Exact(crunes_t const& _text);
            parser_t& Like(crunes_t const& _text);
            parser_t& WhiteSpace();
            parser_t& Is(uchar32 _c);
            parser_t& Word();
            parser_t& EndOfText();
            parser_t& EndOfLine();
            parser_t& Unsigned32(u32 _min = 0, u32 _max = 0xffffffff);
            parser_t& Unsigned64(u64 _min = 0, u64 _max = 0xffffffffffffffffUL);
            parser_t& Integer32(s32 _min = 0, s32 _max = 0x7fffffff);
            parser_t& Integer64(s64 _min = 0, s64 _max = 0x7fffffffffffffffL);
            parser_t& Float32(f32 _min = 0.0f, f32 _max = 3.402823e+38f);
            parser_t& Float64(f64 _min = 0.0, f64 _max = 3.402823e+38f);
            parser_t& Email();
            parser_t& IPv4();
            parser_t& Host();
            parser_t& Date();
            parser_t& Time();
            parser_t& Phone();
            parser_t& ServerAddress();
            parser_t& URI();

            buffer_t m_buffer;
        };

    } // namespace xparser
} // namespace xcore

#endif