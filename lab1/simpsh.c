#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <ctype.h>
#include <getopt.h>
#include <errno.h>
#include <signal.h>
#include <sys/resource.h>
#include <sys/time.h>

int position = 0;
int* descriptors = NULL;
int fileFlags[11] = { 0 };
// 0: O_APPEND, 1: O_CLOEXEC, 2: O_CREAT, 3: O_DIRECTORY, 4: O_DSYNC, 5: O_EXCL, 6: O_NOFOLLOW, 7: O_NONBLOCK, 8: O_RSYNC, 9: O_SYNC, 10: O_TRUNC
int* pids = NULL;
int numProcesses = 0;
char* processes[100];
int profileFlag = 0;

int
timeval_subtract (struct timeval *result, struct timeval *x, struct timeval *y)
{
  /* Perform the carry for the later subtraction by updating y. */
  if (x->tv_usec < y->tv_usec) {
    int nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
    y->tv_usec -= 1000000 * nsec;
    y->tv_sec += nsec;
  }
  if (x->tv_usec - y->tv_usec > 1000000) {
    int nsec = (x->tv_usec - y->tv_usec) / 1000000;
    y->tv_usec += 1000000 * nsec;
    y->tv_sec -= nsec;
  }

  /* Compute the time remaining to wait.
     tv_usec is certainly positive. */
  result->tv_sec = x->tv_sec - y->tv_sec;
  result->tv_usec = x->tv_usec - y->tv_usec;

  /* Return 1 if result is negative. */
  return x->tv_sec < y->tv_sec;
}

void printUErr() {
  fprintf(stderr, "ERROR getrusage - %s\n", strerror(errno));
}

void handler(int sig) {
  fprintf(stderr, "%d caught\n", sig);
  exit(sig);
}

void printOpenError() {
  fprintf(stderr, "ERROR: Could not open file - %s\n", strerror(errno));
}

void resetFileFlags() {
  memset(fileFlags, 0, sizeof(fileFlags));
}

int getFileFlag() {
  int finalFlag = 0;
  for(int i = 0; i < 11; i++) {
    if(fileFlags[i] == 1) {
      switch(i) {
      case 0: finalFlag = finalFlag | O_APPEND; break;
      case 1: finalFlag = finalFlag | O_CLOEXEC; break;
      case 2: finalFlag = finalFlag | O_CREAT; break;
      case 3: finalFlag = finalFlag | O_DIRECTORY; break;
      case 4: finalFlag = finalFlag | O_DSYNC; break;
      case 5: finalFlag = finalFlag | O_EXCL; break;
      case 6: finalFlag = finalFlag | O_NOFOLLOW; break;
      case 7: finalFlag = finalFlag | O_NONBLOCK; break;
      case 8: finalFlag = finalFlag | O_RSYNC; break;
      case 9: finalFlag = finalFlag | O_SYNC; break;
      case 10: finalFlag = finalFlag | O_TRUNC; break;
      default: break;
      }
    }
  }
  resetFileFlags();
  return finalFlag;
}

int handleReadWrite(char* file, int verbose, int rw) {
  int finalFlag = getFileFlag();
  int openedFile;
  switch(rw) {
  case 0:
    openedFile = open(file, O_RDONLY | finalFlag, 0644);
    if(verbose)
      printf("--rdonly %s\n", file);
    break;
  case 1:
    openedFile = open(file, O_WRONLY | finalFlag, 0644);
    if(verbose)
      printf("--wronly %s\n", file);
    break;
  case 2:
    openedFile = open(file, O_RDWR | finalFlag, 0644);
    if(verbose)
      printf("--rdwr %s\n", file);
    break;
  default:
    //this should never be triggered
    fprintf(stderr, "invalid read write option\n");
    break;
  }
  fflush(stdout);
  descriptors = realloc(descriptors, (position+1)*sizeof(int));
  descriptors[position] = openedFile;
  position++;
  if(openedFile < 0) {
    printOpenError();
    return 1;
  }
  return 0;
}

int checkValidFileDescriptor(int fd) {
  if(fd >= position || descriptors[fd] == -1) {
    fprintf(stderr, "Error: Invalid file descriptor index: %d\n", fd);
    return 1;
  }
  return 0;
}

int handleCommandOption(int argc, char*argv[], int verbose) {
  int fileDescriptors[3];
  char* commandArgs[100];
  int i;
  int numberCommands = 0;
  commandArgs[0] = NULL;
  for(i = 0;optind<argc; i++) {
    if(argv[optind][0] == '-' && argv[optind][1] == '-') {
      break;
    }
    if (i<3) {
      fileDescriptors[i] = (*argv[optind] - '0');
    } else if (i == 3) {
      commandArgs[0] = argv[optind];
      numberCommands++;
    } else {
      commandArgs[i-3] = argv[optind];
      numberCommands++;
    }
    optind++;
  }

  if(commandArgs[0] == NULL) {
    fprintf(stderr, "Error: Command not provided\n");
    return 1;
  }

  commandArgs[i-3] = NULL;

  for(int j = 0; j < 3; j++) {
    if(checkValidFileDescriptor(fileDescriptors[j]) == 1) {
      return 1;
    }
  }

  if(verbose) {
    printf("--command %d %d %d ", fileDescriptors[0], fileDescriptors[1], fileDescriptors[2]);
    for(int j = 0; commandArgs[j] != NULL; j++) {
      printf("%s ", commandArgs[j]);
    }
    printf("\n");
    fflush(stdout);
  }

   char* fullCommand = malloc(sizeof(char)*100);
   int charpos = 0;
   for(int i = 0; commandArgs[i] != NULL; i++) {
     for(int j = 0; commandArgs[i][j] != '\0'; j++) {
       fullCommand[charpos] = commandArgs[i][j];
       charpos++;
     }
     fullCommand[charpos] = ' ';
     charpos++;
   }
   fullCommand[charpos-1] = '\0';
   processes[numProcesses] = fullCommand;

  int commandProcess = fork();
  if (commandProcess < 0) {
    fprintf(stderr, "fork failed: %s\n", strerror(errno));
    return 1;
  } else if (commandProcess == 0) {
    close(0);
    close(1);
    close(2);
    dup2(descriptors[fileDescriptors[0]], 0);
    dup2(descriptors[fileDescriptors[1]], 1);
    dup2(descriptors[fileDescriptors[2]], 2);
    close(descriptors[fileDescriptors[0]]);
    close(descriptors[fileDescriptors[1]]);
    close(descriptors[fileDescriptors[2]]);

    for(int i = 0; i < position; i++) {
      close(i+3);
    }
  
    int com;
    if((com = execvp(commandArgs[0], commandArgs)) == -1) {
      fprintf(stderr, "ERROR: error executing command - %s\n", strerror(errno));
      return 1;
    }
  } else if (commandProcess > 0) {
    pids = realloc(pids, (numProcesses+1)*sizeof(pid_t));
    pids[numProcesses] = commandProcess;
    numProcesses++;
  }
  return 0;
}

int handlePipe() {
  int pipefd[2];
  if(pipe(pipefd) == -1) {
    fprintf(stderr, "ERROR: %s\n", strerror(errno));
    return 1;
  }
  descriptors = realloc(descriptors, (position+2)*sizeof(int));
  descriptors[position] = pipefd[0];
  descriptors[position+1] = pipefd[1];
  position += 2;
  return 0;
}

void causeSegFault() {
  int* pointer = NULL;
  *pointer = 1;
}

void handleSignalVerbose(char* optarg, char* act) {
  printf("%s ", act);
  for(int i = 0; optarg[i] != '\0'; i++) {
    printf("%c", optarg[i]);
  }
  printf("\n");
  fflush(stdout);
}

int handleArguments(int argc, char* argv[]) {
  int exitStatus = 0;
  int highestSignal = 0;
  static struct option long_options[] = {
					 {"rdonly",required_argument, 0, 'a'},
					 {"wronly", required_argument, 0, 'b'},
					 {"verbose", no_argument, 0, 'c'},
					 {"command", no_argument, 0, 'd'},
					 {"append", no_argument, 0, 'e'},
					 {"cloexec", no_argument, 0, 'f'},
					 {"creat", no_argument, 0, 'g'},
					 {"directory", no_argument, 0, 'h'},
					 {"dsync", no_argument, 0, 'i'},
					 {"excl", no_argument, 0, 'j'},
					 {"nofollow", no_argument, 0, 'k'},
					 {"nonblock", no_argument, 0, 'l'},
					 {"rsync", no_argument, 0, 'm'},
					 {"async", no_argument, 0, 'n'},
					 {"trunc", no_argument, 0, 'o'},
					 {"rdwr", required_argument, 0, 'p'},
					 {"pipe", no_argument, 0, 'q'},
					 {"abort", no_argument, 0, 'r'},
					 {"catch", required_argument, 0, 's'},
					 {"default", required_argument, 0, 't'},
					 {"pause", no_argument, 0, 'u'},
					 {"ignore", required_argument, 0, 'v'},
					 {"close", required_argument, 0, 'w'},
					 {"wait", no_argument, 0, 'x'},
					 {"profile", no_argument, 0, 'y'},
					 {0,0,0,0}
  };
  int option_index, opt = 0;
  int verbose = 0;

  int who = RUSAGE_SELF;
  struct rusage usage;
  struct timeval utime;
  struct timeval stime;
  char* optionProf;
  int whoC = RUSAGE_CHILDREN;
  struct rusage childUsage;
  struct timeval uctime;
  struct timeval sctime;
  
  while (optind < argc) {
    if ((opt = getopt_long(argc, argv, "", long_options, &option_index)) != -1) {
      if(profileFlag == 1 && optionProf != NULL) {
	if(getrusage(who, &usage) == -1) {
	  printUErr();
	  exitStatus = 1;
	}
	else {
	  utime = usage.ru_utime;
	  stime = usage.ru_stime;
	}
	if(getrusage(whoC, &childUsage) == -1) {
	  printUErr();
	  exitStatus = 1;
	}
	else {
	  uctime = childUsage.ru_utime;
	  sctime = childUsage.ru_stime;
	}
      }
      switch(opt) {
      case 'a':
	optionProf = "rdonly";
	if (handleReadWrite(optarg, verbose, 0) != 0) {
	  exitStatus = 1;
	}
	break;
      case 'b':
	optionProf = "wronly";
	if (handleReadWrite(optarg, verbose, 1) != 0) {
	  exitStatus = 1;
	}
	break;
      case 'c':
	optionProf = "verbose";
	verbose = 1;
	break;
      case 'd':
	optionProf = "command";
	if (handleCommandOption(argc, argv, verbose) != 0) {
	  exitStatus = 1;
	}
	break;
      case 'e': fileFlags[0] = 1; optionProf = "append";
	if(verbose){ printf("--append\n"); } break;
      case 'f': fileFlags[1] = 1; optionProf = "cloexec";
	if(verbose){ printf("--cloexec\n"); } break;
      case 'g': fileFlags[2] = 1; optionProf = "creat";
	if(verbose){ printf("--creat\n"); } break;
      case 'h': fileFlags[3] = 1; optionProf = "directory";
	if(verbose){ printf("--directory\n"); } break;
      case 'i': fileFlags[4] = 1; optionProf = "dsync";
	if(verbose){ printf("--dsync\n"); } break;
      case 'j': fileFlags[5] = 1; optionProf = "excl";
	if(verbose){ printf("--excl\n"); } break;
      case 'k': fileFlags[6] = 1; optionProf = "nofollow";
	if(verbose){ printf("--nofollow\n"); } break;
      case 'l': fileFlags[7] = 1; optionProf = "nonblock";
	if(verbose){ printf("--nonblock\n"); } break;
      case 'm': fileFlags[8] = 1; optionProf = "rsync";
	if(verbose){ printf("--rsync\n"); } break;
      case 'n': fileFlags[9] = 1; optionProf = "sync";
	if(verbose){ printf("--sync\n"); } break;
      case 'o': fileFlags[10] = 1; optionProf = "trunc";
	if(verbose){ printf("--trunc\n"); } break;
      case 'p':
	optionProf = "rdwr";
	if (handleReadWrite(optarg, verbose, 2) != 0) {
	  exitStatus = 1;
	}
	break;
      case 'q':
	optionProf = "pipe";
	if(verbose) {
	  printf("--pipe\n");
	  fflush(stdout);
	}
	if (handlePipe() != 0) {
	  exitStatus = 1;
	}
	break;
      case 'r':
	optionProf = "abort";
	if(verbose) {
	  printf("--abort\n");
	  fflush(stdout);
	}
	causeSegFault();
	break;
      case 's':
	optionProf = "catch";
	if(verbose) {
	  char* act = "--catch";
	  handleSignalVerbose(optarg, act);
	}
	int sig = atoi(optarg);
	signal(sig, handler);
	if(sig > highestSignal) {
	  highestSignal = sig;
	}
	break;
      case 't':
	optionProf = "default";
	if(verbose) {
	  char* actd = "--default";
	  handleSignalVerbose(optarg, actd);
	}
	int sigd = atoi(optarg);
	signal(sigd, SIG_DFL);
	if(sigd > highestSignal) {
	  highestSignal = sigd;
	}
	break;
      case 'u':
	optionProf = "pause";
	if(verbose) {
	  printf("--pause\n");
	  fflush(stdout);
	}
	pause();
	break;
      case 'v':
	optionProf = "ignore";
	if(verbose) {
	  char* acti = "--ignore";
	  handleSignalVerbose(optarg, acti);
	}
	int sigi = atoi(optarg);
	signal(sigi, SIG_IGN);
	if(sigi > highestSignal) {
	  highestSignal = sigi;
	}
	break;
      case 'w':
	optionProf = "close";
	if(verbose) {
	  char *actc = "--close";
	  handleSignalVerbose(optarg, actc);
	}
	close(descriptors[atoi(optarg)]);
	descriptors[atoi(optarg)] = -1;
	break;
      case 'x':
	optionProf = "wait";
	if(verbose) {
	  printf("--wait\n");
	  fflush(stdout);
	}
	if(numProcesses == 0)
	  break;
	int ind;
	int proNum = numProcesses;
	while(proNum > 0) {
	  int childStatus;
	  pid_t wt = waitpid(-1, &childStatus, 0);
	  if(WEXITSTATUS(childStatus) > exitStatus) {
	    exitStatus = WEXITSTATUS(childStatus);
	  }
	  if(WIFEXITED(childStatus)) {
	    printf("exit %d ", WEXITSTATUS(childStatus));
	  } else if (WIFSIGNALED(childStatus)) {
	    if(WTERMSIG(childStatus) > highestSignal) {
	      highestSignal = WTERMSIG(childStatus);
	    }
	    printf("signal %d ", WTERMSIG(childStatus));
	  }

	  for(int i = 0; i<numProcesses; i++) {
	    if(pids[i] == wt) {
	      ind = i;
	      break;
	    }
	  }
	  for(int j = 0; processes[ind][j] != '\0'; j++) {
	    printf("%c", processes[ind][j]);
	  }
	  printf("\n");
	  fflush(stdout);
	  proNum--;
	}
	numProcesses = 0;
	fflush(stdout);
	if(profileFlag == 1 && optionProf != NULL) {
	  if(getrusage(whoC, &childUsage) == -1)
	    printUErr();
	  else {
	    struct timeval resultU;
	    timeval_subtract(&resultU, &childUsage.ru_utime, &uctime);
	    double wholeTimeUser = resultU.tv_sec;
	    double restOfUser = (double) resultU.tv_usec/1000000;
	    double totalUser = wholeTimeUser + restOfUser;
	    struct timeval resultS;
	    timeval_subtract(&resultS, &childUsage.ru_stime, &sctime);
      	    double wholeTimeSystem = resultS.tv_sec;
	    double restOfSys = (double) resultS.tv_usec/1000000;
	    double totalSys = wholeTimeSystem + restOfSys;
	    printf("children time information - User Time: %f s  System Time: %f s\n", totalUser, totalSys);
      }	    
	}
	break;
      case 'y':
	optionProf = NULL;
	profileFlag = 1;
	break;
      case '?':
	fprintf(stderr, "Option does not exist\n");
	exitStatus = 1;
	break;
      default:
	break;
      }
    } else {
      optind++;
      fprintf(stderr, "ERROR: Invalid argument given\n");
      exitStatus = 1;
    }
    if(profileFlag == 1 && optionProf != NULL) {
      if(getrusage(who, &usage) == -1) {
	printUErr();
	exitStatus = 1;
      }
      else {
	struct timeval resultU;
	timeval_subtract(&resultU, &usage.ru_utime, &utime);
	double wholeTimeUser = resultU.tv_sec;
	double restOfUser = (double) resultU.tv_usec/1000000;
	double totalUser = wholeTimeUser + restOfUser;
	struct timeval resultS;
	timeval_subtract(&resultS, &usage.ru_stime, &stime);
	double wholeTimeSystem = resultS.tv_sec;
	double restOfSys = (double) resultS.tv_usec/1000000;
	double totalSys = wholeTimeSystem + restOfSys;
	printf("--%s time information - User Time: %f s  System Time: %f s\n", optionProf, totalUser, totalSys);
      }	
    }
  }
  free(descriptors);
  fflush(stdout);
  if(highestSignal != 0) {
    signal(highestSignal, SIG_DFL);
    raise(highestSignal);
  }
  return exitStatus;
}

int main(int argc, char *argv[]) {
  return handleArguments(argc, argv);
}
