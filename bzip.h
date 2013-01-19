#pragma once
#ifndef bzip
#define bzip

#include "definitions.h"
using namespace std;

byte* BZIPEncode(const byte* in, int inSize, int& outSize, int & lastBytePosition, byte* codesLengths);
void BZIPDecode(const byte* in, int inSize, byte* out, int outSize, int lastBytePosition, const byte* codesLengths);

#endif