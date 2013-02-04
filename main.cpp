#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bziperrors.h"
#include "encode.h"
#include "decode.h"
#define defaultBlockSize 50000
using namespace std;

void help();

int main(int argc, char *argv[])
{
	if (argc != 5 && argc != 6)
	{
		help();
		return 0;
	}
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
		Encode op(argv[2], argv[3], threadsQuantity, blockSize);
		op.performOperation();
	}
	else if (!strcmp(argv[1], "-d"))
	{
		int threadsQuantity = atoi(argv[4]);
		if (threadsQuantity <= 0) help();
		Decode op(argv[2], argv[3], threadsQuantity);
		op.performOperation();
	}
	else
	{
		help();
		return 0;
	}
	return 0;
}

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
}