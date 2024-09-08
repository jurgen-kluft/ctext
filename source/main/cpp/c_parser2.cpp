#include "ccore/c_debug.h"
#include "ccore/c_target.h"
#include "cbase/c_buffer.h"
#include "cbase/c_va_list.h"
#include "cbase/c_runes.h"

#include "ctext/c_parser2.h"

namespace ncore
{
    namespace parser2
    {
        enum eOpcode
        {
            eNOP = 0,
            // Manipulators (scope)
            eNot,
            eOr,
            eAnd,
            eSequence,
            eWithin,
            eTimes,
            eOneOrMore,
            eZeroOrMore,
            eZeroOrOne,
            eWhile,
            eUntil,
            eExtract,
            eEnclosed,
            // Filters
            eAny = 0x80,
            eDigest,
            eIn,
            eBetween,
            eAlphabet,
            eDigit,
            eHex,
            eAlphaNumeric,
            eExact,
            eLike,
            eWhiteSpace,
            eIs,
            eDecimal,
            eWord,
            eEndOfText,
            eEndOfLine,
            eUnsigned32,
            eUnsigned64,
            eInteger32,
            eInteger64,
            eFloat32,
            eFloat64,
            // Utils
            eIPv4 = 0x40,
            eHost,
            eEmail,
            ePhone,
            eServerAddress,
            eUri
        };

        class machine_t
        {
        public:
            machine_t() : m_code(), m_program() {}

            struct operands_t
            {
                static void write(binary_writer_t& writer, u16 opa) { writer.write(opa); }
                static void write(binary_writer_t& writer, u32 opa) { writer.write(opa); }
                static u16  write(binary_writer_t& writer, u32 opa, u32 opb)
                {
                    u16 const offset = (u16)writer.pos();
                    writer.write(opa);
                    writer.write(opb);
                    return offset;
                }
                static void write(binary_writer_t& writer, s32 opa) { writer.write(opa); }
                static u16  write(binary_writer_t& writer, s32 opa, s32 opb)
                {
                    u16 const offset = (u16)writer.pos();
                    writer.write(opa);
                    writer.write(opb);
                    return offset;
                }
                static u16 write(binary_writer_t& writer, s64 opa, s64 opb)
                {
                    u16 const offset = (u16)writer.pos();
                    writer.write(opa);
                    writer.write(opb);
                    return offset;
                }
                static u16 write(binary_writer_t& writer, u64 opa)
                {
                    u16 const offset = (u16)writer.pos();
                    writer.write(opa);
                    return offset;
                }
                static u16 write(binary_writer_t& writer, u64 opa, u64 opb)
                {
                    u16 const offset = (u16)writer.pos();
                    writer.write(opa);
                    writer.write(opb);
                    return offset;
                }
                static u16 write(binary_writer_t& writer, f32 opa, f32 opb)
                {
                    u16 const offset = (u16)writer.pos();
                    writer.write(opa);
                    writer.write(opb);
                    return offset;
                }
                static u16 write(binary_writer_t& writer, f64 opa, f64 opb)
                {
                    u16 const offset = (u16)writer.pos();
                    writer.write(opa);
                    writer.write(opb);
                    return offset;
                }
                static u16 write(binary_writer_t& writer, va_r_t* var)
                {
                    u16 const offset = (u16)writer.pos();
                    writer.write((u64)var);
                    return offset;
                }
                static u16 write(binary_writer_t& writer, nrunes::reader_t const& reader)
                {
                    crunes_t  r      = reader.get_current();
                    u16 const offset = (u16)writer.pos();
                    writer.write(r.get_type());
                    writer.write(r.m_ascii);
                    writer.write(r.m_ascii + r.m_end);
                    return offset;
                }
                static s32     read_s32(binary_reader_t& reader) { return reader.read_s32(); }
                static s64     read_s64(binary_reader_t& reader) { return reader.read_s64(); }
                static u8      read_u8(binary_reader_t& reader) { return reader.read_u8(); }
                static u32     read_u32(binary_reader_t& reader) { return reader.read_u32(); }
                static uchar32 read_uchar32(binary_reader_t& reader) { return (uchar32)reader.read_u32(); }
                static u64     read_u64(binary_reader_t& reader) { return reader.read_u64(); }
                static f32     read_f32(binary_reader_t& reader) { return reader.read_f32(); }
                static f64     read_f64(binary_reader_t& reader) { return reader.read_f64(); }
                static va_r_t* read_var(binary_reader_t& reader)
                {
                    va_r_t* var = (va_r_t*)reader.read_u64();
                    return var;
                }
                static crunes_t read_crunes(binary_reader_t& reader)
                {
                    const char* str_begin = nullptr;
                    const char* str_end   = nullptr;
                    s8          str_type  = 0;
                    reader.view_string(str_begin, str_end, str_type);

                    crunes_t str;
                    str.set_type(str_type);
                    str.m_ascii = str_begin;
                    str.m_str   = 0;
                    str.m_eos   = (u32)(str_end - str_begin);
                    str.m_end   = str.m_eos;
                    return str;
                }
            };

            nrunes::writer_t* get_writer(u32 channel) { return nullptr; }

            binary_writer_t m_code;
            binary_reader_t m_program;

            struct context_t
            {
                context_t(nrunes::reader_t const& _reader) : reader(_reader) {}
                u32              get_cursor() const { return reader.get_cursor(); }
                void             set_cursor(u32 const& c) { reader.set_cursor(c); }
                nrunes::reader_t reader;
            };
            typedef parser_t::pc_t pc_t;

            inline void                emit_instr(eOpcode o) { m_code.write((u16)o); }
            template <typename T> void emit_instr(eOpcode o, T _a)
            {
                emit_instr(o);
                operands_t::write(m_code, _a);
            }
            template <typename T1, typename T2> void emit_instr(eOpcode o, T1 _a, T2 _b)
            {
                emit_instr(o);
                operands_t::write(m_code, _a, _b);
            }
            void emit_instr(eOpcode o, crunes_t const& runes)
            {
                emit_instr(o);
                operands_t::write(m_code, (u8)runes.get_type());
                operands_t::write(m_code, (u64)runes.m_ascii, (u64)runes.m_ascii + runes.m_end);
            }
            void emit_instr(eOpcode o, va_r_t var)
            {
                emit_instr(o);
                operands_t::write(m_code, (u16)var.mType);
                operands_t::write(m_code, (u64)var.mRef);
            }
            void emit_call(pc_t pc1) { m_code.write(pc1); }
            void emit_calls(pc_t pc1)
            {
                m_code.write((u16)1);
                m_code.write(pc1);
            }
            void emit_calls(pc_t pc1, pc_t pc2)
            {
                m_code.write((u16)2);
                m_code.write(pc1);
                m_code.write(pc2);
            }
            void emit_calls(pc_t pc1, pc_t pc2, pc_t pc3)
            {
                m_code.write((u16)3);
                m_code.write(pc1);
                m_code.write(pc2);
                m_code.write(pc3);
            }
            void emit_calls(pc_t pc1, pc_t pc2, pc_t pc3, pc_t pc4)
            {
                m_code.write((u16)4);
                m_code.write(pc1);
                m_code.write(pc2);
                m_code.write(pc3);
                m_code.write(pc4);
            }

            inline pc_t pc() const { return (pc_t)m_program.pos(); }

            inline pc_t exec_jmp()
            {
                pc_t const pos = (pc_t)m_program.pos();
                pc_t const pc  = (pc_t)m_program.read_u16();
                m_program.seek(pc);
                return pos;
            }

            inline void skip_jmp()
            {
                // skip a call entry
                m_program.read_u16();
            }

            bool fnOpcodeIs(eOpcode) const;
            bool fnExec(context_t& ctxt);
            bool fnNot(context_t& ctxt);
            bool fnOr(context_t& ctxt);
            bool fnAnd(context_t& ctxt);
            bool fnSequence(context_t& ctxt);
            bool fnWithin(context_t& ctxt, s32 _min, s32 _max);
            bool fnTimes(context_t& ctxt, s32 _count);
            bool fnOneOrMore(context_t& ctxt);
            bool fnZeroOrMore(context_t& ctxt);
            bool fnZeroOrOne(context_t& ctxt);
            bool fnWhile(context_t& ctxt);
            bool fnUntil(context_t& ctxt);
            bool fnExtract(context_t& ctxt, va_r_t* var);
            bool fnEnclosed(context_t& ctxt, uchar32 _open, uchar32 _close);

            bool fnAny(context_t& ctxt);
            bool fnDigest(context_t& ctxt, u8 flags);
            bool fnIn(context_t& ctxt, nrunes::reader_t _chars);
            bool fnBetween(context_t& ctxt, uchar32 _from, uchar32 _until);
            bool fnAlphabet(context_t& ctxt);
            bool fnDigit(context_t& ctxt);
            bool fnHex(context_t& ctxt);
            bool fnAlphaNumeric(context_t& ctxt);
            bool fnExact(context_t& ctxt, nrunes::reader_t _text); // Case-Sensitive
            bool fnLike(context_t& ctxt, nrunes::reader_t _text);  // Case-Insensitive
            bool fnWhiteSpace(context_t& ctxt);
            bool fnIs(context_t& ctxt, uchar32 _c);
            bool fnWord(context_t& ctxt);
            bool fnEndOfText(context_t& ctxt);
            bool fnEndOfLine(context_t& ctxt);
            bool fnUnsigned32(context_t& ctxt, u32 _min, u32 _max);
            bool fnUnsigned64(context_t& ctxt, u64 _min, u64 _max);
            bool fnInteger32(context_t& ctxt, s32 _min, s32 _max);
            bool fnInteger64(context_t& ctxt, s64 _min, s64 _max);
            bool fnFloat32(context_t& ctxt, f32 _min, f32 _max);
            bool fnFloat64(context_t& ctxt, f64 _min, f64 _max);
            bool fnDecimal(context_t& ctxt);

            parser_t::program_t initialize(buffer_t buffer)
            {
                m_code = buffer;
                return parser_t::program_t(this, 0);
            }

            bool execute(parser_t::program_t const& prog, nrunes::reader_t const& reader, u32& cursor)
            {
                context_t ctxt(reader);
                ctxt.reader.set_cursor(cursor);
                buffer_t code = m_code.get_current_buffer();
                m_program     = binary_reader_t(code);
                m_program.seek(prog.pc());
                if (fnExec(ctxt))
                {
                    cursor = ctxt.get_cursor();
                    return true;
                }
                return false;
            }

            DCORE_CLASS_PLACEMENT_NEW_DELETE
        };

        parser_t::program_t::program_t() : m_machine(nullptr), m_pc(0) {}
        parser_t::program_t::program_t(machine_t* m) : m_machine(m) { m_pc = m->pc(); }
        parser_t::program_t::program_t(machine_t* m, pc_t pc) : m_machine(m), m_pc(pc) {}
        parser_t::program_t::program_t(const program_t& p) : m_machine(p.m_machine), m_pc(p.m_pc) {}

        parser_t::program_t parser_t::program_t::Program(program_t p)
        {
            program_t pc(*this);
            m_machine->emit_call(p.pc());
            return pc;
        }
        parser_t::program_t parser_t::program_t::Not(program_t p)
        {
            program_t pc(*this);
            m_machine->emit_instr(eNot);
            m_machine->emit_calls(p.pc());
            return pc;
        }
        parser_t::program_t parser_t::program_t::Or(program_t lhs, program_t rhs)
        {
            program_t pc(*this);
            m_machine->emit_instr(eOr);
            m_machine->emit_calls(lhs.pc(), rhs.pc());
            return pc;
        }
        parser_t::program_t parser_t::program_t::And(program_t lhs, program_t rhs)
        {
            program_t pc(*this);
            m_machine->emit_instr(eAnd);
            m_machine->emit_calls(lhs.pc(), rhs.pc());
            return pc;
        }
        parser_t::program_t parser_t::program_t::Sequence(program_t p1, program_t p2)
        {
            program_t pc(*this);
            m_machine->emit_instr(eSequence);
            m_machine->emit_calls(p1.pc(), p2.pc());
            return pc;
        }
        parser_t::program_t parser_t::program_t::Within(program_t p, s32 _min, s32 _max)
        {
            program_t pc(*this);
            m_machine->emit_instr(eWithin, _min, _max);
            m_machine->emit_calls(p.pc());
            return pc;
        }
        parser_t::program_t parser_t::program_t::Times(program_t p, s32 _count)
        {
            program_t pc(*this);
            m_machine->emit_instr(eTimes, _count);
            m_machine->emit_calls(p.pc());
            return pc;
        }
        parser_t::program_t parser_t::program_t::OneOrMore(program_t p)
        {
            program_t pc(*this);
            m_machine->emit_instr(eOneOrMore);
            m_machine->emit_calls(p.pc());
            return pc;
        }
        parser_t::program_t parser_t::program_t::ZeroOrMore(program_t p)
        {
            program_t pc(*this);
            m_machine->emit_instr(eZeroOrMore);
            m_machine->emit_calls(p.pc());
            return pc;
        }
        parser_t::program_t parser_t::program_t::ZeroOrOne(program_t p)
        {
            program_t pc(*this);
            m_machine->emit_instr(eZeroOrOne);
            m_machine->emit_calls(p.pc());
            return pc;
        }
        parser_t::program_t parser_t::program_t::While(program_t p)
        {
            program_t pc(*this);
            m_machine->emit_instr(eWhile);
            m_machine->emit_calls(p.pc());
            return pc;
        }
        parser_t::program_t parser_t::program_t::Until(program_t p)
        {
            program_t pc(*this);
            m_machine->emit_instr(eUntil);
            m_machine->emit_calls(p.pc());
            return pc;
        }
        parser_t::program_t parser_t::program_t::Extract(va_r_t* var, program_t p)
        {
            program_t pc(*this);
            m_machine->emit_instr(eExtract, var);
            m_machine->emit_calls(p.pc());
            return pc;
        }
        parser_t::program_t parser_t::program_t::Enclosed(uchar32 _open, uchar32 _close, program_t p)
        {
            program_t pc(*this);
            m_machine->emit_instr(eEnclosed, (u32)_open, (u32)_close);
            m_machine->emit_calls(p.pc());
            return pc;
        }

        parser_t::program_t parser_t::program_t::Any()
        {
            program_t pc(*this);
            m_machine->emit_instr(eAny);
            return pc;
        }
        parser_t::program_t parser_t::program_t::Digest(u8 flags)
        {
            program_t pc(*this);
            m_machine->emit_instr(eDigest, flags);
            return pc;
        }
        parser_t::program_t parser_t::program_t::In(crunes_t const& _chars)
        {
            program_t pc(*this);
            m_machine->emit_instr(eIn, _chars);
            return pc;
        }
        parser_t::program_t parser_t::program_t::Between(uchar32 _from, uchar32 _until)
        {
            program_t pc(*this);
            m_machine->emit_instr(eBetween, (u32)_from, (u32)_until);
            return pc;
        }
        parser_t::program_t parser_t::program_t::Alphabet()
        {
            program_t pc(*this);
            m_machine->emit_instr(eAlphabet);
            return pc;
        }
        parser_t::program_t parser_t::program_t::Digit()
        {
            program_t pc(*this);
            m_machine->emit_instr(eDigit);
            return pc;
        }
        parser_t::program_t parser_t::program_t::Hex()
        {
            program_t pc(*this);
            m_machine->emit_instr(eHex);
            return pc;
        }
        parser_t::program_t parser_t::program_t::AlphaNumeric()
        {
            program_t pc(*this);
            m_machine->emit_instr(eAlphaNumeric);
            return pc;
        }
        parser_t::program_t parser_t::program_t::Exact(crunes_t const& _text)
        {
            program_t pc(*this);
            m_machine->emit_instr(eExact, _text);
            return pc;
        }
        parser_t::program_t parser_t::program_t::Like(crunes_t const& _text)
        {
            program_t pc(*this);
            m_machine->emit_instr(eLike, _text);
            return pc;
        }
        parser_t::program_t parser_t::program_t::WhiteSpace()
        {
            program_t pc(*this);
            m_machine->emit_instr(eWhiteSpace);
            return pc;
        }
        parser_t::program_t parser_t::program_t::Is(uchar32 _c)
        {
            program_t pc(*this);
            m_machine->emit_instr(eIs, _c);
            return pc;
        }
        parser_t::program_t parser_t::program_t::Word()
        {
            program_t pc(*this);
            m_machine->emit_instr(eWord);
            return pc;
        }
        parser_t::program_t parser_t::program_t::EndOfText()
        {
            program_t pc(*this);
            m_machine->emit_instr(eEndOfText);
            return pc;
        }
        parser_t::program_t parser_t::program_t::EndOfLine()
        {
            program_t pc(*this);
            m_machine->emit_instr(eEndOfLine);
            return pc;
        }
        parser_t::program_t parser_t::program_t::Unsigned32(u32 _min, u32 _max)
        {
            program_t pc(*this);
            m_machine->emit_instr(eUnsigned32, _min, _max);
            return pc;
        }
        parser_t::program_t parser_t::program_t::Unsigned64(u64 _min, u64 _max)
        {
            program_t pc(*this);
            m_machine->emit_instr(eUnsigned64, _min, _max);
            return pc;
        }
        parser_t::program_t parser_t::program_t::Integer32(s32 _min, s32 _max)
        {
            program_t pc(*this);
            m_machine->emit_instr(eInteger32, _min, _max);
            return pc;
        }
        parser_t::program_t parser_t::program_t::Integer64(s64 _min, s64 _max)
        {
            program_t pc(*this);
            m_machine->emit_instr(eInteger64, _min, _max);
            return pc;
        }
        parser_t::program_t parser_t::program_t::Float32(f32 _min, f32 _max)
        {
            program_t pc(*this);
            m_machine->emit_instr(eFloat32, _min, _max);
            return pc;
        }
        parser_t::program_t parser_t::program_t::Float64(f64 _min, f64 _max)
        {
            program_t pc(*this);
            m_machine->emit_instr(eFloat64, _min, _max);
            return pc;
        }

        parser_t::program_t parser_t::Program(program_t p)
        {
            program_t prog(m_machine);
            m_machine->emit_call(p.pc());
            return prog;
        }
        parser_t::program_t parser_t::Not(program_t p)
        {
            program_t prog(m_machine);
            m_machine->emit_instr(eNot);
            m_machine->emit_calls(p.pc());
            return prog;
        }
        parser_t::program_t parser_t::Or(program_t lhs, program_t rhs)
        {
            program_t prog(m_machine);
            m_machine->emit_instr(eOr);
            m_machine->emit_calls(lhs.pc(), rhs.pc());
            return prog;
        }
        parser_t::program_t parser_t::And(program_t lhs, program_t rhs)
        {
            program_t prog(m_machine);
            m_machine->emit_instr(eAnd);
            m_machine->emit_calls(lhs.pc(), rhs.pc());
            return prog;
        }
        parser_t::program_t parser_t::Sequence(program_t p1, program_t p2)
        {
            program_t prog(m_machine);
            m_machine->emit_instr(eSequence);
            m_machine->emit_calls(p1.pc(), p2.pc());
            return prog;
        }
        parser_t::program_t parser_t::Sequence(program_t p1, program_t p2, program_t p3)
        {
            program_t prog(m_machine);
            m_machine->emit_instr(eSequence);
            m_machine->emit_calls(p1.pc(), p2.pc(), p3.pc());
            return prog;
        }
        parser_t::program_t parser_t::Sequence(program_t p1, program_t p2, program_t p3, program_t p4)
        {
            program_t prog(m_machine);
            m_machine->emit_instr(eSequence);
            m_machine->emit_calls(p1.pc(), p2.pc(), p3.pc(), p4.pc());
            return prog;
        }
        parser_t::program_t parser_t::Within(program_t p, s32 _min, s32 _max)
        {
            program_t prog(m_machine);
            m_machine->emit_instr(eWithin, _min, _max);
            m_machine->emit_calls(p.pc());
            return prog;
        }
        parser_t::program_t parser_t::Times(s32 _count, program_t p)
        {
            program_t prog(m_machine);
            m_machine->emit_instr(eTimes, _count);
            m_machine->emit_calls(p.pc());
            return prog;
        }
        parser_t::program_t parser_t::OneOrMore(program_t p)
        {
            program_t prog(m_machine);
            m_machine->emit_instr(eOneOrMore);
            m_machine->emit_calls(p.pc());
            return prog;
        }
        parser_t::program_t parser_t::ZeroOrMore(program_t p)
        {
            program_t prog(m_machine);
            m_machine->emit_instr(eZeroOrMore);
            m_machine->emit_calls(p.pc());
            return prog;
        }
        parser_t::program_t parser_t::ZeroOrOne(program_t p)
        {
            program_t prog(m_machine);
            m_machine->emit_instr(eZeroOrOne);
            m_machine->emit_calls(p.pc());
            return prog;
        }
        parser_t::program_t parser_t::While(program_t p)
        {
            program_t prog(m_machine);
            m_machine->emit_instr(eWhile);
            m_machine->emit_calls(p.pc());
            return prog;
        }
        parser_t::program_t parser_t::Until(program_t p)
        {
            program_t prog(m_machine);
            m_machine->emit_instr(eUntil);
            m_machine->emit_calls(p.pc());
            return prog;
        }
        parser_t::program_t parser_t::Extract(va_r_t* var, program_t p)
        {
            program_t prog(m_machine);
            m_machine->emit_instr(eExtract, var);
            m_machine->emit_calls(p.pc());
            return prog;
        }
        parser_t::program_t parser_t::Enclosed(uchar32 _open, uchar32 _close, program_t p)
        {
            program_t prog(m_machine);
            m_machine->emit_instr(eEnclosed, (u32)_open, (u32)_close);
            m_machine->emit_calls(p.pc());
            return prog;
        }

        parser_t::program_t parser_t::Any()
        {
            program_t prog(m_machine);
            m_machine->emit_instr(eAny);
            return prog;
        }
        parser_t::program_t parser_t::Digest(u8 flags)
        {
            program_t prog(m_machine);
            m_machine->emit_instr(eDigest, flags);
            return prog;
        }
        parser_t::program_t parser_t::In(crunes_t const& _chars)
        {
            program_t prog(m_machine);
            m_machine->emit_instr(eIn, _chars);
            return prog;
        }
        parser_t::program_t parser_t::Between(uchar32 _from, uchar32 _until)
        {
            program_t prog(m_machine);
            m_machine->emit_instr(eBetween, (u32)_from, (u32)_until);
            return prog;
        }
        parser_t::program_t parser_t::Alphabet()
        {
            program_t prog(m_machine);
            m_machine->emit_instr(eAlphabet);
            return prog;
        }
        parser_t::program_t parser_t::Digit()
        {
            program_t prog(m_machine);
            m_machine->emit_instr(eDigit);
            return prog;
        }
        parser_t::program_t parser_t::Hex()
        {
            program_t prog(m_machine);
            m_machine->emit_instr(eHex);
            return prog;
        }
        parser_t::program_t parser_t::AlphaNumeric()
        {
            program_t prog(m_machine);
            m_machine->emit_instr(eAlphaNumeric);
            return prog;
        }
        parser_t::program_t parser_t::Exact(crunes_t const& _text)
        {
            program_t prog(m_machine);
            m_machine->emit_instr(eExact, _text);
            return prog;
        }
        parser_t::program_t parser_t::Like(crunes_t const& _text)
        {
            program_t prog(m_machine);
            m_machine->emit_instr(eLike, _text);
            return prog;
        }
        parser_t::program_t parser_t::WhiteSpace()
        {
            program_t prog(m_machine);
            m_machine->emit_instr(eWhiteSpace);
            return prog;
        }
        parser_t::program_t parser_t::Is(uchar32 _c)
        {
            program_t prog(m_machine);
            m_machine->emit_instr(eIs, _c);
            return prog;
        }
        parser_t::program_t parser_t::Word()
        {
            program_t prog(m_machine);
            m_machine->emit_instr(eWord);
            return prog;
        }
        parser_t::program_t parser_t::EndOfText()
        {
            program_t prog(m_machine);
            m_machine->emit_instr(eEndOfText);
            return prog;
        }
        parser_t::program_t parser_t::EndOfLine()
        {
            program_t prog(m_machine);
            m_machine->emit_instr(eEndOfLine);
            return prog;
        }
        parser_t::program_t parser_t::Unsigned32(u32 _min, u32 _max)
        {
            program_t prog(m_machine);
            m_machine->emit_instr(eUnsigned32, _min, _max);
            return prog;
        }
        parser_t::program_t parser_t::Unsigned64(u64 _min, u64 _max)
        {
            program_t prog(m_machine);
            m_machine->emit_instr(eUnsigned64, _min, _max);
            return prog;
        }
        parser_t::program_t parser_t::Integer32(s32 _min, s32 _max)
        {
            program_t prog(m_machine);
            m_machine->emit_instr(eInteger32, _min, _max);
            return prog;
        }
        parser_t::program_t parser_t::Integer64(s64 _min, s64 _max)
        {
            program_t prog(m_machine);
            m_machine->emit_instr(eInteger64, _min, _max);
            return prog;
        }
        parser_t::program_t parser_t::Float32(f32 _min, f32 _max)
        {
            program_t prog(m_machine);
            m_machine->emit_instr(eFloat32, _min, _max);
            return prog;
        }
        parser_t::program_t parser_t::Float64(f64 _min, f64 _max)
        {
            program_t prog(m_machine);
            m_machine->emit_instr(eFloat64, _min, _max);
            return prog;
        }

        parser_t::program_t parser_t::Email()
        {
            crunes_t validchars((ascii::pcrune) "!#$%&'*+/=?^_`{|}~-", 0, 19, 19);

            program_t email_program = Sequence(OneOrMore(Or(AlphaNumeric(), In(validchars))), ZeroOrMore(Sequence(Or(Is('.'), Is('_')), Or(AlphaNumeric(), In(validchars)))), Is('@'), Host());
            return email_program;
        }

        parser_t::program_t parser_t::IPv4()
        {
            // clang-format off
            program_t program = Sequence(
                Times(3,
                    Sequence(
                        And(
                            Within(Digit(), 1,3), Unsigned32(0, 255)
                        ),
                        Is('.')
                    )
                ),
                And(
                    Within(Digit(), 1,3), Unsigned32(0, 255)
                )
            );
            // clang-format on

            return program;
        }

        parser_t::program_t parser_t::Host()
        {
            // clang-format off
            program_t program = Or(
                IPv4(),
                Sequence(
                    OneOrMore(AlphaNumeric()),
                    ZeroOrMore(Sequence(Is('-'), OneOrMore(AlphaNumeric()))),
                    ZeroOrMore(Sequence(
                        Sequence(Is('.'), OneOrMore(AlphaNumeric())),
                        ZeroOrMore(Sequence(Is('-'), OneOrMore(AlphaNumeric())))
                               )
                    )
                )
            );
            // clang-format on

            return program;
        }

        bool machine_t::fnExec(context_t& ctxt)
        {
            bool result = true;

            u16 const pc = exec_jmp();

            eOpcode const o = (eOpcode)m_program.read_u8();
            switch (o)
            {
                case eNOP: break;

                case eIPv4: // TODO
                case eHost:
                case eEmail:
                case ePhone:
                case eServerAddress:
                case eUri: break;

                case eNot: result = fnNot(ctxt); break;
                case eOr: result = fnOr(ctxt); break;
                case eAnd: result = fnAnd(ctxt); break;
                case eSequence: result = fnSequence(ctxt); break;
                case eWithin: result = fnWithin(ctxt, operands_t::read_s32(m_program), operands_t::read_s32(m_program)); break;
                case eTimes: result = fnTimes(ctxt, operands_t::read_s32(m_program)); break;
                case eOneOrMore: result = fnOneOrMore(ctxt); break;
                case eZeroOrMore: result = fnZeroOrMore(ctxt); break;
                case eZeroOrOne: result = fnZeroOrOne(ctxt); break;
                case eWhile: result = fnWhile(ctxt); break;
                case eUntil: result = fnUntil(ctxt); break;
                case eExtract: result = fnExtract(ctxt, operands_t::read_var(m_program)); break;
                case eEnclosed: result = fnEnclosed(ctxt, operands_t::read_uchar32(m_program), operands_t::read_uchar32(m_program)); break;

                case eAny: result = fnAny(ctxt); break;
                case eDigest: result = fnDigest(ctxt, operands_t::read_u8(m_program)); break;
                case eIn: result = fnIn(ctxt, operands_t::read_crunes(m_program)); break;
                case eBetween: result = fnBetween(ctxt, operands_t::read_uchar32(m_program), operands_t::read_uchar32(m_program)); break;
                case eAlphabet: result = fnAlphabet(ctxt); break;
                case eDigit: result = fnDigit(ctxt); break;
                case eHex: result = fnHex(ctxt); break;
                case eAlphaNumeric: result = fnAlphaNumeric(ctxt); break;
                case eExact: result = fnExact(ctxt, operands_t::read_crunes(m_program)); break;
                case eLike: result = fnLike(ctxt, operands_t::read_crunes(m_program)); break;
                case eWhiteSpace: result = fnWhiteSpace(ctxt); break;
                case eIs: result = fnIs(ctxt, operands_t::read_uchar32(m_program)); break;
                case eDecimal: result = fnDecimal(ctxt); break;
                case eWord: result = fnWord(ctxt); break;
                case eEndOfText: result = fnEndOfText(ctxt); break;
                case eEndOfLine: result = fnEndOfLine(ctxt); break;
                case eUnsigned32: result = fnUnsigned32(ctxt, operands_t::read_u32(m_program), operands_t::read_u32(m_program)); break;
                case eUnsigned64: result = fnUnsigned64(ctxt, operands_t::read_u64(m_program), operands_t::read_u64(m_program)); break;
                case eInteger32: result = fnInteger32(ctxt, operands_t::read_s32(m_program), operands_t::read_s32(m_program)); break;
                case eInteger64: result = fnInteger64(ctxt, operands_t::read_s64(m_program), operands_t::read_s64(m_program)); break;
                case eFloat32: result = fnFloat32(ctxt, operands_t::read_f32(m_program), operands_t::read_f32(m_program)); break;
                case eFloat64: result = fnFloat64(ctxt, operands_t::read_f64(m_program), operands_t::read_f64(m_program)); break;
            }

            m_program.seek(pc);
            return result;
        }

        bool machine_t::fnOpcodeIs(eOpcode o) const
        {
            u16 const opcode = m_program.peek_u16();
            return opcode == o;
        }

        bool machine_t::fnNot(context_t& ctxt) { return fnExec(ctxt); }

        bool machine_t::fnOr(context_t& ctxt)
        {
            u32 const cursor = ctxt.get_cursor();

            u16 n = m_program.read_u16(); // number of OR operands
            while (n != 0)
            {
                ctxt.set_cursor(cursor);

                if (fnExec(ctxt))
                    return true;
                skip_jmp();

                n--;
            }
            ctxt.set_cursor(cursor);
            return false;
        }

        bool machine_t::fnAnd(context_t& ctxt)
        {
            u32 const cursor = ctxt.get_cursor();
            u32       best   = ctxt.get_cursor();

            u16 n = m_program.read_u16(); // number of operands
            while (n != 0)
            {
                ctxt.set_cursor(cursor);

                if (!fnExec(ctxt))
                {
                    ctxt.set_cursor(cursor);
                    return false;
                }
                skip_jmp();

                best = ctxt.get_cursor();

                n--;
            }
            return true;
        }

        bool machine_t::fnSequence(context_t& ctxt)
        {
            u32 start = ctxt.get_cursor();

            u16 n = m_program.read_u16(); // number of operands
            while (n != 0)
            {
                if (!fnExec(ctxt))
                {
                    ctxt.set_cursor(start);
                    return false;
                }
                skip_jmp();

                n--;
            }
            return true;
        }

        bool machine_t::fnWithin(context_t& ctxt, s32 _min, s32 _max)
        {
            u32 const cursor = ctxt.get_cursor();
            s32       i      = 0;
            while (i < _max)
            {
                if (!fnExec(ctxt))
                {
                    break;
                }
                i += 1;
            }

            if (i >= _min && i <= _max)
                return true;

            ctxt.set_cursor(cursor);
            return false;
        }
        bool machine_t::fnTimes(context_t& ctxt, s32 _count) { return fnWithin(ctxt, _count, _count); }
        bool machine_t::fnOneOrMore(context_t& ctxt) { return fnWithin(ctxt, 1, 0x7fffffff); }
        bool machine_t::fnZeroOrMore(context_t& ctxt) { return fnWithin(ctxt, 0, 0x7fffffff); }
        bool machine_t::fnZeroOrOne(context_t& ctxt) { return fnWithin(ctxt, 0, 1); }
        bool machine_t::fnWhile(context_t& ctxt) { return fnWithin(ctxt, 0, 0x7fffffff); }
        bool machine_t::fnUntil(context_t& ctxt)
        {
            u32 const cursor = ctxt.get_cursor();
            s32       i      = 0;
            while (!fnEndOfText(ctxt))
            {
                if (fnExec(ctxt))
                {
                    // Encountered Until condition
                    return true;
                }

                ctxt.reader.skip(); // Advance reader
            }
            ctxt.set_cursor(cursor);
            return false;
        }
        bool machine_t::fnExtract(context_t& ctxt, va_r_t* var)
        {
            u32 start = ctxt.get_cursor();
            if (!fnExec(ctxt))
            {
                return false;
            }

            nrunes::reader_t varreader = ctxt.reader.select(start, ctxt.get_cursor());
            crunes_t         varrunes  = varreader.get_current();
            if (!varrunes.is_empty())
            {
                *var = varrunes;
            }
            return true;
        }
        bool machine_t::fnEnclosed(context_t& ctxt, uchar32 _open, uchar32 _close)
        {
            u32 start = ctxt.get_cursor();
            if (ctxt.reader.peek() != _open)
                return false;
            ctxt.reader.skip();

            if (!fnExec(ctxt))
            {
                ctxt.set_cursor(start);
                return false;
            }
            if (ctxt.reader.peek() != _close)
            {
                ctxt.set_cursor(start);
                return false;
            }
            ctxt.reader.skip();
            return true;
        }
        bool machine_t::fnAny(context_t& ctxt)
        {
            ctxt.reader.skip();
            return true;
        }
        bool machine_t::fnDigest(context_t& ctxt, u8 flags)
        {
            while (ctxt.reader.valid())
            {
                while (true)
                {
                    uchar32 s = ctxt.reader.peek();
                    if ((flags & parser_t::cWHITESPACE) == parser_t::cWHITESPACE)
                    {
                        if (s == ' ' || s == '\t' || s == '\r')
                            break;
                    }
                    if ((flags & parser_t::cALPHABET) == parser_t::cALPHABET)
                    {
                        if ((flags & parser_t::cIGNORECASE) == parser_t::cIGNORECASE)
                        {
                            if (nrunes::is_alpha(s))
                                break;
                        }
                        else if ((flags & parser_t::cLOWERCASE) == parser_t::cLOWERCASE)
                        {
                            if (nrunes::is_lower(s))
                                break;
                        }
                        else if ((flags & parser_t::cUPPERCASE) == parser_t::cUPPERCASE)
                        {
                            if (nrunes::is_upper(s))
                                break;
                        }
                    }
                    if ((flags & parser_t::cNUMERIC) == parser_t::cNUMERIC)
                    {
                        if (nrunes::is_digit(s))
                            break;
                    }

                    return false;
                }
                ctxt.reader.skip();
            }
            return false;
        }
        bool machine_t::fnIn(context_t& ctxt, nrunes::reader_t _chars)
        {
            _chars.reset();
            u32           ccursor = _chars.get_cursor();
            uchar32 const s       = ctxt.reader.peek();
            while (_chars.valid())
            {
                uchar32 const c = _chars.read();
                if (c == s)
                {
                    ctxt.reader.skip();
                    return true;
                }
            }
            return false;
        }
        bool machine_t::fnBetween(context_t& ctxt, uchar32 _from, uchar32 _until)
        {
            uchar32 const s = ctxt.reader.peek();
            if (s >= _from && s <= _until)
            {
                ctxt.reader.skip();
                return true;
            }
            return false;
        }
        bool machine_t::fnAlphabet(context_t& ctxt)
        {
            if (!fnBetween(ctxt, 'a', 'z'))
            {
                if (!fnBetween(ctxt, 'A', 'Z'))
                    return false;
            }
            ctxt.reader.skip();
            return true;
        }
        bool machine_t::fnDigit(context_t& ctxt)
        {
            if (!fnBetween(ctxt, '0', '9'))
            {
                return false;
            }
            ctxt.reader.skip();
            return true;
        }
        bool machine_t::fnHex(context_t& ctxt)
        {
            if (fnBetween(ctxt, 'a', 'f') || fnBetween(ctxt, 'A', 'F') || fnBetween(ctxt, '0', '9'))
            {
                ctxt.reader.skip();
                return true;
            }
            return false;
        }
        bool machine_t::fnAlphaNumeric(context_t& ctxt)
        {
            if (fnDigit(ctxt) || fnAlphabet(ctxt))
            {
                ctxt.reader.skip();
                return true;
            }
            return false;
        }
        bool machine_t::fnExact(context_t& ctxt, nrunes::reader_t _text)
        {
            _text.reset();
            u32 tcursor = _text.get_cursor();
            u32 cursor  = ctxt.get_cursor();
            while (_text.valid())
            {
                uchar32 const s = ctxt.reader.peek();
                uchar32 const c = _text.read();
                if (c != s)
                {
                    ctxt.set_cursor(cursor);
                    return false;
                }
            }
            return true;
        }
        bool machine_t::fnLike(context_t& ctxt, nrunes::reader_t _text)
        {
            _text.reset();
            u32 tcursor = _text.get_cursor();
            u32 cursor  = ctxt.get_cursor();
            while (_text.valid())
            {
                uchar32 const s = ctxt.reader.peek();
                uchar32 const c = _text.read();
                if (c != s && nrunes::to_lower(c) != nrunes::to_lower(c))
                {
                    ctxt.set_cursor(cursor);
                    return false;
                }
            }
            return true;
        }
        bool machine_t::fnWhiteSpace(context_t& ctxt)
        {
            uchar32 const s = ctxt.reader.peek();
            if (s == ' ' || s == '\t' || s == '\r')
            {
                ctxt.reader.skip();
                return true;
            }
            return false;
        }
        bool machine_t::fnIs(context_t& ctxt, uchar32 _c)
        {
            uchar32 const s = ctxt.reader.peek();
            if (s == _c)
            {
                ctxt.reader.skip();
                return true;
            }
            return false;
        }
        bool machine_t::fnWord(context_t& ctxt)
        {
            u32 cursor = ctxt.get_cursor();
            if (!fnAlphabet(ctxt))
            {
                ctxt.set_cursor(cursor);
                return false;
            }
            while (fnAlphabet(ctxt)) {}
            return true;
        }

        bool machine_t::fnEndOfText(context_t& ctxt) { return !ctxt.reader.valid(); }

        bool machine_t::fnEndOfLine(context_t& ctxt)
        {
            u32           cursor = ctxt.get_cursor();
            uchar32 const s1     = ctxt.reader.peek();
            ctxt.reader.skip();
            uchar32 const s2 = ctxt.reader.peek();
            if (s1 == '\r' && s2 == '\n')
            {
                ctxt.reader.skip();
                return true;
            }
            else if (s1 == '\n')
            {
                return true;
            }

            ctxt.set_cursor(cursor);
            return false;
        }

        bool machine_t::fnUnsigned32(context_t& ctxt, u32 _min, u32 _max) { return fnUnsigned64(ctxt, _min, _max); }
        bool machine_t::fnUnsigned64(context_t& ctxt, u64 _min, u64 _max)
        {
            u32 cursor = ctxt.get_cursor();

            u64 value = 0;
            while (ctxt.reader.valid())
            {
                uchar32 c = ctxt.reader.peek();
                if (!(c >= '0' && c <= '9'))
                    break;
                value = (value * 10) + nrunes::to_digit(c);
                ctxt.reader.skip();
            }

            if (cursor == ctxt.get_cursor())
                return false;

            if (value >= _min && value <= _max)
            {
                return true;
            }
            ctxt.set_cursor(cursor);
            return false;
        }
        bool machine_t::fnInteger32(context_t& ctxt, s32 _min, s32 _max) { return fnInteger64(ctxt, _min, _max); }
        bool machine_t::fnInteger64(context_t& ctxt, s64 _min, s64 _max)
        {
            u32 cursor = ctxt.get_cursor();

            s64 value = 0;

            uchar32 c = ctxt.reader.peek();

            bool const is_negative = (c == '-');
            if (is_negative)
                ctxt.reader.skip();

            while (ctxt.reader.valid())
            {
                c = ctxt.reader.peek();
                if (!(c >= '0' && c <= '9'))
                    break;
                value = (value * 10) + nrunes::to_digit(c);
                ctxt.reader.skip();
            }
            if (cursor == ctxt.get_cursor())
                return false;

            if (is_negative)
                value = -value;

            if (value >= _min && value <= _max)
            {
                return true;
            }
            ctxt.set_cursor(cursor);
            return false;
        }
        bool machine_t::fnFloat32(context_t& ctxt, f32 _min, f32 _max) { return fnFloat64(ctxt, _min, _max); }
        bool machine_t::fnFloat64(context_t& ctxt, f64 _min, f64 _max)
        {
            u32 cursor = ctxt.get_cursor();

            f64     value       = 0.0;
            uchar32 c           = ctxt.reader.peek();
            bool    is_negative = c == '-';
            if (is_negative)
                ctxt.reader.skip();
            while (ctxt.reader.valid())
            {
                c = ctxt.reader.peek();
                if (!nrunes::is_digit(c))
                    break;
                value = (value * 10.0) + nrunes::to_digit(c);
            }
            if (c == '.')
            {
                ctxt.reader.skip();
                f64 mantissa = 10.0;
                while (ctxt.reader.valid())
                {
                    c = ctxt.reader.peek();
                    if (!nrunes::is_digit(c))
                        break;
                    value = value + f64(nrunes::to_digit(c)) / mantissa;
                    mantissa *= 10.0;
                }
            }
            if (cursor == ctxt.get_cursor())
                return false;
            if (is_negative)
                value = -value;
            if (value >= _min && value <= _max)
            {
                return true;
            }
            ctxt.set_cursor(cursor);
            return false;
        }
        bool machine_t::fnDecimal(context_t& ctxt) { return fnUnsigned64(ctxt, 0, 0xffffffffffffffffUL); }

        void use_case_parser2()
        {
            u8       buffer[1024 + 1];
            parser_t parser(buffer_t(buffer, buffer + 1024));

            // For examples see:
            // - parser_t::Email()
            // - parser_t::IPv4()
            parser_t::program_t prog = parser.Email();

            nrunes::reader_t reader("john.doe@hotmail.com");
            bool             result = parser.parse(prog, reader);
            // result == true !
        }

        parser_t::parser_t(buffer_t buffer)
        {
            m_buffer = buffer;

            buffer_t machine_buffer = buffer(0, sizeof(machine_t));
            buffer_t work_buffer    = buffer(sizeof(machine_t), buffer.size());
            void*    machine_mem    = machine_buffer.data();
            m_machine               = new (machine_mem) machine_t();
            m_machine->initialize(work_buffer);
        }

        bool parser_t::parse(program_t program, nrunes::reader_t& reader)
        {
            u32        cursor = reader.get_cursor();
            machine_t* m      = program.m_machine;
            if (m->execute(program, reader, cursor))
            {
                reader.set_cursor(cursor);
                return true;
            }
            return false;
        }

    } // namespace parser2

} // namespace ncore
