6950 #include "types.h"
6951 #include "x86.h"
6952 
6953 void*
6954 memset(void *dst, int c, uint n)
6955 {
6956   if ((int)dst%4 == 0 && n%4 == 0){
6957     c &= 0xFF;
6958     stosl(dst, (c<<24)|(c<<16)|(c<<8)|c, n/4);
6959   } else
6960     stosb(dst, c, n);
6961   return dst;
6962 }
6963 
6964 int
6965 memcmp(const void *v1, const void *v2, uint n)
6966 {
6967   const uchar *s1, *s2;
6968 
6969   s1 = v1;
6970   s2 = v2;
6971   while(n-- > 0){
6972     if(*s1 != *s2)
6973       return *s1 - *s2;
6974     s1++, s2++;
6975   }
6976 
6977   return 0;
6978 }
6979 
6980 void*
6981 memmove(void *dst, const void *src, uint n)
6982 {
6983   const char *s;
6984   char *d;
6985 
6986   s = src;
6987   d = dst;
6988   if(s < d && s + n > d){
6989     s += n;
6990     d += n;
6991     while(n-- > 0)
6992       *--d = *--s;
6993   } else
6994     while(n-- > 0)
6995       *d++ = *s++;
6996 
6997   return dst;
6998 }
6999 
7000 // memcpy exists to placate GCC.  Use memmove.
7001 void*
7002 memcpy(void *dst, const void *src, uint n)
7003 {
7004   return memmove(dst, src, n);
7005 }
7006 
7007 int
7008 strncmp(const char *p, const char *q, uint n)
7009 {
7010   while(n > 0 && *p && *p == *q)
7011     n--, p++, q++;
7012   if(n == 0)
7013     return 0;
7014   return (uchar)*p - (uchar)*q;
7015 }
7016 
7017 char*
7018 strncpy(char *s, const char *t, int n)
7019 {
7020   char *os;
7021 
7022   os = s;
7023   while(n-- > 0 && (*s++ = *t++) != 0)
7024     ;
7025   while(n-- > 0)
7026     *s++ = 0;
7027   return os;
7028 }
7029 
7030 // Like strncpy but guaranteed to NUL-terminate.
7031 char*
7032 safestrcpy(char *s, const char *t, int n)
7033 {
7034   char *os;
7035 
7036   os = s;
7037   if(n <= 0)
7038     return os;
7039   while(--n > 0 && (*s++ = *t++) != 0)
7040     ;
7041   *s = 0;
7042   return os;
7043 }
7044 
7045 
7046 
7047 
7048 
7049 
7050 int
7051 strlen(const char *s)
7052 {
7053   int n;
7054 
7055   for(n = 0; s[n]; n++)
7056     ;
7057   return n;
7058 }
7059 
7060 
7061 
7062 
7063 
7064 
7065 
7066 
7067 
7068 
7069 
7070 
7071 
7072 
7073 
7074 
7075 
7076 
7077 
7078 
7079 
7080 
7081 
7082 
7083 
7084 
7085 
7086 
7087 
7088 
7089 
7090 
7091 
7092 
7093 
7094 
7095 
7096 
7097 
7098 
7099 
