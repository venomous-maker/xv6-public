4650 // Sleeping locks
4651 
4652 #include "types.h"
4653 #include "defs.h"
4654 #include "param.h"
4655 #include "x86.h"
4656 #include "memlayout.h"
4657 #include "mmu.h"
4658 #include "proc.h"
4659 #include "spinlock.h"
4660 #include "sleeplock.h"
4661 
4662 void
4663 initsleeplock(struct sleeplock *lk, char *name)
4664 {
4665   initlock(&lk->lk, "sleep lock");
4666   lk->name = name;
4667   lk->locked = 0;
4668   lk->pid = 0;
4669 }
4670 
4671 void
4672 acquiresleep(struct sleeplock *lk)
4673 {
4674   acquire(&lk->lk);
4675   while (lk->locked) {
4676     sleep(lk, &lk->lk);
4677   }
4678   lk->locked = 1;
4679   lk->pid = myproc()->pid;
4680   release(&lk->lk);
4681 }
4682 
4683 void
4684 releasesleep(struct sleeplock *lk)
4685 {
4686   acquire(&lk->lk);
4687   lk->locked = 0;
4688   lk->pid = 0;
4689   wakeup(lk);
4690   release(&lk->lk);
4691 }
4692 
4693 
4694 
4695 
4696 
4697 
4698 
4699 
4700 int
4701 holdingsleep(struct sleeplock *lk)
4702 {
4703   int r;
4704 
4705   acquire(&lk->lk);
4706   r = lk->locked && (lk->pid == myproc()->pid);
4707   release(&lk->lk);
4708   return r;
4709 }
4710 
4711 
4712 
4713 
4714 
4715 
4716 
4717 
4718 
4719 
4720 
4721 
4722 
4723 
4724 
4725 
4726 
4727 
4728 
4729 
4730 
4731 
4732 
4733 
4734 
4735 
4736 
4737 
4738 
4739 
4740 
4741 
4742 
4743 
4744 
4745 
4746 
4747 
4748 
4749 
