.PHONY: build build64 run clean

build: bzip2.cpp Makefile
	g++ -O2 -Wall -c main.cpp bzip.cpp bwt.cpp mtf.cpp huffman.cpp
	g++ main.o bzip.o bwt.o mtf.o huffman.o -o bzip2

build64: bzip2.cpp Makefile
	g++ -O2 -Wall -m64 -c main.cpp bzip.cpp bwt.cpp mtf.cpp huffman.cpp
	g++ main.o bzip.o bwt.o mtf.o huffman.o -o bzip2

run:
	./bzip2.out

clean:
	rm -f *.o *.out
