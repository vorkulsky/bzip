.PHONY: build build64 run clean

build: main.cpp Makefile
	g++ -O2 -Wall -c main.cpp bzip.cpp bwt.cpp mtf.cpp huffman.cpp
	g++ main.o bzip.o bwt.o mtf.o huffman.o -o bzip

build64: main.cpp Makefile
	g++ -O2 -Wall -m64 -c main.cpp bzip.cpp bwt.cpp mtf.cpp huffman.cpp
	g++ main.o bzip.o bwt.o mtf.o huffman.o -o bzip

clean:
	rm -f *.o bzip
