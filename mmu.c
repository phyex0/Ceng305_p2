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
   Ayşe Tüncer                   17050111058
   Zeynep Nur Tüncel             17050111011
   Berna Altun                   16050111040
  
 *
 * @version 1.0, 23/12/21
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <math.h>
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
void put_process_to_index(int worst_ind, int frame_size, int process_id, int process_size, int wasted_internal_memory);
int internal_waste_checker(int process_size);
int remaining_frames_count();

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

   while (!feof(fptr))
   {
      fscanf(fptr, "%c %d", &process_type, &process_id);
      if (process_type == 'B')
      {
         fscanf(fptr, "%d", &process_size);
         start_process(process_id, process_size);
      }

      else if (process_type == 'E')
         end_process(process_id);
   }
   return 0;
}

void first_fit(int process_id, int process_size)
{
   int wasted_internal_memory = (4 - process_size % 4) % 4;
   int temp_size = process_size;
   if (wasted_internal_memory != 0)
      process_size += wasted_internal_memory;

   int frame_size = process_size / 4;

   if (insufficent_check(process_id, temp_size, frame_size) == 1)
   {
      int count = 0, left = 0;
      for (int i = 0; i < MEMORY_size; i++)
      {
         if (MEMORY[i] == 0)
         {
            count++;
            if (count == frame_size)
            {
               put_process_to_index(left, frame_size, process_id, process_size, wasted_internal_memory);
               return;
            }
         }
         else
         {
            count = 0;
            left = i + 1;
         }
      }

      printf("B\t%d\t%d\t-> %d frames will be used, ERROR! External fragmentation\n", process_id, temp_size, frame_size);
      EXTERNAL_FRAGMENTATION_COUNT++;
   }
}

void best_fit(int process_id, int process_size)
{
   int wasted_internal_memory = (4 - process_size % 4) % 4;
   int temp_size = process_size;
   if (wasted_internal_memory != 0)
      process_size += wasted_internal_memory;

   int frame_size = process_size / 4;
   if (insufficent_check(process_id, temp_size, frame_size) == 1)
   {
      int left = 0, right = 0, space;
      int best_ind = -1, best_size = MEMORY_size;

      for (; right < MEMORY_size; right++)
      {
         if (MEMORY[right] != 0)
         {
            space = right - left;
            if (space >= frame_size && best_size >= space)
            {
               best_ind = left;
               best_size = space;
            }
            left = right + 1;
         }
      }
      space = right - left;
      if (space >= frame_size && best_size >= space)
      {
         best_ind = left;
         best_size = space;
      }

      if (best_ind != -1)
         put_process_to_index(best_ind, frame_size, process_id, process_size, wasted_internal_memory);

      else
      {
         printf("B\t%d\t%d\t-> %d frames will be used, ERROR! External fragmentation\n", process_id, temp_size, frame_size);
         EXTERNAL_FRAGMENTATION_COUNT++;
      }
   }
}

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
         put_process_to_index(worst_ind, frame_size, process_id, process_size, wasted_internal_memory);

      else
      {
         printf("B\t%d\t%d\t-> %d frames will be used, ERROR! External fragmentation\n", process_id, temp_size, frame_size);
         EXTERNAL_FRAGMENTATION_COUNT++;
      }
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

void end_process(int process_id)
{
   int size = 0;
   for (int i = 0; i < MEMORY_size; i++)
   {
      if (MEMORY[i] == process_id)
      {
         MEMORY[i] = 0;
         size++;
      }
   }
   printf("E\t%d\t\t-> %d frames are deallocated, available #frames: %d \n", process_id, size, remaining_frames_count());
}

int insufficent_check(int process_id, int temp_size, int frame_size)
{
   if (remaining_frames_count() < frame_size)
   {
      printf("B\t%d\t%d\t-> ERROR! Insufficient memory\n", process_id, temp_size);
      INSUFFICIENT_MEMORY_COUNT += 1;
      return 0;
   }
   return 1;
}

void print_report()
{
   int remaining_frames = remaining_frames_count();
   printf("Total free memory in holes: %d frames, %d KB\n", remaining_frames, remaining_frames * 4);
   printf("Total memory wasted as an internal fragmentation: %d KB\n", WASTED_INTERNAL_MEMORY);
   printf("Total number of rejected processes due to external fragmentation: %d\n", EXTERNAL_FRAGMENTATION_COUNT);
   printf("Total number of rejected processes due to insufficient memory: %d\n", INSUFFICIENT_MEMORY_COUNT);

   printf("\vHoles:\n");

   int count = 0, left = 0;
   for (int i = 0; i < MEMORY_size; i++)
   {
      if (MEMORY[i] == 0)
         count++;
      else
      {
         if (count != 0)
            printf("%d %d\n", left, count * 4);
         left = i + 1;
         count = 0;
      }
   }
   if (count != 0)
      printf("%d %d\n", left, count * 4);

   // memory.txt output

   FILE *f = fopen("memory.txt", "w");

   int last = MEMORY[0];
   left = 1;

   int i = 0;
   for (; i < MEMORY_size; i++)
   {
      if (MEMORY[i] != last)
      {
         if (last == 0)
            fprintf(f, "? ");
         else
            fprintf(f, "%d ", last);
         fprintf(f, "%d-%d\n", left - 1, i - 1);
         left = i + 1;
         last = MEMORY[i];
      }
   }
   if (last == 0)
      fprintf(f, "? ");
   else
      fprintf(f, "%d ", last);
   fprintf(f, "%d-%d\n", left - 1, i - 1);

   fclose(f);
}

void put_process_to_index(int index, int frame_size, int process_id, int process_size, int wasted_internal_memory)
{
   WASTED_INTERNAL_MEMORY += wasted_internal_memory;

   for (int i = index; i < frame_size + index; i++)
      MEMORY[i] = process_id;

   printf("B\t%d\t%d\t-> %d frames will be used, remaining #frames: %d\n", process_id, process_size, frame_size, remaining_frames_count());
}

int remaining_frames_count()
{
   int remaining_frames = 0;
   for (int i = 0; i < MEMORY_size; i++)
      if (MEMORY[i] == 0)
         remaining_frames++;
   return remaining_frames;
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
   print_report();
   return 0;
}