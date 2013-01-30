#include "definitions.h"
#include "bwt.h"
#include "huffman.h"
#include "mtf.h"
using namespace std;

byte* BZIPEncode(const byte* in, int inSize, int& outSize, int& lastBytePosition, byte* codesLengths)
{
	byte* BWTMTFEncodedBlock = new byte[inSize];
	lastBytePosition = BWTEncode(in, BWTMTFEncodedBlock, inSize);
	MTFEncode(BWTMTFEncodedBlock, inSize);
	byte* out = HuffmanEncode(BWTMTFEncodedBlock, inSize, outSize, codesLengths);
	delete [] BWTMTFEncodedBlock;
	return out;
}

void BZIPDecode(const byte* in, int inSize, byte* out, int outSize, int lastBytePosition, const byte* codesLengths)
{
	byte* HuffmanDecodedBlock = new byte[outSize];
	HuffmanDecode(in, inSize, HuffmanDecodedBlock, outSize, codesLengths);
	MTFDecode(HuffmanDecodedBlock, outSize);
	BWTDecode(HuffmanDecodedBlock, out, outSize, lastBytePosition);
	delete [] HuffmanDecodedBlock;
}