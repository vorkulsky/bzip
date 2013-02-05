#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdexcept>
#include "operation.h"
#include "encode.h"
#include "decode.h"
#define defaultBlockSize 50000
using namespace std;

void help();

int main(int argc, char *argv[])
{
	if (argc != 5 && argc != 6) help();
	Operation *op = NULL;
	if (!strcmp(argv[1], "-e"))
	{
		int threadsQuantity = atoi(argv[4]);
		if (threadsQuantity <= 0) help();
		int blockSize = defaultBlockSize;
		if (argc == 6)
		{
			blockSize = atoi(argv[5]);
			if (blockSize <= 0) help();
		}
		Encode *encodeOp = new Encode(argv[2], argv[3], threadsQuantity, blockSize);
		op = encodeOp;
	}
	else if (!strcmp(argv[1], "-d"))
	{
		int threadsQuantity = atoi(argv[4]);
		if (threadsQuantity <= 0) help();
		Decode *decodeOp = new Decode(argv[2], argv[3], threadsQuantity);
		op = decodeOp;
	}
	else help();
	try
	{
		// К этому моменту op всегда инициализирована, иначе программа уже завершилась по help().
		op->performOperation();
	}
	catch (const length_error& err)
	{
		fprintf(stderr, "Error reading from file: %s\n", err.what());
	}
	catch (const logic_error& err)
	{
		puts(err.what());
	}
	delete op;
	return 0;
}

/**
*	Выводит help и завершает работу программы.
*/
void help()
{
	puts("usage: bzip -e infile outfile threadsQuantity [block size]");
	puts("       bzip -d infile outfile threadsQuantity");
	puts("");
	puts("positional arguments:");
	puts("-e                   compress file");
	puts("-d                   decompress file");
	puts("threadsQuantity      int > 0");
	puts("[block size]         int in [1, 2147483647] (default 50000)");
	exit(0);
}