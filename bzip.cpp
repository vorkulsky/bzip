#include <vector>
#include <iostream>
#include "definitions.h"
#include "bwt.h"
#include "huffman.h"
#include "mtf.h"
using namespace std;

void BZIPEncode(const vector<byte> & in, vector<byte> & out, int & lastBytePosition, vector<byte> & codesLengths)
{
	int blockSize = (int)in.size();

	cout<<"ishodniy"<<endl;
	for (int i=0; i<min((int)blockSize, 50); i++)
		cout<<(int)in[i]<<' ';
	cout<<endl;
	
	vector<byte> BWTMTFEncodedBlock(blockSize);
	lastBytePosition = BWTEncode(in, BWTMTFEncodedBlock);

	cout<<endl<<"BWTEncode"<<endl;
	for (int i=0; i<min((int)blockSize, 50); i++)
		cout<<(int)BWTMTFEncodedBlock[i]<<' ';
	cout<<endl;

	MTFEncode(BWTMTFEncodedBlock);

	cout<<endl<<"MTFEncode"<<endl;
	for (int i=0; i<min((int)blockSize, 50); i++)
		cout<<(int)BWTMTFEncodedBlock[i]<<' ';
	cout<<endl;

	HuffmanEncode(BWTMTFEncodedBlock, out, codesLengths);

	int size2 = (int)out.size();

	cout<<endl<<"after HuffmanEncode"<<endl;
	for (int i=0; i<min((int)size2, 50); i++)
		cout<<(int)out[i]<<' ';
	cout<<endl;
}

void BZIPDecode(const vector<byte> & in, vector<byte> & out, int lastBytePosition, const vector<byte> & codesLengths)
{
	int blockSize = (int)out.size();

	vector<byte> HuffmanDecodedBlock(blockSize);
	HuffmanDecode(in, blockSize, HuffmanDecodedBlock, codesLengths);

	cout<<endl<<"after HuffmanDecode"<<endl;
	for (int i=0; i<min((int)blockSize, 50); i++)
		cout<<(int)HuffmanDecodedBlock[i]<<' ';
	cout<<endl;

	MTFDecode(HuffmanDecodedBlock);
	cout<<endl<<"MTFDecode"<<endl;
	for (int i=0; i<min((int)blockSize, 50); i++)
		cout<<(int)HuffmanDecodedBlock[i]<<' ';
	cout<<endl;

	cout<<endl<<"BWTDecode"<<endl;
	BWTDecode(HuffmanDecodedBlock, out, lastBytePosition);

	for (int i=0; i<min((int)blockSize, 50); i++)
		cout<<(int)out[i]<<' ';
	cout<<endl;
}