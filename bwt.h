#pragma once
#ifndef bwt
#define bwt

#include "definitions.h"

//Размер in = размер out = size.
int BWTEncode(const byte* in, byte* out, int size);
void BWTDecode(const byte* in, byte* out, int size, int lastBytePosition);

#endif