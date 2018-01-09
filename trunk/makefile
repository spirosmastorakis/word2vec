CC = gcc
CPP = g++
#Using -Ofast instead of -O3 might result in faster code, but is supported only by newer GCC versions
CFLAGS = -fPIC -lm -pthread -O0 -march=native -Wall -funroll-loops -Wno-unused-result -g

LDFLAGS = -shared  # linking flags

TARGET_LIB = libdistance.so # target lib

TARGET_LIB2 = libdistance.dylib

all: word2vec word2phrase distance word-analogy compute-accuracy $(TARGET_LIB) $(TARGET_LIB2)

word2vec : word2vec.c
	$(CC) word2vec.c -o word2vec $(CFLAGS)
word2phrase : word2phrase.c
	$(CC) word2phrase.c -o word2phrase $(CFLAGS)
distance : distance.cpp
	$(CPP) distance.cpp -o distance $(CFLAGS)
word-analogy : word-analogy.c
	$(CC) word-analogy.c -o word-analogy $(CFLAGS)
compute-accuracy : compute-accuracy.c
	$(CC) compute-accuracy.c -o compute-accuracy $(CFLAGS)
	chmod +x *.sh
# distance_lib : distance_lib.c
# 	$(CC) distance_lib.c -o distance_lib $(CFLAGS)
$(TARGET_LIB): distance_lib.o
		$(CPP) $(LDFLAGS) $(CFLAGS) -o $(TARGET_LIB) distance_lib.o
$(TARGET_LIB2): distance_lib.o
		$(CPP) $(LDFLAGS) $(CFLAGS) -o $(TARGET_LIB2) distance_lib.o

clean:
	rm -rf word2vec word2phrase distance word-analogy compute-accuracy $(TARGET_LIB) $(TARGET_LIB2)
