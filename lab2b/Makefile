.SILENT:

default: build

build:
	gcc -lprofiler -pthread -g -Wall -Wextra -o lab2_list lab2_list.c SortedList.c SortedList.h
tests:	default
	chmod +x tests.sh
	-./tests.sh

clean:
	rm lab2_list *.gz
dist:	default graphs profile
	tar -czvf lab2b-.tar.gz SortedList.h SortedList.c lab2_list.c Makefile README tests.sh lab2_list.gp lab2b_list.csv lab2b_1.png lab2b_2.png lab2b_3.png lab2b_4.png lab2b_5.png profile.out

graphs: tests
	chmod +x lab2_list.gp
	./lab2_list.gp
profile: build
	rm profile.out
	LD_PRELOAD=/usr/lib64/libprofiler.so CPUPROFILE=prof.out ./lab2_list --threads=12 --iterations=1000 --sync=s;
	pprof --text ./lab2_list prof.out>profile.out
	pprof --list=threadFunc ./lab2_list prof.out >>profile.out
	rm prof.out
