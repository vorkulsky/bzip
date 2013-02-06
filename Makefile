.PHONY: build build64 clean

build: main.cpp Makefile
	g++ -O2 -Wall -std=c++11 -c main.cpp operation.cpp encode.cpp decode.cpp bzipthread.cpp bzip.cpp bwt.cpp mtf.cpp huffman.cpp
	g++ main.o operation.o encode.o decode.o bzipthread.o bzip.o bwt.o mtf.o huffman.o -o bzip -pthread

build64: main.cpp Makefile
	g++ -O2 -Wall -std=c++11 -m64 -c main.cpp operation.cpp encode.cpp decode.cpp bzipthread.cpp bzip.cpp bwt.cpp mtf.cpp huffman.cpp
	g++ main.o operation.o encode.o decode.o bzipthread.o bzip.o bwt.o mtf.o huffman.o -o bzip -pthread

clean:
	rm -f *.o bzip
