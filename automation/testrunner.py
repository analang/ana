import os
import sys
import signal

def usage():
  print("testrunner: <program> <args>")
  sys.exit(1)

def run(path, args):  
  path = os.path.realpath(path)
  args = os.path.realpath(args)
  """
  run well execute the program located at path with arguments arg. The program
  is executed in a child process, and the parent will wait for it's exit status
  and if any signals were sent to it. run considers a program to have exited
  succesfully if SIGSEV was not sent to it and it's exit status was 0.
  """
  pid = os.fork()

  if pid == 0:
    # In the child
    pipefds = os.pipe()
    piped_stdin = os.pipe()
    piped_stderr = os.pipe()

    # 1 is write, make the stdin be a pipe we can write to 
    os.dup2(piped_stdin[1], 0)
    os.dup2(pipefds[1], 1) # for stdout
    
    os.dup2(piped_stderr[0], 2)


    os.write(piped_stdin[1], "Testing...")

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
