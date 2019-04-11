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
char syncV = 'd';
int numLists = 1;

SortedListElement_t **elements;

typedef struct sublist {
  SortedList_t *list;
  pthread_mutex_t lock;
  int sp;
  int listlength;
} sublist;

sublist *main_list;

long long totalLockTime;

void handler() {
  fprintf(stderr, "Signal Caught!\n");
  exit(1);
}

void lockSync(pthread_mutex_t *sLock, int *sp, long long *threadLockTime) {
  struct timespec sLockTime;
  struct timespec eLockTime;
  if(syncV == 'm') {
    clock_gettime(CLOCK_MONOTONIC, &sLockTime);
    pthread_mutex_lock(sLock);
    clock_gettime(CLOCK_MONOTONIC, &eLockTime);
  }
  if(syncV == 's') {
    clock_gettime(CLOCK_MONOTONIC, &sLockTime);
    while(__sync_lock_test_and_set(sp, 1));
    clock_gettime(CLOCK_MONOTONIC, &eLockTime);
  }
  long long wholeTime = (eLockTime.tv_sec - sLockTime.tv_sec)*1000000000;
  long long nTime = (eLockTime.tv_nsec - sLockTime.tv_nsec);
  long long totalRunTime = wholeTime + nTime;
  *threadLockTime += totalRunTime;
}

void unlockSync(pthread_mutex_t *sLock, int *sp) {
  if(syncV == 'm')
    pthread_mutex_unlock(sLock);
  if(syncV == 's')
    __sync_lock_release(sp);
}

static size_t hash(const char *key) {
  size_t hash = 5381;
  while(*key)
    hash = 33 * hash ^ (unsigned char) *key++;
  return hash;
}

void *threadFunc(void *arg) {
  long long threadLockTime = 0;
  int threadID = *(int *) arg;
  for(int i = threadID; i < numthreads*iterations; i+= numthreads) {
    size_t subListPos = hash(elements[i]->key) % numLists;
    lockSync(&(main_list[subListPos].lock), &(main_list[subListPos].sp), &threadLockTime);
    SortedList_insert(main_list[subListPos].list, elements[i]);
    unlockSync(&(main_list[subListPos].lock), &(main_list[subListPos].sp));
  }

  for(int i = 0; i<numLists; i++) {
    lockSync(&(main_list[i].lock), &(main_list[i].sp), &threadLockTime);
    main_list[i].listlength = SortedList_length(main_list[i].list);
    unlockSync(&(main_list[i].lock), &(main_list[i].sp));
  }

  SortedListElement_t *ptr;
  for(int i = threadID; i < iterations*numthreads; i+= numthreads) {
    size_t subListPos = hash(elements[i]->key) % numLists;
    lockSync(&(main_list[subListPos].lock), &(main_list[subListPos].sp), &threadLockTime);
    if((ptr = SortedList_lookup(main_list[subListPos].list, elements[i]->key)) == NULL) {
      fprintf(stderr, "Error: element cannot be found\n");
      exit(1);
    }
    if(SortedList_delete(ptr) != 0) {
      fprintf(stderr, "Error: Could not delete element\n");
      exit(1);
    }
    unlockSync(&(main_list[subListPos].lock), &(main_list[subListPos].sp));
  }
  totalLockTime += threadLockTime;
  return NULL;
}

int main(int argc, char *argv[]) {
  static struct option long_options[] = {
					 {"threads", required_argument, 0, 'a'},
					 {"iterations", required_argument, 0, 'b'},
					 {"yield", required_argument, 0, 'c'},
					 {"sync", required_argument, 0, 'd'},
					 {"lists", required_argument, 0, 'e'},
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
      case 'e':
	numLists = atoi(optarg);
	break;
      default:
	fprintf(stderr, "Error: invalid argument!\n");
	exit(1);
      }
    }
  }

  signal(SIGSEGV, handler);

  // Initialize pointers to sublists
  main_list = malloc(sizeof(sublist)*numLists);
  // Initialize each sublist
  for(int m = 0; m<numLists; m++) {
    main_list[m].list = malloc(sizeof(SortedList_t));
    main_list[m].list->prev = main_list[m].list;
    main_list[m].list->next = main_list[m].list;
    main_list[m].list->key = NULL;
    main_list[m].sp = 0;
    main_list[m].listlength = 0;
    pthread_mutex_init(&(main_list[m].lock), NULL);
  }

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

  int totalListLength = 0;
  for(int l = 0; l<numLists; l++) {
    totalListLength += SortedList_length(main_list[l].list);
  }
  if(totalListLength != 0) {
    fprintf(stderr, "Error: List length was not zero; it was %d\n", totalListLength);
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
  if(syncV == 'd') {
    totalLockTime = 0;
  }

  printf("%s,%d,%d,%d,%d,%lld,%lld,%lld\n", testName, numthreads, iterations, numLists, totalOps, totalRunTime, avgTime, totalLockTime);
  free(main_list);
  free(threadIDs);
  free(elements);
  return 0;
}
