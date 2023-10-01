5900 //
5901 // File descriptors
5902 //
5903 
5904 #include "types.h"
5905 #include "defs.h"
5906 #include "param.h"
5907 #include "fs.h"
5908 #include "spinlock.h"
5909 #include "sleeplock.h"
5910 #include "file.h"
5911 
5912 struct devsw devsw[NDEV];
5913 struct {
5914   struct spinlock lock;
5915   struct file file[NFILE];
5916 } ftable;
5917 
5918 void
5919 fileinit(void)
5920 {
5921   initlock(&ftable.lock, "ftable");
5922 }
5923 
5924 // Allocate a file structure.
5925 struct file*
5926 filealloc(void)
5927 {
5928   struct file *f;
5929 
5930   acquire(&ftable.lock);
5931   for(f = ftable.file; f < ftable.file + NFILE; f++){
5932     if(f->ref == 0){
5933       f->ref = 1;
5934       release(&ftable.lock);
5935       return f;
5936     }
5937   }
5938   release(&ftable.lock);
5939   return 0;
5940 }
5941 
5942 
5943 
5944 
5945 
5946 
5947 
5948 
5949 
5950 // Increment ref count for file f.
5951 struct file*
5952 filedup(struct file *f)
5953 {
5954   acquire(&ftable.lock);
5955   if(f->ref < 1)
5956     panic("filedup");
5957   f->ref++;
5958   release(&ftable.lock);
5959   return f;
5960 }
5961 
5962 // Close file f.  (Decrement ref count, close when reaches 0.)
5963 void
5964 fileclose(struct file *f)
5965 {
5966   struct file ff;
5967 
5968   acquire(&ftable.lock);
5969   if(f->ref < 1)
5970     panic("fileclose");
5971   if(--f->ref > 0){
5972     release(&ftable.lock);
5973     return;
5974   }
5975   ff = *f;
5976   f->ref = 0;
5977   f->type = FD_NONE;
5978   release(&ftable.lock);
5979 
5980   if(ff.type == FD_PIPE)
5981     pipeclose(ff.pipe, ff.writable);
5982   else if(ff.type == FD_INODE){
5983     begin_op();
5984     iput(ff.ip);
5985     end_op();
5986   }
5987 }
5988 
5989 
5990 
5991 
5992 
5993 
5994 
5995 
5996 
5997 
5998 
5999 
6000 // Get metadata about file f.
6001 int
6002 filestat(struct file *f, struct stat *st)
6003 {
6004   if(f->type == FD_INODE){
6005     ilock(f->ip);
6006     stati(f->ip, st);
6007     iunlock(f->ip);
6008     return 0;
6009   }
6010   return -1;
6011 }
6012 
6013 // Read from file f.
6014 int
6015 fileread(struct file *f, char *addr, int n)
6016 {
6017   int r;
6018 
6019   if(f->readable == 0)
6020     return -1;
6021   if(f->type == FD_PIPE)
6022     return piperead(f->pipe, addr, n);
6023   if(f->type == FD_INODE){
6024     ilock(f->ip);
6025     if((r = readi(f->ip, addr, f->off, n)) > 0)
6026       f->off += r;
6027     iunlock(f->ip);
6028     return r;
6029   }
6030   panic("fileread");
6031 }
6032 
6033 
6034 
6035 
6036 
6037 
6038 
6039 
6040 
6041 
6042 
6043 
6044 
6045 
6046 
6047 
6048 
6049 
6050 // Write to file f.
6051 int
6052 filewrite(struct file *f, char *addr, int n)
6053 {
6054   int r;
6055 
6056   if(f->writable == 0)
6057     return -1;
6058   if(f->type == FD_PIPE)
6059     return pipewrite(f->pipe, addr, n);
6060   if(f->type == FD_INODE){
6061     // write a few blocks at a time to avoid exceeding
6062     // the maximum log transaction size, including
6063     // i-node, indirect block, allocation blocks,
6064     // and 2 blocks of slop for non-aligned writes.
6065     // this really belongs lower down, since writei()
6066     // might be writing a device like the console.
6067     int max = ((MAXOPBLOCKS-1-1-2) / 2) * 512;
6068     int i = 0;
6069     while(i < n){
6070       int n1 = n - i;
6071       if(n1 > max)
6072         n1 = max;
6073 
6074       begin_op();
6075       ilock(f->ip);
6076       if ((r = writei(f->ip, addr + i, f->off, n1)) > 0)
6077         f->off += r;
6078       iunlock(f->ip);
6079       end_op();
6080 
6081       if(r < 0)
6082         break;
6083       if(r != n1)
6084         panic("short filewrite");
6085       i += r;
6086     }
6087     return i == n ? n : -1;
6088   }
6089   panic("filewrite");
6090 }
6091 
6092 
6093 
6094 
6095 
6096 
6097 
6098 
6099 
