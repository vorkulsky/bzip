#include <stdio.h>
#include <stdlib.h>
#include <stdexcept>
#include <sys/stat.h>
#include <string.h>
#include "definitions.h"
#include "bzip.h"
#define defaultBlockSize 50000
using namespace std;


void encode(char* inFileName, char* outFileName, int blockSize);
void decode(char* inFileName, char* outFileName);
void fileNotFoundError(const char* fileName);
void fileNotOpenedError(const char* fileName);
void readError(const char* fileName);
void identicalAddressesError(const char* fileName1, const char* fileName2);
void help();

int main(int argc, char *argv[])
{
	if (argc != 4 && argc != 5)
		help();
	if (!strcmp(argv[1], "-e"))
	{
		int blockSize = defaultBlockSize;
		if (argc == 5)
		{
			blockSize = atoi(argv[4]);
			if (blockSize <= 0) help();
		}
		encode(argv[2], argv[3], blockSize);
	}
	else if (!strcmp(argv[1], "-d"))
		decode(argv[2], argv[3]);
	else
		help();
	return 0;
}

void encode(char* inFileName, char* outFileName, int blockSize)
{
	if (!strcmp(inFileName, outFileName))
		identicalAddressesError(inFileName, outFileName);
	struct stat st;
	if (stat(inFileName, &st)!=0)
		fileNotFoundError(inFileName);
	long fileSize = st.st_size;
	FILE* in = fopen(inFileName, "rb");
	if (!in)
		fileNotOpenedError(inFileName);
	FILE* out = fopen(outFileName, "wb");
	if (!out)
	{
		fclose(in);
		fileNotOpenedError(outFileName);
	}

	int d = fileSize % blockSize;
	if (d < blockSize / 2 && fileSize > blockSize)
		blockSize += d / (int)(fileSize / (double)blockSize) + 1;

	byte* block = NULL;
	try
	{
		while(fileSize)
		{
			if (fileSize < blockSize)
				blockSize = fileSize;
			fileSize -= blockSize;

			block = new byte[blockSize];
			size_t readResult;
			readResult = fread(block, sizeof(byte), blockSize, in);
			if (readResult != (size_t)blockSize) throw length_error(inFileName);

			byte* codesLengths = new byte[ALPHABET];
			int encodedBlockSize;
			int lastBytePosition;
			byte* HuffmanEncodedBlock
				= BZIPEncode(block, blockSize, encodedBlockSize, lastBytePosition, codesLengths);

			delete [] block;
			block = NULL;
			fwrite(&encodedBlockSize, sizeof encodedBlockSize, 1, out);
			fwrite(&blockSize, sizeof blockSize, 1, out);
			fwrite(&lastBytePosition, sizeof lastBytePosition, 1, out);
			fwrite(codesLengths, sizeof(byte), ALPHABET, out);
			delete [] codesLengths;
			fwrite(HuffmanEncodedBlock, sizeof(byte), encodedBlockSize, out);
			delete [] HuffmanEncodedBlock;
		}
	}
	catch (const length_error& err)
	{
		delete [] block;
		fprintf(stderr, "Error reading from file:%s\n", err.what());
	}
	fclose(out);
	fclose(in);
}

void decode(char* inFileName, char* outFileName)
{
	if (!strcmp(inFileName, outFileName))
		identicalAddressesError(inFileName, outFileName);
	struct stat st;
	if (stat(inFileName, &st)!=0)
		fileNotFoundError(inFileName);
	long fileSize = st.st_size;
	FILE* in = fopen(inFileName, "rb");
	if (!in)
		fileNotOpenedError(inFileName);
	FILE* out = fopen(outFileName, "wb");
	if (!out)
	{
		fclose(in);
		fileNotOpenedError(outFileName);
	}

	byte *codesLengths = NULL, *HuffmanEncodedBlock = NULL, *block = NULL;
	try
	{
		while(fileSize)
		{
			int encodedBlockSize, blockSize, lastBytePosition;
			size_t readResult;
			readResult = fread(&encodedBlockSize, sizeof encodedBlockSize, 1, in);
			if (readResult != 1) throw length_error(inFileName);
			readResult = fread(&blockSize, sizeof blockSize, 1, in);
			if (readResult != 1) throw length_error(inFileName);
			readResult = fread(&lastBytePosition, sizeof lastBytePosition, 1, in);
			if (readResult != 1) throw length_error(inFileName);

			codesLengths = new byte[ALPHABET];
			readResult = fread(codesLengths, sizeof(byte), ALPHABET, in);
			if (readResult != ALPHABET) throw length_error(inFileName);

			HuffmanEncodedBlock = new byte[encodedBlockSize];
			readResult = fread(HuffmanEncodedBlock, sizeof(byte), encodedBlockSize, in);
			if (readResult != (size_t)encodedBlockSize) throw length_error(inFileName);

			block = new byte[blockSize];

			BZIPDecode(HuffmanEncodedBlock, encodedBlockSize, block, blockSize, lastBytePosition, codesLengths);

			delete [] codesLengths;
			codesLengths = NULL;
			delete [] HuffmanEncodedBlock;
			HuffmanEncodedBlock = NULL;
			fwrite(block, sizeof(byte), blockSize, out);
			delete [] block;
			block = NULL;

			fileSize -= encodedBlockSize + sizeof encodedBlockSize + sizeof blockSize +
						sizeof lastBytePosition + ALPHABET*(sizeof(byte));
		}
	}
	catch (const length_error& err)
	{
		readError(err.what());
	}
	catch (const logic_error& err)
	{
		fprintf(stderr, "Archive is broken. %s\n", err.what());
	}
	delete [] codesLengths;
	delete [] HuffmanEncodedBlock;
	delete [] block;
	fclose(out);
	fclose(in);
}

void help()
{
	puts("usage: bzip -e infile outfile [block size]");
	puts("       bzip -d infile outfile");
	puts("");
	puts("positional arguments:");
	puts("-e               compress file");
	puts("-d               decompress file");
	puts("[block size]     int in [1, 2147483647] (default 50000)");
	exit(0);
}

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