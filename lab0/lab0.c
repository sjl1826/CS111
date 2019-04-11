#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <signal.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

void causeSegFault() {
  int *pointer = NULL;
  *pointer = 1;
}

// Handler for SIGSEGV that catches the segmentation fault
void handler(int sig) {
  fprintf(stderr, "Error: signal %d - Caught segmentation fault\n", sig);
  exit(4);
}

void dumpCore() {
  signal(SIGSEGV, SIG_DFL);
}

void checkArgumentSyntax(int argc, char * argv[]) {
  int c;  
  static struct option long_options[] = {
					 {"segfault", no_argument, 0, 'a'},
				         {"catch", no_argument, 0, 'b'},
					 {"dump-core", no_argument, 0, 'c'},
				         {"input", required_argument, 0, 'd'},
					 {"output", required_argument, 0, 'e'},
				         {0, 0, 0, 0}
  };
  int option_index = 0;
  while(optind < argc) {
    if ((c = getopt_long(argc, argv, "", long_options, &option_index)) == -1) {
      fprintf(stderr, "Invalid Syntax Error: correct usage of lab0 -- ./lab0 --inputFile=input.txt --output=output.txt --catch --segfault\n");
      exit(1);
      break;
    }
    switch (c) {
    case 'a':
    case 'b':
    case 'c':
    case 'd':
    case 'e':
      break;
    default:
      fprintf(stderr, "Invalid Syntax Error: correct usage of lab0 -- ./lab0 --inputFile=input.txt --output=output.txt --catch --segfault\n");
      exit(1);
      break;
    }
  }
}

// Error handler for input/output open/creat errors                             
int handleErrors(int err, char **file, int exitType) {
  switch(err) {
  case -1:
    fprintf(stderr, "Error: File - %s - %s\n", *file, strerror(errno));
    exit(exitType);
    break;
  case 0:
    return 0;
  default:
    break;
  }
  return err;
}

// Opens the input file, closes stdin, copies input file contents to stdin
void handleInput(char **inputFile) {
  int inputDescrip;
  inputDescrip = open(*inputFile, O_RDONLY);
  if(handleErrors(inputDescrip, inputFile, 2) < 0) {
    fprintf(stderr, "Error: File - %s - %s\n", *inputFile, strerror(errno));
    exit(2);
  }
  close(0);
  dup(inputDescrip);
  close(inputDescrip);
}


// "creat"s the output file closes stdout and sets file as stdout
void handleOutput(char **outputFile) {
  int outputDescrip;
  outputDescrip = creat(*outputFile, S_IRUSR | S_IWUSR);
  if (handleErrors(outputDescrip, outputFile, 3) < 0) {
    fprintf(stderr, "Error: Output file cannot be created - %s\n", strerror(errno));
    exit(3);
  }
  close(1);
  dup(outputDescrip);
  close(outputDescrip);
}

// Handles given arguments to execute corresponding behavior
void handleArguments(int argc, char * argv[],
		     char **inputFile, char **outputFile) {
  int c;
  optind = 0;
  int option_index = 0;
  while(optind < argc) {
    static struct option long_options[] = {
					   {"segfault", no_argument, 0, 'a'},
					   {"catch", no_argument, 0, 'b'},
					   {"dump-core", no_argument, 0, 'c'},
					   {"input", required_argument, 0, 'd'},
					   {"output", required_argument, 0, 'e'},
					   {0, 0, 0, 0}
    };
    c = getopt_long(argc, argv, "", long_options, &option_index);
    if (c == -1) {
      break;
    }
    switch (c) {
    case 'a':
      causeSegFault();
      break;
    case 'b':
      signal(SIGSEGV, handler);
      break;
    case 'c':
      dumpCore();
      break;
    case 'd':
      *inputFile = optarg;
      handleInput(inputFile);
      break;
    case 'e':
      *outputFile = optarg;
      handleOutput(outputFile);
      break;
    default:
      break;
    }
  }
}

int main(int argc, char *argv[]) {
  // Check argument syntax
  checkArgumentSyntax(argc, argv);

  // Execute arguments
  char *inputFile = NULL;
  char *outputFile = NULL;
  handleArguments(argc, argv, &inputFile, &outputFile);

  // Reads/Writes to stdin/stdout
  char buf[1];
  int readFlag;
  while(1) {
    readFlag = read(0, &buf, 1);
    if (readFlag < 0) {
      fprintf(stderr, "Error READing file - %s", strerror(errno));
      exit(2);
    }
    if (readFlag == 0) {
      exit(0);
    }
    if (write(1, &buf, 1) < 0) {
      fprintf(stderr, "Error writing - %s", strerror(errno));
      exit(3);
    }
  }

  // Successful execution of program
  exit(0);
}
