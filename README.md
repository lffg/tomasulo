# tomasulo

## Executing

Use the following make target to run the program:

```bash
make run ARGS="./prog/war-waw.txt"
```

Alternatively, one may want to build and execute in separate steps:

```bash
make
./target/debug/main ./prog/war-waw.txt
```

To compile with optimizations, the `CFG=release` env may be used, e.g.:

```bash
make run CFG=release ARGS="./prog/war-waw.txt"

# or
make CFG=release
./target/release/main ./prog/war-waw.txt
```

## References

- David A. Patterson's Computer Architecture: A Quantitative Approach, 6th
  Edition _(primary reference)_
- [Tomasulo's Algorithm on Wikipedia][wiki-ref]
- [Prof. Dr. Ben H. Juurlink's Dynamic Scheduling Using Tomasulo's Algorithm on
  YouTube][yt-ref]
- Code used to draw an initial understanding:
  - [GZTimeWalker/tomasulo-sim][gh1-ref]
  - [simonpichette/tomasulo][gh2-ref]

[wiki-ref]: https://en.wikipedia.org/wiki/Tomasulo%27s_algorithm
[yt-ref]: https://www.youtube.com/watch?v=y-N0Dsc9LmU
[gh1-ref]: https://github.com/GZTimeWalker/tomasulo-sim
[gh2-ref]: https://github.com/simonpichette/tomasulo

## Implementation remarks

- All registers start with the _value_ 1.
- Memory instructions:
  - Memory stores are essentially a retarded no-op.
  - Memory loads return the effective address. This way we don't have to mock a
    memory while also getting predictable and varying results.
- Division by zero results in a UINT32_MAX value. This way we don't have to
  implement exceptions.
