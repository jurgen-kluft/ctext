#ifndef __CTEXT_XPARSER2_H__
#define __CTEXT_XPARSER2_H__
#include "ccore/c_target.h"
#ifdef USE_PRAGMA_ONCE
#    pragma once
#endif

#include "cbase/c_buffer.h"
#include "cbase/c_va_list.h"
#include "cbase/c_runes.h"

namespace ncore
{
    namespace parser2
    {
        class machine_t;

        class parser_t
        {
        public:
            typedef u16 pc_t;

            static const u8 cWHITESPACE = 1;
            static const u8 cALPHABET   = 2;
            static const u8 cNUMERIC    = 4;
            static const u8 cIGNORECASE = 8;
            static const u8 cLOWERCASE  = 16;
            static const u8 cUPPERCASE  = 32;

            parser_t(buffer_t buffer);

            struct program_t
            {
                program_t();
                program_t(machine_t* m);
                program_t(machine_t* m, pc_t pc);
                program_t(const program_t& p);

                program_t Program(program_t p);
                program_t Not(program_t p);
                program_t Or(program_t lhs, program_t rhs);
                program_t And(program_t lhs, program_t rhs);
                program_t Sequence(program_t p1, program_t p2);
                program_t Within(program_t p, s32 _min = 0, s32 _max = 0x7fffffff);
                program_t Times(program_t p, s32 _count);
                program_t OneOrMore(program_t p);
                program_t ZeroOrMore(program_t p);
                program_t ZeroOrOne(program_t p);
                program_t While(program_t p);
                program_t Until(program_t p);
                program_t Extract(va_r_t* var, program_t p);
                program_t Enclosed(uchar32 _open, uchar32 _close, program_t p);

                program_t Any();
                program_t Digest(u8 flags = cWHITESPACE);
                program_t In(crunes_t const& _chars);
                program_t Between(uchar32 _from, uchar32 _until);
                program_t Alphabet();
                program_t Digit();
                program_t Hex();
                program_t AlphaNumeric();
                program_t Exact(crunes_t const& _text);
                program_t Like(crunes_t const& _text);
                program_t WhiteSpace();
                program_t Is(uchar32 _c);
                program_t Word();
                program_t EndOfText();
                program_t EndOfLine();
                program_t Unsigned32(u32 _min = 0, u32 _max = 0xffffffff);
                program_t Unsigned64(u64 _min = 0, u64 _max = 0xffffffffffffffffUL);
                program_t Integer32(s32 _min = 0, s32 _max = 0x7fffffff);
                program_t Integer64(s64 _min = 0, s64 _max = 0x7fffffffffffffffL);
                program_t Float32(f32 _min = 0.0f, f32 _max = 3.402823e+38f);
                program_t Float64(f64 _min = 0.0, f64 _max = 3.402823e+38f);

            protected:
                friend class machine_t;
                friend class parser_t;
                inline pc_t pc() const { return (pc_t)m_pc; }

            private:
                machine_t* m_machine;
                u32        m_pc;
            };

            static bool parse(program_t program, nrunes::reader_t& reader);

            program_t Program(program_t p);
            program_t Not(program_t p);
            program_t Or(program_t lhs, program_t rhs);
            program_t And(program_t lhs, program_t rhs);
            program_t Sequence(program_t p1, program_t p2);
            program_t Sequence(program_t p1, program_t p2, program_t p3);
            program_t Sequence(program_t p1, program_t p2, program_t p3, program_t p4);
            program_t Within(program_t p, s32 _min = 0, s32 _max = 0x7fffffff);
            program_t Times(s32 _count, program_t p);
            program_t OneOrMore(program_t p);
            program_t ZeroOrMore(program_t p);
            program_t ZeroOrOne(program_t p);
            program_t While(program_t p);
            program_t Until(program_t p);
            program_t Extract(va_r_t* var, program_t p);
            program_t Enclosed(uchar32 _open, uchar32 _close, program_t p);

            program_t Any();
            program_t Digest(u8 flags = cWHITESPACE);
            program_t In(crunes_t const& _chars);
            program_t Between(uchar32 _from, uchar32 _until);
            program_t Alphabet();
            program_t Digit();
            program_t Hex();
            program_t AlphaNumeric();
            program_t Exact(crunes_t const& _text);
            program_t Like(crunes_t const& _text);
            program_t WhiteSpace();
            program_t Is(uchar32 _c);
            program_t Word();
            program_t EndOfText();
            program_t EndOfLine();
            program_t Unsigned32(u32 _min = 0, u32 _max = 0xffffffff);
            program_t Unsigned64(u64 _min = 0, u64 _max = 0xffffffffffffffffUL);
            program_t Integer32(s32 _min = 0, s32 _max = 0x7fffffff);
            program_t Integer64(s64 _min = 0, s64 _max = 0x7fffffffffffffffL);
            program_t Float32(f32 _min = 0.0f, f32 _max = 3.402823e+38f);
            program_t Float64(f64 _min = 0.0, f64 _max = 3.402823e+38f);
            program_t Email();
            program_t IPv4();
            program_t Host();

        private:
            machine_t* m_machine;
            buffer_t   m_buffer;
        };

    } // namespace parser2
} // namespace ncore

#endif
