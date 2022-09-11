# JIT Compiler for Brainf*ck

What is says on the tin.

## Compiling

```console
$ gcc ./bfjit.c ./asm.c -o ./bfjit
```

## Running (Hello World)

Save [this program](https://en.wikipedia.org/wiki/Brainfuck#Hello_World!) to `hello.bf`:
```
++++++++[>++++[>++>+++>+++>+<<<<-]>+>+>->>+[<]<-]>>.>---.+++++++..+++.>>.<-.<.+++.------.--------.>>+.>++.
```

```console
$ ./bfjit ./hello.bf
> Hello World!
```

More examples can be found [here](http://brainfuck.org/).

## Disassembly
Dump the compiled machine code to a file:
```console
$ ./bfjit ./hello.bf ./tmp.bin
```

Then see the disassembly:
```
$ ./disasm.sh ./tmp.bin
```