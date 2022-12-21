/*
name: Heejin Jeong
assignment #2
"sgrep.c" file
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h> /* for skeleton code */
#include <unistd.h> /* for getopt */
#include "str.h"/* for string manipulation */

#define FIND_STR        "-f"
#define REPLACE_STR     "-r"
#define DIFF_STR        "-d"

#define MAX_STR_LEN 1023

#define FALSE 0
#define TRUE  1

typedef enum {
  INVALID,
  FIND,
  REPLACE,
  DIFF
} CommandType;

/* PrintUsage()
   print out the usage of the Simple Grep Program                  */
void 
PrintUsage(const char* argv0) 
{
  const static char *fmt = 
    "Simple Grep (sgrep) Usage:\n"
    "%s [COMMAND] [OPTIONS]...\n"
    "\nCOMMNAD\n"
    "\tFind: -f [search-string]\n"
    "\tReplace: -r [string1] [string2]\n"
    "\tDiff: -d [file1] [file2]\n";

  printf(fmt, argv0);
}

/* DoFind function
This function is to find line that have given string
parameter:
  format: const char*
  the string that you want to search
return value:
  format: int 
  if there is any error, 0
  else, 1
standard input:
  the file that contains many lines
  there are line including parameter
  there are line not including parameter,too 
standard output:
  the file that contains lines including parameter 
standard error:
  the file that contains error messages 
  that occur for improper input */
int
DoFind(const char *pcSearch)
{   
  if (StrGetLength(pcSearch) > MAX_STR_LEN) 
  {
    fprintf(stderr, "Error: arugment is too long\n");
    return FALSE;
  }
  char *buf=(char *)malloc(sizeof(char)*(MAX_STR_LEN+2));
  int len;
  char *output=(char *)malloc(sizeof(char)*2000000);
  while (fgets(buf, sizeof(char)*(MAX_STR_LEN+2), stdin)) 
  {
    /* check input line length */
    if ((len = StrGetLength(buf)) > MAX_STR_LEN) 
    {
      fprintf(stderr, "Error: input line is too long\n");
      return FALSE;
    }
    if (StrSearch(buf,pcSearch))
    {
       StrConcat(output,buf);
    }   
  }
  fprintf(stdout,"%s",output);
  free(output); 
  free(buf);
  return TRUE;
}

/* DoReplace function
This function is to replace string1 to string2 in lines
parameter1:
  format: const char*
  the string that you want to replace to parameter2
parameter2:
  format: const char*
  the string that you use to replace to parameter1
return value:
  format: int 
  if there is any error, 0
  else, 1
standard input:
  the file that contains many string lines 
  before replacing parameter1 to parameter2
standard output:
  the file that contains many string lines 
  after replacing parameter1 to parameter2 
standard error:
  the file that contains error messages 
  that occur for improper input */
int
DoReplace(const char *pcString1, const char *pcString2)
{  
  if ((StrGetLength(pcString1) > MAX_STR_LEN)
||(StrGetLength(pcString2) > MAX_STR_LEN)) 
  {
    fprintf(stderr, "Error: arugment is too long\n");
    return FALSE;
  }
  if (*pcString1=='\0')
  {
    fprintf(stderr, "Error: Can't replace an empty substring\n");
    return FALSE;
  }
  char buf[MAX_STR_LEN+2];
  int len; 
  char *output=(char *)malloc(sizeof(char)*2000000);
  char *pcout=output;
  char *pcbuf;
  size_t strlen1 = StrGetLength(pcString1);
  size_t strlen2 = StrGetLength(pcString2);
  while (fgets(buf, sizeof(char)*(MAX_STR_LEN+2), stdin)) 
  {
    if ((len = StrGetLength(buf)) > MAX_STR_LEN) 
    {
      fprintf(stderr, "Error: input line is too long\n");
      return FALSE;
    }
    pcbuf=buf;  
    while (StrSearch(pcbuf,pcString1))
    {
      while (pcbuf!=StrSearch(pcbuf,pcString1))
      {
        *pcout=*pcbuf;
        pcout++;
        pcbuf++;
      }
      StrConcat(output,pcString2);
      pcout+=strlen2;
      pcbuf+=strlen1;
    }
    while (*pcbuf!='\0')
    {
      *pcout=*pcbuf;
      pcout++;
      pcbuf++;
    }
  }
  fprintf(stdout,"%s",output);
  free(output); 
  return TRUE;
}

/* DoDiff function
This function is to print lines with line number that are different
parameter1:
  format: const char*
  the file name that you want to compare 
parameter2:
  format: const char*
  the file name that you want to compare
return value:
  format: int 
  if there is any error, 0
  else, 1
standard output:
  the file that contains line pairs with line number 
  that are different each other
standard error:
  ther file that contains error messages 
  that occur for improper input */
int
DoDiff(const char *file1, const char *file2)
{
  if ((StrGetLength(file1) > MAX_STR_LEN)||(StrGetLength(file2) > MAX_STR_LEN)) 
    {
      fprintf(stderr, "Error: arugment is too long\n");
      return FALSE;
    }
  FILE *pfile1 = fopen(file1,"r");
  FILE *pfile2 = fopen(file2,"r");
  if ((pfile1==NULL)&&(pfile2==NULL))
  {
    fprintf(stderr,"Error: failed to open file %s and %s\n",file1,file2);
    return FALSE;
  }
  else if (pfile1==NULL)
  {
    fprintf(stderr,"Error: failed to open file %s\n",file1);
    return FALSE;  
  }
  else if (pfile2==NULL)
  {
    fprintf(stderr,"Error: failed to open file %s\n",file2);
    return FALSE;  
  }
  char *buf1=(char *)malloc(sizeof(char)*(MAX_STR_LEN+2));
  char *buf2=(char *)malloc(sizeof(char)*(MAX_STR_LEN+2));
  char *buf3=(char *)malloc(sizeof(char)*(MAX_STR_LEN+2));
  char *buf4=(char *)malloc(sizeof(char)*(MAX_STR_LEN+2));
  int len1; int len2; int linenum=0;
  while ((buf3=fgets(buf1, sizeof(char)*(MAX_STR_LEN+2), pfile1))
&&(buf4=fgets(buf2, sizeof(char)*(MAX_STR_LEN+2), pfile2))) 
  { 
    if (((len1 = StrGetLength(buf1)) > MAX_STR_LEN) 
&& ((len2 = StrGetLength(buf2)) > MAX_STR_LEN)) 
    {
      fprintf(stderr, 
"Error: failed to open file %s and %s\n",file1,file2);
      return FALSE;
    }
    else if ((len1 = StrGetLength(buf1)) > MAX_STR_LEN) 
    {
      fprintf(stderr, "Error: failed to open file %s\n",file1);
      return FALSE;
    }
    else if ((len2 = StrGetLength(buf2)) > MAX_STR_LEN) 
    {
      fprintf(stderr, "Error: failed to open file %s\n",file2);
      return FALSE;
    }
    linenum++;
    if (StrCompare(buf1,buf2))
    {
      fprintf(stdout, "%s@%d:%s",file1,linenum,buf1);
      fprintf(stdout, "%s@%d:%s",file2,linenum,buf2);
    }
  }
  if ((buf3!=0)&&(buf4==0))
  {
    fprintf(stderr,"Error: %s ends early at line %d,"
,file2,linenum);
    fprintf(stderr," where %d is the final line number of %s.\n"
,linenum,file2);
  return FALSE;
  }
  if ((buf4!=0)&&(buf3==0))
  {
    fprintf(stderr,"Error: %s ends early at line %d,"
,file1,linenum);
    fprintf(stderr," where %d is the final line number of %s.\n"
,linenum,file1);
  return FALSE;
  }
  free(buf1);
  free(buf2);
  fclose(pfile1);
  fclose(pfile2);
  return TRUE;
}

/* CommandCheck() 
   - Parse the command and check number of argument. 
   - It returns the command type number
   - This function only checks number of argument. 
   - If the unknown function is given or the number of argument is 
   different from required number, this function returns FALSE.
   
   Note: You SHOULD check the argument rule later                  */
int
CommandCheck(const int argc, const char *argv1)
{
  int cmdtype = INVALID;
   
  /* check minimum number of argument */
  if (argc < 3)
    return cmdtype;
   
  /* check command type */ 
  if (strcmp(argv1, FIND_STR) == 0) {
    if (argc != 3)
      return FALSE;    
    cmdtype = FIND;       
  }
  else if (strcmp(argv1, REPLACE_STR) == 0) {
    if (argc != 4)
      return FALSE;
    cmdtype = REPLACE;
  }
  else if (strcmp(argv1, DIFF_STR) == 0) {
    if (argc != 4)
      return FALSE;
    cmdtype = DIFF;
  }
   
  return cmdtype;
}

int 
main(const int argc, const char *argv[]) 
{
  int type, ret;
   
  /* Do argument check and parsing */
  if (!(type = CommandCheck(argc, argv[1]))) {
    fprintf(stderr, "Error: argument parsing error\n");
    PrintUsage(argv[0]);
    return (EXIT_FAILURE);
  }
   
  /* Do appropriate job */
  switch (type) {
  case FIND:
    ret = DoFind(argv[2]);
    break;
  case REPLACE:
    ret = DoReplace(argv[2], argv[3]);
    break;
  case DIFF:
    ret = DoDiff(argv[2], argv[3]);
    break;
  } 

  return (ret)? EXIT_SUCCESS : EXIT_FAILURE;
}
