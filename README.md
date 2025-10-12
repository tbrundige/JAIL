JAIL - Just Another Interrupted Language

This is a tiny interpreter for the JAIL language implemented in C with no external libraries.

Compile time errors... right to jail.
Compiled binaries... right to jail.

[![Right to jail](https://img.youtube.com/vi/4zG_eTys-Mw/0.jpg)](https://www.youtube.com/watch?v=4zG_eTys-Mw)

Features in this initial version:
- String types only
- Variable declaration: var name = "value"
- Function definitions: func name(param) { ... }
- Function calls: name(arg)
- String concatenation with +
- Built-in print(...) function

Build:

```
make
./jail example.jail
```

Example:
example.jail

```
var greeting = "Hello"
var name = "World"

func sayHello(who) {
  var msg = greeting + ", " + who + "!!"
  print(msg)
}

sayHello(name)
```
To run:
```
./bin/jail example.jail
```
