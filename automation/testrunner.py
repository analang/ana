import os
import sys
import signal


def usage():
  print("testrunner: <program> <args>")
  sys.exit(1)

def run(path, args):  
  path = os.path.realpath(path)
  args = os.path.realpath(args)
  pid = os.fork()

  if pid == 0:
    # In the child
    pipefds = os.pipe()
    os.dup2(pipefds[1], 1) # for stdout
    os.execl(path, path.split("/").pop(), args)
    
  else:
    # Wait for the child to exit
    pid, status = os.waitpid(pid, 0)

    if os.WIFEXITED(status):
      if os.WEXITSTATUS(status) != 0:
        print('\033[91m' + "FAIL: " + '\033[0m' + path + " " + args)

    if os.WIFSIGNALED(status):
      
      signum = os.WTERMSIG(status)

      print('\033[91m' + "FAIL: " + '\033[0m' + path + " " + args)

    else:
      print('\033[0;32m' + "PASS: " + '\033[0m' + path + " " + args)

def main():
  if len(sys.argv) < 3:
    usage()
  else:
    run(sys.argv[1], sys.argv[2])

if __name__ == "__main__":
  main()
