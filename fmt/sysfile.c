6100 //
6101 // File-system system calls.
6102 // Mostly argument checking, since we don't trust
6103 // user code, and calls into file.c and fs.c.
6104 //
6105 
6106 #include "types.h"
6107 #include "defs.h"
6108 #include "param.h"
6109 #include "stat.h"
6110 #include "mmu.h"
6111 #include "proc.h"
6112 #include "fs.h"
6113 #include "spinlock.h"
6114 #include "sleeplock.h"
6115 #include "file.h"
6116 #include "fcntl.h"
6117 
6118 // Fetch the nth word-sized system call argument as a file descriptor
6119 // and return both the descriptor and the corresponding struct file.
6120 static int
6121 argfd(int n, int *pfd, struct file **pf)
6122 {
6123   int fd;
6124   struct file *f;
6125 
6126   if(argint(n, &fd) < 0)
6127     return -1;
6128   if(fd < 0 || fd >= NOFILE || (f=myproc()->ofile[fd]) == 0)
6129     return -1;
6130   if(pfd)
6131     *pfd = fd;
6132   if(pf)
6133     *pf = f;
6134   return 0;
6135 }
6136 
6137 
6138 
6139 
6140 
6141 
6142 
6143 
6144 
6145 
6146 
6147 
6148 
6149 
6150 // Allocate a file descriptor for the given file.
6151 // Takes over file reference from caller on success.
6152 static int
6153 fdalloc(struct file *f)
6154 {
6155   int fd;
6156   struct proc *curproc = myproc();
6157 
6158   for(fd = 0; fd < NOFILE; fd++){
6159     if(curproc->ofile[fd] == 0){
6160       curproc->ofile[fd] = f;
6161       return fd;
6162     }
6163   }
6164   return -1;
6165 }
6166 
6167 int
6168 sys_dup(void)
6169 {
6170   struct file *f;
6171   int fd;
6172 
6173   if(argfd(0, 0, &f) < 0)
6174     return -1;
6175   if((fd=fdalloc(f)) < 0)
6176     return -1;
6177   filedup(f);
6178   return fd;
6179 }
6180 
6181 int
6182 sys_read(void)
6183 {
6184   struct file *f;
6185   int n;
6186   char *p;
6187 
6188   if(argfd(0, 0, &f) < 0 || argint(2, &n) < 0 || argptr(1, &p, n) < 0)
6189     return -1;
6190   return fileread(f, p, n);
6191 }
6192 
6193 
6194 
6195 
6196 
6197 
6198 
6199 
6200 int
6201 sys_write(void)
6202 {
6203   struct file *f;
6204   int n;
6205   char *p;
6206 
6207   if(argfd(0, 0, &f) < 0 || argint(2, &n) < 0 || argptr(1, &p, n) < 0)
6208     return -1;
6209   return filewrite(f, p, n);
6210 }
6211 
6212 int
6213 sys_close(void)
6214 {
6215   int fd;
6216   struct file *f;
6217 
6218   if(argfd(0, &fd, &f) < 0)
6219     return -1;
6220   myproc()->ofile[fd] = 0;
6221   fileclose(f);
6222   return 0;
6223 }
6224 
6225 int
6226 sys_fstat(void)
6227 {
6228   struct file *f;
6229   struct stat *st;
6230 
6231   if(argfd(0, 0, &f) < 0 || argptr(1, (void*)&st, sizeof(*st)) < 0)
6232     return -1;
6233   return filestat(f, st);
6234 }
6235 
6236 
6237 
6238 
6239 
6240 
6241 
6242 
6243 
6244 
6245 
6246 
6247 
6248 
6249 
6250 // Create the path new as a link to the same inode as old.
6251 int
6252 sys_link(void)
6253 {
6254   char name[DIRSIZ], *new, *old;
6255   struct inode *dp, *ip;
6256 
6257   if(argstr(0, &old) < 0 || argstr(1, &new) < 0)
6258     return -1;
6259 
6260   begin_op();
6261   if((ip = namei(old)) == 0){
6262     end_op();
6263     return -1;
6264   }
6265 
6266   ilock(ip);
6267   if(ip->type == T_DIR){
6268     iunlockput(ip);
6269     end_op();
6270     return -1;
6271   }
6272 
6273   ip->nlink++;
6274   iupdate(ip);
6275   iunlock(ip);
6276 
6277   if((dp = nameiparent(new, name)) == 0)
6278     goto bad;
6279   ilock(dp);
6280   if(dp->dev != ip->dev || dirlink(dp, name, ip->inum) < 0){
6281     iunlockput(dp);
6282     goto bad;
6283   }
6284   iunlockput(dp);
6285   iput(ip);
6286 
6287   end_op();
6288 
6289   return 0;
6290 
6291 bad:
6292   ilock(ip);
6293   ip->nlink--;
6294   iupdate(ip);
6295   iunlockput(ip);
6296   end_op();
6297   return -1;
6298 }
6299 
6300 // Is the directory dp empty except for "." and ".." ?
6301 static int
6302 isdirempty(struct inode *dp)
6303 {
6304   int off;
6305   struct dirent de;
6306 
6307   for(off=2*sizeof(de); off<dp->size; off+=sizeof(de)){
6308     if(readi(dp, (char*)&de, off, sizeof(de)) != sizeof(de))
6309       panic("isdirempty: readi");
6310     if(de.inum != 0)
6311       return 0;
6312   }
6313   return 1;
6314 }
6315 
6316 
6317 
6318 
6319 
6320 
6321 
6322 
6323 
6324 
6325 
6326 
6327 
6328 
6329 
6330 
6331 
6332 
6333 
6334 
6335 
6336 
6337 
6338 
6339 
6340 
6341 
6342 
6343 
6344 
6345 
6346 
6347 
6348 
6349 
6350 int
6351 sys_unlink(void)
6352 {
6353   struct inode *ip, *dp;
6354   struct dirent de;
6355   char name[DIRSIZ], *path;
6356   uint off;
6357 
6358   if(argstr(0, &path) < 0)
6359     return -1;
6360 
6361   begin_op();
6362   if((dp = nameiparent(path, name)) == 0){
6363     end_op();
6364     return -1;
6365   }
6366 
6367   ilock(dp);
6368 
6369   // Cannot unlink "." or "..".
6370   if(namecmp(name, ".") == 0 || namecmp(name, "..") == 0)
6371     goto bad;
6372 
6373   if((ip = dirlookup(dp, name, &off)) == 0)
6374     goto bad;
6375   ilock(ip);
6376 
6377   if(ip->nlink < 1)
6378     panic("unlink: nlink < 1");
6379   if(ip->type == T_DIR && !isdirempty(ip)){
6380     iunlockput(ip);
6381     goto bad;
6382   }
6383 
6384   memset(&de, 0, sizeof(de));
6385   if(writei(dp, (char*)&de, off, sizeof(de)) != sizeof(de))
6386     panic("unlink: writei");
6387   if(ip->type == T_DIR){
6388     dp->nlink--;
6389     iupdate(dp);
6390   }
6391   iunlockput(dp);
6392 
6393   ip->nlink--;
6394   iupdate(ip);
6395   iunlockput(ip);
6396 
6397   end_op();
6398 
6399   return 0;
6400 bad:
6401   iunlockput(dp);
6402   end_op();
6403   return -1;
6404 }
6405 
6406 static struct inode*
6407 create(char *path, short type, short major, short minor)
6408 {
6409   struct inode *ip, *dp;
6410   char name[DIRSIZ];
6411 
6412   if((dp = nameiparent(path, name)) == 0)
6413     return 0;
6414   ilock(dp);
6415 
6416   if((ip = dirlookup(dp, name, 0)) != 0){
6417     iunlockput(dp);
6418     ilock(ip);
6419     if(type == T_FILE && ip->type == T_FILE)
6420       return ip;
6421     iunlockput(ip);
6422     return 0;
6423   }
6424 
6425   if((ip = ialloc(dp->dev, type)) == 0)
6426     panic("create: ialloc");
6427 
6428   ilock(ip);
6429   ip->major = major;
6430   ip->minor = minor;
6431   ip->nlink = 1;
6432   iupdate(ip);
6433 
6434   if(type == T_DIR){  // Create . and .. entries.
6435     dp->nlink++;  // for ".."
6436     iupdate(dp);
6437     // No ip->nlink++ for ".": avoid cyclic ref count.
6438     if(dirlink(ip, ".", ip->inum) < 0 || dirlink(ip, "..", dp->inum) < 0)
6439       panic("create dots");
6440   }
6441 
6442   if(dirlink(dp, name, ip->inum) < 0)
6443     panic("create: dirlink");
6444 
6445   iunlockput(dp);
6446 
6447   return ip;
6448 }
6449 
6450 int
6451 sys_open(void)
6452 {
6453   char *path;
6454   int fd, omode;
6455   struct file *f;
6456   struct inode *ip;
6457 
6458   if(argstr(0, &path) < 0 || argint(1, &omode) < 0)
6459     return -1;
6460 
6461   begin_op();
6462 
6463   if(omode & O_CREATE){
6464     ip = create(path, T_FILE, 0, 0);
6465     if(ip == 0){
6466       end_op();
6467       return -1;
6468     }
6469   } else {
6470     if((ip = namei(path)) == 0){
6471       end_op();
6472       return -1;
6473     }
6474     ilock(ip);
6475     if(ip->type == T_DIR && omode != O_RDONLY){
6476       iunlockput(ip);
6477       end_op();
6478       return -1;
6479     }
6480   }
6481 
6482   if((f = filealloc()) == 0 || (fd = fdalloc(f)) < 0){
6483     if(f)
6484       fileclose(f);
6485     iunlockput(ip);
6486     end_op();
6487     return -1;
6488   }
6489   iunlock(ip);
6490   end_op();
6491 
6492   f->type = FD_INODE;
6493   f->ip = ip;
6494   f->off = 0;
6495   f->readable = !(omode & O_WRONLY);
6496   f->writable = (omode & O_WRONLY) || (omode & O_RDWR);
6497   return fd;
6498 }
6499 
6500 int
6501 sys_mkdir(void)
6502 {
6503   char *path;
6504   struct inode *ip;
6505 
6506   begin_op();
6507   if(argstr(0, &path) < 0 || (ip = create(path, T_DIR, 0, 0)) == 0){
6508     end_op();
6509     return -1;
6510   }
6511   iunlockput(ip);
6512   end_op();
6513   return 0;
6514 }
6515 
6516 int
6517 sys_mknod(void)
6518 {
6519   struct inode *ip;
6520   char *path;
6521   int major, minor;
6522 
6523   begin_op();
6524   if((argstr(0, &path)) < 0 ||
6525      argint(1, &major) < 0 ||
6526      argint(2, &minor) < 0 ||
6527      (ip = create(path, T_DEV, major, minor)) == 0){
6528     end_op();
6529     return -1;
6530   }
6531   iunlockput(ip);
6532   end_op();
6533   return 0;
6534 }
6535 
6536 
6537 
6538 
6539 
6540 
6541 
6542 
6543 
6544 
6545 
6546 
6547 
6548 
6549 
6550 int
6551 sys_chdir(void)
6552 {
6553   char *path;
6554   struct inode *ip;
6555   struct proc *curproc = myproc();
6556 
6557   begin_op();
6558   if(argstr(0, &path) < 0 || (ip = namei(path)) == 0){
6559     end_op();
6560     return -1;
6561   }
6562   ilock(ip);
6563   if(ip->type != T_DIR){
6564     iunlockput(ip);
6565     end_op();
6566     return -1;
6567   }
6568   iunlock(ip);
6569   iput(curproc->cwd);
6570   end_op();
6571   curproc->cwd = ip;
6572   return 0;
6573 }
6574 
6575 int
6576 sys_exec(void)
6577 {
6578   char *path, *argv[MAXARG];
6579   int i;
6580   uint uargv, uarg;
6581 
6582   if(argstr(0, &path) < 0 || argint(1, (int*)&uargv) < 0){
6583     return -1;
6584   }
6585   memset(argv, 0, sizeof(argv));
6586   for(i=0;; i++){
6587     if(i >= NELEM(argv))
6588       return -1;
6589     if(fetchint(uargv+4*i, (int*)&uarg) < 0)
6590       return -1;
6591     if(uarg == 0){
6592       argv[i] = 0;
6593       break;
6594     }
6595     if(fetchstr(uarg, &argv[i]) < 0)
6596       return -1;
6597   }
6598   return exec(path, argv);
6599 }
6600 int
6601 sys_pipe(void)
6602 {
6603   int *fd;
6604   struct file *rf, *wf;
6605   int fd0, fd1;
6606 
6607   if(argptr(0, (void*)&fd, 2*sizeof(fd[0])) < 0)
6608     return -1;
6609   if(pipealloc(&rf, &wf) < 0)
6610     return -1;
6611   fd0 = -1;
6612   if((fd0 = fdalloc(rf)) < 0 || (fd1 = fdalloc(wf)) < 0){
6613     if(fd0 >= 0)
6614       myproc()->ofile[fd0] = 0;
6615     fileclose(rf);
6616     fileclose(wf);
6617     return -1;
6618   }
6619   fd[0] = fd0;
6620   fd[1] = fd1;
6621   return 0;
6622 }
6623 
6624 
6625 
6626 
6627 
6628 
6629 
6630 
6631 
6632 
6633 
6634 
6635 
6636 
6637 
6638 
6639 
6640 
6641 
6642 
6643 
6644 
6645 
6646 
6647 
6648 
6649 
