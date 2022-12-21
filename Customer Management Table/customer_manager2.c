/*
Name: Heejin Jeong
Number of assignment: 3
File name: customer_manager2.c

This file helps to manage customers.
In this file, we can make database that contains customers' id,
name, and purchase. we can freely register and unregister customers' 
information, and sum the purchase of customers in database."
*/

#include <assert.h> //assert()
#include <stdlib.h> //calloc()
#include <stdio.h>
#include <string.h> //strcmp()
#include "customer_manager.h" // the header file of declaration of 
                              // functions related database 
                              // and FUNCPTR_T, DB_T

enum {BUCKET_COUNT = 1024};     // the number of the elements of HashTable
enum {HASH_MULTIPLIER = 65599}; // the number help to make hash number

// user informaion structure
// that contains user's name, id, and purchase,
// the pointer of next linked UserInfo in ID HashTable,
// the pointer of next linked UserInfo in name HashTable
struct UserInfo {
  char *name;                // customer name
  char *id;                  // customer id
  int purchase;              // purchase amount (> 0)
  struct UserInfo *nextid;   // the pointer of next linked UserInfo 
                             // in ID HashTable
  struct UserInfo *nextname; // the pointer of next linked UserInfo 
                             // in name HashTable
};

// HashTable structure
struct HashTable{
  struct UserInfo *array[BUCKET_COUNT]; // the array each elements
                                        // contains the pointer of
                                        // UserInfo
};

typedef struct HashTable* HashTable_T; // declaration of the pointer 
                                       // of HashTable

// database structure
// that contains the pointers of ID and name HashTables, 
// and stored number of items
struct DB {
  HashTable_T ht_id;         // the pointer of ID HashTable  
  HashTable_T ht_name;       // the pointer of name HashTable
  int numItems;              // # of stored items, needed to determine
			     // whether the array should be expanded
			     // or not
};

/*---------------------------------------------------------------------
hash_function function
THis function make strings a hash number
parameter: the pointer of key, number of the elements of HashTable
return value: hash number
---------------------------------------------------------------------*/
static int hash_function(const char *pcKey, int iBucketCount)
{
   int i;
   unsigned int uiHash = 0U;
   for (i = 0; pcKey[i] != '\0'; i++)
      uiHash = uiHash * (unsigned int)HASH_MULTIPLIER
               + (unsigned int)pcKey[i];
   return (int)(uiHash % (unsigned int)iBucketCount);
}

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
  d->ht_id = (HashTable_T)calloc(1,sizeof(struct HashTable));
  d->ht_name = (HashTable_T)calloc(1,sizeof(struct HashTable));
  if ((d->ht_id  == NULL) || (d->ht_name == NULL)) {
    fprintf(stderr, "Can't allocate a memory for hashtable\n");   
                                             // standard error
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
  struct UserInfo *p;
  struct UserInfo *nextp;
  int i;
  for (i=0;i<BUCKET_COUNT;i++){
    for (p = d->ht_id->array[i]; p != NULL ; p = nextp){
      nextp = p->nextid;
      free(p->name);
      free(p->id);
      free(p);
    }
  }
  free(d->ht_id);
  free(d->ht_name);
  free(d);
  d = NULL;
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
  if ((d == NULL) || (id == NULL) || (name == NULL) || (purchase <= 0)) {
    fprintf(stderr,"The input is improper\n"); // standard error
    return (-1);
  }
  struct UserInfo *p = (struct UserInfo*)calloc(1,sizeof(struct UserInfo));
  if (p == NULL) {
    fprintf(stderr,"Can't allocate a memory for user's information\n");
                                               // standard error
    return (-1);
  }
  int hash_id = hash_function(id,BUCKET_COUNT);
  int hash_name = hash_function(name,BUCKET_COUNT);
  char *copyid=strdup(id);
  char *copyname=strdup(name);
  if ((copyid == NULL) || (copyname == NULL)) {
    free(p);
    fprintf(stderr, "Can't allocate a memory for ID or name\n");
                                                // standard error
    return (-1);
  } 
  struct UserInfo *k;
  for (k = d->ht_id->array[hash_id]; k != NULL; k = k->nextid){
    if (strcmp(k->id,id) == 0){
      free(p);
      fprintf(stderr,"The customer is already registered\n");
                                                 // standard error
      return (-1);
    }
  }
  for (k = d->ht_name->array[hash_name]; k != NULL; k = k->nextname){
    if (strcmp(k->name,name) == 0){
      free(p);
      fprintf(stderr,"The customer is already registered\n");
                                                  // standard error
      return (-1);
    }
  }
  p->name = copyname;
  p->id = copyid;
  p->purchase = purchase;
  p->nextid = d->ht_id->array[hash_id];
  d->ht_id->array[hash_id] = p;
  p->nextname = d->ht_name->array[hash_name];
  d->ht_name->array[hash_name] = p;
  return 0;
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
  if ((d == NULL) || (id == NULL)) {
    fprintf(stderr,"The input is improper\n"); // standard error
    return (-1);
  }
  struct UserInfo *p;
  struct UserInfo *k;
  struct UserInfo *p_beforeid;
  struct UserInfo *k_beforename;
  char *name;
  int count1=0; int i;
  int count2=0; int j;
  int hash_id = hash_function(id,BUCKET_COUNT);
  int hash_name;
  for (p = d->ht_id->array[hash_id];p != NULL; p = p->nextid){
    if (strcmp(p->id,id) == 0){
      p_beforeid = d->ht_id->array[hash_id];
      for (i=0;i<count1-1;i++){
        p_beforeid = p_beforeid->nextid;
      }
      if (count1 == 0){
        d->ht_id->array[hash_id] = p->nextid;
      }
      else {
        p_beforeid->nextid = p->nextid;
      }
      name = p->name;
      hash_name = hash_function(name,BUCKET_COUNT);
      for (k = d->ht_name->array[hash_name];k != NULL; k = k->nextname){
        if (strcmp(k->name,name) == 0){
          k_beforename = d->ht_name->array[hash_name];
          for (j=0;j<count2-1;j++){
            k_beforename = k_beforename->nextname;
          }
          if (count2 == 0){
            d->ht_name->array[hash_name] = k->nextname;
          }
          else {
            k_beforename->nextname = k->nextname;
          }
          free(p->id);
          free(p->name);
          free(p);
          return 0;
        }
        count2++;
      }
    }
    count1++;
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
  if ((d == NULL) || (name == NULL)) {
    fprintf(stderr,"The input is improper\n"); // standard error
    return (-1);
  }
  struct UserInfo *p;
  struct UserInfo *k;
  struct UserInfo *p_beforeid;
  struct UserInfo *k_beforename;
  char *id;
  int count1=0; int i;
  int count2=0; int j;
  int hash_name = hash_function(name,BUCKET_COUNT);
  int hash_id;
  for (k = d->ht_name->array[hash_name];k != NULL; k = k->nextname){
    if (strcmp(k->name,name) == 0){
      k_beforename = d->ht_name->array[hash_name];
      for (i=0;i<count1-1;i++){
        k_beforename = k_beforename->nextname;
      }
      if (count1 == 0){
        d->ht_name->array[hash_name] = k->nextname;
      }
      else {
        k_beforename->nextname = k->nextname;
      }
      id = k->id;
      hash_id = hash_function(id,BUCKET_COUNT);
      for (p = d->ht_id->array[hash_id];p != NULL; p = p->nextid){
        if (strcmp(p->id,id) == 0){
          p_beforeid = d->ht_id->array[hash_id];
          for (j=0;j<count2-1;j++){
            p_beforeid = p_beforeid->nextid;
          }
          if (count2 == 0){
            d->ht_id->array[hash_id] = p->nextid;
          }
          else {
            p_beforeid->nextid = p->nextid;
          }
          free(k->id);
          free(k->name);
          free(k);
          return 0;
        }
        count2++;
      }
    }
    count1++;
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
  if ((d == NULL) || (id == NULL)) {
    fprintf(stderr,"The input is improper\n"); // standard error
    return (-1);
  }
  struct UserInfo *p;
  int hash_id = hash_function(id,BUCKET_COUNT);
  for (p = d->ht_id->array[hash_id];p != NULL; p = p->nextid){
    if (strcmp(p->id,id) == 0) {
      return p->purchase;
    }
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
  if ((d == NULL) || (name == NULL)) {
    fprintf(stderr,"The input is improper\n"); // standard error
    return (-1);
  }
  struct UserInfo *k;
  int hash_name = hash_function(name,BUCKET_COUNT);
  for (k = d->ht_name->array[hash_name];k != NULL; k = k->nextname){
    if (strcmp(k->name,name) == 0) {
      return k->purchase;
    }
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
    fprintf(stderr,"The input is invalid\n"); //standard error
    return (-1);
  }
  int sum=0;
  int i;
  struct UserInfo *p;
  for (i=0;i < BUCKET_COUNT;i++){ 
    for (p = d->ht_id->array[i];p != NULL; p = p->nextid){
      sum += fp(p->id,p->name,p->purchase);
    }
  }
  return sum;
}



