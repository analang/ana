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
