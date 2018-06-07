#!/usr/bin/env python

import sys
import os
import re

def getStruct(path):
  regex = re.compile('\s*typedef\s*enum\s*node_kind\s*')
  regexend = re.compile("\s*}\s*node_kind;\s*")
  lines = [x.strip(" ") for x in open(path, "r").read().split("\n")]
  thestruct = []
  instruct = 0

  for line in lines:
    if regex.match(line):
      instruct = 1
      continue
    if(regexend.match(line)):
      break
    if instruct:
      buffer = []
      for char in line:
        if char == ' ' or char == '/' or char == ',':
          break
        else:
          buffer.append(char)
      thestruct.append(''.join(buffer))


  return thestruct


def getAstPath():
  return os.path.realpath("./../src/include/ana_ast.h");


thestruct = getStruct(getAstPath())

lines = [];

lines.append("static const char * const kind_str[] = {");

for i, item in enumerate(thestruct):
    if i + 1 == len(thestruct):
        lines.append('  "%s"' % item);
    else:
        lines.append('  "%s",' % item);

lines.append("};");

output = open('./../src/parser/astkind.c', 'w');

for line in lines:
    if len(line) > 0:
        print(line)
        output.write(line + "\n");

output.close();
