.SILENT:

default:
	gcc -o lab0 -g -Wall -Wextra lab0.c
check:	default
	touch errorFlag.txt; \
	#Checking Segfault
	./lab0 --segfault &> /dev/null; \
	if [[ $$? -eq 139 ]]; then \
		echo "Segfault successfully triggered with code 139..."; \
	else \
		echo "**Error - Segfault not triggered**"; \
		echo "err" >> errorFlag.txt; \
	fi;
	#Checking Catch
	./lab0 --catch --segfault &> /dev/null; \
	if [[ $$? -eq 4 ]]; then \
		echo "Successfully caught segmentation fault..."; \
	else \
		echo "**Error - Segfault not caught**"; \
		echo "err" >> errorFlag.txt; \
	fi;
	#Checking dump-core
	./lab0 --catch --dump-core --segfault &> /dev/null; \
	if [[ $$? -eq 139 ]]; then \
		echo "Successfully dumped core..."; \
	else \
		echo "**Error - Core not dumped**"; \
		echo "err" >> errorFlag.txt; \
	fi;
	#Checking Input/Output action
	echo "this is a test" > inputFile.txt; \
	./lab0 --input=inputFile.txt --output=outputFile.txt; \
	if [[ $$? -eq 0 ]]; then \
		echo "Input and Output correctly executed..."; \
	else \
		echo "**Error - Input/Output incorrectly executed**"; \
		echo "err" >> errorFlag.txt; \
	fi;
	#Check if input was copied correctly to output
	diff inputFile.txt outputFile.txt; \
	if [[ $$? -eq 0 ]]; then \
		echo "Successfully copied input to output..."; \
	else \
		echo "**Error - Input was not successfully copied to output**";\
		echo "err" >> errorFlag.txt; \
	fi;
	if [[ -s errorFlag.txt ]]; then \
		echo "Failed tests"; \
	else \
		echo "All tests succeeded"; \
	fi;
	rm -f inputFile.txt outputFile.txt errorFlag.txt;
clean:
	rm lab0 lab0-.tar.gz
dist:
	tar -czvf lab0-.tar.gz lab0.c Makefile README backtrace.png breakpoint.png
