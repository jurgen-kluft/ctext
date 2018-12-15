#ifndef __XTEXT_TEXTREADER_H__
#define __XTEXT_TEXTREADER_H__
#include "xbase/x_target.h"
#ifdef USE_PRAGMA_ONCE 
#pragma once 
#endif

#include "xstring/x_string.h"

namespace xtext
{

    class xtextreader
    {
    public:
        virtual bool readLine(xstring& line) = 0;
    };

    class xtextwriter
    {
    public:
        virtual bool writeLine(xstring const& line) = 0;
    };

};

#endif // __XTEXT_TEXTREADER_H__
