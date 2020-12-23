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

            void Pop();
            void Extract(va_r_t* var);
            void Not();
            void Or();
            void And();
            void Sequence();
            void Within(s32 _min = 0, s32 _max = 0x7fffffff);
            void Times(s32 _count);
            void Digest(u32 flags = cWHITESPACE);
            void OneOrMore();
            void ZeroOrMore();
            void ZeroOrOne();
            void While();
            void Until();
            void Enclosed(uchar32 _open, uchar32 _close);
            void Any();
            void In(crunes_t const& _chars);
            void Between(uchar32 _from, uchar32 _until);
            void Alphabet();
            void Digit();
            void Hex();
            void AlphaNumeric();
            void Exact(crunes_t const& _text);
            void Like(crunes_t const& _text);
            void WhiteSpace();
            void Is(uchar32 _c);
            void Word();
            void EndOfText();
            void EndOfLine();
            void Unsigned32(u32 _min = 0, u32 _max = 0xffffffff);
            void Unsigned64(u64 _min = 0, u64 _max = 0xffffffffffffffffUL);
            void Integer32(s32 _min = 0, s32 _max = 0x7fffffff);
            void Integer64(s64 _min = 0, s64 _max = 0x7fffffffffffffffL);
            void Float32(f32 _min = 0.0f, f32 _max = 3.402823e+38f);
            void Float64(f64 _min = 0.0, f64 _max = 3.402823e+38f);
            void Email();
            void IPv4();
            void Host();
            void Date();
            void Time();
            void Phone();
            void ServerAddress();
            void URI();

            buffer_t m_buffer;
        };

    } // namespace xparser
} // namespace xcore

#endif