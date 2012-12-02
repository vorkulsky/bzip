#pragma once
#ifndef bwt
#define bwt

#include "definitions.h"

int BWTEncode(byte* in, byte* out, int size);
void BWTDecode(byte* in, byte* out, int size, int lastBytePosition);

#endif