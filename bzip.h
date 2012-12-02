#pragma once
#ifndef bzip
#define bzip

#include "definitions.h"
#include <vector>
using namespace std;

void BZIPEncode(const vector<byte> & in, vector<byte> & out, int & lastBytePosition, vector<byte> & codesLengths);
void BZIPDecode(const vector<byte> & in, vector<byte> & out, int lastBytePosition, const vector<byte> & codesLengths);

#endif