all: WTF WTFserver

WTFserver: WTFserver.c
	mkdir server
	gcc -g -pthread WTFserver.c -o server/WTFserver
WTF: WTF.c
	mkdir client
	gcc -g WTF.c -o client/WTF
test: WTFtest.c
	gcc -g WTFtest.c -o WTFtest
	mkdir server/projA server/projB server/projC server/projD server/projE
	mkdir server/projA/1
	touch server/projA/1/testCodeA.c
	touch server/projA/1/.manifest
	echo "1" > server/projA/1/.manifest
	mkdir server/projB/1
	touch server/projB/1/testCodeA.c
	mkdir server/projC/1
	touch server/projC/1/testCodeA.c
	mkdir server/projD/1
	touch server/projD/1/testCodeA.c
	mkdir server/projE/1
	touch server/projE/1/testCodeA.c

clean:
	rm -rf WTFtest server client
