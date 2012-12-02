#include <vector>
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include "definitions.h"
#include "bzip.h"
using namespace std;

int main()
{
	char fileName[] = "1.htm";
	struct stat st;
	if (stat(fileName, &st)!=0)
	{
		cout<<"NO FILE"<<endl;
		return 0;
	}
	long fileSize = st.st_size;
	ifstream in(fileName, ifstream::binary|ifstream::in);
	char * bufFile = new char[fileSize];
	in.read(bufFile, fileSize);
	in.close();
	int blockSize = fileSize;
	vector<byte> block(fileSize);
	for (int i = 0; i<blockSize; ++i)
	{
		block[i] = (byte)bufFile[i];
	}
	delete [] bufFile;

	vector<byte> HuffmanEncodedBlock;
	int lastBytePosition;
	vector<byte> codesLengths(alphabet);
	BZIPEncode(block, HuffmanEncodedBlock, lastBytePosition, codesLengths);
	
	ofstream out("out.txt", ifstream::binary|ifstream::out);
	out.write((char*)&blockSize, sizeof blockSize);
	out.write((char*)&lastBytePosition, sizeof lastBytePosition);
	for(int i=0; i<alphabet; ++i)
	{
		out.write((char*)&(codesLengths[i]), sizeof codesLengths[i]);
	}
	for(int i=0; i<(int)HuffmanEncodedBlock.size(); ++i)
	{
		out.write((char*)&(HuffmanEncodedBlock[i]), sizeof HuffmanEncodedBlock[i]);
	}
	out.close();

	vector<byte> block4(blockSize);
	BZIPDecode(HuffmanEncodedBlock, block4, lastBytePosition, codesLengths);

	ofstream out2("2.htm", ifstream::binary|ifstream::out);
	for(int i=0; i<int(blockSize); ++i)
	{
		out2.write((char*)&(block4[i]), sizeof block4[i]);
	}
	out2.close();

	return 0;
}
