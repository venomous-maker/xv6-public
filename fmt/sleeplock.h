3950 // Long-term locks for processes
3951 struct sleeplock {
3952   uint locked;       // Is the lock held?
3953   struct spinlock lk; // spinlock protecting this sleep lock
3954 
3955   // For debugging:
3956   char *name;        // Name of lock.
3957   int pid;           // Process holding lock
3958 };
3959 
3960 
3961 
3962 
3963 
3964 
3965 
3966 
3967 
3968 
3969 
3970 
3971 
3972 
3973 
3974 
3975 
3976 
3977 
3978 
3979 
3980 
3981 
3982 
3983 
3984 
3985 
3986 
3987 
3988 
3989 
3990 
3991 
3992 
3993 
3994 
3995 
3996 
3997 
3998 
3999 
