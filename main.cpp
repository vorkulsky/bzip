#include "stdio.h"
#include "stdlib.h"
#include <sys/stat.h>
#include <string.h>
#include "definitions.h"
#include "bzip.h"
using namespace std;

void encode(char* inFileName, char* outFileName);
void decode(char* inFileName, char* outFileName);
void fileNotFoundError(char* fileName);
void fileNotOpenedError(char* fileName);
void identicalAddressesError(char* fileName1, char* fileName2);
void help();

int main(int argc, char *argv[])
{
	if (argc!=4)
		help();
	if (!strcmp(argv[1], "-e"))
		encode(argv[2], argv[3]);
	else if (!strcmp(argv[1], "-d"))
		decode(argv[2], argv[3]);
	else
		help();
	return 0;
}

void encode(char* inFileName, char* outFileName)
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
	if (!in)
	{
		fclose(in);
		fileNotOpenedError(inFileName);
	}

	int blockSize = 50000;
	int d = fileSize % blockSize;
	if (d < blockSize / 2 && fileSize > blockSize)
		blockSize += d / (int)(fileSize / (double)blockSize) + 1;

	while(fileSize)
	{
		if (fileSize < blockSize)
			blockSize = fileSize;
		fileSize -= blockSize;

		byte* block = new byte[blockSize];
		fread(block, sizeof(byte), blockSize, in);

		byte* codesLengths = new byte[ALPHABET];
		int encodedBlockSize;
		int lastBytePosition;
		byte* HuffmanEncodedBlock
			= BZIPEncode(block, blockSize, encodedBlockSize, lastBytePosition, codesLengths);

		delete [] block;
		fwrite(&encodedBlockSize, sizeof encodedBlockSize, 1, out);
		fwrite(&blockSize, sizeof blockSize, 1, out);
		fwrite(&lastBytePosition, sizeof lastBytePosition, 1, out);
		fwrite(codesLengths, sizeof(byte), ALPHABET, out);
		delete [] codesLengths;
		fwrite(HuffmanEncodedBlock, sizeof(byte), encodedBlockSize, out);
		delete [] HuffmanEncodedBlock;
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
	if (!in)
	{
		fclose(in);
		fileNotOpenedError(inFileName);
	}
	try {
		while(fileSize)
		{
			int encodedBlockSize, blockSize, lastBytePosition;
			fread(&encodedBlockSize, sizeof encodedBlockSize, 1, in);
			fread(&blockSize, sizeof blockSize, 1, in);
			fread(&lastBytePosition, sizeof lastBytePosition, 1, in);

			byte* codesLengths = new byte[ALPHABET];
			fread(codesLengths, sizeof(byte), ALPHABET, in);

			byte* HuffmanEncodedBlock = new byte[encodedBlockSize];
			fread(HuffmanEncodedBlock, sizeof(byte), encodedBlockSize, in);

			byte* block = new byte[blockSize];

			BZIPDecode(HuffmanEncodedBlock, encodedBlockSize, block, blockSize, lastBytePosition, codesLengths);

			delete [] codesLengths;
			delete [] HuffmanEncodedBlock;
			fwrite(block, sizeof(byte), blockSize, out);
			delete [] block;

			fileSize -= encodedBlockSize + sizeof encodedBlockSize + sizeof blockSize +
						sizeof lastBytePosition + ALPHABET*(sizeof(byte));
		}
	}
	catch(...)
	{
		fputs("The archive is damaged\n", stderr);
	}
	fclose(out);
	fclose(in);
}

void help()
{
	puts("usage: bzip -e infile outfile");
	puts("       bzip -d infile outfile");
	puts("");
	puts("positional arguments:");
	puts("-e     compress file");
	puts("-d     decompress file");
	exit(0);
}

void fileNotFoundError(char* fileName)
{
	fprintf(stderr, "File not found: %s\n", fileName);
	exit(1);
}

void fileNotOpenedError(char* fileName)
{
	fprintf(stderr, "File not opened: %s\n", fileName);
	exit(1);
}

void identicalAddressesError(char* fileName1, char* fileName2)
{
	fprintf(stderr, "Addresses of files are identical: %s, %s\n", fileName1, fileName2);
	exit(1);
}
