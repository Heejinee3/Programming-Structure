/*
This is a word counting program named wc209.
The following codes are the assignment 1 coded by Heejin Jeong.
*/



#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>



/*
CountLineNum function is for counting line number
of decommented string.
The input of the function is decommented string. (type: array)
The output of the function is counted line number. (type: integer)
*/
int CountLineNum(char DecomStr[]){
  int LineNum=1; int i;
  if (DecomStr[0]=='\0'){
    return 0;
  }
  for (i=0; i<=(int)strlen(DecomStr); i++){
    if (DecomStr[i] == '\n'){
      LineNum++;
    }
  }
  return LineNum;
}



/*
CountWordNum function is for counting word number of 
decommented string.
Word numbers are distinguished by a kind of spaces
like space, new line or tab. 
The input of the function is decommented string. (type: array)
The output of the function is counted word number. (type: integer)
*/
int CountWordNum(char DecomStr[]){
  int WordNum=0; int i=0;
  while (DecomStr[i] != '\0'){
    if (isspace(DecomStr[i])){
      i++;
      continue;
    }
    WordNum++;
    i++;
    while (DecomStr[i] != '\0'){
      if (isspace(DecomStr[i])){
	i++;
	break;
      }
      i++;
    }
  }
  return WordNum;
}



int main(){
  // The following codes make to bring input stream in a file.
  // I expressed input string naming InputStr.
  char InputStr[2000000];
  fscanf(stdin,"%[^NULL]s",InputStr);
/*
The following codes show the process of decommenting input string.
I expressed decommented string naming DecomStr. (type: array)
Decommented string is easy to count lines, words, and characters.
Decommented string differ from input string
in point of removing comment.
*/
  char DecomStr[2000000]="";
  int i=0; int k=0; int ErrLineNum=1;
  while (InputStr[i] != '\0'){
    if (InputStr[i] != '/' || InputStr[i+1] != '*'){
      DecomStr[k] = InputStr[i];
      i++;
      k++;
      continue;
    }
    ErrLineNum = CountLineNum(DecomStr);
    i+=2;
    DecomStr[k]=' ';
    k++;
    while (1){
      if (InputStr[i] == '\n'){
	DecomStr[k] = InputStr[i];
	k++;
	i++;
      }
      else if (InputStr[i] == '*' && InputStr[i+1] == '/'){
	i+=2;
	break;
      }
      // When there is unterminated comment in input string,
      // the following codes output a file with error stream. 
      else if (InputStr[i] == '\0'){
	fprintf(stderr,"Error: line %d: unterminates comment\n",ErrLineNum);
	return EXIT_FAILURE;
      }
      else{
	i++;
      }
    }
  }
  //Finally when there is not any problem,
  //the following codes output a file
  //with the number of lines,world, and characters.
  fprintf(stdout,"%d %d %d\n",CountLineNum(DecomStr),
	  CountWordNum(DecomStr),(int)strlen(DecomStr));
  return EXIT_SUCCESS;
}
