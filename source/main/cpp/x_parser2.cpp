#include "xbase/x_target.h"
#include "xbase/x_debug.h"
#include "xbase/x_buffer.h"
#include "xbase/x_runes.h"

namespace xcore
{
    namespace xparser
    {
        class machine_t
        {
        public:
            enum eOpcode
            {
                eNOP = 0,
                ePop = 1,
                // Manipulators (scope)
                eUsesPop = 0x8000,
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
                eAny = 0x1000,
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
                eIPv4 = 0x2000,
                eHost,
                eEmail,
                ePhone,
                eServerAddress,
                eUri
            };

            enum eOperandType
            {
                OPERAND_UCHAR32,
                OPERAND_U32,
                OPERAND_S32,
                OPERAND_U64,
                OPERAND_S64,
                OPERAND_F32,
                OPERAND_F64,
                OPERAND_READER,
            };

        private:
            struct instr_t
            {
                instr_t()
                    : m_opcode(eNOP)
                    , m_operand(0xffff)
                    , m_pop_loc(0xffff)
                {
                }
                instr_t(eOpcode o)
                    : m_opcode(o)
                    , m_operand(0xffff)
                    , m_pop_loc(0xffff)
                {
                }
                u16 m_opcode;
                u16 m_operand;
                u16 m_pop_loc;
            };

            struct operands_t
            {
                operands_t()
                    : m_writer(m_data, 1024)
                    , m_reader(m_data, 1024)
                {
                }
                xbyte          m_data[1024];
                xbinary_writer m_writer;
                xbinary_reader m_reader;

                void reset()
                {
                    m_writer.reset();
                    m_reader.reset();
                }

                u16 write(u32 opa) { return (u16)m_writer.write(opa); }
                u16 write(u32 opa, u32 opb)
                {
                    u16 const offset = (u16)m_writer.write(opa);
                    m_writer.write(opb);
                    return offset;
                }
                u16 write(s32 opa) { return (u16)m_writer.write(opa); }
                u16 write(s32 opa, s32 opb)
                {
                    u16 const offset = (u16)m_writer.write(opa);
                    m_writer.write(opb);
                    return offset;
                }
                u16 write(s64 opa, s64 opb)
                {
                    u16 const offset = (u16)m_writer.write(opa);
                    m_writer.write(opb);
                    return offset;
                }
                u16 write(u64 opa, u64 opb)
                {
                    u16 const offset = (u16)m_writer.write(opa);
                    m_writer.write(opb);
                    return offset;
                }
                u16 write(f32 opa, f32 opb)
                {
                    u16 const offset = (u16)m_writer.write(opa);
                    m_writer.write(opb);
                    return offset;
                }
                u16 write(f64 opa, f64 opb)
                {
                    u16 const offset = (u16)m_writer.write(opa);
                    m_writer.write(opb);
                    return offset;
                }
                u16 write(runes_reader_t const&) { return 0; }
                s32 read_s32(instr_t& instr, s32 i)
                {
                    s32 value;
                    m_reader.read(value);
                    return value;
                }
                s64 read_s64(instr_t& instr, s32 i)
                {
                    s64 value;
                    m_reader.read(value);
                    return value;
                }
                u32 read_u32(instr_t& instr, s32 i)
                {
                    u32 value;
                    m_reader.read(value);
                    return value;
                }
                uchar32 read_uchar32(instr_t& instr, s32 i) { return (uchar32)read_u32(instr, i); }
                u64     read_u64(instr_t& instr, s32 i)
                {
                    u64 value;
                    m_reader.read(value);
                    return value;
                }
                f32 read_f32(instr_t& instr, s32 i)
                {
                    f32 value;
                    m_reader.read(value);
                    return value;
                }
                f64 read_f64(instr_t& instr, s32 i)
                {
                    f64 value;
                    m_reader.read(value);
                    return value;
                }
                runes_reader_t read_reader(instr_t& instr, s32 i) { return runes_reader_t(); }
            };

            runes_writer_t* get_writer(u32 channel) { return nullptr; }

            s32        m_pc;
            s32        m_code_size;
            s32        m_code_max;
            instr_t    m_code[512];
            operands_t m_operands;
            u16        m_stack[256];
            s32        m_stack_size;
            s32        m_stack_max;

            struct context_t
            {
                context_t(runes_reader_t const& _reader)
                    : reader(_reader)
                {
                }

                crunes_t::ptr_t get_cursor() const { return reader.get_cursor(); }
                void            set_cursor(crunes_t::ptr_t const& c) { reader.set_cursor(c); }

                runes_reader_t reader;
            };

            void handle_pop(eOpcode o)
            {
                if ((o & eUsesPop) == eUsesPop)
                {
                    m_stack[m_stack_size] = m_code_size;
                    m_stack_size++;
                }
                else if (o == ePop)
                {
                    m_stack_size -= 1;
                    u16 pc               = m_stack[m_stack_size];
                    m_code[pc].m_pop_loc = m_code_size;
                }
            }
            machine_t& emit_instr(eOpcode o)
            {
                handle_pop(o);
                ASSERT(m_code_size < m_code_max);
                m_code[m_code_size++] = instr_t(o);
                return *this;
            }
            template <typename T> machine_t& emit_instr(eOpcode o, T _a)
            {
                handle_pop(o);
                ASSERT(m_code_size < m_code_max);
                instr_t instr(o);
                instr.m_operand       = m_operands.write(_a);
                m_code[m_code_size++] = instr;
                return *this;
            }
            template <typename T1, typename T2> machine_t& emit_instr(eOpcode o, T1 _a, T2 _b)
            {
                handle_pop(o);
                ASSERT(m_code_size < m_code_max);
                instr_t instr(o);
                instr.m_operand       = m_operands.write(_a, _b);
                m_code[m_code_size++] = instr;
                return *this;
            }

        public:
            bool execute(runes_reader_t const& reader, crunes_t::ptr_t& cursor)
            {
                context_t ctxt(reader);
                ctxt.reader.set_cursor(cursor);
                return fnExec(ctxt);
            }

            machine_t& Pop() { return emit_instr(ePop); }
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
            machine_t& Extract(u32 channel) { return emit_instr(eTimes, (u32)channel); }
            machine_t& Enclosed(uchar32 _open, uchar32 _close) { return emit_instr(eEnclosed, (u32)_open, (u32)_close); }
            machine_t& Any() { return emit_instr(eAny); }
            machine_t& In(runes_reader_t const& _chars) { return emit_instr(eIn, _chars); }
            machine_t& Between(uchar32 _from, uchar32 _until) { return emit_instr(eBetween, (u32)_from, (u32)_until); }
            machine_t& Alphabet() { return emit_instr(eAlphabet); }
            machine_t& Digit() { return emit_instr(eDigit); }
            machine_t& Hex() { return emit_instr(eHex); }
            machine_t& AlphaNumeric() { return emit_instr(eAlphaNumeric); }
            machine_t& Exact(runes_reader_t const& _text) { return emit_instr(eExact, _text); }
            machine_t& Like(runes_reader_t const& _text) { return emit_instr(eLike, _text); }
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

            bool fnExec(context_t& ctxt);
            bool fnOpcodeIs(eOpcode o) const;
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
            bool fnExtract(context_t& ctxt, u32 channel);
            bool fnEnclosed(context_t& ctxt, uchar32 _open, uchar32 _close);
            bool fnAny(context_t& ctxt);
            bool fnIn(context_t& ctxt, runes_reader_t& _chars);
            bool fnBetween(context_t& ctxt, uchar32 _from, uchar32 _until);
            bool fnAlphabet(context_t& ctxt);
            bool fnDigit(context_t& ctxt);
            bool fnHex(context_t& ctxt);
            bool fnAlphaNumeric(context_t& ctxt);
            bool fnExact(context_t& ctxt, runes_reader_t& _text); // Case-Sensitive
            bool fnLike(context_t& ctxt, runes_reader_t& _text);  // Case-Insensitive
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
        };

        machine_t& machine_t::Email()
        {
            runes_reader_t validchars((ascii::pcrune) "!#$%&'*+/=?^_`{|}~-", 19);
            // clang-format off
            Sequence();
                Extract(0);        // e.g. john.doe
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
                Pop();
                Is('@');
                Extract(1);        // e.g. hotmail.com
                    Host();
                Pop();
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
            instr_t instr = m_code[m_pc++];

            //@NOTE: When a fn that requires a Pop returns false we need to jump over any existing opcodes to arrive
            //       at the Pop opcode.

            bool    result;
            eOpcode o = (eOpcode)instr.m_opcode;
            switch (o)
            {
                case eOr: result = fnOr(ctxt); break;
                case eNot: result = fnNot(ctxt); break;
                case eAnd: result = fnAnd(ctxt); break;
                case eSequence: result = fnSequence(ctxt); break;
                case eWithin: result = fnWithin(ctxt, m_operands.read_s32(instr, 0), m_operands.read_s32(instr, 1)); break;
                case eTimes: result = fnTimes(ctxt, m_operands.read_s32(instr, 0)); break;
                case eOneOrMore: result = fnOneOrMore(ctxt); break;
                case eZeroOrMore: result = fnZeroOrMore(ctxt); break;
                case eZeroOrOne: result = fnZeroOrOne(ctxt); break;
                case eWhile: result = fnWhile(ctxt); break;
                case eUntil: result = fnUntil(ctxt); break;
                case eExtract: result = fnExtract(ctxt, m_operands.read_u32(instr, 0)); break;
                case eEnclosed: result = fnEnclosed(ctxt, m_operands.read_uchar32(instr, 0), m_operands.read_uchar32(instr, 1)); break;
                case eAny: result = fnAny(ctxt); break;
                case eIn: result = fnIn(ctxt, m_operands.read_reader(instr, 0)); break;
                case eBetween: result = fnBetween(ctxt, m_operands.read_uchar32(instr, 0), m_operands.read_uchar32(instr, 1)); break;
                case eAlphabet: result = fnAlphabet(ctxt); break;
                case eDigit: result = fnDigit(ctxt); break;
                case eHex: result = fnHex(ctxt); break;
                case eAlphaNumeric: result = fnAlphaNumeric(ctxt); break;
                case eExact: result = fnExact(ctxt, m_operands.read_reader(instr, 0)); break;
                case eLike: result = fnLike(ctxt, m_operands.read_reader(instr, 0)); break;
                case eWhiteSpace: result = fnWhiteSpace(ctxt); break;
                case eIs: result = fnIs(ctxt, m_operands.read_uchar32(instr, 0)); break;
                case eWord: result = fnWord(ctxt); break;
                case eEndOfText: result = fnEndOfText(ctxt); break;
                case eEndOfLine: result = fnEndOfLine(ctxt); break;
                case eUnsigned32: result = fnUnsigned32(ctxt, m_operands.read_u32(instr, 0), m_operands.read_u32(instr, 1)); break;
                case eUnsigned64: result = fnUnsigned64(ctxt, m_operands.read_u64(instr, 0), m_operands.read_u64(instr, 1)); break;
                case eInteger32: result = fnInteger32(ctxt, m_operands.read_s32(instr, 0), m_operands.read_s32(instr, 1)); break;
                case eInteger64: result = fnInteger64(ctxt, m_operands.read_s64(instr, 0), m_operands.read_s64(instr, 1)); break;
                case eFloat32: result = fnFloat32(ctxt, m_operands.read_f32(instr, 0), m_operands.read_f32(instr, 1)); break;
                case eFloat64: result = fnFloat64(ctxt, m_operands.read_f64(instr, 0), m_operands.read_f64(instr, 1)); break;
                default: result = false;
            }
            return result;
        }

        bool machine_t::fnOpcodeIs(eOpcode o) const { return m_code[m_pc].m_opcode == (u8)o; }

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
        bool machine_t::fnExtract(context_t& ctxt, u32 channel)
        {
            crunes_t::ptr_t start = ctxt.get_cursor();
            while (!fnOpcodeIs(ePop))
            {
                if (!fnExec(ctxt))
                {
                    return false;
                }
            };
            runes_reader_t  view   = ctxt.reader.select(start, ctxt.get_cursor());
            runes_writer_t* writer = get_writer(channel);
            if (writer != nullptr)
            {
                writer->write(view.get_current());
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
        bool machine_t::fnIn(context_t& ctxt, runes_reader_t& _chars)
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
        bool machine_t::fnExact(context_t& ctxt, runes_reader_t& _text)
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
        bool machine_t::fnLike(context_t& ctxt, runes_reader_t& _text)
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

        static void use_case()
        {
            runez_t<ascii::rune, 128> namestr;
            runes_writer_t            name(namestr);
            runez_t<ascii::rune, 128> domainstr;
            runes_writer_t            domain(domainstr);

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

    } // namespace xparser

} // namespace xcore
