6800 #include "types.h"
6801 #include "defs.h"
6802 #include "param.h"
6803 #include "mmu.h"
6804 #include "proc.h"
6805 #include "fs.h"
6806 #include "spinlock.h"
6807 #include "sleeplock.h"
6808 #include "file.h"
6809 
6810 #define PIPESIZE 512
6811 
6812 struct pipe {
6813   struct spinlock lock;
6814   char data[PIPESIZE];
6815   uint nread;     // number of bytes read
6816   uint nwrite;    // number of bytes written
6817   int readopen;   // read fd is still open
6818   int writeopen;  // write fd is still open
6819 };
6820 
6821 int
6822 pipealloc(struct file **f0, struct file **f1)
6823 {
6824   struct pipe *p;
6825 
6826   p = 0;
6827   *f0 = *f1 = 0;
6828   if((*f0 = filealloc()) == 0 || (*f1 = filealloc()) == 0)
6829     goto bad;
6830   if((p = (struct pipe*)kalloc()) == 0)
6831     goto bad;
6832   p->readopen = 1;
6833   p->writeopen = 1;
6834   p->nwrite = 0;
6835   p->nread = 0;
6836   initlock(&p->lock, "pipe");
6837   (*f0)->type = FD_PIPE;
6838   (*f0)->readable = 1;
6839   (*f0)->writable = 0;
6840   (*f0)->pipe = p;
6841   (*f1)->type = FD_PIPE;
6842   (*f1)->readable = 0;
6843   (*f1)->writable = 1;
6844   (*f1)->pipe = p;
6845   return 0;
6846 
6847 
6848 
6849 
6850  bad:
6851   if(p)
6852     kfree((char*)p);
6853   if(*f0)
6854     fileclose(*f0);
6855   if(*f1)
6856     fileclose(*f1);
6857   return -1;
6858 }
6859 
6860 void
6861 pipeclose(struct pipe *p, int writable)
6862 {
6863   acquire(&p->lock);
6864   if(writable){
6865     p->writeopen = 0;
6866     wakeup(&p->nread);
6867   } else {
6868     p->readopen = 0;
6869     wakeup(&p->nwrite);
6870   }
6871   if(p->readopen == 0 && p->writeopen == 0){
6872     release(&p->lock);
6873     kfree((char*)p);
6874   } else
6875     release(&p->lock);
6876 }
6877 
6878 
6879 int
6880 pipewrite(struct pipe *p, char *addr, int n)
6881 {
6882   int i;
6883 
6884   acquire(&p->lock);
6885   for(i = 0; i < n; i++){
6886     while(p->nwrite == p->nread + PIPESIZE){  //DOC: pipewrite-full
6887       if(p->readopen == 0 || myproc()->killed){
6888         release(&p->lock);
6889         return -1;
6890       }
6891       wakeup(&p->nread);
6892       sleep(&p->nwrite, &p->lock);  //DOC: pipewrite-sleep
6893     }
6894     p->data[p->nwrite++ % PIPESIZE] = addr[i];
6895   }
6896   wakeup(&p->nread);  //DOC: pipewrite-wakeup1
6897   release(&p->lock);
6898   return n;
6899 }
6900 int
6901 piperead(struct pipe *p, char *addr, int n)
6902 {
6903   int i;
6904 
6905   acquire(&p->lock);
6906   while(p->nread == p->nwrite && p->writeopen){  //DOC: pipe-empty
6907     if(myproc()->killed){
6908       release(&p->lock);
6909       return -1;
6910     }
6911     sleep(&p->nread, &p->lock); //DOC: piperead-sleep
6912   }
6913   for(i = 0; i < n; i++){  //DOC: piperead-copy
6914     if(p->nread == p->nwrite)
6915       break;
6916     addr[i] = p->data[p->nread++ % PIPESIZE];
6917   }
6918   wakeup(&p->nwrite);  //DOC: piperead-wakeup
6919   release(&p->lock);
6920   return i;
6921 }
6922 
6923 
6924 
6925 
6926 
6927 
6928 
6929 
6930 
6931 
6932 
6933 
6934 
6935 
6936 
6937 
6938 
6939 
6940 
6941 
6942 
6943 
6944 
6945 
6946 
6947 
6948 
6949 
