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
ana --opcodes tests/grammar/if-statements/if-statement.ana
```

```
Ana Virtual Machine Disassembler
  Frame: __main__
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

0    : LOAD_CONST               0
1    : LOAD_CONST               1
2    : DEFINE_FUNCTION          0
3    : LOAD_CONST               2
4    : LOAD_CONST               3
5    : DEFINE_FUNCTION          0
6    : LOAD_CONST               4
7    : LOAD_CONST               5
8    : DEFINE_FUNCTION          0
9    : LOAD_CONST               6
10   : LOAD_CONST               7
11   : DEFINE_FUNCTION          0
12   : LOAD_CONST               10
13   : LOAD_CONST               11
14   : DEFINE_FUNCTION          0
15   : LOAD_CONST               15
16   : LOAD_CONST               16
17   : DEFINE_FUNCTION          0
18   : LOAD_CONST               17
19   : LOAD_NAME                12
20   : CALL                     1
21   : LOAD_CONST               18
22   : LOAD_NAME                13
23   : CALL                     2
24   : LOAD_CONST               19
25   : LOAD_NAME                14
26   : CALL                     1
27   : LOAD_CONST               20
28   : IUNARYMINUS              0
29   : LOAD_NAME                15
30   : CALL                     2
31   : LOAD_CONST               21
32   : IRETURN                  0
```
## Building
- `git clone https://github.com/analang/ana`
- `cd ana`
- `make`
- `ana --version`

## Features

-   Virtual machine
-   source to bytecode compiler, intepreted by the VM
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
