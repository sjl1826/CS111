.SILENT:

default:
	gcc -g -lmraa -lm -Wall -Wextra lab4b.c -o lab4b

check:  default
	chmod +x test.sh
	./test.sh
dist:
	tar -czvf lab4b-.tar.gz lab4b.c Makefile README test.sh
clean:
	rm -f lab4b *.gz
