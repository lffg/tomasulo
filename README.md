# tomasulo

## Description

This repository provides a software simulator for the Tomasulo Algorithm, which
is employed for scheduling instructions in computer processors. It allows
out-of-order execution of instructions, which greatly improves the throughput
when using multiple processing units. The algorithm introduces key features like
register renaming, reservation stations, and a common data bus to achieve this
parallel execution. This approach overcomes limitations of earlier techniques
like scoreboarding and enables faster processing.

A naive implementation of out-of-order executions could find not only
read-after-write hazards, but also write-after-write and write-after-read
hazards. Tomasulo's Algorithm solves these issues with the use of the register
renaming technique and a tight control of when instructions buffered in
reservation stations may start executing (i.e., only when all the required
operands are available).

This software allows one to simulate such an algorithm in a step-by-step fashion
by showing the current state of the register file and of each reservation
station. It also allows "time travel" by letting the user rewind to a previous
iteration, which helps when trying to understand the behavior of the algorithm
for more complex programs.

Let's start by examining the simulation for this tiny program:

```
div r1,r6,r7
add r1,r2,r3
```

When one runs the program for that code, one gets the following output:

```txt
Showing cycle number (0)
Current instruction (#0) is [add r2,r2,r2]
==================================================================================
| Kind   | Id    | Busy  | Op    | Qj    | Vj    | Qk    | Vk    | A     | Rem   |
| add    | 1     | 0     |       |       |       |       |       |       |       |
| add    | 2     | 0     |       |       |       |       |       |       |       |
| add    | 3     | 0     |       |       |       |       |       |       |       |
| add    | 4     | 0     |       |       |       |       |       |       |       |
| mul    | 5     | 0     |       |       |       |       |       |       |       |
| mul    | 6     | 0     |       |       |       |       |       |       |       |
| mem    | 7     | 0     |       |       |       |       |       |       |       |
==================================================================================
| reg1  | reg2  | reg3  | reg4  | reg5  | reg6  | reg7  | reg8  |
| 1     | 1     | 1     | 1     | 1     | 1     | 1     | 1     |
==================================================================================
[n]ext, [p]rev, [q]uit =>
```

The following data are displayed:

- The current cycle
- The instruction (if any) which will be issued on the _next_ cycle
- Each available reservation station and its corresponding state
- The register file and the value stored in each register

The user also gets a prompt to forward to the next cycle, rewind to the
previous, or quit the program.

When forwarding to the next iteration,

```txt
Showing cycle number (1)
Current instruction (#1) is [add r2,r2,r2]
==================================================================================
| Kind   | Id    | Busy  | Op    | Qj    | Vj    | Qk    | Vk    | A     | Rem   |
| add    | 1     | 1     | add   | 0     | 1     | 0     | 1     | 0     | 3     |
| add    | 2     | 0     |       |       |       |       |       |       |       |
| add    | 3     | 0     |       |       |       |       |       |       |       |
| add    | 4     | 0     |       |       |       |       |       |       |       |
| mul    | 5     | 0     |       |       |       |       |       |       |       |
| mul    | 6     | 0     |       |       |       |       |       |       |       |
| mem    | 7     | 0     |       |       |       |       |       |       |       |
==================================================================================
| reg1  | reg2  | reg3  | reg4  | reg5  | reg6  | reg7  | reg8  |
| 1     | ->1   | 1     | 1     | 1     | 1     | 1     | 1     |
==================================================================================
[n]ext, [p]rev, [q]uit =>
```

The first instruction (`add r2,r2,r2`) gets allocated to the first additive
reservation station.

Notice also that the destination register, `r2`, gets updated so that it now
_points_ to the reservation station that will _eventually_ produce a value which
it will store. In this case, such reservation station has the id `1`.

The `->N` notation indicates that the register doesn't _actually_ contain a
value; it indicates that a value will be made available by the `N`th reservation
station. If the arrow is not present, the register contains the value being
displayed. Notice also that at the start of the simulation, all registers start
with the value of` `1`.

## Executing

The code was developed using the **clang 16** compiler. when compiling, make
sure the correct version is installed.

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

### Code structure

The representation of instructions, operands, and "operand classes" are defined
in the `inst.cc` and `inst.hh` files. An ad-hoc instruction
recursive-descent-like parser is also implemented in `inst.cc`.

The following grammar is used to parse instructions. Notice that the grammar is
incomplete and doesn't follow any ISAs.

```bnf
<ins>            ::= <op_name> " " <reg> "," <regs_or_offset>
<reg>            ::= "r" [0-9]+
<regs_or_offset> ::= <regs> | <offset>
<regs>           ::= <reg> "," <reg>
<offset>         ::= [0-9]+ "(" <reg> ")"
<op_name>        ::= "add" | "sub" | "mul" | "div" | "sw" | "lw"
```

The registers representation, as well as the register file representation (and
some utility functions related to such concerns) are defined in the `reg.cc` and
`reg.hh` sources. Also, the reservation station representation and corresponding
routines are defined in `station.cc` and `station.hh`.

The main simulation components for the Tomasulo Algorithm are in the `sim.cc`
and `sim.hh` files. Almost all the actual algorithm is implemented there. The
other bits, such as the "time travel" facilities, as well as user-input
handling, are defined in the `main.cc` file.

As usual, the code also contains relevant comments when appropriate, which are
used to describe why some decisions were made, some bug fixes, etc.

### Other remarks

- All registers start with the _value_ 1.
- Memory instructions:
  - Memory stores are essentially a retarded no-op.
  - Memory loads return the effective address. This way we don't have to mock a
    memory while also getting predictable and varying results.
- Division by zero results in a UINT32_MAX value. This way we don't have to
  implement exceptions.
