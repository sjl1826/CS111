# CS111

Descriptions:

Project 0: A program that copies its standard input to its standard output by read(2)-ing from file descriptor 0 (until encountering an end of file) and write(2)-ing to file descriptor 1. If no errors (other than EOF) are encountered, your program should exit(2) with status 0.

Project 1: A stripped down shell, simpsh does not use a scripting language at all, and you do not interact with it at a terminal or give it a script to run. Instead, developers invoke the simpsh command by passing it arguments telling it which files to access, which pipes to create, and which subcommands to invoke. It then creates or accesses all the files and creates all the pipes processes needed to run the subcommands, and reports the processes's exit statuses as they exit.

Project 2A: Write a multithreaded application (using pthreads) that performs parallel updates to a shared variable.
Demonstrate the race condition in the provided add routine, and address it with different synchronization mechanisms.
Do performance instrumentation and measurement.
Analyze and explain the observed performance.

Project 2B: Implement the four routines described in SortedList.h: SortedList_insert, SortedList_delete, SortedList_lookup, SortedList_length.
Write a multi-threaded application, using pthread that performs, parallel updates to a sorted doubly linked list data structure (using methods from the above step).
Recognize and demonstrate the race conditions when performing linked list operations, and address them with different synchronization mechanisms.
Do performance instrumentation and measurement.
Analyze and explain the observed performance.

Project 3A: Reads a file system image, whose name is specified as a command line argument. For example, we may run your program with the above file system image using the a command like:

	./lab3a EXT2_test.img
	
Analyzes the provided file system image and produces (to standard out) CSV summaries of what it finds. The contents of these CSV lines described below. Your program must output these files with exactly the same formats as shown below. We will use sort(1) and diff(1) to compare your csv output with ours, so the order of output lines does not matter, but a different format (even extra white space) will make your program fail the test.

Project 3B: Write a program to analyze a file system summary (a .csv file in the same format produced for the previous file system project) and report on all discovered inconsistencies. Detected inconsistencies should be reported to standard out. Execution errors (e.g., invalid arguments or unable to open required files) should be reported to standard error.

Project 4B: A program that builds and runs on your embedded system.
uses the AIO functions of the MRAA library to get readings from your temperature sensor.
samples a temperature sensor at a configurable rate (defaulting to 1/second, and controlled by an optional --period=# command line parameter that specifies a sampling interval in seconds).
converts the sensor value into a temperature. By default, temperatures should be reported in degrees Fahrenheit, but this can be controlled with an optional --scale=C (or --scale=F) command line parameter.
creates a report for each sample that includes:
time of the sample (e.g. 17:25:58) in the local timezone
a single blank/space
a decimal temperature in degrees and tenths (e.g. 98.6)
a newline character (\n)
writes that report to the stdout (fd 1).
appends that report to a logfile (which it creates on your embedded system) if that logging has been enabled with an optional --log=filename parameter.
uses the GPIO functions of the MRAA library to samples the state of the button (from your Grove sensor kit) and when it is pushed ...
outputs (and logs) a final sample with the time and the string SHUTDOWN (instead of a temperature).
exits
Your program can assume that the sensors are connected as recommended by the Grove documentation.

Project 4C: A program that builds and runs on your Beaglebone.
is based on the temperature sensor application you built previously (including the --period=, --scale= and --log= options).
accepts the following (mandatory) new parameters:
--id=9-digit-number
--host=name or address
--log=filename
port number
Note that there is no --port= in front of the port number. This is a non-switch parameter.
accepts the same commands and generates the same reports as the previous Beaglebone project, but now the input and output are from/to a network connection to a server.
open a TCP connection to the server at the specified address and port
immediately send (and log) an ID terminated with a newline:
ID=ID-number 
This new report enables the server to keep track of which devices it has received reports from.
as before, send (and log) newline terminated temperature reports over the connection
as before, process (and log) newline-terminated commands received over the connection
If your temperature reports are mis-formatted, the server will return a LOG command with a description of the error.
Having logged these commands will help you find and fix any problems with your reports.
as before, the last command sent by the server will be an OFF.
Unlike the previous project, the button will not be used as an alternative manual shutdown mechanism.
Do not accept commands from standard input, or send received commands or generated reports to standard output.
