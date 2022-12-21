/*********************************************************************/
/*Name : Heejin Jeong                                                */
/*Student ID : 20170616                                              */
/*File name : ish.c                                                  */
/*********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>
#include <wait.h>
#include <string.h>
#include <signal.h>
#include "dynarray.h"

enum { MAX_LINE_SIZE = 1024 };
enum { FALSE, TRUE };
#define AGAIN 2
enum TokenType { ORDINARY, SPECIAL };

// Token : The storage of information of an input from shell
struct Token {
  
   enum TokenType ValueType;
   int number;
   char *pcValue;
  
};
/*********************************************************************/
/* lexLine function                                                  */
/* This function filters wrong inputs that is not enclosed in double */
/* quotes.                                                           */
/*********************************************************************/
int lexLine(char inputLine[], DynArray_T Tokens){

  assert(inputLine != NULL);
  assert(Tokens != NULL);

  int i = 0;
  int j = 0;
  int inQuote = 0;
  int casenum;
  int isNotErr = 0;
  struct Token *aToken;

  while (1){

    if (inputLine[j] == '\n')
      casenum = 1;
    else if (inputLine[j] == '|')
      casenum = 2;
    else if (isspace(inputLine[j]))
      casenum = 4;
    else if (inputLine[j] == '\"')
      casenum = 5;
    else
      casenum = 3;

    switch (casenum){

      case 1:
        if (inQuote == 0){
	  if (i != 0){
            aToken->pcValue[i]='\0';
	    isNotErr = DynArray_add(Tokens,aToken);
            if (!isNotErr){
              free(aToken->pcValue);
              free(aToken);
              return FALSE;
            }
	  }
	  return TRUE;
	}
	else{
	  if (i != 0){
            free(aToken->pcValue);
            free(aToken);
          }
	  return AGAIN;
	}
	break;

      case 2:
        if (inQuote == 0){
	  if (i != 0){
            aToken->pcValue[i]='\0';
	    isNotErr = DynArray_add(Tokens,aToken);
            if (!isNotErr){
              free(aToken->pcValue);
              free(aToken);
              return FALSE;
            }
	    i = 0;
	  }
          aToken = (struct Token *)malloc(sizeof(struct Token));
          if (aToken == NULL)
            return FALSE;
	  aToken->pcValue = (char *)malloc(MAX_LINE_SIZE);
          if (aToken->pcValue == NULL){
            free(aToken);
            return FALSE;
          }
	  aToken->ValueType = SPECIAL;
          aToken->number = 0;
	  aToken->pcValue[i]='|';
          aToken->pcValue[i+1]='\0';
	  isNotErr = DynArray_add(Tokens,aToken);
          if (!isNotErr){
            free(aToken->pcValue);
            free(aToken);
            return FALSE;
          }
	  j++;
	}
	else{
	  if (i == 0){
            aToken = (struct Token *)malloc(sizeof(struct Token));
            if (aToken == NULL)
              return FALSE;
	    aToken->pcValue = (char *)malloc(MAX_LINE_SIZE);
            if (aToken->pcValue == NULL){
              free(aToken);
              return FALSE;
            }
	    aToken->ValueType = ORDINARY;
            aToken->number = 0;
	  }
	  aToken->pcValue[i]='|';
	  i++;
	  j++;
	}
        break;
	
      case 3:
	if (i == 0){
          aToken = (struct Token *)malloc(sizeof(struct Token));
          if (aToken == NULL)
            return FALSE;
	  aToken->pcValue = (char *)malloc(MAX_LINE_SIZE);
          if (aToken->pcValue == NULL){
            free(aToken);
            return FALSE;
          }
	  aToken->ValueType = ORDINARY;
          aToken->number = 0;
        }
        aToken->pcValue[i]= inputLine[j];
	i++;
	j++;
        break;
    
      case 4:
        if (inQuote == 0){
	  if (i != 0){
            aToken->pcValue[i]='\0';
	    isNotErr = DynArray_add(Tokens,aToken);
            if (!isNotErr){
              free(aToken->pcValue);
              free(aToken);
              return FALSE;
            }
	    i = 0;
	  }
	  j++;
	}
	else{
	  if (i == 0){
            aToken = (struct Token *)malloc(sizeof(struct Token));
            if (aToken == NULL)
              return FALSE;
	    aToken->pcValue = (char *)malloc(MAX_LINE_SIZE);
            if (aToken->pcValue == NULL){
              free(aToken);
              return FALSE;
            }
	    aToken->ValueType = ORDINARY;
            aToken->number = 0;
	  }
	  aToken->pcValue[i]= inputLine[j];
	  i++;
	  j++;
	}
        break;

      case 5:
        if (inQuote == 0){
	  inQuote = 1;
	  j++;
	}
	else{
	  inQuote = 0;
	  j++;
	}
        break;
	
    }
  }  
}
/*********************************************************************/
/* freeToken function                                                */
/* This Function frees tokens in dynarray.                           */
/*********************************************************************/
void freeToken(void *pvElement, void *pvExtra){
 
  assert(pvElement != NULL);  

  if (pvExtra != NULL){
    pvExtra = pvElement;
  }
  free(((struct Token *)pvElement)->pcValue);
  free((struct Token *)pvElement);
}
/*********************************************************************/
/* synLine function                                                  */
/* This function filters wrong inputs. IF pipes are located wrongly, */
/* then function filter the input. Also it filters tokens array that */
/* is not include any token.                                         */
/*********************************************************************/
int synLine(DynArray_T Tokens){

  assert(Tokens != NULL);

  int inCommand = 0;
  int i = DynArray_getLength(Tokens) - 1;
  int pipenum = 0;
  int elenum = 0;
  int inArgument = 0;
   
  if (DynArray_getLength(Tokens) == 0)
    return -2;

  if (((struct Token *)DynArray_get(Tokens,i))->ValueType == SPECIAL)
    return -1;

  for (i=0;i<DynArray_getLength(Tokens);i++){
    if (inCommand == 0){
      if (((struct Token *)DynArray_get(Tokens,i))->ValueType == SPECIAL)
        return -1;
      else
        inCommand = 1;
    }
    else{
      if (((struct Token *)DynArray_get(Tokens,i))->ValueType == SPECIAL)
        inCommand = 0;
    }
  }

  for (i=0;i<DynArray_getLength(Tokens);i++){
    elenum++;
    if ((inArgument == 0) && (((struct Token *)DynArray_get(Tokens,i))->ValueType == ORDINARY)){
      inArgument = 1;
    }
    else if(((struct Token *)DynArray_get(Tokens,i))->ValueType == SPECIAL){
      ((struct Token *)DynArray_get(Tokens,i-elenum+1))->number = elenum-1;
      inArgument = 0;
      pipenum++;
      elenum = 0;
    }
  }   
  ((struct Token *)DynArray_get(Tokens,i-elenum))->number = elenum;   

  return pipenum;

} 
/*********************************************************************/
/* builtCommand                                                      */
/* This function handles "cd","setenv","unsetenv".and "exit" command.*/
/*********************************************************************/
int builtCommand(DynArray_T Tokens, int pipenum,char * errorfile){
 
  assert(Tokens != NULL);
  assert(pipenum >= 0);

  int isSuccessful;  

  if (strcmp(((struct Token *)DynArray_get(Tokens,0))->pcValue,"cd") == 0){

    if ( (pipenum !=0) || (((struct Token *)DynArray_get(Tokens,0))->number > 2) ){
      printf("%s: cd takes one parameter\n",errorfile);
      return AGAIN;  
    }   

    if (((struct Token *)DynArray_get(Tokens,0))->number == 1)
        isSuccessful = chdir("/mnt/home");
    else
        isSuccessful = chdir(((struct Token *)DynArray_get(Tokens,1))->pcValue);
      
    if (isSuccessful == -1)
      printf("%s: No such file or directory\n",errorfile);

  return AGAIN;
  }

  if (strcmp(((struct Token *)DynArray_get(Tokens,0))->pcValue,"setenv") == 0){

    if ( (pipenum !=0) || (((struct Token *)DynArray_get(Tokens,0))->number > 3) || (((struct Token *)DynArray_get(Tokens,0))->number == 1) ){
      printf("%s: setenv takes one or two parameters\n",errorfile);
      return AGAIN;  
    }
  
    if (((struct Token *)DynArray_get(Tokens,0))->number == 2)
      isSuccessful = setenv(((struct Token *)DynArray_get(Tokens,1))->pcValue,"",1);
    else
      isSuccessful = setenv(((struct Token *)DynArray_get(Tokens,1))->pcValue,((struct Token *)DynArray_get(Tokens,2))->pcValue,1);
      
    if (isSuccessful == -1)
      printf("%s: setenv failed\n",errorfile);

  return AGAIN;
  }

  if (strcmp(((struct Token *)DynArray_get(Tokens,0))->pcValue,"unsetenv") == 0){

    if ( (pipenum !=0) || (((struct Token *)DynArray_get(Tokens,0))->number != 2) ){
      printf("%s: unsetenv takes one parameter\n",errorfile);
      return AGAIN;  
    }
  
    isSuccessful = unsetenv(((struct Token *)DynArray_get(Tokens,1))->pcValue);
      
    if (isSuccessful == -1)
      printf("%s: unsetenv failed\n",errorfile);

  return AGAIN;
  }

  if (strcmp(((struct Token *)DynArray_get(Tokens,0))->pcValue,"exit") == 0){

    if ( (pipenum !=0) || (((struct Token *)DynArray_get(Tokens,0))->number != 1) ){
      printf("%s: exit does not take any parameters\n",errorfile);
      return AGAIN;
    }  
      
    DynArray_map(Tokens,freeToken,NULL);
    DynArray_free(Tokens);
    exit(0);
  }

  return TRUE;
}
/*********************************************************************/
/* sigquitHandler                                                    */
/* This Function handles SIGQUIT signal                              */
/*********************************************************************/
static void sigquitHandler(int iSig){

  void (*pfSignalRet)(int);

  alarm(5);

  pfSignalRet = signal(SIGQUIT,SIG_DFL);
  if (pfSignalRet == SIG_ERR){  
    fprintf(stderr,"\nSIGQUIT signal handling failed\n");
    exit(EXIT_FAILURE);
  }

  printf("\nType Ctrl-\\ again within 5 seconds to exit\n");

}
/*********************************************************************/
/* sigalarmHandler                                                   */
/* This Function handles SIGALRM signal                              */
/*********************************************************************/
static void sigalarmHandler(int iSig){

  void (*pfSignalRet)(int);

  pfSignalRet = signal(SIGQUIT,sigquitHandler);
  if (pfSignalRet == SIG_ERR){  
    fprintf(stderr,"SIGQUIT signal handling failed\n");
    exit(EXIT_FAILURE);
  }
}
/*********************************************************************/
/*********************************************************************/
int main(int argc, char *argv[]){
  
  char inputLine[MAX_LINE_SIZE];
  DynArray_T Tokens;
  int isSuccessful;
  int pipenum;
  void (*pfSignalRet)(int);
  char *pfFgetsRet;
  sigset_t sigSet;
  pid_t iPid;
  int pipeRtn;
  int sigRtn;
  int i;
  int j = 0;
  int k = 0;
  FILE *fp = fopen(".ishrc","r");
  int state = 0;

  sigRtn = sigaddset(&sigSet, SIGINT);
  if (sigRtn == -1){  
    perror(argv[0]);
    return EXIT_FAILURE;
  }

  sigRtn = sigaddset(&sigSet, SIGQUIT);
  if (sigRtn == -1){  
    perror(argv[0]);
    return EXIT_FAILURE;
  }

  sigRtn = sigaddset(&sigSet, SIGALRM);
  if (sigRtn == -1){  
    perror(argv[0]);
    return EXIT_FAILURE;
  }

  sigRtn = sigprocmask(SIG_UNBLOCK,&sigSet,NULL);
  if (sigRtn == -1){  
    perror(argv[0]);
    return EXIT_FAILURE;
  }

  pfSignalRet = signal(SIGINT,SIG_IGN);
  if (pfSignalRet == SIG_ERR){  
    perror(argv[0]);
    return EXIT_FAILURE;
  }

  pfSignalRet = signal(SIGQUIT,sigquitHandler);
  if (pfSignalRet == SIG_ERR){  
    perror(argv[0]);
    return EXIT_FAILURE;
  }

  pfSignalRet = signal(SIGALRM, sigalarmHandler);
  if (pfSignalRet == SIG_ERR){  
    perror(argv[0]);
    return EXIT_FAILURE;
  }

  // The position of shell input starting
  while(1){
    
    printf("%% ");

    if (state == 1)
      pfFgetsRet = fgets(inputLine, MAX_LINE_SIZE, stdin);
    
    else if (!feof(fp)){
      pfFgetsRet = fgets(inputLine, MAX_LINE_SIZE, fp);
      printf("%s",inputLine);
    }
    
    else{
      state = 1;
      fclose(fp);
      continue;
    } 

    if(pfFgetsRet != NULL){
       
      Tokens = DynArray_new(0);

      if (Tokens == NULL){
          perror(argv[0]);
          return EXIT_FAILURE;
      }    

      // Lexical analysis
      isSuccessful = lexLine(inputLine, Tokens); 

      if (isSuccessful == FALSE){
        DynArray_map(Tokens,freeToken,NULL);
        DynArray_free(Tokens);  
        perror(argv[0]);
        return EXIT_FAILURE;
      }

      if (isSuccessful == AGAIN){
        DynArray_map(Tokens,freeToken,NULL);
        DynArray_free(Tokens); 
        printf("%s: Could not find quote pair\n",argv[0]); 
        continue;
      }

      // Syntactic analysis
      isSuccessful = synLine(Tokens);

      if (isSuccessful == -1){
        DynArray_map(Tokens,freeToken,NULL);
        DynArray_free(Tokens);  
        printf("%s: Pipe or redirection destination not specified\n",argv[0]);
        continue;
      }

      if (isSuccessful == -2){
        DynArray_map(Tokens,freeToken,NULL);
        DynArray_free(Tokens);  
        continue;
      }

      // Buitin command handling
      pipenum = isSuccessful; 
      isSuccessful = builtCommand(Tokens,pipenum,argv[0]);   

      if (isSuccessful == AGAIN){        
        DynArray_map(Tokens,freeToken,NULL);
        DynArray_free(Tokens);  
        continue;
      }

/*********************************************************************/ 
     
      // A Command handling
      if (pipenum == 0){

        fflush(NULL);        
        iPid = fork();  
      
        if (iPid == -1){
          DynArray_map(Tokens,freeToken,NULL);
          DynArray_free(Tokens);  
          perror(argv[0]);
          return EXIT_FAILURE;
        }

        if (iPid == 0){

          pfSignalRet = signal(SIGINT,SIG_DFL);
          if (pfSignalRet == SIG_ERR){
            DynArray_map(Tokens,freeToken,NULL);
            DynArray_free(Tokens);   
            perror(argv[0]);
            return EXIT_FAILURE;
          }

          pfSignalRet = signal(SIGQUIT,SIG_DFL);
          if (pfSignalRet == SIG_ERR){  
            DynArray_map(Tokens,freeToken,NULL);
            DynArray_free(Tokens); 
            perror(argv[0]);
            return EXIT_FAILURE;
          }

          char *apcArgv[((struct Token *)DynArray_get(Tokens,0))->number+1];

          for (i=0; i<((struct Token *)DynArray_get(Tokens,0))->number; i++)
            apcArgv[i] = ((struct Token *)DynArray_get(Tokens,i))->pcValue;
          apcArgv[((struct Token *)DynArray_get(Tokens,0))->number]=NULL; 

          execvp(apcArgv[0],apcArgv);

          DynArray_map(Tokens,freeToken,NULL);
          DynArray_free(Tokens);  
          perror(apcArgv[0]);
          return EXIT_FAILURE;
        }

        iPid=wait(NULL);
        DynArray_map(Tokens,freeToken,NULL);
        DynArray_free(Tokens);
        continue;
      }

/*********************************************************************/

      // Commands handling that include one pipe
      if (pipenum == 1){

        int fd[2];

        pipeRtn = pipe(fd);
        if (pipeRtn == -1){ 
          DynArray_map(Tokens,freeToken,NULL);
          DynArray_free(Tokens);  
          perror(argv[0]);
          return EXIT_FAILURE;
        }

        fflush(NULL);
        iPid = fork();

        if (iPid == -1){
          DynArray_map(Tokens,freeToken,NULL);
          DynArray_free(Tokens);  
          perror(argv[0]);
          return EXIT_FAILURE;
        }
        if (iPid == 0){

          pfSignalRet = signal(SIGINT,SIG_DFL);
          if (pfSignalRet == SIG_ERR){  
            DynArray_map(Tokens,freeToken,NULL);
            DynArray_free(Tokens); 
            perror(argv[0]);
            return EXIT_FAILURE;
          }

          pfSignalRet = signal(SIGQUIT,SIG_DFL);
          if (pfSignalRet == SIG_ERR){  
            DynArray_map(Tokens,freeToken,NULL);
            DynArray_free(Tokens); 
            perror(argv[0]);
            return EXIT_FAILURE;
          }
          
          char *apcArgv[((struct Token *)DynArray_get(Tokens,0))->number+1];
          close(fd[0]);
          dup2(fd[1],1);
          close(fd[1]);

          for (i=0; i<((struct Token *)DynArray_get(Tokens,0))->number; i++)
            apcArgv[i] = ((struct Token *)DynArray_get(Tokens,i))->pcValue;
          apcArgv[((struct Token *)DynArray_get(Tokens,0))->number]=NULL; 

          execvp(apcArgv[0],apcArgv);

          DynArray_map(Tokens,freeToken,NULL);
          DynArray_free(Tokens);  
          perror(apcArgv[0]);
          return EXIT_FAILURE;
        }    
 
 /////////////////////////////////////////////////////////////////////////////////
  
        iPid = fork();

        if (iPid == -1){
          DynArray_map(Tokens,freeToken,NULL);
          DynArray_free(Tokens);  
          perror(argv[0]);
          return EXIT_FAILURE;
        }
        if (iPid == 0){

          pfSignalRet = signal(SIGINT,SIG_DFL);
          if (pfSignalRet == SIG_ERR){  
            DynArray_map(Tokens,freeToken,NULL);
            DynArray_free(Tokens); 
            perror(argv[0]);
            return EXIT_FAILURE;
          }

          pfSignalRet = signal(SIGQUIT,SIG_DFL);
          if (pfSignalRet == SIG_ERR){  
            DynArray_map(Tokens,freeToken,NULL);
            DynArray_free(Tokens); 
            perror(argv[0]);
            return EXIT_FAILURE;
          }

          j+=((struct Token *)DynArray_get(Tokens,0))->number +1;
          char *apcArgv2[((struct Token *)DynArray_get(Tokens,j))->number+1];
          close(fd[1]);
          dup2(fd[0],0);
          close(fd[0]);

          for (i=0; i<((struct Token *)DynArray_get(Tokens,j))->number; i++)
            apcArgv2[i] = ((struct Token *)DynArray_get(Tokens,i+j))->pcValue;
          apcArgv2[((struct Token *)DynArray_get(Tokens,j))->number]=NULL; 

          execvp(apcArgv2[0],apcArgv2);

          DynArray_map(Tokens,freeToken,NULL);
          DynArray_free(Tokens);  
          perror(apcArgv2[0]);
          return EXIT_FAILURE;
        }

        close(fd[0]);
        close(fd[1]); 
        iPid=wait(NULL);
        iPid=wait(NULL);
        DynArray_map(Tokens,freeToken,NULL);
        DynArray_free(Tokens);
        continue;
      }

/*********************************************************************/

      // Commands handling that include two pipe
      if (pipenum == 2){
        int fd1[2];
        int fd2[2];

        pipeRtn = pipe(fd1);
        if (pipeRtn == -1){ 
          DynArray_map(Tokens,freeToken,NULL);
          DynArray_free(Tokens);  
          perror(argv[0]);
          return EXIT_FAILURE;
        }

        pipeRtn = pipe(fd2);
        if (pipeRtn == -1){ 
          DynArray_map(Tokens,freeToken,NULL);
          DynArray_free(Tokens);  
          perror(argv[0]);
          return EXIT_FAILURE;
        }

        fflush(NULL);
        iPid = fork();

        if (iPid == -1){
          DynArray_map(Tokens,freeToken,NULL);
          DynArray_free(Tokens);  
          perror(argv[0]);
          return EXIT_FAILURE;
        }

        if (iPid == 0){

          pfSignalRet = signal(SIGINT,SIG_DFL);
          if (pfSignalRet == SIG_ERR){ 
            DynArray_map(Tokens,freeToken,NULL);
            DynArray_free(Tokens);  
            perror(argv[0]);
            return EXIT_FAILURE;
          }

          pfSignalRet = signal(SIGQUIT,SIG_DFL);
          if (pfSignalRet == SIG_ERR){  
            DynArray_map(Tokens,freeToken,NULL);
            DynArray_free(Tokens); 
            perror(argv[0]);
            return EXIT_FAILURE;
          }
          
          char *apcArgv[((struct Token *)DynArray_get(Tokens,0))->number+1];
          close(fd1[0]);
          close(fd2[0]);
          close(fd2[1]);
          dup2(fd1[1],1);
          close(fd1[1]);

          for (i=0; i<((struct Token *)DynArray_get(Tokens,0))->number; i++)
            apcArgv[i] = ((struct Token *)DynArray_get(Tokens,i))->pcValue;
          apcArgv[((struct Token *)DynArray_get(Tokens,0))->number]=NULL; 

          execvp(apcArgv[0],apcArgv);

          DynArray_map(Tokens,freeToken,NULL);
          DynArray_free(Tokens);  
          perror(apcArgv[0]);
          return EXIT_FAILURE;
        }     

 /////////////////////////////////////////////////////////////////////////////////
  
        iPid = fork();

        if (iPid == -1){
          DynArray_map(Tokens,freeToken,NULL);
          DynArray_free(Tokens);  
          perror(argv[0]);
          return EXIT_FAILURE;
        }

        if (iPid == 0){

          pfSignalRet = signal(SIGINT,SIG_DFL);
          if (pfSignalRet == SIG_ERR){  
            DynArray_map(Tokens,freeToken,NULL);
            DynArray_free(Tokens); 
            perror(argv[0]);
            return EXIT_FAILURE;
          }

          pfSignalRet = signal(SIGQUIT,SIG_DFL);
          if (pfSignalRet == SIG_ERR){  
            DynArray_map(Tokens,freeToken,NULL);
            DynArray_free(Tokens); 
            perror(argv[0]);
            return EXIT_FAILURE;
          }

          j+=((struct Token *)DynArray_get(Tokens,0))->number +1;
          char *apcArgv2[((struct Token *)DynArray_get(Tokens,j))->number+1];
          close(fd1[1]);
          close(fd2[0]);
          dup2(fd1[0],0);
          dup2(fd2[1],1);
          close(fd1[0]);
          close(fd2[1]);

          for (i=0; i<((struct Token *)DynArray_get(Tokens,j))->number; i++)
            apcArgv2[i] = ((struct Token *)DynArray_get(Tokens,i+j))->pcValue;
          apcArgv2[((struct Token *)DynArray_get(Tokens,j))->number]=NULL; 

          execvp(apcArgv2[0],apcArgv2);

          DynArray_map(Tokens,freeToken,NULL);
          DynArray_free(Tokens);  
          perror(apcArgv2[0]);
          return EXIT_FAILURE;
        }

 /////////////////////////////////////////////////////////////////////////////////
  
        iPid = fork();

        if (iPid == -1){
          DynArray_map(Tokens,freeToken,NULL);
          DynArray_free(Tokens);  
          perror(argv[0]);
          return EXIT_FAILURE;
        }

        if (iPid == 0){

          pfSignalRet = signal(SIGINT,SIG_DFL);
          if (pfSignalRet == SIG_ERR){  
            DynArray_map(Tokens,freeToken,NULL);
            DynArray_free(Tokens); 
            perror(argv[0]);
            return EXIT_FAILURE;
          }

          pfSignalRet = signal(SIGQUIT,SIG_DFL);
          if (pfSignalRet == SIG_ERR){  
            DynArray_map(Tokens,freeToken,NULL);
            DynArray_free(Tokens); 
            perror(argv[0]);
            return EXIT_FAILURE;
          }

          k+=((struct Token *)DynArray_get(Tokens,0))->number +1;
          k+=((struct Token *)DynArray_get(Tokens,k))->number +1;
          char *apcArgv3[((struct Token *)DynArray_get(Tokens,k))->number+1];
          close(fd1[1]);
          close(fd1[0]);
          close(fd2[1]);
          dup2(fd2[0],0);
          close(fd2[0]);

          for (i=0; i<((struct Token *)DynArray_get(Tokens,k))->number; i++)
            apcArgv3[i] = ((struct Token *)DynArray_get(Tokens,i+k))->pcValue;
          apcArgv3[((struct Token *)DynArray_get(Tokens,k))->number]=NULL; 

          execvp(apcArgv3[0],apcArgv3);

          DynArray_map(Tokens,freeToken,NULL);
          DynArray_free(Tokens);  
          perror(apcArgv3[0]);
          return EXIT_FAILURE;
        }
        close(fd1[0]);
        close(fd1[1]); 
        close(fd2[0]);
        close(fd2[1]); 
        iPid=wait(NULL);
        iPid=wait(NULL);
        iPid=wait(NULL);
        DynArray_map(Tokens,freeToken,NULL);
        DynArray_free(Tokens);
        continue;
      }

/*********************************************************************/
      if (pipenum > 2){
        printf("%s: No such file or directory\n",((struct Token *)DynArray_get(Tokens,k))->pcValue);
        DynArray_map(Tokens,freeToken,NULL);
        DynArray_free(Tokens);
        continue;
      }  
    }

    else{
  
        continue;

    }
  }

  return 0;

}
/*********************************************************************/






















































