4750 #include "types.h"
4751 #include "defs.h"
4752 #include "param.h"
4753 #include "spinlock.h"
4754 #include "sleeplock.h"
4755 #include "fs.h"
4756 #include "buf.h"
4757 
4758 // Simple logging that allows concurrent FS system calls.
4759 //
4760 // A log transaction contains the updates of multiple FS system
4761 // calls. The logging system only commits when there are
4762 // no FS system calls active. Thus there is never
4763 // any reasoning required about whether a commit might
4764 // write an uncommitted system call's updates to disk.
4765 //
4766 // A system call should call begin_op()/end_op() to mark
4767 // its start and end. Usually begin_op() just increments
4768 // the count of in-progress FS system calls and returns.
4769 // But if it thinks the log is close to running out, it
4770 // sleeps until the last outstanding end_op() commits.
4771 //
4772 // The log is a physical re-do log containing disk blocks.
4773 // The on-disk log format:
4774 //   header block, containing block #s for block A, B, C, ...
4775 //   block A
4776 //   block B
4777 //   block C
4778 //   ...
4779 // Log appends are synchronous.
4780 
4781 // Contents of the header block, used for both the on-disk header block
4782 // and to keep track in memory of logged block# before commit.
4783 struct logheader {
4784   int n;
4785   int block[LOGSIZE];
4786 };
4787 
4788 struct log {
4789   struct spinlock lock;
4790   int start;
4791   int size;
4792   int outstanding; // how many FS sys calls are executing.
4793   int committing;  // in commit(), please wait.
4794   int dev;
4795   struct logheader lh;
4796 };
4797 
4798 
4799 
4800 struct log log;
4801 
4802 static void recover_from_log(void);
4803 static void commit();
4804 
4805 void
4806 initlog(int dev)
4807 {
4808   if (sizeof(struct logheader) >= BSIZE)
4809     panic("initlog: too big logheader");
4810 
4811   struct superblock sb;
4812   initlock(&log.lock, "log");
4813   readsb(dev, &sb);
4814   log.start = sb.logstart;
4815   log.size = sb.nlog;
4816   log.dev = dev;
4817   recover_from_log();
4818 }
4819 
4820 // Copy committed blocks from log to their home location
4821 static void
4822 install_trans(void)
4823 {
4824   int tail;
4825 
4826   for (tail = 0; tail < log.lh.n; tail++) {
4827     struct buf *lbuf = bread(log.dev, log.start+tail+1); // read log block
4828     struct buf *dbuf = bread(log.dev, log.lh.block[tail]); // read dst
4829     memmove(dbuf->data, lbuf->data, BSIZE);  // copy block to dst
4830     bwrite(dbuf);  // write dst to disk
4831     brelse(lbuf);
4832     brelse(dbuf);
4833   }
4834 }
4835 
4836 // Read the log header from disk into the in-memory log header
4837 static void
4838 read_head(void)
4839 {
4840   struct buf *buf = bread(log.dev, log.start);
4841   struct logheader *lh = (struct logheader *) (buf->data);
4842   int i;
4843   log.lh.n = lh->n;
4844   for (i = 0; i < log.lh.n; i++) {
4845     log.lh.block[i] = lh->block[i];
4846   }
4847   brelse(buf);
4848 }
4849 
4850 // Write in-memory log header to disk.
4851 // This is the true point at which the
4852 // current transaction commits.
4853 static void
4854 write_head(void)
4855 {
4856   struct buf *buf = bread(log.dev, log.start);
4857   struct logheader *hb = (struct logheader *) (buf->data);
4858   int i;
4859   hb->n = log.lh.n;
4860   for (i = 0; i < log.lh.n; i++) {
4861     hb->block[i] = log.lh.block[i];
4862   }
4863   bwrite(buf);
4864   brelse(buf);
4865 }
4866 
4867 static void
4868 recover_from_log(void)
4869 {
4870   read_head();
4871   install_trans(); // if committed, copy from log to disk
4872   log.lh.n = 0;
4873   write_head(); // clear the log
4874 }
4875 
4876 // called at the start of each FS system call.
4877 void
4878 begin_op(void)
4879 {
4880   acquire(&log.lock);
4881   while(1){
4882     if(log.committing){
4883       sleep(&log, &log.lock);
4884     } else if(log.lh.n + (log.outstanding+1)*MAXOPBLOCKS > LOGSIZE){
4885       // this op might exhaust log space; wait for commit.
4886       sleep(&log, &log.lock);
4887     } else {
4888       log.outstanding += 1;
4889       release(&log.lock);
4890       break;
4891     }
4892   }
4893 }
4894 
4895 
4896 
4897 
4898 
4899 
4900 // called at the end of each FS system call.
4901 // commits if this was the last outstanding operation.
4902 void
4903 end_op(void)
4904 {
4905   int do_commit = 0;
4906 
4907   acquire(&log.lock);
4908   log.outstanding -= 1;
4909   if(log.committing)
4910     panic("log.committing");
4911   if(log.outstanding == 0){
4912     do_commit = 1;
4913     log.committing = 1;
4914   } else {
4915     // begin_op() may be waiting for log space,
4916     // and decrementing log.outstanding has decreased
4917     // the amount of reserved space.
4918     wakeup(&log);
4919   }
4920   release(&log.lock);
4921 
4922   if(do_commit){
4923     // call commit w/o holding locks, since not allowed
4924     // to sleep with locks.
4925     commit();
4926     acquire(&log.lock);
4927     log.committing = 0;
4928     wakeup(&log);
4929     release(&log.lock);
4930   }
4931 }
4932 
4933 // Copy modified blocks from cache to log.
4934 static void
4935 write_log(void)
4936 {
4937   int tail;
4938 
4939   for (tail = 0; tail < log.lh.n; tail++) {
4940     struct buf *to = bread(log.dev, log.start+tail+1); // log block
4941     struct buf *from = bread(log.dev, log.lh.block[tail]); // cache block
4942     memmove(to->data, from->data, BSIZE);
4943     bwrite(to);  // write the log
4944     brelse(from);
4945     brelse(to);
4946   }
4947 }
4948 
4949 
4950 static void
4951 commit()
4952 {
4953   if (log.lh.n > 0) {
4954     write_log();     // Write modified blocks from cache to log
4955     write_head();    // Write header to disk -- the real commit
4956     install_trans(); // Now install writes to home locations
4957     log.lh.n = 0;
4958     write_head();    // Erase the transaction from the log
4959   }
4960 }
4961 
4962 // Caller has modified b->data and is done with the buffer.
4963 // Record the block number and pin in the cache with B_DIRTY.
4964 // commit()/write_log() will do the disk write.
4965 //
4966 // log_write() replaces bwrite(); a typical use is:
4967 //   bp = bread(...)
4968 //   modify bp->data[]
4969 //   log_write(bp)
4970 //   brelse(bp)
4971 void
4972 log_write(struct buf *b)
4973 {
4974   int i;
4975 
4976   if (log.lh.n >= LOGSIZE || log.lh.n >= log.size - 1)
4977     panic("too big a transaction");
4978   if (log.outstanding < 1)
4979     panic("log_write outside of trans");
4980 
4981   acquire(&log.lock);
4982   for (i = 0; i < log.lh.n; i++) {
4983     if (log.lh.block[i] == b->blockno)   // log absorbtion
4984       break;
4985   }
4986   log.lh.block[i] = b->blockno;
4987   if (i == log.lh.n)
4988     log.lh.n++;
4989   b->flags |= B_DIRTY; // prevent eviction
4990   release(&log.lock);
4991 }
4992 
4993 
4994 
4995 
4996 
4997 
4998 
4999 
