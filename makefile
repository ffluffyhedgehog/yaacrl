all: main.o specgram.o fingerprint.o
	gcc main.o specgram.o fingerprint.o sha1.o kiss_fft.o -o yaacrl -lm

main.o: main.c
	gcc main.c -c -g

specgram.o: specgram.c specgram.h kiss_fft.o
	gcc specgram.c -c -g

fingerprint.o: fingerprint.c fingerprint.h sha1.o
	gcc fingerprint.c -c -g

sha1.o: sha1/sha1.c sha1/sha1.h
	gcc sha1/sha1.c -c -g

kiss_fft.o: kiss_fft/kiss_fft.c kiss_fft/kiss_fft.h
	gcc kiss_fft/kiss_fft.c -c -g

clean:
	rm -rf *.o yaacrl

cpp:
	g++ -o main main.cpp yaacrl.cpp database.cpp specgram.c fingerprint.c sha1/sha1.c kiss_fft/kiss_fft.c -L/usr/lib  -lmysqlclient -lpthread -lz -lm -ldl -lssl -lcrypto -I/usr/include/mysql -g

