#pragma once
#ifndef bwt
#define bwt

#include "definitions.h"
#include <vector>
using namespace std;

int BWTEncode(const vector<byte> & in, vector<byte> & out);
void BWTDecode(const vector<byte> & in, vector<byte> & out, int lastBytePosition);

#endif