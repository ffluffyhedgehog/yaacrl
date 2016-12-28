full: yaacrl.o database.o specgram.o fingerprint.o sha1.o kiss_fft.o
	g++ -shared -fPIC -o libyaacrl.so yaacrl.o database.o specgram.o fingerprint.o sha1.o kiss_fft.o -L/usr/lib  -lmysqlclient -lpthread -lz -lm -ldl -lssl -lcrypto -I/usr/include/mysql -g

install:
	rm -rf /usr/lib/libyaacrl.so /usr/include/yaacrl
	cp libyaacrl.so /usr/lib/libyaacrl.so
	mkdir /usr/include/yaacrl
	cp yaacrl.h /usr/include/yaacrl/yaacrl.h
	cp database.h /usr/include/yaacrl/database.h

yaacrl.o:
	g++ -fPIC -c yaacrl.cpp

database.o:
	g++ -fPIC -c database.cpp

specgram.o:
	g++ -fPIC -c specgram.c

fingerprint.o:
	g++ -fPIC -c fingerprint.c

sha1.o:
	g++ -fPIC -c sha1/sha1.c

kiss_fft.o:
	g++ -fPIC -c kiss_fft/kiss_fft.c

small: cspecgram.o cfingerprint.o ckiss_fft.o
	gcc -shared -o libyaacrl-c.so specgram.o fingerprint.o sha1.o kiss_fft.o

cspecgram.o: specgram.c specgram.h kiss_fft.o
	gcc -fPIC -c specgram.c

cfingerprint.o: fingerprint.c fingerprint.h sha1.o
	gcc -fPIC -c fingerprint.c

csha1.o: sha1/sha1.c sha1/sha1.h
	gcc -fPIC -c sha1/sha1.c

ckiss_fft.o: kiss_fft/kiss_fft.c kiss_fft/kiss_fft.h
	gcc -fPIC -c kiss_fft/kiss_fft.c

clean:
	rm -rf *.o libyaacrl.so

exe:
	g++ -o main main.cpp yaacrl.cpp database.cpp specgram.c fingerprint.c sha1/sha1.c kiss_fft/kiss_fft.c -L/usr/lib  -lmysqlclient -lpthread -lz -lm -ldl -lssl -lcrypto -I/usr/include/mysql -g
