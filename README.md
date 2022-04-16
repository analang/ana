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

Here's an advanced example which implements a JSON parser. The JSON is provided via `stdin` and the `ana` program reads from stdin and converts
the JSON string into an abstract syntax tree. The full source code is [jsonparser.ana](tests/grammar/jsonparser.ana). Note that this JSON parser 
is incomplete. It currently supports objects, integers, and strings. The strings are primitive, and advanced character handling is not supported yet.

```javascript
class AstNode {
  function AstNode(kind) {
    self.kind = kind;
    self.children = [];
  }
}

class Buffer
{
  function Buffer(value)
  {
    self.stream   = value;
    self.position = 0;
  }

  function advance()
  {
    if(self.position == self.stream.length())
    {
      throw "Can't advance buffer. Unexpected EOF";
    }

    return stream[self.position++];
  }

  function top()
  {
    return stream[self.position];
  }
}

function parse_int(buffer)
{
  val = 0;

  while(buffer.top() >= '0' && buffer.top() <= '9')
  {
    val = val * 10 + (int(buffer.top()) - int('0'));

    buffer.advance();
  }

  return val;
}

function parse_str(buffer)
{
  val = '';

  while(buffer.top() != '"')
  {
    val += buffer.top();

    buffer.advance();
  }

  buffer.advance();

  return val;
}

function is_int(val)
{
  return val >= '0' && val <= '9';
}

function is_string(val)
{
  return val == '"';
}

function skip_white(buffer)
{
  while(buffer.top() == ' ' || buffer.top() == '\t')
  {
    buffer.advance();
  }
}

function parse_key(buffer)
{
  skip_white(buffer);

  if(buffer.top() != '"')
  {
    throw 'Expected \'"\', not \'' + buffer.top() + '\' to start a key value pair at position ' + buffer.position;
  }

  key = '';
  buffer.advance();

  while(buffer.top() != '"')
  {
    key = key + buffer.top();

    buffer.advance();
  }

  if(buffer.top() != '"')
  {
    throw "Unexpected end of string literal, missing \"";
  }

  buffer.advance();

  return key;
}

class KeyValuePair
{
  function KeyValuePair(key, value)
  {
    self.key = key;
    self.value = value;
  }
}

function parse_object(buffer)
{ 
  node = AstNode("object");

  if(buffer.top() != '{')
  {
    throw "Expected '{' at position " + buffer.position;
  }

  buffer.advance();

  while(buffer.top() != '}')
  { 
    key = parse_key(buffer);

    skip_white(buffer);

    if(buffer.top() != ':')
    {
      throw "Expected ':' after key";
    }

    buffer.advance();

    skip_white(buffer);

    value = parse(buffer);

    node.children.push(KeyValuePair(key, value));

    buffer.advance();
  }

  return node;
}

function parse(buffer)
{
  if(buffer.top() == '{')
  {
    return parse_object(buffer);
  }
  else if(is_int(buffer.top()))
  {
    return parse_int(buffer);
  }
  else if(is_string(buffer.top()))
  {
    buffer.advance();
    return parse_str(buffer);
  }
  else
  {
    throw "Invalid start of JSON object at position " + buffer.position + ", got '" + buffer.top() + "'";
  }
}

function main()
{
  buffer = '';

  while(line = readline())
  {
    buffer = buffer + line;
  }

  buffer = Buffer(buffer);

  result = parse(buffer);

  foreach(kvpair in result.children)
  {
    print(kvpair.key + ":" + kvpair.value);
  }
}

main();
```