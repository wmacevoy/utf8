## utf8

Small permissive UTF-8-like encode/decode library for unsigned integers.

This library provides helpers to encode and decode sequences of unsigned 32-bit integers to and from a UTF-8-style byte stream. It intentionally permits encodings beyond the official Unicode UTF-8 limits:

- Supports 1 through 6 byte sequences (up to 31-bit values, 0x7FFFFFFF)
- Decoding is permissive: invalid leading/continuation patterns yield a 1-byte fallback of the raw byte value
- The goal is lossless round-trip of arbitrary 8-bit bytes via decoding and arbitrary 32-bit values via encoding/decoding

API (see `include/utf8.h`):

- `int utf8declen(const char *utf8, int n)`: length (1..6) of first sequence, or -1 if invalid prefix/insufficient bytes
- `int32_t utf8decval(const char *utf8, int len)`: decode a single sequence of length `len` into a value
- `int utf8enclen(uint32_t val)`: number of bytes (1..6) to encode `val`
- `void utf8encval(char *utf8, uint32_t val, int enclen)`: encode `val` into `utf8` using `enclen`
- `int utf8decode(const char *utf8, int utf8len, uint32_t *u32, int u32cap)`: decode buffer to array of values; invalid bytes are copied as their 8-bit value
- `int utf8encode(const uint32_t *u32, int u32len, char *utf8, int utf8cap)`: encode array to bytes; returns required length even if `utf8cap` is too small

Behavior notes:

- Continuation bytes are validated only for structure; overlong forms and out-of-range code points are not rejected
- If `utf8declen` returns -1, `utf8decode` consumes 1 byte and outputs that raw byte value
- `utf8encode` never writes partial sequences; if capacity is exceeded, it stops writing but still returns the full required length

Build and test:

```
make check   # build and run facts tests; compares against `expected/`
make expected # refresh golden output after changes
```

License: MIT (see `LICENSE`).