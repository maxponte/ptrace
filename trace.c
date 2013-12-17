#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/ptrace.h>
#include <sys/user.h>
#include <string.h>

#define printreg(REGNAME) printf("%3s: 0x%016lx\n", #REGNAME, regs.REGNAME)

int child_status;
int main() {
  pid_t child;
  if((child = fork()) == 0) {
    ptrace (PTRACE_TRACEME, 0, NULL, NULL);
    char *envp[] = { NULL };
    char *argv[] = { "./fact", NULL };
    int status = execve("./fact", envp, argv);
    exit(status);
  } else {
    while(1) {
      waitpid(child, &child_status, WUNTRACED);
      if(WIFSTOPPED(child_status)) {
        printf("Child stopped with status %d.\n", child_status);

        struct user_regs_struct regs;
        ptrace(PTRACE_GETREGS, child, NULL, &regs);

        printreg(rax);
        printreg(rbx);
        printreg(rcx);
        printreg(rdx);
        printreg(rsi);
        printreg(rdi);
        printreg(rsp);
        printreg(rbp);
        printreg(r8);
        printreg(r9);
        printreg(r10);
        printreg(r11);
        printreg(r12);
        printreg(r13);
        printreg(r14);
        printreg(r15);

        char cmdbuf[10];
        for(int i = 0; i < 10; i++) {
          void* rsp_addr = (void*)((unsigned long int)(regs.rsp + 8*i));
          long rsp_val = ptrace(PTRACE_PEEKDATA, child, rsp_addr, NULL);
          if(rsp_val != -1) {
            printf("(rsp + %d): 0x%016lx\n", i, rsp_val);
          }
        }

        while(1) {
          printf("> ");
          scanf("%s", cmdbuf);
          if(!strcmp(cmdbuf, "n")) {
            ptrace(PTRACE_CONT, child, NULL, 0);
            break;
          } else if(!strcmp(cmdbuf, "q")) {
            return 0;
          } else {
            printf("Command not recognized. Commands: n (goto next thing), q (quit)\n");
          }
        }
      } else if(WIFEXITED(child_status)) {
        ptrace(PTRACE_DETACH, child, NULL, 0);
        printf("Child exited with status %d.\n", child_status);
        return 0;
      }
    }
  }
  return 0;
}
