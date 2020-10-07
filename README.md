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
    u32         chr;
    u32         lor;
};

struct item
{
    u64     m_left;
    u64     m_right;
    state   m_state;

    void    parse_setup(parser& p)
    {
        // Character conversion
        p.convert('.', '0');
        p.convert('#', '1');

        // Counters
        p.counter("0", &m_state.chr);
        p.counter("1", &m_state.chr);
        p.counter("/", &m_state.row);
        p.counter("=>", &m_state.lor);
        p.counter("\n", &m_state.line);

        // Resets
        p.reset("/", &m_state.chr);
        p.reset("=>", &m_state.row);
        p.reset("\n", &m_state.row);
        p.reset("\n", &m_state.lor);
    }

    void    parse_event(parser& p)
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