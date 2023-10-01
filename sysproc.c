#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

int
sys_head(void){
    cprintf("Head command being executed in kernel mode\n");
    char **lines = NULL;
    int line_count;
    char *error = "";

    // Retrieve a pointer argument using argptr
    if (argptr(1, (char**)&lines, sizeof(char*)) < 0 ||
        argint(0, &line_count) < 0 || argstr(2, &error) < 0) {
        cprintf("Head: An error(2) occurred!\n");
        return -1;  // Error handling
    }
    if (strncmp(error, "",1) < 0){
        cprintf("%s\n", error);
    }
    for (int i = 0; i < line_count; i++) {
        cprintf("%s\n", lines[i]);
    }
    return 0;
}

int
sys_uniq(void){
    cprintf("Uniq command being executed in kernel mode\n");
    char** original_lines = NULL;
    int ignore_case_flag, duplicate_flag, count_flag = 0;
    char **lines = NULL;
    int line_count;
    char *error = "";
    // head(ignore_case_flag, duplicate_flag, count_flag, lines, linecount)
    // Retrieve integer arguments using argint
    if (argint(0, &ignore_case_flag) < 0 ||
        argint(1, &duplicate_flag) < 0 ||
        argint(2, &count_flag) < 0) {
        cprintf("Uniq: An error(1) occurred!\n");
        return -1;  // Error handling
    }

    // Retrieve a pointer argument using argptr
    if (argptr(3, (char**)&lines, sizeof(char*)) < 0 ||
        argint(4, &line_count) < 0 || argstr(5, &error) < 0) {
        cprintf("Uniq: An error(2) occurred!\n");
        return -1;  // Error handling
    }
    if (strncmp(error,  "", strlen("")) < 0){
        cprintf("%s\n", error);
        return -1;
    }
    if (line_count == 0){
        cprintf("Uniq: Empty file\n");
        return -1;
    }
    if (ignore_case_flag) {
        original_lines = lines;
        for(int i = 0; i < line_count; i++) {

            strncpy(original_lines[i], lines[i], strlen(original_lines[i]));  // Copy the original line
            strnlower(lines[i]);  // Convert the copied line to lowercase
        }
    }

    int uniq_counter[line_count];
    char *uniq[line_count];
    char *uniq_print[line_count];
    int uniqs = (line_count > 0) ? 1 : 0;
    if (uniqs) {
        uniq[0] = lines[0];
        uniq_counter[0] = 1;
        if (ignore_case_flag) {
            uniq_print[0] = original_lines[0];
        }
    }
    int temp[2] = {0,0};
    for (int i = 1; i < line_count; i++) {
        int *result = temp;
        strnavail(lines[i], (const char **) uniq, uniqs, result);
        //cprintf("Uniqs: %d %d : %d  %s : %s\n", uniqs, result[0], result[1], lines[i], uniq[result[1]]);
        if (result[0]) {
            uniq_counter[result[1]] = uniq_counter[result[1]] + 1;
        } else {
            // Add the new line to the uniq list
            uniqs++;

            // Add the new line and set its count
            uniq[uniqs - 1] = lines[i];
            uniq_counter[uniqs - 1] = 1;

            if (ignore_case_flag) {
                uniq_print[uniqs - 1] = original_lines[i];
            }
        }
    }

    if (ignore_case_flag) {
        for (int i = 0; i < uniqs; i++)
        uniq[i] = uniq_print[i];
    }
    // Print the unique lines with counts if -c flag is set
    for (int i = 0; i < uniqs; i++) {
        if (duplicate_flag) {
            if (count_flag) {
                if (uniq_counter[i] > 1) {
                    cprintf("%d %s\n", uniq_counter[i], uniq[i]);
                }
            } else {
                if (uniq_counter[i] > 1) {
                    cprintf("%s\n", uniq[i]);
                }
            }
        } else if (count_flag) {
            cprintf("%d %s\n", uniq_counter[i], uniq[i]);
        } else {
            cprintf("%s\n", uniq[i]);
        }
    }
    return 0;
}

int
sys_getpinfo(void)
{
  int pid;
  struct pstat *pstat;

  // Validate user-space arguments.
  if (argint(0, &pid) < 0 || argptr(1, (void *)&pstat, sizeof(*pstat)) < 0)
    return -1;

  // Ensure that the provided `pid` is valid.
  if (pid < 0 || pid >= NPROC)
    return -1;

  // Get the process statistics and copy them to the user's memory.
  return procinfo(pstat);

  return 0; // Return success
}

int
sys_ps(void)
{
  return ps();
}

int
sys_hpr(void)
{
  int pid, priority;
  if(argint(0, &pid) < 0)
    return -1;
  if(argint(1, &priority) < 0)
    return -1;

  return hpr(pid, priority);
}
