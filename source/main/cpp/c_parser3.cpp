#include "ccore/c_target.h"
#include "cbase/c_buffer.h"
#include "cbase/c_allocator.h"
#include "ccore/c_debug.h"
#include "cbase/c_va_list.h"
#include "cbase/c_runes.h"

#include "ctext/c_parser3.h"

namespace ncore
{
    namespace parser3
    {
        // Call parser_t when starting a new code block.
        // You will not be able to write any additional commands to this block when it has been
        // handed to another code block as input. From that moment on this code-block will be locked.
        parser_t::code_t* parser_t::code_t::Extract(va_r_t* var, code_t* lhs) { return this; }
        parser_t::code_t* parser_t::code_t::Not(code_t* lhs) { return this; }
        parser_t::code_t* parser_t::code_t::Or(code_t* lhs, code_t* rhs) { return this; }
        parser_t::code_t* parser_t::code_t::And(code_t* lhs, code_t* rhs) { return this; }
        parser_t::code_t* parser_t::code_t::Sequence(code_t* lhs) { return this; }
        parser_t::code_t* parser_t::code_t::Within(s32 _min, s32 _max) { return this; }
        parser_t::code_t* parser_t::code_t::Times(s32 _count) { return this; }
        parser_t::code_t* parser_t::code_t::OneOrMore(code_t* code) { return this; }
        parser_t::code_t* parser_t::code_t::ZeroOrMore(code_t* code) { return this; }
        parser_t::code_t* parser_t::code_t::ZeroOrOne(code_t* code) { return this; }
        parser_t::code_t* parser_t::code_t::While(code_t* code) { return this; }
        parser_t::code_t* parser_t::code_t::Until(code_t* until, code_t* code) { return this; }
        parser_t::code_t* parser_t::code_t::Enclosed(uchar32 _open, uchar32 _close) { return this; }
        parser_t::code_t* parser_t::code_t::Any() { return this; }
        parser_t::code_t* parser_t::code_t::In(crunes_t const& _chars) { return this; }
        parser_t::code_t* parser_t::code_t::Between(uchar32 _from, uchar32 _until) { return this; }
        parser_t::code_t* parser_t::code_t::Alphabet() { return this; }
        parser_t::code_t* parser_t::code_t::Digit() { return this; }
        parser_t::code_t* parser_t::code_t::Hex() { return this; }
        parser_t::code_t* parser_t::code_t::AlphaNumeric() { return this; }
        parser_t::code_t* parser_t::code_t::Exact(crunes_t const& _text) { return this; }
        parser_t::code_t* parser_t::code_t::Like(crunes_t const& _text) { return this; }
        parser_t::code_t* parser_t::code_t::WhiteSpace(u8 flags) { return this; }
        parser_t::code_t* parser_t::code_t::Is(uchar32 _c) { return this; }
        parser_t::code_t* parser_t::code_t::Word() { return this; }
        parser_t::code_t* parser_t::code_t::EndOfText() { return this; }
        parser_t::code_t* parser_t::code_t::EndOfLine() { return this; }
        parser_t::code_t* parser_t::code_t::EOL() { return this; }
        parser_t::code_t* parser_t::code_t::Unsigned32(u32 _min, u32 _max) { return this; }
        parser_t::code_t* parser_t::code_t::Unsigned64(u64 _min, u64 _max) { return this; }
        parser_t::code_t* parser_t::code_t::Integer32(s32 _min, s32 _max) { return this; }
        parser_t::code_t* parser_t::code_t::Integer64(s64 _min, s64 _max) { return this; }
        parser_t::code_t* parser_t::code_t::Float32(f32 _min, f32 _max) { return this; }
        parser_t::code_t* parser_t::code_t::Float64(f64 _min, f64 _max) { return this; }
        parser_t::code_t* parser_t::code_t::Email() { return this; }
        parser_t::code_t* parser_t::code_t::IPv4() { return this; }
        parser_t::code_t* parser_t::code_t::Host() { return this; }
        parser_t::code_t* parser_t::code_t::Date() { return this; }
        parser_t::code_t* parser_t::code_t::Time() { return this; }
        parser_t::code_t* parser_t::code_t::Phone() { return this; }
        parser_t::code_t* parser_t::code_t::ServerAddress() { return this; }
        parser_t::code_t* parser_t::code_t::URI() { return this; }

        parser_t::parser_t(buffer_t buffer) {}

        bool parser_t::Parse(runes_reader_t&) { return false; }

        static parser_t::code_t sNullCode;

        parser_t::code_t* parser_t::Extract(va_r_t* var, code_t* lhs) { return &sNullCode; }
        parser_t::code_t* parser_t::Not(code_t* lhs) { return &sNullCode; }
        parser_t::code_t* parser_t::Or(code_t* lhs, code_t* rhs) { return &sNullCode; }
        parser_t::code_t* parser_t::And(code_t* lhs, code_t* rhs) { return &sNullCode; }
        parser_t::code_t* parser_t::Sequence(code_t* lhs) { return &sNullCode; }
        parser_t::code_t* parser_t::Within(s32 _min, s32 _max) { return &sNullCode; }
        parser_t::code_t* parser_t::Times(s32 _count) { return &sNullCode; }
        parser_t::code_t* parser_t::OneOrMore(code_t* code) { return &sNullCode; }
        parser_t::code_t* parser_t::ZeroOrMore(code_t* code) { return &sNullCode; }
        parser_t::code_t* parser_t::ZeroOrOne(code_t* code) { return &sNullCode; }
        parser_t::code_t* parser_t::While(code_t* code) { return &sNullCode; }
        parser_t::code_t* parser_t::Until(code_t* until, code_t* code) { return &sNullCode; }
        parser_t::code_t* parser_t::Enclosed(uchar32 _open, uchar32 _close) { return &sNullCode; }
        parser_t::code_t* parser_t::Any() { return &sNullCode; }
        parser_t::code_t* parser_t::In(crunes_t const& _chars) { return &sNullCode; }
        parser_t::code_t* parser_t::Between(uchar32 _from, uchar32 _until) { return &sNullCode; }
        parser_t::code_t* parser_t::Alphabet() { return &sNullCode; }
        parser_t::code_t* parser_t::Digit() { return &sNullCode; }
        parser_t::code_t* parser_t::Hex() { return &sNullCode; }
        parser_t::code_t* parser_t::AlphaNumeric() { return &sNullCode; }
        parser_t::code_t* parser_t::Exact(crunes_t const& _text) { return &sNullCode; }
        parser_t::code_t* parser_t::Like(crunes_t const& _text) { return &sNullCode; }
        parser_t::code_t* parser_t::WhiteSpace(u8 flags) { return &sNullCode; }
        parser_t::code_t* parser_t::Is(uchar32 _c) { return &sNullCode; }
        parser_t::code_t* parser_t::Word() { return &sNullCode; }
        parser_t::code_t* parser_t::EndOfText() { return &sNullCode; }
        parser_t::code_t* parser_t::EndOfLine() { return &sNullCode; }
        parser_t::code_t* parser_t::EOL() { return &sNullCode; }
        parser_t::code_t* parser_t::Unsigned32(u32 _min, u32 _max) { return &sNullCode; }
        parser_t::code_t* parser_t::Unsigned64(u64 _min, u64 _max) { return &sNullCode; }
        parser_t::code_t* parser_t::Integer32(s32 _min, s32 _max) { return &sNullCode; }
        parser_t::code_t* parser_t::Integer64(s64 _min, s64 _max) { return &sNullCode; }
        parser_t::code_t* parser_t::Float32(f32 _min, f32 _max) { return &sNullCode; }
        parser_t::code_t* parser_t::Float64(f64 _min, f64 _max) { return &sNullCode; }
        parser_t::code_t* parser_t::Email() { return &sNullCode; }
        parser_t::code_t* parser_t::IPv4() { return &sNullCode; }
        parser_t::code_t* parser_t::Host() { return &sNullCode; }
        parser_t::code_t* parser_t::Date() { return &sNullCode; }
        parser_t::code_t* parser_t::Time() { return &sNullCode; }
        parser_t::code_t* parser_t::Phone() { return &sNullCode; }
        parser_t::code_t* parser_t::ServerAddress() { return &sNullCode; }
        parser_t::code_t* parser_t::URI() { return &sNullCode; }

        static void use_case_1()
        {
            alloc_t* alloc = context_t::heap_alloc();
            u8* data = (u8*)alloc->allocate(1024);
            buffer_t buffer(1024, data);
            parser_t p(buffer);

            s32 idx;
            s32 lints[8];
            s32 rints[8];
            va_r_t index(&idx);
            va_r_t lvars((s32*)lints, 8);
            va_r_t rvars((s32*)rints, 8);
            va_r_t r1c;
            auto rule1 = p.Sequence(
                p.Extract(&index, p.Integer32())->Is(':')->
                WhiteSpace(cZeroOrMore)->
                Or(
                    p.Sequence(
                        p.Is('"')->Extract(&r1c, p.Until(p.Is('"'), p.Any()))->
                        WhiteSpace(cZeroOrMore)->
                        EOL()
                    ),
                    p.Sequence(
                        p.WhiteSpace(cZeroOrMore)->
                        Until(p.Or(p.Is('|'), p.EOL()),
                            p.Sequence(
                                p.WhiteSpace(cZeroOrMore)->
                                Extract(&lvars, p.Integer32())->
                                WhiteSpace(cZeroOrMore)
                            )
                        )->
                        Or(
                            p.EOL(),
                            p.Sequence(
                                p.Until(p.EOL(),
                                    p.Sequence(
                                        p.WhiteSpace(cZeroOrMore)->
                                        Extract(&rvars, p.Integer32())->
                                        WhiteSpace(cZeroOrMore)
                                    )
                                )
                            )
                        )
                    )
                )
            );
        }
    } // namespace parser3
} // namespace ncore

/*

1   000   OP_INDEX
18  000   +va_r_t
1   001   OP_IS
4   000   +uchar32 = ':'
1   002   OP_DIGEST
1   000   +u16
3   000   ->  __ = JMP 025
1   003   OP_IS
4   000   +uchar32 = '"'
3   000   ->  __ = JMP 007
1   004   OP_IS
4   000   +uchar32 = '"'
1   000   ->  __ = RET
1   005   OP_ANY
1   000   ->  __ = RET
1   006   OP_UNTIL
2   000   UNTIL = JSR 004
2   000   EXEC  = JSR 005
1   000   ->  __ = RET
1   007   OP_EXTRACT
18  000   +va_r_t
2   000   UNTIL = JSR 006
1   008   OP_DIGEST
1   000   +u16
1   009   OP_EOL
1   000   ->  __ = RET
1   010   OP_DIGEST
1   000   +u16
3   000   ->  __ = JMP 017
1   011   OP_IS
4   000   +uchar32 = '|'
1   000   ->  __ = RET
1   012   OP_EOL
1   000   ->  __ = RET
1   013   OP_OR
2   000   LHS = JSR 011
2   000   RHS = JSR 012
1   000   ->  __ = RET
1   014   OP_DIGEST
1   000   +u16
1   015   OP_U32
1   000   +u16
18  000   +va_r_t
1   016   OP_DIGEST
1   000   +u16
1   000   ->  __ = RET
1   017   OP_UNTIL
2   000   UNTIL = JSR 013
2   000   EXEC  = JSR 014
3   000   ->  __ = JMP 024
1   018   OP_EOL
1   000   ->  __ = RET
1   019   OP_EOL
1   000   ->  __ = RET
1   020   OP_DIGEST
1   000   +u16
1   021   OP_U32
1   000   +u16
18  000   +va_r_t
1   022   OP_DIGEST
1   000   +u16
1   000   ->  __ = RET
1   023   OP_UNTIL
2   000   UNTIL = JSR 019
2   000   EXEC  = JSR 020
1   000   ->  __ = RET
1   024   OP_OR
2   000   LHS = JSR 018
2   000   RHS = JSR 023
1   000   ->  __ = RET
1   025   OP_OR
2   000   LHS = JSR 003
2   000   RHS = JSR 010
1   000   RTS

144 bytes


*/