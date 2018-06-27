#!/usr/bin/env python3

import os
import sys
import signal
import json

TIMEOUT = 30
FAIL = '\033[91m' + "FAIL: " + '\033[0m'
PASS = '\033[0;32m' + "PASS: " + '\033[0m'

def get_files(basepath, basedir):
  return [dir for dir in os.listdir(basepath) if os.path.isdir(os.path.join(basepath, dir))]


def main():
  if len(sys.argv) < 3:
    print("usage: runtests.py <path to ana> <path to test suite>")
    sys.exit(1)

  ana = os.path.realpath(sys.argv[1])
  basepath = os.path.realpath(sys.argv[2])
  files = get_files(basepath, sys.argv[2])

  failed = 0
  passed = 0

  print("Path to Ana Runtime: %s" % ana)
  print("Test Suite Parent Directory: %s" % basepath)

  for dir in files:

    for testcase in os.listdir(os.path.join(basepath, dir)):

      fullpath = os.path.join(os.path.join(basepath, dir), testcase)

      if not os.path.isfile(fullpath):
        
        print("warning: skipping %s/%s, as it's not a regular file" % (dir, testcase))
        continue

      contents = open(fullpath).read().split(os.linesep)
      expecation = "PASS"

      if contents[0][:2] == '//':
        try:
          spec = json.loads(contents[0][2:])
          expecation = spec['Expect'].upper()
        except json.JSONDecodeError as e:
          print(e)

      pid = os.fork()

      if pid == 0:
        # In the child  
        stdout = open("/dev/null");
        # Stdout
        os.dup2(stdout.fileno(), 1)
        # Stderr
        os.dup2(stdout.fileno(), 2)

        os.execl(ana, ana.split("/").pop(), (fullpath))

        stdout.close()

      else:

        while True:

          childpid, status = os.waitpid(pid, 0)

          # This might be for SIGGSEV, but in any case any signal is fatal
          if os.WIFSIGNALED(status) and expecation != 'FAIL':
            
            print("%s: %s/%s " % (FAIL, dir, fullpath.split("/").pop()))
            
            failed += 1

          # ana runtime sets exit status to 1 in case of an exception
          elif os.WIFEXITED(status) and os.WEXITSTATUS(status) != 0 and expecation != 'FAIL':

            print("%s: %s/%s " % (FAIL, dir, fullpath.split("/").pop()))
            
            failed += 1

          else:

            print("%s: %s/%s " % (PASS, dir, fullpath.split("/").pop()))
            passed +=1

          break;

  print("")
  print("Failures: %d" % failed)
  print("Passed: %d" % passed)

  return failed > 0

if __name__ == "__main__":
  sys.exit(main())