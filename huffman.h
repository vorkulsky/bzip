#pragma once
#ifndef huffman
#define huffman

#include "definitions.h"
#include <vector>
using namespace std;

void HuffmanEncode(const vector<byte> & in, vector<byte> & out, vector<byte> & codesLengths);
//
void HuffmanDecode(const vector<byte> & in, vector<byte> & out, const vector<byte> & codesLengths);
//	out должен содержать столько элементов, сколько должно быть символов в незакодированном тексте.

#endif