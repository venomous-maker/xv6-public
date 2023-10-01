3900 struct buf {
3901   int flags;
3902   uint dev;
3903   uint blockno;
3904   struct sleeplock lock;
3905   uint refcnt;
3906   struct buf *prev; // LRU cache list
3907   struct buf *next;
3908   struct buf *qnext; // disk queue
3909   uchar data[BSIZE];
3910 };
3911 #define B_VALID 0x2  // buffer has been read from disk
3912 #define B_DIRTY 0x4  // buffer needs to be written to disk
3913 
3914 
3915 
3916 
3917 
3918 
3919 
3920 
3921 
3922 
3923 
3924 
3925 
3926 
3927 
3928 
3929 
3930 
3931 
3932 
3933 
3934 
3935 
3936 
3937 
3938 
3939 
3940 
3941 
3942 
3943 
3944 
3945 
3946 
3947 
3948 
3949 
