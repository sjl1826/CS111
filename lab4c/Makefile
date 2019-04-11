.SILENT:

default:
	gcc -g -lmraa -lm -Wall -Wextra lab4c_tcp.c -o lab4c_tcp
	gcc -lssl -lcrypto -g -lmraa -lm -Wall -Wextra lab4c_tls.c -o lab4c_tls
dist:
	tar -czvf lab4c-.tar.gz lab4c_tcp.c lab4c_tls.c Makefile README
clean:
	rm -f lab4c_tcp lab4c_tls *.gz
