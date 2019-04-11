.SILENT:

default:
	chmod +x lab3b.py
	rm -f lab3b
	ln -s lab3b.py lab3b

clean:
	rm -f lab3b *.gz

dist:
	tar -czvf lab3b-.tar.gz lab3b.py README Makefile
