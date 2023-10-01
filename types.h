typedef unsigned int   uint;
typedef unsigned short ushort;
typedef unsigned char  uchar;
typedef uint pde_t;
typedef __SIZE_TYPE__ size_t;
#define NULL ((void*)0)
#define MAX_LINE_LENGTH 1024
#define MAX_LINES 256

struct pstat {
  uint creation_time;
  uint end_time;
  uint total_time;
  char* name;
  uint status;
};
