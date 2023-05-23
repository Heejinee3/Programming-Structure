/*
Name: Heejin Jeong
Number of assignment: 3
File name: customer_manager1.c

This file helps to manage customers.
In this file, we can make database that contains customers' id,
name, and purchase. we can freely register and unregister customers' 
information, and sum the purchase of customers in database."
*/

#include <assert.h> //assert()
#include <stdlib.h> //calloc(),realloc()
#include <stdio.h>
#include <string.h> //strcmp()
#include "customer_manager.h" // the header file of declaration of 
                              // functions related database 
                              // and FUNCPTR_T, DB_T

#define UNIT_ARRAY_SIZE 1024 // number of arrays that can be expanded
                             // one time

// user informaion structure
// that contains user's name, id, and purchase
struct UserInfo {
  char *name;                // customer name
  char *id;                  // customer id
  int purchase;              // purchase amount (> 0)
};

// database structure
// that contains the pointer to the array(lots of user information),
// expanded array size, and stored number of items
struct DB {
  struct UserInfo *pArray;   // pointer to the array
  int curArrSize;            // current array size (max # of elements)
  int numItems;              // # of stored items, needed to determine
                 	     // whether the array should be expanded
			     // or not
};

/*---------------------------------------------------------------------
CreateCustomerDB function
THis function create customer database.
return value: (if there is an error) NULL
              (else) the pointer of database
standard error: if it can't allocate a memory for database,
                it writes error stream.
---------------------------------------------------------------------*/
DB_T
CreateCustomerDB(void)
{
  DB_T d;
  d = (DB_T) calloc(1, sizeof(struct DB));
  if (d == NULL) {
    fprintf(stderr, "Can't allocate a memory for DB_T\n");
                                             // standard error
    return NULL;
  }
  d->curArrSize = UNIT_ARRAY_SIZE; // start with 1024 elements
  d->pArray = (struct UserInfo *)calloc(d->curArrSize,
               sizeof(struct UserInfo));
  if (d->pArray == NULL) {
    fprintf(stderr, "Can't allocate a memory for array of size %d\n", 
	    d->curArrSize); // standard error  
    free(d);
    return NULL;
  }
  return d;
}
/*---------------------------------------------------------------------
DestroyCustomerDB function
THis function destroy customer database.
parameter: the pointer of database
error: it occurs when parameter is NULL 
---------------------------------------------------------------------*/
void
DestroyCustomerDB(DB_T d)
{
  assert(d!=NULL);
  struct UserInfo *curpArr = d->pArray;
  int i;
  for (i=0;i<(d->numItems);i++){
    free(curpArr->id);
    free(curpArr->name);
    curpArr++;
  }
  free(d->pArray);
  free(d);
  d=NULL;
}
/*---------------------------------------------------------------------
RegisterCustomer function
THis function register a customer into the database.
parameter: the pointer of database, the pointer of id char,
           the pointer of name char, the purchase typed int
return value: (if there is an error) int -1
              (else) int 0
standard error: an error occurs in the following situation
                1. the input is improper
                2. it can't allocate a memory for database,
                3. the customer is alreay registered
---------------------------------------------------------------------*/
int
RegisterCustomer(DB_T d, const char *id,
		 const char *name, const int purchase)
{
  if ((d == NULL)||(id == NULL)||(name == NULL)||(purchase<=0)){
    fprintf(stderr,"The input is improper\n"); // standard error 
    return (-1);
  }
  char *copyid=strdup(id);
  char *copyname=strdup(name);
  if ((copyid == NULL) || (copyname == NULL)) {
    fprintf(stderr, "Can't allocate a memory for ID and name\n");
                                     // standard error 
    return (-1);
   } 
  struct UserInfo *curpArr;
  if ((d->numItems) == (d->curArrSize)){
    curpArr = (struct UserInfo *)realloc(d->pArray,
           (sizeof(struct UserInfo))*((d->curArrSize)+UNIT_ARRAY_SIZE));
    if (curpArr == NULL){
      fprintf(stderr, "Can't allocate a memory for data base\n");
                                    // standard error 
      return (-1);  
    }
    d->pArray = curpArr;
    (d->curArrSize) += UNIT_ARRAY_SIZE;
  }
  else{
    curpArr=d->pArray;
  }
  for (int i=0;i<(d->numItems);i++){
    if (curpArr->id != 0){
      if ((strcmp(curpArr->id,copyid) == 0) ||
              (strcmp(curpArr->name,copyname) == 0)){
        fprintf(stderr,"The customer is alreay registered\n");
                                   // standard error 
        return (-1);
       }
    }
    curpArr++;
  }
  (curpArr->id) = copyid;
  (curpArr->name) = copyname;
  (curpArr->purchase) = purchase;
  (d->numItems)++;
  return (0);
}
/*---------------------------------------------------------------------
UnregisterCustomerByID function
THis function unregister a customer in the database using id.
parameter: the pointer of database, the pointer of id char
return value: (if there is an error) int -1
              (else) int 0
standard error: an error occurs in the following situation
                1. the input is improper
                2. the customer is alreay registered
---------------------------------------------------------------------*/
int
UnregisterCustomerByID(DB_T d, const char *id)
{
  if ((d == NULL)||(id == NULL)){
    fprintf(stderr,"The input is improper\n"); // standard error 
    return (-1);
  }
  struct UserInfo *curpArr = d->pArray;
  int i=0;
  for (;i<(d->numItems);i++){
    if (curpArr->id != 0){
      if (strcmp(curpArr->id,id) == 0){
        free(curpArr->id);
        curpArr->id = 0;
        free(curpArr->name);
        curpArr->name = 0;
        curpArr->purchase =0;
        return 0;
      }
    }
    curpArr++;
  }
  fprintf(stderr,"The customer is not registered\n"); // standard error
  return (-1);
}
/*---------------------------------------------------------------------
UnregisterCustomerByName function
THis function unregister a customer in the database using name.
parameter: the pointer of database, the pointer of name char
return value: (if there is an error) int -1
              (else) int 0
standard error: an error occurs in the following situation
                1. the input is improper
                2. the customer is alreay registered
---------------------------------------------------------------------*/
int
UnregisterCustomerByName(DB_T d, const char *name)
{
  if ((d == NULL)||(name == NULL)){
    fprintf(stderr,"The input is improper\n"); // standard error
    return (-1);
  }
  struct UserInfo *curpArr = d->pArray;
  int i=0;
  for (;i<(d->numItems);i++){
    if (curpArr->name != 0){
      if (strcmp(curpArr->name,name) == 0){
        free(curpArr->id);
        curpArr->id = 0;
        free(curpArr->name);
        curpArr->name = 0;
        curpArr->purchase = 0;
        return 0;
      }
    }
    curpArr++;
  }
  fprintf(stderr,"The customer is not registered\n"); // standard error
  return (-1);
}
/*---------------------------------------------------------------------
GetPurchaseByID function
THis function get purchase of a customer in the database using id.
parameter: the pointer of database, the pointer of id char
return value: (if there is an error) int -1
              (else) int 0
standard error: an error occurs in the following situation
                1. the input is improper
                2. the customer is alreay registered
---------------------------------------------------------------------*/
int
GetPurchaseByID(DB_T d, const char* id)
{
  if ((d == NULL)||(id == NULL)){
    fprintf(stderr,"The input is improper\n"); // standard error
    return (-1);
  }
  struct UserInfo *curpArr = d->pArray;
  int i=0;
  for (;i<(d->numItems);i++){
    if (curpArr->id != 0){
      if (strcmp(curpArr->id,id) == 0){
        return curpArr->purchase;
      }
    }
    curpArr++;
  }
  fprintf(stderr,"The customer is not registered\n"); // standard error
  return (-1);
}
/*---------------------------------------------------------------------
GetPurchaseByName function
THis function get purchase of a customer in the database using name.
parameter: the pointer of database, the pointer of name char
return value: (if there is an error) int -1
              (else) int 0
standard error: an error occurs in the following situation
                1. the input is improper
                2. the customer is alreay registered
---------------------------------------------------------------------*/
int
GetPurchaseByName(DB_T d, const char* name)
{
  if ((d == NULL)||(name == NULL)){
    fprintf(stderr,"The input is improper\n"); // standard error
    return (-1);
  }
  struct UserInfo *curpArr = d->pArray;
  int i=0;
  for (;i<(d->numItems);i++){
    if (curpArr->name != 0){
      if (strcmp(curpArr->name,name) == 0){
        return curpArr->purchase;
      }
    }
    curpArr++;
  }
  fprintf(stderr,"The customer is not registered\n"); // standard error
  return (-1);
}
/*---------------------------------------------------------------------
GetSumCustomerPurchase function
fp is a pointer of function which parameters are 
the pointer of id char,the pointer of name char, the purchase typed int
THis function sum the return value of fp changing parameter value
inputing users' information in database.
parameter: the pointer of database, the pointer of function
return value: (if there is an error) int -1
              (else) int sum (of return value of function)
standard error: an error occurs when the input is improper
---------------------------------------------------------------------*/
int
GetSumCustomerPurchase(DB_T d, FUNCPTR_T fp)
{
  if ((d == NULL)||(fp == NULL)){
    fprintf(stderr,"The input is improper\n"); //standard error
    return (-1);
  }
  int sum=0;
  int i=0;
  struct UserInfo *curpArr = d->pArray;
  for (;i<(d->numItems);i++){
    if (curpArr->name != 0){
      sum += fp (curpArr->id,curpArr->name,curpArr->purchase);
    }
    curpArr++;
  }
  return sum;
}
