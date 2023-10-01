#include "types.h"
#include "x86.h"

void*
memset(void *dst, int c, uint n)
{
  if ((int)dst%4 == 0 && n%4 == 0){
    c &= 0xFF;
    stosl(dst, (c<<24)|(c<<16)|(c<<8)|c, n/4);
  } else
    stosb(dst, c, n);
  return dst;
}

int
memcmp(const void *v1, const void *v2, uint n)
{
  const uchar *s1, *s2;

  s1 = v1;
  s2 = v2;
  while(n-- > 0){
    if(*s1 != *s2)
      return *s1 - *s2;
    s1++, s2++;
  }

  return 0;
}

void*
memmove(void *dst, const void *src, uint n)
{
  const char *s;
  char *d;

  s = src;
  d = dst;
  if(s < d && s + n > d){
    s += n;
    d += n;
    while(n-- > 0)
      *--d = *--s;
  } else
    while(n-- > 0)
      *d++ = *s++;

  return dst;
}

// memcpy exists to placate GCC.  Use memmove.
void*
memcpy(void *dst, const void *src, uint n)
{
  return memmove(dst, src, n);
}

int
strncmp(const char *p, const char *q, uint n)
{
  while(n > 0 && *p && *p == *q){
      n--; p++; q++;
  }
  if(n == 0)
    return 0;
  return (uchar)*p - (uchar)*q;
}

char*
strncpy(char *s, const char *t, int n)
{
  char *os;

  os = s;
  while(n-- > 0 && (*s++ = *t++) != 0)
    ;
  while(n-- > 0)
    *s++ = 0;
  return os;
}

// Like strncpy but guaranteed to NUL-terminate.
char*
safestrcpy(char *s, const char *t, int n)
{
  char *os;

  os = s;
  if(n <= 0)
    return os;
  while(--n > 0 && (*s++ = *t++) != 0)
    ;
  *s = 0;
  return os;
}

int
strlen(const char *s)
{
  int n;

  for(n = 0; s[n]; n++)
    ;
  return n;
}

int
Tolower(int c) {
    if (c >= 'A' && c <= 'Z') {
        return c + 'a' - 'A';
    }
    return c;
}

char *strnstr(const char *haystack, const char *needle) {
    int i, j;
    for (i = 0; haystack[i]; i++) {
        for (j = 0; needle[j]; j++) {
            if (Tolower(haystack[i + j]) != Tolower(needle[j])) {
                break;
            }
        }
        if (needle[j] == '\0') {
            return (char *)(haystack + i);
        }
    }
    return NULL;
}

void strnlower(char *str) {
    if (str == 0) {
        return; // Handle NULL input
    }

    for (int i = 0; str[i]; i++) {
        str[i] = Tolower(str[i]);
    }
}
// Function to check if a sentence is available in an array of strings
void strnavail(const char* string, const char** strings, int numStrings, int* result) {
    if (string == NULL || strings == NULL || numStrings <= 0) {
        return; // Handle invalid input
    }
    int min1;
    int min2 = strlen(string);
    for (int i = 0; i < numStrings; i++) {
        min1 = strlen(strings[i]);
        if (min1 < min2 || min1 > min2) {
            // Skip strings in the array that are shorter or longer than the target string
            continue;
        }
        if (strncmp(strings[i], string, min2) == 0) {
            result[0] = 1;
            result[1] = i;
            return; // Sentence found in one of the strings
        }
    }

    // Sentence not found in any of the strings
    result[0] = 0;
}


