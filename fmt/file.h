4200 struct file {
4201   enum { FD_NONE, FD_PIPE, FD_INODE } type;
4202   int ref; // reference count
4203   char readable;
4204   char writable;
4205   struct pipe *pipe;
4206   struct inode *ip;
4207   uint off;
4208 };
4209 
4210 
4211 // in-memory copy of an inode
4212 struct inode {
4213   uint dev;           // Device number
4214   uint inum;          // Inode number
4215   int ref;            // Reference count
4216   struct sleeplock lock; // protects everything below here
4217   int valid;          // inode has been read from disk?
4218 
4219   short type;         // copy of disk inode
4220   short major;
4221   short minor;
4222   short nlink;
4223   uint size;
4224   uint addrs[NDIRECT+1];
4225 };
4226 
4227 // table mapping major device number to
4228 // device functions
4229 struct devsw {
4230   int (*read)(struct inode*, char*, int);
4231   int (*write)(struct inode*, char*, int);
4232 };
4233 
4234 extern struct devsw devsw[];
4235 
4236 #define CONSOLE 1
4237 
4238 
4239 
4240 
4241 
4242 
4243 
4244 
4245 
4246 
4247 
4248 
4249 
