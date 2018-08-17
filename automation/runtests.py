#!/usr/bin/env python3

import os
import sys
import signal
import json
import fcntl
import platform
import traceback
from xml.dom import minidom
import valgrindparser

TIMEOUT = 30
FAIL = '\033[91m' + "FAIL" + '\033[0m'
PASS = '\033[0;32m' + "PASS " + '\033[0m'

def get_files(basepath, basedir):
  return [dir for dir in os.listdir(basepath) if os.path.isdir(os.path.join(basepath, dir))]


def valgrind_test(valgrind, anapath, fullpath):
  success = False
  args = [valgrind, "--xml-fd=2", "--xml=yes", "--leak-check=summary", "--show-leak-kinds=all", \
          "-q", anapath, fullpath]
  pipefds = os.pipe()
  readfd = pipefds[0]
  writefd = pipefds[1]
  childex = None
  fl = fcntl.fcntl(readfd, fcntl.F_GETFL)
  fcntl.fcntl(readfd, fcntl.F_SETFL, fl | os.O_NONBLOCK)
  devnull = open("/dev/null");

  pid = os.fork()

  if pid == 0:
    # In the child  

    # Stdout    
    # Ignore Ana language output
    os.dup2(devnull.fileno(), 1)
    # Stderr
    # This has the valgrind output
    os.dup2(writefd, 2)

    try:
      os.execvp(valgrind, args)
    except Exception as e:
      buffer = '-------\n';
      buffer += str(args);
      buffer += '\n';
      buffer += '------\n';
      os.write(writefd, buffer.encode())
      os.write(writefd, traceback.format_exc().encode())
      os._exit(1)
  else:

    while True:
      childpid, status = os.waitpid(pid, os.WNOHANG)

      if childpid == 0:
        continue

      if os.WIFEXITED(status):
        if os.WEXITSTATUS(status) == 0:
          #print("%s: %s: status is %d " % (PASS, fullpath.split("/").pop(), 
          #  os.WEXITSTATUS(status))) 
          contents = b''
          while 1:
            try:
              contents += os.read(readfd, 4096)

            except BlockingIOError:
              break;
              
          #
          # Todo, parse XML, and return False if this has errors
          # Output the leak
          #
          try:
            output = valgrindparser.parseValgrindOutput(contents)
            success = '';
            if len(output) > 0:
              for error in output:
                success += "  %s\n" % error['kind']
                success += "  %s\n" % error['message']
              success = success.encode()
            else:
              success = None
          except:
            success = -1
            print("error parsing valrgind output:")
            print(contents)
            print("end")
            break

        else:
          contents = "error"
          while 1:
            try:
              contents = os.read(readfd, 4096)

            except BlockingIOError:
              break;
          
          success = contents.decode("utf-8");

        break

  os.close(writefd)
  os.close(readfd)
  devnull.close()

  return success

def run_test(anapath, basedir, testpath):
  fullpath = basedir + "/" + testpath

  print(fullpath)

  contents = open(fullpath).read().split(os.linesep)
  expecation = "PASS"
  stdin_data = None

  if contents[0][:2] == '//':
    try:
      spec = json.loads(contents[0][2:])
      expecation = spec['Expect'].upper()
      stdin_data = spec.get('Input')
    except json.JSONDecodeError as e:
      print("warning json.loads:" + e)

  pid = os.fork()

  stdin_pipe = os.pipe()
  stdin_readfd = stdin_pipe[0]
  stdin_writefd = stdin_pipe[1]

  if stdin_data is not None:
    for line in stdin_data:
      os.write(stdin_writefd, line.encode() + b"\n");

  if pid == 0:
    # In the child  
    stdout = open("/dev/null");
    # Stdout
    os.dup2(stdout.fileno(), 1)
    # Stderr
    os.dup2(stdout.fileno(), 2)
    # Stdin
    os.dup2(stdin_readfd, sys.stdin.fileno())

    os.close(stdin_readfd)
    
    os.close(stdin_writefd)

    os.execl(anapath, anapath.split("/").pop(), (fullpath))

    stdout.close()

  else:
    while True:

      childpid, status = os.waitpid(pid, 0)

      # This might be for SIGGSEV, but in any case any signal is fatal
      if os.WIFSIGNALED(status) and expecation != 'FAIL':
        
        print("%s: %s " % (FAIL, fullpath.split("/").pop()))
        print("    Exited with Signal %d, %s" % (os.WTERMSIG(status), \
            signal.Signals(os.WTERMSIG(status)).name)) 

      # ana runtime sets exit status to 1 in case of an exception
      elif os.WIFEXITED(status) and os.WEXITSTATUS(status) != 0 and expecation != 'FAIL':
        print("%s: %s " % (FAIL, fullpath.split("/").pop()))
        print("  Exit with Non Zero exit status, %d" % os.WEXITSTATUS(status))
        
      else:
        if expecation != 'FAIL':
          # valgrindresult = valgrind_test(valgrind, ana, fullpath)
          # if valgrindresult:
          #   print("%s: %s " % (FAIL, fullpath.split("/").pop()))
          #   print(valgrindresult.decode())
          # else:
            print("%s: %s " % (PASS, fullpath.split("/").pop()))
        else:
          print("%s: %s " % (PASS, fullpath.split("/").pop()))

      break;

def main():
  if len(sys.argv) < 3:
    print("usage: runtests.py <path to ana> <path to test suite>")
    sys.exit(1)

  ana = os.path.realpath(sys.argv[1])
  basepath = os.path.realpath(sys.argv[2])
  files = get_files(basepath, sys.argv[2])
  only_one_dir = False

  if len(sys.argv) > 3:
    only_one_dir = sys.argv[3]

  valgrind = os.environ.get("PATH_TO_VALGRIND") or 'valgrind'
  failed = 0
  passed = 0

  print("Ana Runtime Test Suite");
  print("Path to Ana Runtime: %s" % ana)
  print("Valgrind Path: %s" % valgrind)
  print("Python version: %s" % platform.python_version())
  print("Test Suite Base Path: %s\n" % basepath);


  if only_one_dir:
    print("Running one test: `%s`" % only_one_dir)
    return run_test(ana, basepath, only_one_dir)

  for dir in files:

    for testcase in os.listdir(os.path.join(basepath, dir)):

      fullpath = os.path.join(os.path.join(basepath, dir), testcase)

      if not os.path.isfile(fullpath):
        
        print("warning: skipping %s/%s, as it's not a regular file" % (dir, testcase))
        continue

      if testcase[:1] == '.':
        continue

      contents = open(fullpath).read().split(os.linesep)
      
      expecation = "PASS"
      stdin_data = None
      should_skip = False

      if contents[0][:2] == '//':
        try:
          spec = json.loads(contents[0][2:])
          expecation = spec['Expect'].upper()
          stdin_data = spec.get('Input')
          should_skip = spec.get("Skip")
        except json.JSONDecodeError as e:
          print("warning json.loads:" + str(e))

      if should_skip:
        print("%s: %s/%s " % ("SKIP", dir, fullpath.split("/").pop()))
        continue

      print("%s/%s " % (dir, fullpath.split("/").pop()))

      stdin_pipe = os.pipe()
      stdin_readfd = stdin_pipe[0]
      stdin_writefd = stdin_pipe[1]

      if stdin_data is not None:
        for line in stdin_data:
          os.write(stdin_writefd, line.encode() + b"\n");

      pid = os.fork()

      if pid == 0:
        # In the child  
        stdout = open("/dev/null");

        # Stdout
        os.dup2(stdout.fileno(), 1)
        # Stderr
        os.dup2(stdout.fileno(), 2)

        # Stdin
        os.dup2(stdin_readfd, sys.stdin.fileno())

        os.close(stdin_readfd)
        
        os.close(stdin_writefd)

        os.execl(ana, ana.split("/").pop(), (fullpath))

        stdout.close()

      else:

        while True:

          childpid, status = os.waitpid(pid, 0)

          # This might be for SIGGSEV, but in any case any signal is fatal
          if os.WIFSIGNALED(status) and expecation != 'FAIL':
            
            print("%s: %s/%s " % (FAIL, dir, fullpath.split("/").pop()))
            print("    Exited with Signal %d, %s" % (os.WTERMSIG(status), \
                signal.Signals(os.WTERMSIG(status)).name)) 
            failed += 1

          # ana runtime sets exit status to 1 in case of an exception
          elif os.WIFEXITED(status) and os.WEXITSTATUS(status) != 0 and expecation != 'FAIL':

            print("%s: %s/%s " % (FAIL, dir, fullpath.split("/").pop()))
            
            failed += 1

          else:
            if expecation != 'FAIL':
              valgrindresult = valgrind_test(valgrind, ana, fullpath)
              if valgrindresult is not None and valgrindresult is not -1:
                print("%s: %s/%s " % (FAIL, dir, fullpath.split("/").pop()))
                print(valgrindresult.decode())
                failed +=1  
              elif valgrindresult == -1:
                print("%s: %s/%s " % (FAIL, dir, fullpath.split("/").pop()))               
              else:
                print("%s: %s/%s " % (PASS , dir, fullpath.split("/").pop()))
                passed +=1             
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