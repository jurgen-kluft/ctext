# xtext - text parsing

@NOTE: Very much a work in progress! Nothing to see here, move along!

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

