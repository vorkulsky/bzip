#include <vector>
#include "stdio.h"
#include <sys/stat.h>
#include <string.h>
#include "definitions.h"
#include "bzip.h"
using namespace std;

void encode(char* inFileName, char* outFileName)
{
	struct stat st;
	if (stat(inFileName, &st)!=0)
	{
		fprintf(stderr, "File not found: %s\n", inFileName);
		exit(1);
	}
	long fileSize = st.st_size;
	FILE * in;
	fopen_s(&in, inFileName, "rb");
	//FILE * in = fopen(inFileName, "rb");
	int blockSize = 50000;
	int d = fileSize % blockSize;
	if (d < blockSize / 2 && fileSize > blockSize)
		blockSize += d / (int)(fileSize / (double)blockSize) + 1;
	FILE * out;
	fopen_s(&out, outFileName, "wb");
	//FILE * out = fopen(outFileName, "wb");
	while(fileSize)
	{
		if (fileSize < blockSize)
			blockSize = fileSize;
		fileSize -= blockSize;
		byte * bufFile = new byte[blockSize];
		fread(bufFile, sizeof bufFile[0], blockSize, in);
		vector<byte> block(bufFile, bufFile + blockSize);
		delete [] bufFile;

		vector<byte> HuffmanEncodedBlock;
		int lastBytePosition;
		vector<byte> codesLengths(ALPHABET);
		BZIPEncode(block, HuffmanEncodedBlock, lastBytePosition, codesLengths);
		int encodedBlockSize = (int)HuffmanEncodedBlock.size();

		fwrite(&encodedBlockSize, sizeof encodedBlockSize, 1, out);
		fwrite(&blockSize, sizeof blockSize, 1, out);
		fwrite(&lastBytePosition, sizeof lastBytePosition, 1, out);
		for(int i=0; i<ALPHABET; ++i)
		{
			fwrite(&(codesLengths[i]), sizeof codesLengths[i], 1, out);
		}
		for(int i=0; i<encodedBlockSize; ++i)
		{
			fwrite(&(HuffmanEncodedBlock[i]), sizeof HuffmanEncodedBlock[i], 1, out);
		}
	}
	fclose(out);
	fclose(in);
}

void decode(char* inFileName, char* outFileName)
{
	struct stat st;
	if (stat(inFileName, &st)!=0)
	{
		fprintf(stderr, "File not found: %s\n", inFileName);
		exit(1);
	}
	long fileSize = st.st_size;
	FILE * in;
	fopen_s(&in, inFileName, "rb");
	//FILE * in = fopen(inFileName, "rb");
	FILE * out;
	fopen_s(&out, outFileName, "wb");
	//FILE * out = fopen(outFileName, "wb");
	try {
		while(fileSize)
		{
			int encodedBlockSize, blockSize, lastBytePosition;
			fread(&encodedBlockSize, sizeof encodedBlockSize, 1, in);
			fread(&blockSize, sizeof blockSize, 1, in);
			fread(&lastBytePosition, sizeof lastBytePosition, 1, in);
			vector<byte> codesLengths(ALPHABET);
			byte codeLength = 0;
			byte codesLengthsarr[ALPHABET];
			fread(codesLengthsarr, sizeof codeLength, ALPHABET, in);
			for (int i=0; i<ALPHABET; ++i)
			{
				codesLengths[i] = codesLengthsarr[i];
			}
			vector<byte> HuffmanEncodedBlock(encodedBlockSize);
			byte encodedByte;
			byte* HuffmanEncodedBlockarr = new byte[encodedBlockSize];
			fread(HuffmanEncodedBlockarr, sizeof encodedByte, encodedBlockSize, in);
			for (int i=0; i<encodedBlockSize; ++i)
			{
				HuffmanEncodedBlock[i] = HuffmanEncodedBlockarr[i];
			}
			delete [] HuffmanEncodedBlockarr;
			vector<byte> block(blockSize);
			BZIPDecode(HuffmanEncodedBlock, block, lastBytePosition, codesLengths);

			for(int i=0; i<blockSize; ++i)
			{
				fwrite(&(block[i]), sizeof block[i], 1, out);
			}

			fileSize -= encodedBlockSize + sizeof encodedBlockSize + sizeof blockSize +
						sizeof lastBytePosition + ALPHABET*(sizeof codeLength);
		}
	}
	catch(...)
	{
		fputs("The archive is damaged", stderr);
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
}

int main(int argc, char *argv[])
{
	if (argc!=4)
	{
		help();
		return 1;
	}
	if (!strcmp(argv[1], "-e"))
		encode(argv[2], argv[3]);
	else if (!strcmp(argv[1], "-d"))
		decode(argv[2], argv[3]);
	else
	{
		help();
		return 1;
	}
	return 0;
}
