#!/usr/bin/env python3

import os
import sys
import signal

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
        # returns (pid, exit status)
        while True:
          childpid, status = os.waitpid(pid, 0)

          if os.WIFSIGNALED(status):
            
            print("%s: %s " % (FAIL, fullpath.split("/").pop()))
            failed += 1

          elif os.WIFEXITED(status) and os.WEXITSTATUS(status) != 0:

            print("%s: %s/%s " % (FAIL, dir, fullpath.split("/").pop()))
            failed += 1

          else:

            print("%s: %s/%s " % (PASS, dir, fullpath.split("/").pop()))
            passed +=1


          break;

  print("")
  print("Failures: %d" % failed)
  print("Passed: %d" % passed)



if __name__ == "__main__":
  main()