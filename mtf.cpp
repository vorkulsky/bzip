#include <list>
#include "definitions.h"
using namespace std;

void MTFDecode(byte* block, int size)
{
	list<byte> encoding(ALPHABET);
	byte counter = 0;
	for (list<byte>::iterator it = encoding.begin(), end = encoding.end(); it != end; ++it, ++counter)
		*it = counter;
	byte current_byte = 0;
	for (int i=0; i<size; ++i)
	{
		current_byte = block[i];
		list<byte>::iterator it = encoding.begin();
		for (byte j=0; j < current_byte; ++j) ++it;
		block[i] = *it;
		encoding.erase(it);
		encoding.push_front(block[i]);
	}
}

void MTFEncode(byte* block, int size)
{
	list<byte> encoding(ALPHABET);
	byte counter = 0;
	for (list<byte>::iterator it = encoding.begin(), end = encoding.end(); it != end; ++it, ++counter)
		*it = counter;
	byte current_byte = 0;
	for (int i=0; i<size; ++i)
	{
		current_byte = block[i];
		list<byte>::iterator pos = encoding.begin();
		byte counter = 0;
		for (; ;++pos, ++counter)
			if (*pos == current_byte)
				break;
		block[i] = counter;
		encoding.erase(pos);
		encoding.push_front(current_byte);
	}
}
