4100 // On-disk file system format.
4101 // Both the kernel and user programs use this header file.
4102 
4103 
4104 #define ROOTINO 1  // root i-number
4105 #define BSIZE 512  // block size
4106 
4107 // Disk layout:
4108 // [ boot block | super block | log | inode blocks |
4109 //                                          free bit map | data blocks]
4110 //
4111 // mkfs computes the super block and builds an initial file system. The
4112 // super block describes the disk layout:
4113 struct superblock {
4114   uint size;         // Size of file system image (blocks)
4115   uint nblocks;      // Number of data blocks
4116   uint ninodes;      // Number of inodes.
4117   uint nlog;         // Number of log blocks
4118   uint logstart;     // Block number of first log block
4119   uint inodestart;   // Block number of first inode block
4120   uint bmapstart;    // Block number of first free map block
4121 };
4122 
4123 #define NDIRECT 12
4124 #define NINDIRECT (BSIZE / sizeof(uint))
4125 #define MAXFILE (NDIRECT + NINDIRECT)
4126 
4127 // On-disk inode structure
4128 struct dinode {
4129   short type;           // File type
4130   short major;          // Major device number (T_DEV only)
4131   short minor;          // Minor device number (T_DEV only)
4132   short nlink;          // Number of links to inode in file system
4133   uint size;            // Size of file (bytes)
4134   uint addrs[NDIRECT+1];   // Data block addresses
4135 };
4136 
4137 
4138 
4139 
4140 
4141 
4142 
4143 
4144 
4145 
4146 
4147 
4148 
4149 
4150 // Inodes per block.
4151 #define IPB           (BSIZE / sizeof(struct dinode))
4152 
4153 // Block containing inode i
4154 #define IBLOCK(i, sb)     ((i) / IPB + sb.inodestart)
4155 
4156 // Bitmap bits per block
4157 #define BPB           (BSIZE*8)
4158 
4159 // Block of free map containing bit for block b
4160 #define BBLOCK(b, sb) (b/BPB + sb.bmapstart)
4161 
4162 // Directory is a file containing a sequence of dirent structures.
4163 #define DIRSIZ 14
4164 
4165 struct dirent {
4166   ushort inum;
4167   char name[DIRSIZ];
4168 };
4169 
4170 
4171 
4172 
4173 
4174 
4175 
4176 
4177 
4178 
4179 
4180 
4181 
4182 
4183 
4184 
4185 
4186 
4187 
4188 
4189 
4190 
4191 
4192 
4193 
4194 
4195 
4196 
4197 
4198 
4199 
