//NAME: Samuel Lee
//EMAIL: heysamlee@gmail.com
//ID: 504969381

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <pthread.h>
#include <time.h>
#include <sched.h>
#include <unistd.h>
#include <string.h>
#include "SortedList.h"
#include <string.h>
#include <signal.h>

int numthreads = 1;
int iterations = 1;
int yieldI = 0;
int yieldD = 0;
int yieldL = 0;
int opt_yield = 0;
int listlength = 0;
char syncV = 'd';

SortedList_t *list;
SortedListElement_t **elements;
pthread_mutex_t lock;
int sp = 0;

void handler() {
  fprintf(stderr, "Signal Caught!\n");
  exit(1);
}

void lockSync() {
  if(syncV == 'm')
    pthread_mutex_lock(&lock);
  if(syncV == 's')
    while(__sync_lock_test_and_set(&sp, 1));
}

void unlockSync() {
  if(syncV == 'm')
    pthread_mutex_unlock(&lock);
  if(syncV == 's')
    __sync_lock_release(&sp);
}
void *threadFunc(void *arg) {
  int threadID = *(int *) arg;
  lockSync();
  for(int i = threadID; i < numthreads*iterations; i+=numthreads) {
    SortedList_insert(list, elements[i]);
  }

  SortedListElement_t *ptr;
  listlength = SortedList_length(list);

  for(int i = threadID; i < iterations*numthreads; i+=numthreads) {
    if((ptr = SortedList_lookup(list, elements[i]->key)) == NULL) {
      fprintf(stderr, "Error: element cannot be found\n");
      exit(1);
    }
    if(SortedList_delete(ptr) != 0) {
      fprintf(stderr, "Error: Could not delete element\n");
      exit(1);
    }
  }

  unlockSync();
  return NULL;
}

int main(int argc, char *argv[]) {
  static struct option long_options[] = {
					 {"threads", required_argument, 0, 'a'},
					 {"iterations", required_argument, 0, 'b'},
					 {"yield", required_argument, 0, 'c'},
					 {"sync", required_argument, 0, 'd'},
					 {0, 0, 0, 0}
  };
  int option_index, opt = 0;
  while(optind < argc) {
    if((opt = getopt_long(argc, argv, "", long_options, &option_index)) != -1) {
      switch(opt) {
      case 'a':
	if(optarg != NULL)
	  numthreads = atoi(optarg);
	break;
      case 'b':
	if(optarg != NULL)
	  iterations = atoi(optarg);
	break;
      case 'c':
	for(int i = 0; optarg[i] != '\0'; i++) {
	  switch(optarg[i]) {
	  case 'i':
	    opt_yield |= INSERT_YIELD;
	    yieldI = 1;
	    break;
	  case 'd':
	    opt_yield |= DELETE_YIELD;
	    yieldD = 1;
	    break;
	  case 'l':
	    opt_yield |= LOOKUP_YIELD;
	    yieldL = 1;
	    break;
	  default:
	    fprintf(stderr, "Error: invalid yield argument\n");
	    exit(1);
	  }
	}
	break;
      case 'd':
	syncV = optarg[0];
	break;
      default:
	fprintf(stderr, "Error: invalid argument!\n");
	exit(1);
      }
    }
  }

  signal(SIGSEGV, handler);
  

  list = malloc(sizeof(SortedList_t));
  list->prev = list;
  list->next = list;
  list->key = NULL;

  int maxElements = numthreads*iterations;
  elements = malloc(numthreads*iterations*sizeof(SortedListElement_t*));
  
  for(int i = 0; i< maxElements; ++i) {
    elements[i] = malloc(sizeof(SortedListElement_t));
    elements[i]->next = NULL;
    elements[i]->prev = NULL;
    static const char alpha[] = "abcdefghijklmnopqrstuvwxyz0123456789";
    char *randkey = malloc(4*sizeof(char));
    for(int j = 0; j<5; j++) {
      randkey[j] = alpha[rand() % 35];
    }
    elements[i]->key = randkey;
  }


  struct timespec startingTime;
  struct timespec endingTime;
  clock_gettime(CLOCK_MONOTONIC, &startingTime);
  pthread_t threads[numthreads];
  int rc;
  long t;
  int* threadIDs = malloc(sizeof(int)*numthreads);
  for(t=0; t<numthreads; t++) {
    threadIDs[t] = t;
    rc = pthread_create(&threads[t], NULL, threadFunc, (void *)(threadIDs+t));
    if(rc) {
      fprintf(stderr, "Error; could not create thread\n");
      exit(1);
    }
  }

  int rd;
  long i;
  for(i = 0; i < numthreads; i++) {
    rd = pthread_join(threads[i], NULL);
    if(rd) {
      fprintf(stderr, "Error: could not join thread\n");
      exit(1);
    }
  }

  clock_gettime(CLOCK_MONOTONIC, &endingTime);

  listlength = SortedList_length(list);
  if(listlength != 0) {
    fprintf(stderr, "Error: List length was not zero; it was %d\n", listlength);
    exit(1);
  }
  
  char testName[40];
  strcat(testName, "list-");
  if(yieldI == 1)
    strcat(testName, "i");
  if(yieldD == 1)
    strcat(testName, "d");
  if(yieldL == 1)
    strcat(testName, "l");
  if(yieldI == 0 && yieldD == 0 && yieldL == 0)
    strcat(testName, "none");
  if(syncV == 'd')
    strcat(testName, "-none");
  else if(syncV == 'm')
    strcat(testName, "-m");
  else if(syncV == 's')
    strcat(testName, "-s");

  int totalOps = numthreads*iterations*3;
  long long wholeTime = (endingTime.tv_sec - startingTime.tv_sec)*1000000000;
  long long nTime = (endingTime.tv_nsec - startingTime.tv_nsec);
  long long totalRunTime = wholeTime + nTime;
  long long avgTime = totalRunTime/totalOps;
  int numLists = 1;

  printf("%s,%d,%d,%d,%d,%lld,%lld\n", testName, numthreads, iterations, numLists, totalOps, totalRunTime, avgTime);
  free(list);
  free(elements);
  return 0;
}
