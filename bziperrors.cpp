#include <stdio.h>
#include <stdlib.h>

void fileNotFoundError(const char* fileName)
{
	fprintf(stderr, "File not found: %s\n", fileName);
	exit(1);
}

void fileNotOpenedError(const char* fileName)
{
	fprintf(stderr, "File not opened: %s\n", fileName);
	exit(1);
}

void readError(const char* fileName)
{
	fprintf(stderr, "Error reading from file: %s\n", fileName);
}

void identicalAddressesError(const char* fileName1, const char* fileName2)
{
	fprintf(stderr, "Addresses of files are identical: %s, %s\n", fileName1, fileName2);
	exit(2);
}