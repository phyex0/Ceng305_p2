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
   Halit Burak Yeşildal          18050111043
   Enes Güler                    18050111005
   Yakup Batuhan Ördek           18050111041
   Ayşe Tüncer                   170501110580
   Zeynep Nur Tüncel             17050111011
   Berna Altun                   16050111040
  
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

//defined by us
int WASTED_INTERNAL_MEMORY = 0;
int EXTERNAL_FRAGMENTATION_COUNT = 0;
int INSUFFICIENT_MEMORY_COUNT = 0;

void start_process(int process_id, int process_size);
void end_process(int process_id);
int insufficent_check(int process_id, int temp_size, int frame_size);
void print_report();
void first_fit(int process_id, int process_size);
void best_fit(int process_id, int process_size);
void worst_fit(int process_id, int process_size);
void put_process_to_index(int worst_ind, int frame_size, int process_id, int process_size);

void *threadFun(void *filename)
{
   int myid;

   pthread_mutex_lock(&lock);
   myid = threadnum++;
   pthread_mutex_unlock(&lock);

   char my_filename[50];
   sprintf(my_filename, "%s_%d.txt", (char *)filename, myid);

   printf("Thread %d will read %s \n", myid, (char *)my_filename);

   //  your code goes here.

   FILE *fptr = fopen(my_filename, "r");
   char process_type;
   int process_id, process_size;

   while (feof(fptr) != 1)
   {
      fscanf(fptr, "%c %d", &process_type, &process_id);
      if (process_type == 'B')
      {
         fscanf(fptr, "%d", &process_size);
         start_process(process_id, process_size);
      }

      else
         end_process(process_id);
   }
   return 0;
}
// use insufficent to be sure empty spaces exist on mem !
//@phyex
void first_fit(int process_id, int process_size)
{
   int wasted_internal_memory = (4 - process_size % 4) % 4;
   int temp_size = process_size;
   if (wasted_internal_memory != 0)
      process_size += wasted_internal_memory;

   int frame_size = process_size / 4;
   if (insufficent_check(process_id, temp_size, frame_size) == 1)
   {
      // Fill this
   }
}

//@ark
void best_fit(int process_id, int process_size)
{
   int wasted_internal_memory = (4 - process_size % 4) % 4;
   int temp_size = process_size;
   if (wasted_internal_memory != 0)
      process_size += wasted_internal_memory;

   int frame_size = process_size / 4;
   if (insufficent_check(process_id, temp_size, frame_size) == 1)
   {
      //Fill this
   }
}

//@fuzuli
void worst_fit(int process_id, int process_size)
{
   int wasted_internal_memory = (4 - process_size % 4) % 4;
   int temp_size = process_size;
   if (wasted_internal_memory != 0)
      process_size += wasted_internal_memory;

   int frame_size = process_size / 4;
   if (insufficent_check(process_id, temp_size, frame_size) == 1)
   {
      int left = 0, right = 0, space;
      int worst_ind = -1, worst_size = 0;

      for (; right < MEMORY_size; right++)
      {
         if (MEMORY[right] != 0)
         {
            space = right - left;
            if (space >= frame_size && worst_size <= space)
            {
               worst_ind = left;
               worst_size = space;
            }
            left = right + 1;
         }
      }
      space = right - left;
      if (space >= frame_size && worst_size <= space)
      {
         worst_ind = left;
         worst_size = space;
      }

      if (worst_ind != -1)
         put_process_to_index(worst_ind, frame_size, process_id, process_size);

      else
         printf("B\t%d\t%d\t-> %d frames will be used, ERROR! External fragmentation\n", process_id, temp_size, frame_size);
   }
}

void start_process(int process_id, int process_size)
{
   switch (method)
   {
   case 1:
      first_fit(process_id, process_size);
      break;
   case 2:
      best_fit(process_id, process_size);
      break;
   case 3:
      worst_fit(process_id, process_size);
      break;
   default:
      printf("Wrong method !\n");
      break;
   }
}

//@ark
void end_process(int process_id) {}

//@fuzuli
int insufficent_check(int process_id, int temp_size, int frame_size)
{
   int remaining_frames = 0;
   for (int i = 0; i < MEMORY_size; i++)
      if (MEMORY[i] == 0)
         remaining_frames++;

   if (remaining_frames < frame_size)
   {
      printf("B\t%d\t%d\t-> ERROR! Insufficient memory\n", process_id, temp_size);
      INSUFFICIENT_MEMORY_COUNT += 1;
      return 0;
   }
   return 1;
}

//@phyex
void print_report() {}

void put_process_to_index(int index, int frame_size, int process_id, int process_size)
{

   for (int i = index; i < frame_size + index; i++)
      MEMORY[i] = process_id;

   int remaining_frames = 0;
   for (int i = 0; i < MEMORY_size; i++)
      if (MEMORY[i] == 0)
         remaining_frames++;

   printf("B\t%d\t%d\t-> %d frames will be used, remaining #frames: %d\n", process_id, process_size, frame_size, remaining_frames);
}

//
// Do not modify main function
//
int main(int argc, char *argv[])
{
   int size = atoi(argv[1]);
   MEMORY_size = size / 4;
   MEMORY = calloc(MEMORY_size, sizeof(int)); // each block is 4KB

   method = atoi(argv[3]);

   pthread_mutex_init(&lock, NULL);
   char method_str[50];
   if (method == 1)
   {
      sprintf(method_str, "First_fit");
   }
   else if (method == 2)
   {
      sprintf(method_str, "Best_fit");
   }
   else if (method == 3)
   {
      sprintf(method_str, "Worst_fit");
   }
   printf("Program has launched, %s strategy will be used\n", method_str);
   pthread_t tid[4];
   for (int i = 0; i < 4; i++)
   {
      pthread_create(&tid[i], NULL, threadFun, argv[2]);
   }
   for (int i = 0; i < 4; i++)
   {
      pthread_join(tid[i], NULL);
   }
   return 0;
}
