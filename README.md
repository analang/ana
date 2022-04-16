## Welcome to Ana
[![Build Status](https://travis-ci.com/analang/ana.svg?branch=master)](https://travis-ci.com/analang/ana)

Ana is a Python, PHP, and C inspired dynamically typed scripting language that implements object-oriented programing along with imperativeness. Learn more.

![Hello World code sample](/assets/HelloWorld.png)


## Bytecode example. 

This screenshot was produced by running:

```bash
ana --opcodes -c 'print(1);'
```

![Compilation output](/assets/CompilationOutput2.png)

## Bytecode example with jump targets

```
ana --opcodes tests/grammar/if-statements/if-statement.ana --function test_if_statement```
```

```
Ana Virtual Machine Disassembler
  Frame: test_if_statement
  Constant Table:
   Index          Value        Type
   0              <function 'r function
   1              readline     string
   2              <function 'p function
   3              print        string
   4              <function 'i function
   5              int          string
   6              <function 't function
   7              typeof       string
   8              1            long
   9              1            long
   10             <function 't function
   11             test_if_stat string
   12             Unexpected r string
   13             ,expected    string
   14             0            long
   15             <function 'e function
   16             expect       string
   17             1            long
   18             1            long
   19             0            long
   20             1            long
   21             0            long
  Jump Targets:
   0              5            long
   1              5            long
  VM Symbols:
   0              push
   1              length
   2              getType
   3              first
   4              length
   5              getBytes
   6              getType
   7              getType
   8              value
   9              value
   10             value
   11             expected
   12             test_if_statement
   13             expect
   14             test_if_statement
   15             expect

0    : LOAD_NAME                8
1    : JMPZ                     0
2    : LOAD_CONST               8
3    : IRETURN                  0
```


## Building
- `git clone https://github.com/analang/ana`
- `cd ana`
- `make`
- `ana --version`

## Features

- Virtual machine
- source to bytecode compiler, intepreted by the VM
-	classes with single layered inheritance
-	dynamically sized arrays
-	built in hash map support
-	variadic functions
-	simple module system
-	built in types
-	exceptions
-	C based runtime extension system
-	garbage collection
-	first class functions

```javascript
import String;

class Logger 
{
    function Logger(buffered)
    {
      self._buffered = buffered;
      self._lines = [];
    }

    function log(fmt, ...args)
    {
      line = String.format(fmt, args);

      if(!_buffered)
      {
        print(line); 
      }
      else 
      {
        self._lines.push(line);
      }
    }
}

logger = Logger(false);

logger.log("{}", "Welcome to Ana Lang v.0.0.1");
```

To run this example, simply execute `ana logger.ana` at your shell of choice. Hint: Make sure `String.ana` is located in the directory of `logger.ana`. It's located in the `lib` directory.
