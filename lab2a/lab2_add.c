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

long long counter = 0;
int numthreads = 1;
int iterations = 1;
int opt_yield = 0;
char syncV = 'l';
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
int sp = 0;

void add(long long *pointer, long long value) {
  long long sum = *pointer + value;
  if(opt_yield)
    sched_yield();
  *pointer = sum;
}

void addMutex(long long *pointer, long long value) {
  pthread_mutex_lock(&lock);
  long long sum = *pointer + value;
  if(opt_yield)
    sched_yield();
  *pointer = sum;
  pthread_mutex_unlock(&lock);
}

void addSpin(long long *pointer, long long value) {
  while(__sync_lock_test_and_set(&sp, 1));
  long long sum = *pointer + value;
  if(opt_yield)
    sched_yield();
  *pointer = sum;
  __sync_lock_release(&sp);
}

void addValues(long long *pointer, long long value) {
  for(int i = 0; i<iterations; i++) {
    switch(syncV) {
    case 'l':
      add(pointer, value);
      break;
    case 'm':
      addMutex(pointer, value);
      break;
    case 's':
      addSpin(pointer, value);
      break;
    case 'c':
      {
      long long prev, sum;
      do {
	prev = *pointer;
	if(opt_yield)
	  sched_yield();
	sum = prev + value;
      } while(__sync_val_compare_and_swap(pointer, prev, sum) != prev);
      break;
      }
    }
  }
}

void *threadAdd() {
  addValues(&counter, 1);
  addValues(&counter, -1);
  return NULL;
}

int main(int argc, char *argv[]) {
  static struct option long_options[] = {
					 {"threads", required_argument, 0, 'a'},
					 {"iterations", required_argument, 0, 'b'},
					 {"yield", no_argument, 0, 'c'},
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
	opt_yield = 1;
	break;
      case 'd':
	if(optarg[0] != 'm' && optarg[0] != 's' && optarg[0] != 'c') {
	  fprintf(stderr, "Invalid sync argument!\n");
	  exit(1);
	}
	syncV = optarg[0];
	break;
      default:
	fprintf(stderr, "Error: invalid argument!\n");
	exit(1);
      }
    }
  }

  struct timespec startingTime;
  struct timespec endingTime;
  clock_gettime(CLOCK_MONOTONIC, &startingTime);
  
  pthread_t threads[numthreads];
  int rc;
  long t;
  for(t=0; t<numthreads; t++) {
    rc = pthread_create(&threads[t], NULL, threadAdd, NULL);
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

  char* testName;
  if(opt_yield == 0 && syncV == 'l') {
    testName = "add-none";
  } else if(opt_yield == 0 && syncV == 'm') {
    testName = "add-m";
  } else if(opt_yield == 0 && syncV == 's') {
    testName = "add-s";
  } else if(opt_yield == 0 && syncV == 'c') {
    testName = "add-c";
  } else if(opt_yield == 1 && syncV == 'l') {
    testName = "add-yield-none";
  } else if(opt_yield == 1 && syncV == 'm') {
    testName = "add-yield-m";
  } else if(opt_yield == 1 && syncV == 's') {
    testName = "add-yield-s";
  } else if(opt_yield == 1 && syncV == 'c') {
    testName = "add-yield-c";
  }

  int totalOps = numthreads*iterations*2;
  long long wholeTime = (endingTime.tv_sec - startingTime.tv_sec)*1000000000;
  long long nTime = (endingTime.tv_nsec - startingTime.tv_nsec);
  long long totalRunTime = wholeTime + nTime;
  long long avgTime = totalRunTime/totalOps;

  printf("%s,%d,%d,%d,%lld,%lld,%lld\n", testName, numthreads, iterations, totalOps, totalRunTime, avgTime, counter);
  return 0;
}
