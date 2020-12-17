#ifndef __XTEXT_XPARSER2_H__
#define __XTEXT_XPARSER2_H__
#include "xbase/x_target.h"
#ifdef USE_PRAGMA_ONCE
#pragma once
#endif

#include "xbase/x_buffer.h"
#include "xbase/va_list_t.h"
#include "xbase/x_runes.h"

namespace xcore
{
    namespace xparser
    {
        // Minimal parser interface / API
        struct parser_t
        {
            buffer_t m_buffer;

            bool      Parse(runes_reader_t&);
            parser_t& Extract(va_r_t const& var);
            parser_t& Pop();
            parser_t& Not();
            parser_t& Or();
            parser_t& And();
            parser_t& Sequence();
            parser_t& Within(s32 _min = 0, s32 _max = 0x7fffffff);
            parser_t& Times(s32 _count);
            parser_t& OneOrMore();
            parser_t& ZeroOrMore();
            parser_t& ZeroOrOne();
            parser_t& While();
            parser_t& Until();
            parser_t& Enclosed(uchar32 _open, uchar32 _close);
            parser_t& Any();
            parser_t& In(runes_reader_t const& _chars);
            parser_t& Between(uchar32 _from, uchar32 _until);
            parser_t& Alphabet();
            parser_t& Digit();
            parser_t& Hex();
            parser_t& AlphaNumeric();
            parser_t& Exact(runes_reader_t const& _text);
            parser_t& Like(runes_reader_t const& _text);
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
        };

    }
} // namespace xcore

#endif