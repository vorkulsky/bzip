#include <vector>
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include "definitions.h"
#include "bzip.h"
using namespace std;

void encode(char* inFileName, char* outFileName)
{
	struct stat st;
	if (stat(inFileName, &st)!=0)
	{
		cerr<<"File not found: "<<inFileName<<endl;
		return;
	}
	long fileSize = st.st_size;
	ifstream in(inFileName, ifstream::binary|ifstream::in);
	int blockSize = 50000;
	int d = fileSize % blockSize;
	if (d < blockSize / 2 && fileSize > blockSize)
		blockSize += d / (int)(fileSize / (double)blockSize)+1;
	ofstream out(outFileName, ifstream::binary|ifstream::out);
	while(fileSize)
	{
		if (fileSize < blockSize)
			blockSize = fileSize;
		fileSize -= blockSize;
		char * bufFile = new char[blockSize];
		in.read(bufFile, blockSize);
	
		vector<byte> block(blockSize);
		for (int i = 0; i<blockSize; ++i)
		{
			block[i] = (byte)bufFile[i];
		}
		delete [] bufFile;

		vector<byte> HuffmanEncodedBlock;
		int lastBytePosition;
		vector<byte> codesLengths(alphabet);
		BZIPEncode(block, HuffmanEncodedBlock, lastBytePosition, codesLengths);
		int encodedBlockSize = (int)HuffmanEncodedBlock.size();

		out.write((char*)&encodedBlockSize, sizeof encodedBlockSize);
		out.write((char*)&blockSize, sizeof blockSize);
		out.write((char*)&lastBytePosition, sizeof lastBytePosition);
		for(int i=0; i<alphabet; ++i)
		{
			out.write((char*)&(codesLengths[i]), sizeof codesLengths[i]);
		}
		for(int i=0; i<encodedBlockSize; ++i)
		{
			out.write((char*)&(HuffmanEncodedBlock[i]), sizeof HuffmanEncodedBlock[i]);
		}
	}
	out.close();
	in.close();
}

void decode(char* inFileName, char* outFileName)
{
	struct stat st;
	if (stat(inFileName, &st)!=0)
	{
		cerr<<"File not found: "<<inFileName<<endl;
		return;
	}
	long fileSize = st.st_size;
	ifstream in(inFileName, ifstream::binary|ifstream::in);
	ofstream out(outFileName, ifstream::binary|ifstream::out);
	try {
		while(fileSize)
		{
			int encodedBlockSize, blockSize, lastBytePosition;
			in.read((char*)&(encodedBlockSize), sizeof encodedBlockSize);
			in.read((char*)&(blockSize), sizeof blockSize);
			in.read((char*)&(lastBytePosition), sizeof lastBytePosition);
			vector<byte> codesLengths(alphabet);
			byte codeLength;
			for (int i=0; i<alphabet; ++i)
			{
				in.read((char*)&(codeLength), sizeof codeLength);
				codesLengths[i] = codeLength;
			}
			vector<byte> HuffmanEncodedBlock(encodedBlockSize);
			byte encodedByte;
			for (int i=0; i<encodedBlockSize; ++i)
			{
				in.read((char*)&(encodedByte), sizeof encodedByte);
				HuffmanEncodedBlock[i] = encodedByte;
			}

			vector<byte> block(blockSize);
			BZIPDecode(HuffmanEncodedBlock, block, lastBytePosition, codesLengths);

			for(int i=0; i<blockSize; ++i)
			{
				out.write((char*)&(block[i]), sizeof block[i]);
			}

			fileSize -= encodedBlockSize + sizeof encodedBlockSize + sizeof blockSize +
						sizeof lastBytePosition + alphabet*(sizeof codeLength);
		}
	}
	catch(...)
	{
		cerr<<"The archive is damaged"<<endl;
	}
	out.close();
	in.close();
}

int main(int argc, char *argv[])
{
	if (argc!=4) return 1;
	if (!strcmp(argv[1], "-c"))
		encode(argv[2], argv[3]);
	else if (!strcmp(argv[1], "-d"))
		decode(argv[2], argv[3]);
	else return 1;
	return 0;
}

void help()
{
	cout
	<<"usage: bzip -e infile outfile"<<endl
	<<"       bzip -d infile outfile"<<endl
	<<endl
	<<"positional arguments:"<<endl
	<<"-c     compress file"<<endl
	<<"-d     decompress file"<<endl;
}