#!/usr/bin/env python3

import os
import sys
import json
from xml.dom import minidom

def parse_Leak_DefinitelyLost(errornode):
  pass

def parse_error_node(errornode):
  kind = errornode.getElementsByTagName("kind")[0].firstChild.data
  text = errornode.getElementsByTagName("xwhat")[0] \
            .getElementsByTagName("text")[0].firstChild.data

  return { 'kind' : kind, 'message': text }

def parseValgrindOutput(xmlstr):
  output = []
  xmldoc = minidom.parseString(xmlstr)
  for error in xmldoc.getElementsByTagName("error"):
    kind = error.getElementsByTagName("kind")[0].firstChild
    output.append(parse_error_node(error))

  return output

if __name__ == "__main__":
  sys.exit(parseValgrindOutput(sys.stdin.read()));
