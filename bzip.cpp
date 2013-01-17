#include <vector>
#include "definitions.h"
#include "bwt.h"
#include "huffman.h"
#include "mtf.h"
using namespace std;

void BZIPEncode(const vector<byte> & in, vector<byte> & out, int & lastBytePosition, vector<byte> & codesLengths)
{
	int blockSize = (int)in.size();

	vector<byte> BWTMTFEncodedBlock(blockSize);
	lastBytePosition = BWTEncode(in, BWTMTFEncodedBlock);

	MTFEncode(BWTMTFEncodedBlock);

	HuffmanEncode(BWTMTFEncodedBlock, out, codesLengths);
}

void BZIPDecode(const vector<byte> & in, vector<byte> & out, int lastBytePosition, const vector<byte> & codesLengths)
{
	int blockSize = (int)out.size();

	vector<byte> HuffmanDecodedBlock(blockSize);
	HuffmanDecode(in, HuffmanDecodedBlock, codesLengths);

	MTFDecode(HuffmanDecodedBlock);

	BWTDecode(HuffmanDecodedBlock, out, lastBytePosition);
}