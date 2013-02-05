#include <stddef.h>
#include "definitions.h"
#include "bwt.h"
#include "huffman.h"
#include "mtf.h"
using namespace std;

byte* BZIPEncode(const byte* in, int inSize, int& outSize, int& lastBytePosition, byte* codesLengths)
{
	if (!inSize) return NULL;
	byte* out = NULL;
	byte* BWTMTFEncodedBlock = new byte[inSize];
	try
	{
		lastBytePosition = BWTEncode(in, BWTMTFEncodedBlock, inSize);
		MTFEncode(BWTMTFEncodedBlock, inSize);
		out = HuffmanEncode(BWTMTFEncodedBlock, inSize, outSize, codesLengths);
	}
	catch(...)
	{
		delete [] BWTMTFEncodedBlock;
		throw;
	}
	delete [] BWTMTFEncodedBlock;
	return out;
}

void BZIPDecode(const byte* in, int inSize, byte* out, int outSize, int lastBytePosition, const byte* codesLengths)
{
	if (!inSize || !outSize) return;
	byte* HuffmanDecodedBlock = new byte[outSize];
	try
	{
		HuffmanDecode(in, inSize, HuffmanDecodedBlock, outSize, codesLengths);
		MTFDecode(HuffmanDecodedBlock, outSize);
		BWTDecode(HuffmanDecodedBlock, out, outSize, lastBytePosition);
	}
	catch(...)
	{
		delete [] HuffmanDecodedBlock;
		throw;
	}
	delete [] HuffmanDecodedBlock;
}