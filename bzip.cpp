#include <vector>
#include <algorithm>
#include "definitions.h"
#include "bwt.h"
#include "huffman.h"
#include "mtf.h"
using namespace std;

byte* BZIPEncode(const byte* in, int inSize, int& outSize, int& lastBytePosition, byte* codesLengths)
{
	vector<byte> inVector(in, in + inSize);
	vector<byte> outVector;
	vector<byte> codesLengthsVector(ALPHABET);
	vector<byte> BWTMTFEncodedBlock(inSize);
	lastBytePosition = BWTEncode(inVector, BWTMTFEncodedBlock);
	MTFEncode(BWTMTFEncodedBlock);
	HuffmanEncode(BWTMTFEncodedBlock, outVector, codesLengthsVector);
	copy(codesLengthsVector.begin(), codesLengthsVector.end(), codesLengths);
	outSize = (int)outVector.size();
	byte* out = new byte[outSize];
	copy(outVector.begin(), outVector.end(), out);
	return out;
}

void BZIPDecode(const byte* in, int inSize, byte* out, int outSize, int lastBytePosition, const byte* codesLengths)
{
	vector<byte> inVector(in, in + inSize);
	vector<byte> outVector(outSize);
	vector<byte> codesLengthsVector(codesLengths, codesLengths + ALPHABET);
	vector<byte> HuffmanDecodedBlock(outSize);
	HuffmanDecode(inVector, HuffmanDecodedBlock, codesLengthsVector);
	MTFDecode(HuffmanDecodedBlock);
	BWTDecode(HuffmanDecodedBlock, outVector, lastBytePosition);
	copy(outVector.begin(), outVector.end(), out);
}