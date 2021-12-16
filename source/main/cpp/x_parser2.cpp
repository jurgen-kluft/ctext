#include "xbase/x_debug.h"
#include "xbase/x_target.h"
#include "xbase/x_buffer.h"
#include "xbase/x_va_list.h"
#include "xbase/x_runes.h"

#include "xtext/x_parser2.h"

namespace xcore
{
    namespace xparser2
    {
        class machine_t
        {
        public:
            enum eOpcode
            {
                eNOP = 0,
                // Manipulators (scope)
                ePop = 0xC0,
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

        private:
            struct operands_t
            {
                static u16 write(binary_writer_t& writer, u16 opa) { return (u16)writer.write(opa); }
                static u16 write(binary_writer_t& writer, u32 opa) { return (u16)writer.write(opa); }
                static u16 write(binary_writer_t& writer, u32 opa, u32 opb)
                {
                    u16 const offset = (u16)writer.pos();
                    writer.write(opa);
                    writer.write(opb);
                    return offset;
                }
                static u16 write(binary_writer_t& writer, s32 opa) { return (u16)writer.write(opa); }
                static u16 write(binary_writer_t& writer, s32 opa, s32 opb)
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
                static u16 write(binary_writer_t& writer, runes_reader_t const& reader)
                {
                    crunes_t  r      = reader.get_current();
                    u16 const offset = (u16)writer.pos();
                    writer.write((u16)r.m_type);
                    writer.write((uptr)r.m_ascii.m_str);
                    writer.write((uptr)r.m_ascii.m_end);
                    return offset;
                }
                static s32     read_s32(binary_reader_t& reader) { return reader.read_s32(); }
                static s64     read_s64(binary_reader_t& reader) { return reader.read_s64(); }
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
                    crunes_t str;
                    str.m_type                = (s32)reader.read_u16();
                    str.m_ascii.m_bos = (ascii::pcrune)reader.read_u64();
                    str.m_ascii.m_str = str.m_ascii.m_bos;
                    str.m_ascii.m_eos = (ascii::pcrune)reader.read_u64();
                    str.m_ascii.m_end = str.m_ascii.m_eos;
                    return str;
                }
            };

            runes_writer_t* get_writer(u32 channel) { return nullptr; }

            s32             m_pc;
            binary_writer_t m_code;
            binary_reader_t m_program;
            u16*            m_stack;
            s32             m_stack_size;
            s32             m_stack_max;

            struct context_t
            {
                context_t(runes_reader_t const& _reader) : reader(_reader) {}

                crunes_t::ptr_t get_cursor() const { return reader.get_cursor(); }
                void            set_cursor(crunes_t::ptr_t const& c) { reader.set_cursor(c); }

                runes_reader_t reader;
            };

            void emit_pop_handle(eOpcode o)
            {
                if (o == ePop)
                {
                    u16 const       pc           = m_stack[--m_stack_size];
                    binary_writer_t write_poploc = m_code.range(pc, pc + sizeof(u16));
                    write_poploc.write((u16)m_code.pos());
                }
                else if ((o & ePop) == ePop)
                {
                    m_stack[m_stack_size++] = m_code.pos() + 2;
                }
            }
            machine_t& emit_instr(eOpcode o)
            {
                emit_pop_handle(o);
                m_code.write((u16)o);
                if ((o & ePop) == ePop)
                    m_code.write((u16)0);
                return *this;
            }
            template <typename T> machine_t& emit_instr(eOpcode o, T _a)
            {
                emit_instr(o);
                operands_t::write(m_code, _a);
                return *this;
            }
            template <typename T1, typename T2> machine_t& emit_instr(eOpcode o, T1 _a, T2 _b)
            {
                emit_pop_handle(o);
                emit_instr(o);
                operands_t::write(m_code, _a, _b);
                return *this;
            }
            machine_t& emit_instr(eOpcode o, crunes_t const& runes)
            {
                emit_pop_handle(o);
                emit_instr(o);
                operands_t::write(m_code, (u16)runes.m_type);
                operands_t::write(m_code, (u64)runes.m_ascii.m_str, (u64)runes.m_ascii.m_end);
                return *this;
            }
            machine_t& emit_instr(eOpcode o, va_r_t var)
            {
                emit_pop_handle(o);
                emit_instr(o);
                operands_t::write(m_code, (u16)var.mType);
                operands_t::write(m_code, (u64)var.mRef[0]);
                return *this;
            }

        public:
            machine_t() : m_code(), m_program(), m_stack(nullptr), m_stack_size(0), m_stack_max(0) {}
            void initialize(buffer_t buffer)
            {
                // Give some memory to code-block and stack
                u32 const buffersize   = buffer.size();
                buffer_t  code_buffer  = buffer(0, (buffersize * 8) / 10);          // 80 %
                buffer_t  stack_buffer = buffer(code_buffer.size(), buffer.size()); // 20 %
                m_code                 = code_buffer;
                m_stack                = (u16*)stack_buffer.data();
                m_stack_max            = stack_buffer.size() / sizeof(u16);
                m_stack_size           = 0;
            }

            bool execute(runes_reader_t const& reader, crunes_t::ptr_t& cursor)
            {
                context_t ctxt(reader);
                ctxt.reader.set_cursor(cursor);
                buffer_t code = m_code.get_current_buffer();
                m_program     = binary_reader_t(code);
                return fnExec(ctxt);
            }

            machine_t& Pop()
            {
                emit_pop_handle(ePop);
                return *this;
            }
            machine_t& Not() { return emit_instr(eNot); }
            machine_t& Or() { return emit_instr(eOr); }
            machine_t& And() { return emit_instr(eAnd); }
            machine_t& Sequence() { return emit_instr(eSequence); }
            machine_t& Within(s32 _min = 0, s32 _max = 0x7fffffff) { return emit_instr(eWithin, _min, _max); }
            machine_t& Times(s32 _count) { return emit_instr(eTimes, _count); }
            machine_t& OneOrMore() { return emit_instr(eOneOrMore); }
            machine_t& ZeroOrMore() { return emit_instr(eZeroOrMore); }
            machine_t& ZeroOrOne() { return emit_instr(eZeroOrOne); }
            machine_t& While() { return emit_instr(eWhile); }
            machine_t& Until() { return emit_instr(eUntil); }
            machine_t& Extract(va_r_t* var) { return emit_instr(eExtract, var); }
            machine_t& Enclosed(uchar32 _open, uchar32 _close) { return emit_instr(eEnclosed, (u32)_open, (u32)_close); }
            machine_t& Any() { return emit_instr(eAny); }
            machine_t& In(crunes_t const& _chars) { return emit_instr(eIn, _chars); }
            machine_t& Between(uchar32 _from, uchar32 _until) { return emit_instr(eBetween, (u32)_from, (u32)_until); }
            machine_t& Alphabet() { return emit_instr(eAlphabet); }
            machine_t& Digit() { return emit_instr(eDigit); }
            machine_t& Hex() { return emit_instr(eHex); }
            machine_t& AlphaNumeric() { return emit_instr(eAlphaNumeric); }
            machine_t& Exact(crunes_t const& _text) { return emit_instr(eExact, _text); }
            machine_t& Like(crunes_t const& _text) { return emit_instr(eLike, _text); }
            machine_t& WhiteSpace() { return emit_instr(eWhiteSpace); }
            machine_t& Is(uchar32 _c) { return emit_instr(eIs, _c); }
            machine_t& Word() { return emit_instr(eWord); }
            machine_t& EndOfText() { return emit_instr(eEndOfText); }
            machine_t& EndOfLine() { return emit_instr(eEndOfLine); }
            machine_t& Unsigned32(u32 _min = 0, u32 _max = 0xffffffff) { return emit_instr(eUnsigned32, _min, _max); }
            machine_t& Unsigned64(u64 _min = 0, u64 _max = 0xffffffffffffffffUL) { return emit_instr(eUnsigned64, _min, _max); }
            machine_t& Integer32(s32 _min = 0, s32 _max = 0x7fffffff) { return emit_instr(eInteger32, _min, _max); }
            machine_t& Integer64(s64 _min = 0, s64 _max = 0x7fffffffffffffffL) { return emit_instr(eInteger64, _min, _max); }
            machine_t& Float32(f32 _min = 0.0f, f32 _max = 3.402823e+38f) { return emit_instr(eFloat32, _min, _max); }
            machine_t& Float64(f64 _min = 0.0, f64 _max = 3.402823e+38f) { return emit_instr(eFloat64, _min, _max); }
            machine_t& Email();
            machine_t& IPv4();
            machine_t& Host();
            machine_t& Date() { return *this; }
            machine_t& Time() { return *this; }
            machine_t& Phone() { return *this; }
            machine_t& ServerAddress() { return *this; }
            machine_t& URI() { return *this; }

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
            bool fnIn(context_t& ctxt, runes_reader_t _chars);
            bool fnBetween(context_t& ctxt, uchar32 _from, uchar32 _until);
            bool fnAlphabet(context_t& ctxt);
            bool fnDigit(context_t& ctxt);
            bool fnHex(context_t& ctxt);
            bool fnAlphaNumeric(context_t& ctxt);
            bool fnExact(context_t& ctxt, runes_reader_t _text); // Case-Sensitive
            bool fnLike(context_t& ctxt, runes_reader_t _text);  // Case-Insensitive
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

            XCORE_CLASS_PLACEMENT_NEW_DELETE
        };

        machine_t& machine_t::Email()
        {
            crunes_t validchars((ascii::pcrune) "!#$%&'*+/=?^_`{|}~-", 19);
            // clang-format off
            Sequence();
                OneOrMore();
                    Or();
                        AlphaNumeric();
                        In(validchars);
                    Pop();
                Pop();
                ZeroOrMore();
                    Or();
                        Is('.');
                        Is('_');
                    Pop();
                    Or();
                        AlphaNumeric();
                        In(validchars);
                    Pop();
                Pop();
                Is('@');
                Host();
            Pop();
            // clang-format on
            return *this;
        }

        machine_t& machine_t::IPv4()
        {
            // clang-format off
            Sequence();
                Times(3);
                    And();
                        Within(1,3);
                            Digit();
                        Pop();
                        Unsigned32(0, 255);
                    Pop();
                    Is('.');
                Pop();
                And();
                    Within(1,3);
                        Digit();
                    Pop();
                    Unsigned32(0, 255);
                Pop();
            Pop();
            // clang-format on
            return *this;
        }

        machine_t& machine_t::Host()
        {
            // clang-format off
            Or();
                IPv4();
                Sequence();
                    OneOrMore();
                        AlphaNumeric();
                    Pop();
                    ZeroOrMore();
                        Is('-');
                        OneOrMore();
                            AlphaNumeric();
                        Pop();
                    Pop();
                    ZeroOrMore();
                        Is('.');
                        OneOrMore();
                            AlphaNumeric();
                        Pop();
                        ZeroOrMore();
                            Is('-');
                            OneOrMore();
                                AlphaNumeric();
                            Pop();
                        Pop();
                    Pop();
                Pop();
            Pop();
            // clang-format on
            return *this;
        }

        bool machine_t::fnExec(context_t& ctxt)
        {
            //@NOTE: When a fn that requires a Pop returns false we need to jump over any existing opcodes to arrive
            //       at the Pop opcode.

            bool    result = true;
            u32     jmp    = 0;
            eOpcode o      = (eOpcode)m_program.read_u16();
            if ((o & ePop) == ePop)
            {
                jmp = m_program.read_u16();
            }
            switch (o)
            {
                case ePop:
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

            if ((o & ePop) == ePop)
            {
                m_program.seek(jmp);
            }

            return result;
        }

        bool machine_t::fnOpcodeIs(eOpcode o) const
        {
            u16 const opcode = m_program.peek_u16();
            return opcode == o;
        }

        bool machine_t::fnNot(context_t& ctxt) { return !(fnExec(ctxt)); }

        bool machine_t::fnOr(context_t& ctxt)
        {
            while (!fnOpcodeIs(machine_t::ePop))
            {
                if (!fnExec(ctxt))
                    return false;
            }
            return true;
        }

        bool machine_t::fnAnd(context_t& ctxt)
        {
            crunes_t::ptr_t const cursor = ctxt.get_cursor();
            crunes_t::ptr_t       best   = ctxt.get_cursor();
            while (!fnOpcodeIs(machine_t::ePop))
            {
                ctxt.set_cursor(cursor);
                if (!fnExec(ctxt))
                {
                    ctxt.set_cursor(cursor);
                    return false;
                }
                best = ctxt.get_cursor().furthest(best);
            }
            return true;
        }

        bool machine_t::fnSequence(context_t& ctxt)
        {
            crunes_t::ptr_t start = ctxt.get_cursor();
            while (!fnOpcodeIs(ePop))
            {
                if (!fnExec(ctxt))
                {
                    ctxt.get_cursor() = start;
                    return false;
                }
            }
            return true;
        }

        bool machine_t::fnWithin(context_t& ctxt, s32 _min, s32 _max)
        {
            crunes_t::ptr_t const cursor = ctxt.get_cursor();
            s32                   pc     = m_pc;
            s32                   i      = 0;
            while (i < _max)
            {
                do
                {
                    if (!fnExec(ctxt))
                    {
                        ctxt.set_cursor(cursor);
                        return false;
                    }
                } while (!fnOpcodeIs(ePop));

                m_pc = pc;
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
            crunes_t::ptr_t const cursor = ctxt.get_cursor();
            s32                   pc     = m_pc;
            s32                   i      = 0;
            while (!fnEndOfText(ctxt))
            {
                do
                {
                    if (!fnExec(ctxt))
                        break;
                } while (!fnOpcodeIs(ePop));

                if (fnOpcodeIs(ePop))
                    return true; // We found the 'until'

                m_pc = pc;          // Reset program-counter
                ctxt.reader.skip(); // Advance reader
            }
            ctxt.set_cursor(cursor);
            return false;
        }
        bool machine_t::fnExtract(context_t& ctxt, va_r_t* var)
        {
            crunes_t::ptr_t start = ctxt.get_cursor();
            while (!fnOpcodeIs(ePop))
            {
                if (!fnExec(ctxt))
                {
                    return false;
                }
            };
            runes_reader_t varreader = ctxt.reader.select(start, ctxt.get_cursor());
            crunes_t       varrunes  = varreader.get_current();
            if (!varrunes.is_empty())
            {
                *var = varrunes;
            }
            return true;
        }
        bool machine_t::fnEnclosed(context_t& ctxt, uchar32 _open, uchar32 _close)
        {
            crunes_t::ptr_t start = ctxt.get_cursor();
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
        bool machine_t::fnIn(context_t& ctxt, runes_reader_t _chars)
        {
            _chars.reset();
            crunes_t::ptr_t ccursor = _chars.get_cursor();
            uchar32 const   s       = ctxt.reader.peek();
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
        bool machine_t::fnExact(context_t& ctxt, runes_reader_t _text)
        {
            _text.reset();
            crunes_t::ptr_t tcursor = _text.get_cursor();
            crunes_t::ptr_t cursor  = ctxt.get_cursor();
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
        bool machine_t::fnLike(context_t& ctxt, runes_reader_t _text)
        {
            _text.reset();
            crunes_t::ptr_t tcursor = _text.get_cursor();
            crunes_t::ptr_t cursor  = ctxt.get_cursor();
            while (_text.valid())
            {
                uchar32 const s = ctxt.reader.peek();
                uchar32 const c = _text.read();
                if (c != s && to_lower(c) != to_lower(c))
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
            crunes_t::ptr_t cursor = ctxt.get_cursor();
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
            crunes_t::ptr_t cursor = ctxt.get_cursor();
            uchar32 const   s1     = ctxt.reader.peek();
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
            crunes_t::ptr_t cursor = ctxt.get_cursor();

            u64 value = 0;
            while (ctxt.reader.valid())
            {
                uchar32 c = ctxt.reader.peek();
                if (!(c >= '0' && c <= '9'))
                    break;
                value = (value * 10) + to_digit(c);
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
            crunes_t::ptr_t cursor = ctxt.get_cursor();

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
                value = (value * 10) + to_digit(c);
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
            crunes_t::ptr_t cursor = ctxt.get_cursor();

            f64     value       = 0.0;
            uchar32 c           = ctxt.reader.peek();
            bool    is_negative = c == '-';
            if (is_negative)
                ctxt.reader.skip();
            while (ctxt.reader.valid())
            {
                c = ctxt.reader.peek();
                if (!is_digit(c))
                    break;
                value = (value * 10.0) + to_digit(c);
            }
            if (c == '.')
            {
                ctxt.reader.skip();
                f64 mantissa = 10.0;
                while (ctxt.reader.valid())
                {
                    c = ctxt.reader.peek();
                    if (!is_digit(c))
                        break;
                    value = value + f64(to_digit(c)) / mantissa;
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
            // clang-format off
            machine_t m;

            m.Email();

            // For examples see:
            // - machine_t::Email()
            // - machine_t::IPv4()

            // clang-format on

            runes_reader_t  reader("john.doe@hotmail.com");
            crunes_t::ptr_t cursor;
            bool            result = m.execute(reader, cursor);
            // result == true !
        }

        parser_t::parser_t(buffer_t buffer)
        {
            buffer_t   machine_buffer = buffer(0, sizeof(machine_t));
            buffer_t   work_buffer    = buffer(sizeof(machine_t), buffer.size());
            void*      machine_mem    = machine_buffer.data();
            machine_t* machine        = new (machine_mem) machine_t();
            machine->initialize(work_buffer);
        }

        bool parser_t::Parse(runes_reader_t& reader)
        {
            machine_t*      machine = (machine_t*)m_buffer.data();
            crunes_t::ptr_t cursor  = reader.get_cursor();
            return machine->execute(reader, cursor);
        }

        parser_t& parser_t::Extract(va_r_t* var)
        {
            machine_t* machine = (machine_t*)m_buffer.data();
            machine->Extract(var);
            return *this;
        }
        parser_t& parser_t::Pop()
        {
            machine_t* machine = (machine_t*)m_buffer.data();
            machine->Pop();
            return *this;
        }
        parser_t& parser_t::Not()
        {
            machine_t* machine = (machine_t*)m_buffer.data();
            machine->Not();
            return *this;
        }

        parser_t& parser_t::Or()
        {
            machine_t* machine = (machine_t*)m_buffer.data();
            machine->Or();
            return *this;
        }

        parser_t& parser_t::And()
        {
            machine_t* machine = (machine_t*)m_buffer.data();
            machine->And();
            return *this;
        }

        parser_t& parser_t::Sequence()
        {
            machine_t* machine = (machine_t*)m_buffer.data();
            machine->Sequence();
            return *this;
        }

        parser_t& parser_t::Within(s32 _min, s32 _max)
        {
            machine_t* machine = (machine_t*)m_buffer.data();
            machine->Within(_min, _max);
            return *this;
        }

        parser_t& parser_t::Times(s32 _count)
        {
            machine_t* machine = (machine_t*)m_buffer.data();
            machine->Times(_count);
            return *this;
        }

        parser_t& parser_t::OneOrMore()
        {
            machine_t* machine = (machine_t*)m_buffer.data();
            machine->OneOrMore();
            return *this;
        }

        parser_t& parser_t::ZeroOrMore()
        {
            machine_t* machine = (machine_t*)m_buffer.data();
            machine->ZeroOrMore();
            return *this;
        }

        parser_t& parser_t::ZeroOrOne()
        {
            machine_t* machine = (machine_t*)m_buffer.data();
            machine->ZeroOrOne();
            return *this;
        }

        parser_t& parser_t::While()
        {
            machine_t* machine = (machine_t*)m_buffer.data();
            machine->While();
            return *this;
        }

        parser_t& parser_t::Until()
        {
            machine_t* machine = (machine_t*)m_buffer.data();
            machine->Until();
            return *this;
        }

        parser_t& parser_t::Enclosed(uchar32 _open, uchar32 _close)
        {
            machine_t* machine = (machine_t*)m_buffer.data();
            machine->Enclosed(_open, _close);
            return *this;
        }

        parser_t& parser_t::Any()
        {
            machine_t* machine = (machine_t*)m_buffer.data();
            machine->Any();
            return *this;
        }

        parser_t& parser_t::In(crunes_t const& _chars)
        {
            machine_t* machine = (machine_t*)m_buffer.data();
            machine->In(_chars);
            return *this;
        }

        parser_t& parser_t::Between(uchar32 _from, uchar32 _until)
        {
            machine_t* machine = (machine_t*)m_buffer.data();
            machine->Between(_from, _until);
            return *this;
        }

        parser_t& parser_t::Alphabet()
        {
            machine_t* machine = (machine_t*)m_buffer.data();
            machine->Alphabet();
            return *this;
        }

        parser_t& parser_t::Digit()
        {
            machine_t* machine = (machine_t*)m_buffer.data();
            machine->Digit();
            return *this;
        }

        parser_t& parser_t::Hex()
        {
            machine_t* machine = (machine_t*)m_buffer.data();
            machine->Hex();
            return *this;
        }

        parser_t& parser_t::AlphaNumeric()
        {
            machine_t* machine = (machine_t*)m_buffer.data();
            machine->AlphaNumeric();
            return *this;
        }

        parser_t& parser_t::Exact(crunes_t const& _text)
        {
            machine_t* machine = (machine_t*)m_buffer.data();
            machine->Exact(_text);
            return *this;
        }

        parser_t& parser_t::Like(crunes_t const& _text)
        {
            machine_t* machine = (machine_t*)m_buffer.data();
            machine->Like(_text);
            return *this;
        }

        parser_t& parser_t::WhiteSpace()
        {
            machine_t* machine = (machine_t*)m_buffer.data();
            machine->WhiteSpace();
            return *this;
        }

        parser_t& parser_t::Is(uchar32 _c)
        {
            machine_t* machine = (machine_t*)m_buffer.data();
            machine->Is(_c);
            return *this;
        }

        parser_t& parser_t::Word()
        {
            machine_t* machine = (machine_t*)m_buffer.data();
            machine->Word();
            return *this;
        }

        parser_t& parser_t::EndOfText()
        {
            machine_t* machine = (machine_t*)m_buffer.data();
            machine->EndOfText();
            return *this;
        }

        parser_t& parser_t::EndOfLine()
        {
            machine_t* machine = (machine_t*)m_buffer.data();
            machine->EndOfLine();
            return *this;
        }

        parser_t& parser_t::Unsigned32(u32 _min, u32 _max)
        {
            machine_t* machine = (machine_t*)m_buffer.data();
            machine->Unsigned32(_min, _max);
            return *this;
        }

        parser_t& parser_t::Unsigned64(u64 _min, u64 _max)
        {
            machine_t* machine = (machine_t*)m_buffer.data();
            machine->Unsigned64(_min, _max);
            return *this;
        }

        parser_t& parser_t::Integer32(s32 _min, s32 _max)
        {
            machine_t* machine = (machine_t*)m_buffer.data();
            machine->Integer32(_min, _max);
            return *this;
        }

        parser_t& parser_t::Integer64(s64 _min, s64 _max)
        {
            machine_t* machine = (machine_t*)m_buffer.data();
            machine->Integer64(_min, _max);
            return *this;
        }

        parser_t& parser_t::Float32(f32 _min, f32 _max)
        {
            machine_t* machine = (machine_t*)m_buffer.data();
            machine->Float32(_min, _max);
            return *this;
        }

        parser_t& parser_t::Float64(f64 _min, f64 _max)
        {
            machine_t* machine = (machine_t*)m_buffer.data();
            machine->Float64(_min, _max);
            return *this;
        }

        parser_t& parser_t::Email()
        {
            machine_t* machine = (machine_t*)m_buffer.data();
            machine->Email();
            return *this;
        }

        parser_t& parser_t::IPv4()
        {
            machine_t* machine = (machine_t*)m_buffer.data();
            machine->IPv4();
            return *this;
        }

        parser_t& parser_t::Host()
        {
            machine_t* machine = (machine_t*)m_buffer.data();
            machine->Host();
            return *this;
        }

        parser_t& parser_t::Date()
        {
            machine_t* machine = (machine_t*)m_buffer.data();
            machine->Date();
            return *this;
        }

        parser_t& parser_t::Time()
        {
            machine_t* machine = (machine_t*)m_buffer.data();
            machine->Time();
            return *this;
        }

        parser_t& parser_t::Phone()
        {
            machine_t* machine = (machine_t*)m_buffer.data();
            machine->Phone();
            return *this;
        }

        parser_t& parser_t::ServerAddress()
        {
            machine_t* machine = (machine_t*)m_buffer.data();
            machine->ServerAddress();
            return *this;
        }

        parser_t& parser_t::URI()
        {
            machine_t* machine = (machine_t*)m_buffer.data();
            machine->URI();
            return *this;
        }

    } // namespace xparser2

} // namespace xcore
