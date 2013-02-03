.PHONY: build build64 clean

build: main.cpp Makefile
	g++ -O2 -Wall -std=c++11 -c main.cpp bzip.cpp bwt.cpp mtf.cpp huffman.cpp
	g++ main.o bzip.o bwt.o mtf.o huffman.o -o bzip -pthread

build64: main.cpp Makefile
	g++ -O2 -Wall -std=c++11 -m64 -c main.cpp bzip.cpp bwt.cpp mtf.cpp huffman.cpp
	g++ main.o bzip.o bwt.o mtf.o huffman.o -o bzip -pthread

clean:
	rm -f *.o bzip
