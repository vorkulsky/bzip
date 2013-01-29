#include <vector>
#include <algorithm>
#include "definitions.h"
#include "bwt.h"
#include "huffman.h"
#include "mtf.h"
using namespace std;

byte* BZIPEncode(const byte* in, int inSize, int& outSize, int& lastBytePosition, byte* codesLengths)
{
	vector<byte> outVector;
	vector<byte> codesLengthsVector(ALPHABET);
	byte* BWTMTFEncodedBlock = new byte[inSize];
	lastBytePosition = BWTEncode(in, BWTMTFEncodedBlock, inSize);
	vector<byte> BWTMTFEncodedBlockVector(BWTMTFEncodedBlock, BWTMTFEncodedBlock + inSize);
	MTFEncode(BWTMTFEncodedBlockVector);
	HuffmanEncode(BWTMTFEncodedBlockVector, outVector, codesLengthsVector);
	copy(codesLengthsVector.begin(), codesLengthsVector.end(), codesLengths);
	outSize = (int)outVector.size();
	byte* out = new byte[outSize];
	copy(outVector.begin(), outVector.end(), out);
	delete [] BWTMTFEncodedBlock;
	return out;
}

void BZIPDecode(const byte* in, int inSize, byte* out, int outSize, int lastBytePosition, const byte* codesLengths)
{
	vector<byte> inVector(in, in + inSize);
	vector<byte> outVector(outSize);
	vector<byte> codesLengthsVector(codesLengths, codesLengths + ALPHABET);
	vector<byte> HuffmanDecodedBlockVector(outSize);
	HuffmanDecode(inVector, HuffmanDecodedBlockVector, codesLengthsVector);
	MTFDecode(HuffmanDecodedBlockVector);
	byte* HuffmanDecodedBlock = new byte[outSize];
	copy(HuffmanDecodedBlockVector.begin(), HuffmanDecodedBlockVector.end(), HuffmanDecodedBlock);
	BWTDecode(HuffmanDecodedBlock, out, outSize, lastBytePosition);
}