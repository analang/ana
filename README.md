## Welcome to Ana
[![Build Status](https://travis-ci.com/analang/ana.svg?branch=master)](https://travis-ci.com/analang/ana)

Ana is a Python, PHP, and C inspired dynamically typed scripting language that implements object-oriented programing along with imperativeness. Learn more.

## Features

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
class Logger 
{
    function Logger()
    {
      self._lines = [];
    }

    function log(fmt, ...args)
    {
      line = String.format(fmt, args);
      
      self._lines.push(line);
    }
}

logger = Logger();

logger.log("{}", "Welcome to Ana Lang v.0.0.1");
```

To run this example, simply execute `ana logger.ana` at your shell of choice.
