# xtext - text parsing

- JSON
- XML
- Custom

It will also consider BOM:

| Bytes       | Utf    | Endian        |
| ----------- | ------ | ------------- |
| 00 00 FE FF | UTF-32 | big-endian    |
| FF FE 00 00 | UTF-32 | little-endian |
| FE FF       | UTF-16 | big-endian    |
| FF FE       | UTF-16 | little-endian |
| EF BB BF    | UTF-8  | na            |

## Warning, below are just wild thoughts, nothing concrete

Thinking of a declaration to be able to parse simple data files.

```c++
struct state
{
    u32         line;
    u32         row;
    u32         reg;
    u32         chr;
    u32         lor;
};

class parser
{
public:
    enum { IGNORE_CASE = 1 };
    
};

struct item
{
    static const u32 SILENT = 0;
    static inline u32 EVENT(u32 i) { return 0x80000000 | i; }

    u64     m_left;
    u64     m_right;
    state   m_state;

    void    parse_setup(parser& p)
    {
        // Character conversion
        p.convert('.', '0', SILENT);
        p.convert('#', '1', SILENT);

        // Shifts
        p.shift("1", 1, &s.reg, SILENT);
        p.shift("0", 0, &s.reg, SILENT);

        // Events
        p.when(" ", EVENT(4));
        p.range('a', 'z', parser::IGNORE_CASE, EVENT(4));
        p.range('0', '9', parser::IGNORE_CASE, EVENT(5));

        // Counters
        p.count("0", &s.chr, SILENT);
        p.count("1", &s.chr, SILENT);
        p.count("/", &s.row, EVENT(1));
        p.count("=>", &s.lor, EVENT(2));
        p.count("\n", &s.line, EVENT(3));

        // Sets
        p.set("/", &s.chr, 0);
        p.set("/", &s.reg, 0);
        p.set("=>", &s.row, 0);
        p.set("\n", &s.row, 0);
        p.set("\n", &s.lor, 0);
    }

    void    parse_event(parser& p, state& s, u32 e)
    {
        // This function is called whenever one of the above 'events' happen.
        // So lets say the input text is 5 lines like this:
        //    .../.../... => .###/..##/.#../###.
        //    #../.../... => ##.#/.###/#.../##.#
        //    .#./.../... => ..../#.##/..../.#.#
        //    ##./.../... => ..#./#.../#.../.###
        //    #.#/.../... => ..##/####/#.#./..##
        //
        // Then we get here on '/', '/', ' => ' etc...
        // Basically whenever our state is changing.
        //

    }

};
```
