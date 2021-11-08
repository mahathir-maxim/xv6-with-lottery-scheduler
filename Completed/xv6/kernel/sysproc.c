#include "types.h"
#include "x86.h"
#include "defs.h"
#include "param.h"
#include "mmu.h"
#include "proc.h"
#include "sysfunc.h"
#include "pstat.h"

/* The following code is added/modified by Mahathir Maxim (MHM180000) & Karan Sahu(KXS190007)
// Implementing syscall settickets*/

int sys_settickets(void)
{
  int ticket_count;
  argint(0, &ticket_count);
  if(ticket_count <= 0)  
    return -1;

  acquire(&ptable.lock);
  setticketsforprocess(proc, ticket_count);
  release(&ptable.lock);

  return 0;
}

/* End of code added/modified */

/* The following code is added/modified by Mahathir Maxim (MHM180000) & Karan Sahu(KXS190007)
// Implementing syscall getpinfo*/

int sys_getpinfo(void)
{
  struct pstat* aim;
  struct proc* process;

  argptr(0,(void*)&aim,sizeof(*aim));

  if(aim == NULL)
    return -1;

  acquire(&ptable.lock);

  process=ptable.proc;
  while(process != &(ptable.proc[NPROC]))
    {
      const int i = process - ptable.proc;
      if(process->state == UNUSED)
      {
        process++;
        continue;
      }
      else {
          aim->inuse[i] = process->inuse;
          aim->tickets[i] = process->tickets;
          aim->pid[i] = process->pid;
          aim->ticks[i] = process->ticks;
          process++;
      }
    }

  release(&ptable.lock);
  return 0;
}
/* End of code added/modified */


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
  return proc->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = proc->sz;
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
    if(proc->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since boot.
int
sys_uptime(void)
{
  uint xticks;
  
  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}
