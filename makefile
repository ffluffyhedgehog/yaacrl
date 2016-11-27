options= -c -fPIC -g

all: decoder.o specgram.o fingerprint.o
	gcc -shared -o yaacrl.so decoder.o specgram.o fingerprint.o sha1.o kiss_fft.o -lm

decoder.o: decoder.c decoder.h
	gcc $(options) decoder.c

specgram.o: specgram.c specgram.h kiss_fft.o
	gcc $(options) specgram.c

fingerprint.o: fingerprint.c fingerprint.h sha1.o
	gcc $(options) fingerprint.c

sha1.o: sha1/sha1.c sha1/sha1.h
	gcc $(options) sha1/sha1.c

kiss_fft.o: kiss_fft/kiss_fft.c kiss_fft/kiss_fft.h
	gcc $(options) kiss_fft/kiss_fft.c

clean:
	rm *.o yaacrl.so


cpp:
	g++ -o main main.cpp database.cpp -L/usr/lib  -lmysqlclient -lpthread -lz -lm -ldl -lssl -lcrypto -I/usr/include/mysql
