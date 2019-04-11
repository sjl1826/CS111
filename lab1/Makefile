.SILENT:

default:
	gcc -Wall -Wextra -o simpsh -g simpsh.c
check:	default
	touch errorFlag.txt
	touch a.txt
	echo "Sam Lee is a student in CS111." > a.txt
	./simpsh &> /dev/null;
	if [[ $$? -eq 0 ]]; then \
		echo "No Arg Passed"; \
else \
		echo "Fail!" > errorFlag.txt; \
	fi;
	./simpsh --rdonly a.txt &> /dev/null;
	if [[ $$? -eq 0 ]]; then \
		echo "Read only Passed"; \
        else \
                echo "**Error - Read only failed!" > errorFlag.txt; \
        fi;
	touch b.txt
	touch err.txt
	./simpsh --rdonly a.txt --wronly b.txt --wronly err.txt --command 0 1 2 tr 'a' 'a' &> /dev/null;
	if [[ $(a.txt) -eq $(b.txt) ]]; then \
		echo "Command, Read, Write Passed"; \
else \
echo "Test Failed" > errorFlag.txt; \
fi;
	./simpsh --wronly b.txt &> /dev/null;
	if [[ $$? -eq 0 ]]; then \
		echo "Write only Passed"; \
else \
		echo "Error failed" > errorFlag.txt; \
fi;
	./simpsh --creat --rdonly sam.txt &> /dev/null;
	if [[ $$? -eq 0 ]]; then \
		echo "Flags passed"; \
else \
		echo "Flags failed" > errorFlag.txt; \
fi;
	touch a1.txt; \
	ln -s a1.txt a1s.txt; \
	./simpsh --nofollow --rdonly a1s.txt --rdwr a1s.txt --wronly a1.txt &> /dev/null; \
	if [[ $$? -eq 1 ]]; then \
		echo "no follow passed"; \
else \
		echo "Error failed no follow" > errorFlag.txt; \
fi;
	./simpsh --catch 11 --abort > te.txt 2>terr.txt; \
	if grep -q "11 caught" terr.txt; then \
		echo "passed catch abort"; \
else \
		echo "Error failed" > errorFlag.txt; \
fi;
	if [[ -s errorFlag.txt ]]; then \
		echo "Failed Tests"; \
else \
echo "All tests succeeded"; \
fi;
	rm -f *.txt
clean: 
	rm -f simpsh *.gz *.txt
dist: 
	tar -czvf lab1-.tar.gz simpsh.c Makefile README report.pdf
