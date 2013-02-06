#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdexcept>
#include <thread>
#include "operation.h"
#include "encode.h"
#include "decode.h"
#define defaultBlockSize 50000
#define defaultThreadsQuantity 1
using namespace std;

void help();

char* programName;

int main(int argc, char *argv[])
{
	programName = argv[0];
    if (argc != 5 && argc != 6) help();

	int threadsQuantity = atoi(argv[4]);
	if (threadsQuantity < 0) help();
	if (!threadsQuantity)
	{
		threadsQuantity = thread::hardware_concurrency();
		if (!threadsQuantity)
		{
			threadsQuantity = defaultThreadsQuantity;
			fputs("The number of threads is not set and can not be computed.", stderr);
			fprintf(stderr, "%i worker threads will be used.\n", defaultThreadsQuantity);
		}
	}

	Operation *op = NULL;
    if (!strcmp(argv[1], "-e"))
    {
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
		fputs(err.what(), stderr);
    }
    delete op;
    return 0;
}

/**
*    Выводит help и завершает работу программы.
*/
void help()
{
	printf("usage: %s -e inFile outFile threadsQuantity [blockSize]\n", programName);
	printf("       %s -d inFile outFile threadsQuantity\n\n", programName);
    puts("positional arguments:");
    puts("  -e                   compress file");
    puts("  -d                   decompress file");
	puts("  inFile               input file");
	puts("  outFile              output file");
    puts("  threadsQuantity      number of worker threads. If 0 the program will try");
	puts("                       to determine number of logical processors.");
	puts("");
	puts("optional positional arguments:");
    puts("  blockSize            int, >0, default 50000");
    exit(0);
}
