/*
name: Heejin Jeong
assignment #2
"str.c" file
*/

#include <assert.h> 
#include <stdio.h>
#include "str.h"

/* StrGetLength function
This function is for string to get length
parameter:
  format: const char*
  the string that you want to get length
return value:
  format: size_t
  length of string */
size_t StrGetLength(const char* pcSrc)
{
  const char *pcEnd;
  assert(pcSrc); 
  pcEnd = pcSrc;
	
  while (*pcEnd) 
    pcEnd++;

  return (size_t)(pcEnd - pcSrc);
}

/* StrCopy function
This function is for string1 to change like string2
parameter1:
  format: char*
  the string that you want to change like parameter2
parameter2:
  format: const char*
  the string that is copied to parameter1
return value:
  format: char *
  parameter1 that changed by this function */
char *StrCopy(char *pcDest, const char* pcSrc)
{
  assert((pcDest != NULL) && (pcSrc != NULL));
  char *Origin = pcDest;
  while (*pcSrc != '\0') 
  {
    *pcDest = *pcSrc;
    pcSrc++;
    pcDest++;
  }
  while (*pcDest != '\0') 
  {
    *pcDest = '\0';
    pcDest++;
  }  
  return Origin;
}

/* StrCompare function
This function is to compare string1 and string2
parameter1:
  format: const char*
  the string that you want to compare parameter2
parameter2:
  format: const char*
  the string that you want to compare parameter1
return value:
  format: int 
  if parameter1=parameter2, 0
  else if parameter1!=parameter2, other value except 0 */
int StrCompare(const char* pcS1, const char* pcS2)
{
  assert((pcS1 != NULL) && (pcS2 != NULL));
  int i;
  while (*pcS1 == *pcS2) 
  {
    if (*pcS1 == '\0') 
    {
      return 0;
    }
    pcS1++;
    pcS2++;
  }
  i = *pcS1 - *pcS2;
  return i;
}

/* StrSearch function
This function is to search string1 in string2
parameter1:
  format: const char*
  the string where string2 is searched in 
parameter2:
  format: const char*
  the string that you want to search in string1
return value:
  format: char *
  if there is parameter2 in parameter1, 
    the point where first parameter2 exist 
  else if there is not parameter2 in parameter1,
    0 */
char *StrSearch(const char* pcHaystack, const char *pcNeedle)
{
  assert((pcHaystack != NULL) && (pcNeedle != NULL));
  const char *Location = pcHaystack;
  const char *ComeBack = pcNeedle;
  if (*pcNeedle == '\0') 
  {
    return (char *)Location;
  }
  while (*pcHaystack != '\0') 
  {
    if (*pcHaystack != *pcNeedle)
    {
      pcHaystack++;
    }
    else 
    {
      Location = pcHaystack;
      pcHaystack++;
      pcNeedle++;
      while (1)
      {
	if (*pcNeedle == '\0') 
        {
	  return (char *)Location;
	}
	else 
        {
	  if (*pcHaystack == '\0') 
          {
	    return 0;
	  }
	  if (*pcHaystack == *pcNeedle) 
          {
	    pcHaystack++;
	    pcNeedle++;
	  }
	  else 
          {
	    pcNeedle = ComeBack;
	    break;
	  }
	}
      }
    }
  }
  return 0;
} 

/* StrConcat function
This function is to add string2 to string2
parameter1:
  format: char*
  the string where parameter2 adds 
parameter2:
  format: const char*
  the string that added in string1
return value:
  format: char *
  the point of parameter1 that changed by this function*/
char *StrConcat(char *pcDest, const char *pcSrc)
{
  assert((pcDest != NULL) && (pcSrc != NULL));
  char *Origin = pcDest;
  pcDest += StrGetLength(pcDest);
  while (*pcSrc != '\0') 
  {
    *pcDest = *pcSrc;
    pcDest++;
    pcSrc++;
  }
  *pcDest='\0';
  return Origin;
}
