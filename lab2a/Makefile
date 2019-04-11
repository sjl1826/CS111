.SILENT:

default: build

build:
	gcc -pthread -g -Wall -Wextra -o lab2_add lab2_add.c
	gcc -pthread -g -Wall -Wextra -o lab2_list lab2_list.c SortedList.c SortedList.h
tests:	default
	chmod +x tests.sh
	-./tests.sh

clean:
	rm lab2_add lab2_list *.gz
dist:	default tests graphs
	tar -czvf lab2a-.tar.gz lab2_add.c SortedList.h SortedList.c lab2_list.c Makefile README tests.sh lab2_add.gp lab2_list.gp lab2_add.csv lab2_list.csv lab2_add-1.png lab2_add-2.png lab2_add-3.png lab2_add-4.png lab2_add-5.png lab2_list-1.png lab2_list-2.png lab2_list-3.png lab2_list-4.png

graphs:
	chmod +x lab2_list.gp
	chmod +x lab2_add.gp
	./lab2_list.gp
	./lab2_add.gp
