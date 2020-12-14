#ifndef __XTEXT_XTEXT_H__
#define __XTEXT_XTEXT_H__
#include "xbase/x_target.h"
#ifdef USE_PRAGMA_ONCE
#pragma once
#endif

#include "xbase/x_runes.h"

namespace xcore
{
    namespace xtext
    {
        class reader_t;
        
        class writer_t
        {
        public:
            struct cursor_t
            {
                union runeptr
                {
                    inline runeptr() : m_ascii(nullptr) {}
                    ascii::prune m_ascii;
                    utf8::prune  m_utf8;
                    utf16::prune m_utf16;
                    utf32::prune m_utf32;
                };

                cursor_t& operator=(const cursor_t&);
                bool      operator<(const cursor_t&) const;
                bool      operator>(const cursor_t&) const;
                bool      operator<=(const cursor_t&) const;
                bool      operator>=(const cursor_t&) const;
                bool      operator==(const cursor_t&) const;
                bool      operator!=(const cursor_t&) const;

                runeptr m_cursor;
            };

            writer_t();
            writer_t(char* str, char* end);
            writer_t(utf32::prune str, utf32::prune end);
            writer_t(ascii::runes const& str);
            writer_t(utf8::runes const& str);
            writer_t(utf16::runes const& str);
            writer_t(utf32::runes const& str);
            writer_t(const writer_t&);

            cursor_t get_cursor() const;

            void reset(cursor_t&) const;
            bool valid(cursor_t&) const;
            void write(cursor_t&, uchar32 c);
            void write(cursor_t&, reader_t const& str);

        private:
            union runes
            {
                inline runes() : m_ascii() {}
                ascii::runes m_ascii;
                utf8::runes  m_utf8;
                utf16::runes m_utf16;
                utf32::runes m_utf32;
            };
            runes m_runes;
            s32   m_type;
        };

        class reader_t
        {
        public:
            struct cursor_t
            {
                inline cursor_t() : m_cursor() {}

                union runeptr
                {
                    inline runeptr() : m_ascii(nullptr) {}
                    ascii::pcrune m_ascii;
                    utf8::pcrune  m_utf8;
                    utf16::pcrune m_utf16;
                    utf32::pcrune m_utf32;
                };

                cursor_t& operator=(const cursor_t&);
                bool      operator<(const cursor_t&) const;
                bool      operator>(const cursor_t&) const;
                bool      operator<=(const cursor_t&) const;
                bool      operator>=(const cursor_t&) const;
                bool      operator==(const cursor_t&) const;
                bool      operator!=(const cursor_t&) const;

                cursor_t  furthest(const cursor_t& c) const; 

                runeptr m_cursor;
            };

            reader_t();
            reader_t(const char* str);
            reader_t(utf32::pcrune str);
            reader_t(ascii::crunes const& str);
            reader_t(utf8::crunes const& str);
            reader_t(utf16::crunes const& str);
            reader_t(utf32::crunes const& str);
            reader_t(const writer_t&);
            reader_t(const reader_t&);
            reader_t(const reader_t&, const cursor_t& begin, const cursor_t& end);

            cursor_t get_cursor() const;

            s64     size() const;
            s64     size(cursor_t const&) const;
            void    reset(cursor_t&) const;
            bool    valid(cursor_t const&) const;
            uchar32 peek(cursor_t const&) const;
            uchar32 read(cursor_t&) const;
            void    skip(cursor_t&) const;

            reader_t select(cursor_t const& from, cursor_t const& to) const;

            reader_t& operator=(const reader_t& other);

            bool operator==(const reader_t& other) const;
            bool operator!=(const reader_t& other) const;

        private:
            union crunes
            {
                inline crunes() : m_ascii() {}
                ascii::crunes m_ascii;
                utf8::crunes  m_utf8;
                utf16::crunes m_utf16;
                utf32::crunes m_utf32;
            };
            crunes   m_runes;
            s32      m_type;
            cursor_t m_begin;
            cursor_t m_end;
        };
    } // namespace xtext

} // namespace xcore

#endif