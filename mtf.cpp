#include <vector>
#include <list>
#include <algorithm>
#include "definitions.h"
using namespace std;

void MTFDecode(vector<byte> & block)
{
	size_t size = block.size();
	list<byte> encoding(alphabet);
	byte counter = 0;
	for (list<byte>::iterator it = encoding.begin(), end = encoding.end(); it != end; ++it, ++counter)
		*it = counter;
	byte current_byte = 0;
	for (size_t i=0; i<size; ++i)
	{
		current_byte = block[i];
		list<byte>::iterator it = encoding.begin();
		for (byte j=0; j < current_byte; ++j) ++it;
		block[i] = *it;
		encoding.erase(it);
		encoding.push_front(block[i]);
	}
}

void MTFEncode(vector<byte> & block)
{
	size_t size = block.size();
	list<byte> encoding(alphabet);
	byte counter = 0;
	for (list<byte>::iterator it = encoding.begin(), end = encoding.end(); it != end; ++it, ++counter)
		*it = counter;
	byte current_byte = 0;
	for (size_t i=0; i<size; ++i)
	{
		current_byte = block[i];
		list<byte>::iterator pos = find(encoding.begin(), encoding.end(), current_byte);
		block[i] = (byte)distance(encoding.begin(), pos);
		encoding.erase(pos);
		encoding.push_front(current_byte);
	}
}
