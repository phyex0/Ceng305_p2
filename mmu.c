/**
 *
 * CENG305 Project-2
 *
 * A Simple Memory Management Unit for a Multithreaded Program.
 * This program simulates a contiguous memory management unit.
 *
 * Usage:  mmu <size_of_memory> <file_name> <alloc_strategy>
 *
 * @group_id 00
 * @author  your names
 *
 * @version 1.0, 23/12/21
 */


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
int threadnum = 0;
int *MEMORY;
int MEMORY_size;
pthread_mutex_t lock;
int method;

void *threadFun(void *filename){
   int myid;

   pthread_mutex_lock(&lock);
   myid=threadnum++;
   pthread_mutex_unlock(&lock);

   char my_filename[50];
   sprintf(my_filename,"%s_%d",(char*)filename,myid);

   printf("Thread %d will read %s \n",myid,(char*)my_filename);

//  your code goes here.


}

//
// Do not modify main function
//
int main(int argc, char *argv[]){
   int size = atoi(argv[1]);
   MEMORY_size=size/4;
   MEMORY = calloc(MEMORY_size,sizeof(int));  // each block is 4KB

   method = atoi(argv[3]);

   pthread_mutex_init(&lock, NULL);
   char method_str[50];
   if(method == 1){
      sprintf(method_str,"First_fit");
   }
   else if(method == 2){
      sprintf(method_str,"Best_fit");
   }
   else if(method == 3){
      sprintf(method_str,"Worst_fit");
   }
   printf("Program has launched, %s strategy will be used\n",method_str);
   pthread_t tid[4];
   for(int i=0;i<4;i++){
      pthread_create(&tid[i], NULL, threadFun, argv[2]);
   }
   for(int i=0;i<4;i++){
      pthread_join(tid[i], NULL);
   }
   return 0;
}
