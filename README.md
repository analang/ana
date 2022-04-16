## Welcome to Ana
[![Build Status](https://travis-ci.com/analang/ana.svg?branch=master)](https://travis-ci.com/analang/ana)

Ana is a Python, PHP, and C inspired dynamically typed scripting language that implements object-oriented programing along with imperativeness. Learn more.

![Hello World code sample](/assets/HelloWorld.png)


## Bytecode example. 

This screenshot was produced by running ``ana --opcodes -c 'print(1);`

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


```
cd tests/grammar/import
```

```
ana mod.ana
```

The source code of `mod.ana` is below:

```javascript
import logger;
import system.collections.generic.list;

logger.log("Hello World", "Second argument");

list = list.List();
list.add("Hello World");
list.add(logger);

print(list._items);

logger.log("{}", "Welcome to Ana Lang v.0.0.1");
```
