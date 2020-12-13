#include "xbase/x_target.h"
#include "xbase/x_debug.h"
#include "xbase/x_runes.h"
#include "xtext/x_text.h"

namespace xcore
{
    namespace parser_design
    {
        class machine_t
        {
        public:
            enum eOpcode
            {
                eNOP,
                ePop,
                eNot,
                eOr,
                eAnd,
                eSequence,
                eSequence3,
                eWithin,
                eTimes,
                eOneOrMore,
                eZeroOrMore,
                eZeroOrOne,
                eWhile,
                eUntil,
                eExtract,
                eReturnToCallback,
                eEnclosed,
                eAny,
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
                eIPv4,
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
                OPERAND_WRITER,
            };

            enum eExtractChannels
            {
                EMAIL_NAME = 0,
                EMAIL_DOMAIN = 1,
            };

        private:
            struct instr_t
            {
                instr_t() : m_opcode(eNOP), m_operand_cnt(0), m_pop_loc(0) {}
                instr_t(eOpcode o) : m_opcode(o), m_operand_cnt(0), m_pop_loc(0) {}
                u8  m_opcode;
                u8  m_operand_cnt;
                u8  m_operand_type[2];
                u16 m_operand[2];
                u32 m_pop_loc;
            };

            s32     m_pc;
            s32     m_pc_cur;
            s32     m_pc_max;
            instr_t m_code[256];
            u32     m_operands_write_at;
            u32     m_operands_write_max;
            u64*    m_operands;

            void             write_operand(instr_t& instr, s32 operand) {}
            void             write_operand(instr_t& instr, u32 operand) {}
            void             write_operand(instr_t& instr, s64 operand) {}
            void             write_operand(instr_t& instr, u64 operand) {}
            void             write_operand(instr_t& instr, f32 operand) {}
            void             write_operand(instr_t& instr, f64 operand) {}
            void             write_operand(instr_t& instr, xtext::reader_t const&) {}
            s32              popop_s32();
            s64              popop_s64();
            u32              popop_u32();
            uchar32          popop_uchar32();
            u64              popop_u64();
            f32              popop_f32();
            f64              popop_f64();
            xtext::reader_t& popop_reader();

            xtext::writer_t* get_writer(eExtractChannels channel);

            struct context_t
            {
                context_t(xtext::reader_t const& _reader, xtext::reader_t::cursor_t& _cursor) : reader(_reader), cursor(_cursor) {}
                const xtext::reader_t&     reader;
                xtext::reader_t::cursor_t& cursor;
            };

        public:
            bool execute(xtext::reader_t const& reader, xtext::reader_t::cursor_t& cursor);

            machine_t& Pop();
            machine_t& Run(machine_t const& m);

            machine_t& Not()
            {
                m_code[m_pc_cur++] = instr_t(eNot);
                return *this;
            }
            machine_t& Or()
            {
                m_code[m_pc_cur++] = instr_t(eOr);
                return *this;
            }
            machine_t& And()
            {
                m_code[m_pc_cur++] = instr_t(eAnd);
                return *this;
            }
            machine_t& Sequence()
            {
                m_code[m_pc_cur++] = instr_t(eSequence);
                return *this;
            }
            machine_t& Within(s32 _min = 0, s32 _max = 0x7fffffff)
            {
                instr_t instr(eWithin);
                write_operand(instr, _min);
                write_operand(instr, _max);
                m_code[m_pc_cur++] = instr;
                return *this;
            }
            machine_t& Times(s32 _count)
            {
                instr_t instr(eTimes);
                write_operand(instr, _count);
                m_code[m_pc_cur++] = instr;
                return *this;
            }
            machine_t& OneOrMore()
            {
                m_code[m_pc_cur++] = instr_t(eOneOrMore);
                return *this;
            }
            machine_t& ZeroOrMore()
            {
                m_code[m_pc_cur++] = instr_t(eZeroOrMore);
                return *this;
            }
            machine_t& ZeroOrOne()
            {
                m_code[m_pc_cur++] = instr_t(eZeroOrOne);
                return *this;
            }
            machine_t& While()
            {
                m_code[m_pc_cur++] = instr_t(eWhile);
                return *this;
            }
            machine_t& Until()
            {
                m_code[m_pc_cur++] = instr_t(eUntil);
                return *this;
            }
            machine_t& Extract(eExtractChannels channel)
            {
                instr_t instr(eExtract);
                write_operand(instr, (u32)channel);
                m_code[m_pc_cur++] = instr;
                return *this;
            }
            machine_t& Enclosed(uchar32 _open, uchar32 _close)
            {
                instr_t instr(eEnclosed);
                write_operand(instr, _open);
                write_operand(instr, _close);
                m_code[m_pc_cur++] = instr;
                return *this;
            }
            machine_t& Any()
            {
                m_code[m_pc_cur++] = instr_t(eAny);
                return *this;
            }
            machine_t& In(xtext::reader_t const& _chars)
            {
                instr_t instr(eIn);
                write_operand(instr, _chars);
                m_code[m_pc_cur++] = instr;
                return *this;
            }
            machine_t& Between(uchar32 _from, uchar32 _until)
            {
                instr_t instr(eBetween);
                write_operand(instr, _from);
                write_operand(instr, _until);
                m_code[m_pc_cur++] = instr;
                return *this;
            }
            machine_t& Alphabet()
            {
                m_code[m_pc_cur++] = instr_t(eAlphabet);
                return *this;
            }
            machine_t& Digit()
            {
                m_code[m_pc_cur++] = instr_t(eDigit);
                return *this;
            }
            machine_t& Hex()
            {
                m_code[m_pc_cur++] = instr_t(eHex);
                return *this;
            }
            machine_t& AlphaNumeric()
            {
                m_code[m_pc_cur++] = instr_t(eAlphaNumeric);
                return *this;
            }
            machine_t& Exact(xtext::reader_t const& _text)
            {
                instr_t instr(eExact);
                write_operand(instr, _text);
                m_code[m_pc_cur++] = instr;
                return *this;
            }
            machine_t& Like(xtext::reader_t const& _text)
            {
                instr_t instr(eLike);
                write_operand(instr, _text);
                m_code[m_pc_cur++] = instr;
                return *this;
            }
            machine_t& WhiteSpace()
            {
                m_code[m_pc_cur++] = instr_t(eWhiteSpace);
                return *this;
            }
            machine_t& Is(uchar32 _c)
            {
                instr_t instr(eIs);
                write_operand(instr, _c);
                m_code[m_pc_cur++] = instr;
                return *this;
            }
            machine_t& Word()
            {
                m_code[m_pc_cur++] = instr_t(eWord);
                return *this;
            }
            machine_t& EndOfText()
            {
                m_code[m_pc_cur++] = instr_t(eEndOfText);
                return *this;
            }
            machine_t& EndOfLine()
            {
                m_code[m_pc_cur++] = instr_t(eEndOfLine);
                return *this;
            }
            machine_t& Unsigned32(u32 _min = 0, u32 _max = 0xffffffff)
            {
                instr_t instr(eUnsigned32);
                write_operand(instr, _min);
                write_operand(instr, _max);
                m_code[m_pc_cur++] = instr;
                return *this;
            }
            machine_t& Unsigned64(u64 _min = 0, u64 _max = 0xffffffffffffffffUL)
            {
                instr_t instr(eUnsigned64);
                write_operand(instr, _min);
                write_operand(instr, _max);
                m_code[m_pc_cur++] = instr;
                return *this;
            }
            machine_t& Integer32(s32 _min = 0, s32 _max = 0x7fffffff)
            {
                instr_t instr(eInteger32);
                write_operand(instr, _min);
                write_operand(instr, _max);
                m_code[m_pc_cur++] = instr;
                return *this;
            }
            machine_t& Integer64(s64 _min = 0, s64 _max = 0x7fffffffffffffffL)
            {
                instr_t instr(eInteger64);
                write_operand(instr, _min);
                write_operand(instr, _max);
                m_code[m_pc_cur++] = instr;
                return *this;
            }
            machine_t& Float32(f32 _min = 0.0f, f32 _max = 3.402823e+38f)
            {
                instr_t instr(eFloat32);
                write_operand(instr, _min);
                write_operand(instr, _max);
                m_code[m_pc_cur++] = instr;
                return *this;
            }
            machine_t& Float64(f64 _min = 0.0, f64 _max = 3.402823e+38f)
            {
                instr_t instr(eFloat64);
                write_operand(instr, _min);
                write_operand(instr, _max);
                m_code[m_pc_cur++] = instr;
                return *this;
            }

            machine_t& Email();
            machine_t& Host();
            machine_t& Date() { return *this; }
            machine_t& Time() { return *this; }
            machine_t& IPv4();
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
            bool fnExtract(context_t& ctxt, eExtractChannels channel);
            bool fnEnclosed(context_t& ctxt, uchar32 _open, uchar32 _close);
            bool fnAny(context_t& ctxt);
            bool fnIn(context_t& ctxt, xtext::reader_t const& _chars);
            bool fnBetween(context_t& ctxt, uchar32 _from, uchar32 _until);
            bool fnAlphabet(context_t& ctxt);
            bool fnDigit(context_t& ctxt);
            bool fnHex(context_t& ctxt);
            bool fnAlphaNumeric(context_t& ctxt);
            bool fnExact(context_t& ctxt, xtext::reader_t const& _text); // Case-Sensitive
            bool fnLike(context_t& ctxt, xtext::reader_t const& _text);  // Case-Insensitive
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
            // clang-format off
            Sequence();
                Extract(EMAIL_NAME);        // e.g. john.doe
                    OneOrMore();
                        Or();
                            AlphaNumeric();
                            In("!#$%&'*+/=?^_`{|}~-");
                        Pop();
                    Pop();
                    ZeroOrMore();
                        Sequence();
                            Or();
                                Is('.');
                                Is('_');
                            Pop();
                            Or();
                                AlphaNumeric();
                                In("!#$%&'*+/=?^_`{|}~-");
                            Pop();
                        Pop();
                    Pop();
                Pop();
                Is('@');
                Extract(EMAIL_DOMAIN);        // e.g. hotmail.com
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
                    Sequence();
                        And();
                            Within(1,3);
                                Digit();
                            Pop();
                            Unsigned32(0, 255);
                        Pop();
                        Is('.');
                    Pop();
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

        bool machine_t::fnExec(context_t& ctxt)
        {
            eOpcode o = (eOpcode)m_code[m_pc].m_opcode;

            //@NOTE: When a fn that requires a Pop returns false we need to jump over any existing opcodes to arrive
            //       at the Pop opcode.

            bool result;
            switch (o)
            {
                case eOr: result = fnOr(ctxt); break;
                case eNot: result = fnNot(ctxt); break;
                case eAnd: result = fnAnd(ctxt); break;
                case eSequence: result = fnSequence(ctxt); break;
                case eWithin: result = fnWithin(ctxt, popop_s32(), popop_s32()); break;
                case eTimes: result = fnTimes(ctxt, popop_s32()); break;
                case eOneOrMore: result = fnOneOrMore(ctxt); break;
                case eZeroOrMore: result = fnZeroOrMore(ctxt); break;
                case eZeroOrOne: result = fnZeroOrOne(ctxt); break;
                case eWhile: result = fnWhile(ctxt); break;
                case eUntil: result = fnUntil(ctxt); break;
                case eExtract: result = fnExtract(ctxt, (eExtractChannels)popop_u32()); break;
                case eEnclosed: result = fnEnclosed(ctxt, popop_uchar32(), popop_uchar32()); break;
                case eAny: result = fnAny(ctxt); break;
                case eIn: result = fnIn(ctxt, popop_reader()); break;
                case eBetween: result = fnBetween(ctxt, popop_uchar32(), popop_uchar32()); break;
                case eAlphabet: result = fnAlphabet(ctxt); break;
                case eDigit: result = fnDigit(ctxt); break;
                case eHex: result = fnHex(ctxt); break;
                case eAlphaNumeric: result = fnAlphaNumeric(ctxt); break;
                case eExact: result = fnExact(ctxt, popop_reader()); break;
                case eLike: result = fnLike(ctxt, popop_reader()); break;
                case eWhiteSpace: result = fnWhiteSpace(ctxt); break;
                case eIs: result = fnIs(ctxt, popop_uchar32()); break;
                case eWord: result = fnWord(ctxt); break;
                case eEndOfText: result = fnEndOfText(ctxt); break;
                case eEndOfLine: result = fnEndOfLine(ctxt); break;
                case eUnsigned32: result = fnUnsigned32(ctxt, popop_u32(), popop_u32()); break;
                case eUnsigned64: result = fnUnsigned64(ctxt, popop_u64(), popop_u64()); break;
                case eInteger32: result = fnInteger32(ctxt, popop_s32(), popop_s32()); break;
                case eInteger64: result = fnInteger64(ctxt, popop_s64(), popop_s64()); break;
                case eFloat32: result = fnFloat32(ctxt, popop_f32(), popop_f32()); break;
                case eFloat64: result = fnFloat64(ctxt, popop_f64(), popop_f64()); break;
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
            xtext::reader_t::cursor_t const cursor = ctxt.cursor;
            xtext::reader_t::cursor_t       best   = ctxt.cursor;
            while (!fnOpcodeIs(machine_t::ePop))
            {
                ctxt.cursor = cursor;
                if (!fnExec(ctxt))
                {
                    ctxt.cursor = cursor;
                    return false;
                }
                best = ctxt.cursor.furthest(best);
            }
            return true;
        }

        bool machine_t::fnSequence(context_t& ctxt)
        {
            xtext::reader_t::cursor_t start = ctxt.cursor;
            while (!fnOpcodeIs(ePop))
            {
                if (!fnExec(ctxt))
                {
                    ctxt.cursor = start;
                    return false;
                }
            }
            return true;
        }

        bool machine_t::fnWithin(context_t& ctxt, s32 _min, s32 _max)
        {
            xtext::reader_t::cursor_t const cursor = ctxt.cursor;
            s32                             pc     = m_pc;
            s32                             i      = 0;
            while (i < _max)
            {
                do
                {
                    if (!fnExec(ctxt))
                    {
                        ctxt.cursor = cursor;
                        return false;
                    }
                } while (!fnOpcodeIs(ePop));

                m_pc = pc;
                i += 1;
            }

            if (i >= _min && i <= _max)
                return true;

            ctxt.cursor = cursor;
            return false;
        }
        bool machine_t::fnTimes(context_t& ctxt, s32 _count) { return fnWithin(ctxt, _count, _count); }
        bool machine_t::fnOneOrMore(context_t& ctxt) { return fnWithin(ctxt, 1, 0x7fffffff); }
        bool machine_t::fnZeroOrMore(context_t& ctxt) { return fnWithin(ctxt, 0, 0x7fffffff); }
        bool machine_t::fnZeroOrOne(context_t& ctxt) { return fnWithin(ctxt, 0, 1); }
        bool machine_t::fnWhile(context_t& ctxt) { return fnWithin(ctxt, 0, 0x7fffffff); }
        bool machine_t::fnUntil(context_t& ctxt)
        {
            xtext::reader_t::cursor_t const cursor = ctxt.cursor;
            s32                             pc     = m_pc;
            s32                             i      = 0;
            while (!fnEndOfText(ctxt))
            {
                do
                {
                    if (!fnExec(ctxt))
                        break;
                } while (!fnOpcodeIs(ePop));

                if (fnOpcodeIs(ePop))
                    return true; // We found the 'until'

                m_pc = pc;                     // Reset program-counter
                ctxt.reader.skip(ctxt.cursor); // Advance reader
            }
            ctxt.cursor = cursor;
            return false;
        }
        bool machine_t::fnExtract(context_t& ctxt, eExtractChannels channel)
        {
            xtext::reader_t::cursor_t start = ctxt.cursor;
            while (!fnOpcodeIs(ePop))
            {
                if (!fnExec(ctxt))
                {
                    return false;
                }
            };
            xtext::reader_t           view      = ctxt.reader.select(start, ctxt.cursor);
            xtext::writer_t*          writer    = get_writer(channel);
            if (writer != nullptr)
            {
                xtext::writer_t::cursor_t outcursor = writer->get_cursor();
                writer->write(outcursor, view);
            }
            return true;
        }
        bool machine_t::fnEnclosed(context_t& ctxt, uchar32 _open, uchar32 _close)
        {
            xtext::reader_t::cursor_t start = ctxt.cursor;
            if (ctxt.reader.peek(ctxt.cursor) != _open)
                return false;
            ctxt.reader.skip(ctxt.cursor);
            if (!fnExec(ctxt))
            {
                ctxt.cursor = start;
                return false;
            }
            if (ctxt.reader.peek(ctxt.cursor) != _close)
            {
                ctxt.cursor = start;
                return false;
            }
            ctxt.reader.skip(ctxt.cursor);
            return true;
        }
        bool machine_t::fnAny(context_t& ctxt)
        {
            ctxt.reader.skip(ctxt.cursor);
            return true;
        }
        bool machine_t::fnIn(context_t& ctxt, xtext::reader_t const& _chars)
        {
            xtext::reader_t::cursor_t ccursor = _chars.get_cursor();
            uchar32 const             s       = ctxt.reader.peek(ctxt.cursor);
            while (_chars.valid(ccursor))
            {
                uchar32 const c = _chars.read(ccursor);
                if (c == s)
                {
                    ctxt.reader.skip(ctxt.cursor);
                    return true;
                }
            }
            return false;
        }
        bool machine_t::fnBetween(context_t& ctxt, uchar32 _from, uchar32 _until)
        {
            uchar32 const s = ctxt.reader.peek(ctxt.cursor);
            if (s >= _from && s <= _until)
            {
                ctxt.reader.skip(ctxt.cursor);
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
            ctxt.reader.skip(ctxt.cursor);
            return true;
        }
        bool machine_t::fnDigit(context_t& ctxt)
        {
            if (!fnBetween(ctxt, '0', '9'))
            {
                return false;
            }
            ctxt.reader.skip(ctxt.cursor);
            return true;
        }
        bool machine_t::fnHex(context_t& ctxt)
        {
            if (fnBetween(ctxt, 'a', 'f') || fnBetween(ctxt, 'A', 'F') || fnBetween(ctxt, '0', '9'))
            {
                ctxt.reader.skip(ctxt.cursor);
                return true;
            }
            return false;
        }
        bool machine_t::fnAlphaNumeric(context_t& ctxt)
        {
            if (fnDigit(ctxt) || fnAlphabet(ctxt))
            {
                ctxt.reader.skip(ctxt.cursor);
                return true;
            }
            return false;
        }
        bool machine_t::fnExact(context_t& ctxt, xtext::reader_t const& _text)
        {
            xtext::reader_t::cursor_t tcursor = _text.get_cursor();
            xtext::reader_t::cursor_t cursor  = ctxt.cursor;
            while (_text.valid(tcursor))
            {
                uchar32 const s = ctxt.reader.peek(cursor);
                uchar32 const c = _text.read(tcursor);
                if (c != s)
                {
                    return false;
                }
            }
            ctxt.cursor = cursor;
            return true;
        }
        bool machine_t::fnLike(context_t& ctxt, xtext::reader_t const& _text)
        {
            xtext::reader_t::cursor_t tcursor = _text.get_cursor();
            xtext::reader_t::cursor_t cursor  = ctxt.cursor;
            while (_text.valid(tcursor))
            {
                uchar32 const s = ctxt.reader.peek(cursor);
                uchar32 const c = _text.read(tcursor);
                if (c != s && utf32::to_lower(c) != utf32::to_lower(c))
                {
                    return false;
                }
            }
            ctxt.cursor = cursor;
            return true;
        }
        bool machine_t::fnWhiteSpace(context_t& ctxt)
        {
            uchar32 const s = ctxt.reader.peek(ctxt.cursor);
            if (s == ' ' || s == '\t' || s == '\r')
            {
                ctxt.reader.skip(ctxt.cursor);
                return true;
            }
            return false;
        }
        bool machine_t::fnIs(context_t& ctxt, uchar32 _c)
        {
            uchar32 const s = ctxt.reader.peek(ctxt.cursor);
            if (s == _c)
            {
                ctxt.reader.skip(ctxt.cursor);
                return true;
            }
            return false;
        }
        bool machine_t::fnWord(context_t& ctxt)
        {
            xtext::reader_t::cursor_t cursor = ctxt.cursor;
            if (!fnAlphabet(ctxt))
            {
                ctxt.cursor = cursor;
                return false;
            }
            while (fnAlphabet(ctxt)) {}
            return true;
        }

        bool machine_t::fnEndOfText(context_t& ctxt) { return !ctxt.reader.valid(ctxt.cursor); }

        bool machine_t::fnEndOfLine(context_t& ctxt)
        {
            xtext::reader_t::cursor_t cursor = ctxt.cursor;
            uchar32 const             s1     = ctxt.reader.peek(ctxt.cursor);
            ctxt.reader.skip(ctxt.cursor);
            uchar32 const s2 = ctxt.reader.peek(ctxt.cursor);
            if (s1 == '\r' && s2 == '\n')
            {
                ctxt.reader.skip(ctxt.cursor);
                return true;
            }
            else if (s1 == '\n')
            {
                return true;
            }

            ctxt.cursor = cursor;
            return false;
        }

        bool machine_t::fnUnsigned32(context_t& ctxt, u32 _min, u32 _max) { return fnUnsigned64(ctxt, _min, _max); }
        bool machine_t::fnUnsigned64(context_t& ctxt, u64 _min, u64 _max)
        {
            xtext::reader_t::cursor_t cursor = ctxt.cursor;

            u64 value = 0;
            while (ctxt.reader.valid(cursor))
            {
                uchar32 c = ctxt.reader.peek(cursor);
                if (!(c >= '0' && c <= '9'))
                    break;
                value = (value * 10) + utf32::to_digit(c);
                ctxt.reader.skip(cursor);
            }

            if (cursor == ctxt.cursor)
                return false;

            if (value >= _min && value <= _max)
            {
                ctxt.cursor = cursor;
                return true;
            }
            return false;
        }
        bool machine_t::fnInteger32(context_t& ctxt, s32 _min, s32 _max) { return fnInteger64(ctxt, _min, _max); }
        bool machine_t::fnInteger64(context_t& ctxt, s64 _min, s64 _max)
        {
            xtext::reader_t::cursor_t cursor = ctxt.cursor;

            s64 value = 0;

            uchar32 c = ctxt.reader.peek(cursor);

            bool const is_negative = (c == '-');
            if (is_negative)
                ctxt.reader.skip(cursor);

            while (ctxt.reader.valid(cursor))
            {
                c = ctxt.reader.peek(cursor);
                if (!(c >= '0' && c <= '9'))
                    break;
                value = (value * 10) + utf32::to_digit(c);
                ctxt.reader.skip(cursor);
            }
            if (cursor == ctxt.cursor)
                return false;

            if (is_negative)
                value = -value;

            if (value >= _min && value <= _max)
            {
                ctxt.cursor = cursor;
                return true;
            }
            return false;
        }
        bool machine_t::fnFloat32(context_t& ctxt, f32 _min, f32 _max) { return fnFloat64(ctxt, _min, _max); }
        bool machine_t::fnFloat64(context_t& ctxt, f64 _min, f64 _max)
        {
            xtext::reader_t::cursor_t cursor = ctxt.cursor;

            f64     value       = 0.0;
            uchar32 c           = ctxt.reader.peek(cursor);
            bool    is_negative = c == '-';
            if (is_negative)
                ctxt.reader.skip(cursor);
            while (ctxt.reader.valid(cursor))
            {
                c = ctxt.reader.peek(cursor);
                if (!utf32::is_digit(c))
                    break;
                value = (value * 10.0) + utf32::to_digit(c);
            }
            if (c == '.')
            {
                ctxt.reader.skip(cursor);
                f64 mantissa = 10.0;
                while (ctxt.reader.valid(cursor))
                {
                    c = ctxt.reader.peek(cursor);
                    if (!utf32::is_digit(c))
                        break;
                    value = value + f64(utf32::to_digit(c)) / mantissa;
                    mantissa *= 10.0;
                }
            }
            if (cursor == ctxt.cursor)
                return false;
            if (is_negative)
                value = -value;
            if (value >= _min && value <= _max)
            {
                ctxt.cursor = cursor;
                return true;
            }
            return false;
        }

        static void use_case()
        {
            // Example: This is the run that can validate email addresses
            // Comment: Pretty and readable :-)
            // Thoughts: We can even precompile certain runs to be reused
            //           We can also extract certain parts

            ascii::runez<128> namestr;
            xtext::writer_t   name(namestr);
            ascii::runez<128> domainstr;
            xtext::writer_t   domain(domainstr);

            // clang-format off
            machine_t m;

            // For examples see:
            // - machine_t::Host()
            // - machine_t::IPv4()

            // clang-format on

            xtext::reader_t           reader("john.doe@hotmail.com");
            xtext::reader_t::cursor_t cursor;
            bool                      result = m.execute(reader, cursor);
            // result == true !
        }

    } // namespace parser_design

} // namespace xcore
