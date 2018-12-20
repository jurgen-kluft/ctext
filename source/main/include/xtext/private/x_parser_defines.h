#define MANIPULATOR1(_NAME_)                                 \
    class _NAME_ : public TokenizerInterface                 \
    {                                                        \
    private:                                                 \
        TokenizerInterface& _Tokenizer;                      \
                                                             \
    public:                                                  \
        _NAME_(TokenizerInterface& tok) : _Tokenizer(tok) {} \
        virtual bool Check(StringReader&);                   \
    }

#define MANIPULATOR2(_NAME_)                                                                                 \
    class _NAME_ : public TokenizerInterface                                                                 \
    {                                                                                                        \
    private:                                                                                                 \
        TokenizerInterface& _TokenizerA;                                                                     \
        TokenizerInterface& _TokenizerB;                                                                     \
    public:                                                                                                  \
        _NAME_(TokenizerInterface& toka, TokenizerInterface& tokb) : _TokenizerA(toka), _TokenizerB(tokb) {} \
        virtual bool Check(StringReader&);                                                                   \
    }

#define MANIPULATOR11(_NAME_, _MTYPE1_, _MNAME1_)                                          \
    class _NAME_ : public TokenizerInterface                                               \
    {                                                                                      \
    private:                                                                               \
        _MTYPE1_            _MNAME1_;                                                      \
        TokenizerInterface& _TokenizerA;                                                   \
    public:                                                                                \
        _NAME_(_MTYPE1_ m1, TokenizerInterface& toka) : _MNAME1_(m1), _TokenizerA(toka) {} \
        virtual bool Check(StringReader&);                                                 \
    }

#define MANIPULATOR_MINMAX(_NAME_, _MTYPE_, _MIN_, _MAX_)                                                 \
    class _NAME_ : public TokenizerInterface                                                                          \
    {                                                                                                                 \
    private:                                                                                                          \
        _MTYPE_            _Min;                                                                                 \
        _MTYPE_            _Max;                                                                                 \
        TokenizerInterface& _TokenizerA;                                                                              \
    public:                                                                                                           \
        _NAME_(_MTYPE_ min, _MTYPE_ max, TokenizerInterface& toka) : _Min(min), _Max(max), _TokenizerA(toka) {} \
        _NAME_(_MTYPE_ max, TokenizerInterface& toka) : _Min(_MIN_), _Max(max), _TokenizerA(toka) {} \
        _NAME_(TokenizerInterface& toka) : _Min(_MIN_), _Max(_MAX_), _TokenizerA(toka) {} \
        virtual bool Check(StringReader&);                                                                            \
    }

#define MANIPULATOR12(_NAME_, _MTYPE1_, _MNAME1_, _MTYPE2_, _MNAME2_)                                                 \
    class _NAME_ : public TokenizerInterface                                                                          \
    {                                                                                                                 \
    private:                                                                                                          \
        _MTYPE1_            _MNAME1_;                                                                                 \
        _MTYPE2_            _MNAME2_;                                                                                 \
        TokenizerInterface& _TokenizerA;                                                                              \
    public:                                                                                                           \
        _NAME_(_MTYPE1_ m1, _MTYPE2_ m2, TokenizerInterface& toka) : _MNAME1_(m1), _MNAME2_(m2), _TokenizerA(toka) {} \
        virtual bool Check(StringReader&);                                                                            \
    }

#define FILTER1(_NAME_)                      \
    class _NAME_ : public TokenizerInterface \
    {                                        \
    public:                                  \
        _NAME_() {}                          \
        virtual bool Check(StringReader&);   \
    }

#define FILTER11(_NAME_, _MTYPE1_, _MNAME1_)  \
    class _NAME_ : public TokenizerInterface  \
    {                                         \
    private:                                  \
        _MTYPE1_ _MNAME1_;                    \
                                              \
    public:                                   \
        _NAME_(_MTYPE1_ m1) : _MNAME1_(m1) {} \
        virtual bool Check(StringReader&);    \
    }

#define FILTER12(_NAME_, _MTYPE1_, _MNAME1_, _MTYPE2_, _MNAME2_)         \
    class _NAME_ : public TokenizerInterface                             \
    {                                                                    \
    private:                                                             \
        _MTYPE1_ _MNAME1_;                                               \
        _MTYPE2_ _MNAME2_;                                               \
                                                                         \
    public:                                                              \
        _NAME_(_MTYPE1_ m1, _MTYPE2_ m2) : _MNAME1_(m1), _MNAME2_(m2) {} \
        virtual bool Check(StringReader&);                               \
    }

#define FILTER_MINMAX(_NAME_, _MTYPE_, _MIN_, _MAX_)                    \
    class _NAME_ : public TokenizerInterface                       \
    {                                                              \
    private:                                                       \
        _MTYPE_ _Min;                                              \
        _MTYPE_ _Max;                                              \
                                                                   \
    public:                                                        \
        _NAME_(_MTYPE_ min, _MTYPE_ max) : _Min(min), _Max(max) {} \
        _NAME_(_MTYPE_ max) : _Min(_MIN_), _Max(max) {}            \
        _NAME_() : _Min(_MIN_), _Max(_MAX_) {}                     \
        virtual bool Check(StringReader&);                         \
    }


#define UTIL1(_NAME_)                        \
    class _NAME_ : public TokenizerInterface \
    {                                        \
    public:                                  \
        virtual bool Check(StringReader&);   \
    }
