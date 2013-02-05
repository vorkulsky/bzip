#include <list>
#include "definitions.h"
using namespace std;

void MTFDecode(byte* block, int size)
{
    list<byte> encoding(ALPHABET);
    byte counter = 0;
    for (list<byte>::iterator it = encoding.begin(), end = encoding.end(); it != end; ++it, ++counter)
        *it = counter;
    byte currentByte = 0;
    for (int i=0; i<size; ++i)
    {
        currentByte = block[i];
        list<byte>::iterator it = encoding.begin();
        for (byte j=0; j < currentByte; ++j) ++it;
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
    byte currentByte = 0;
    for (int i=0; i<size; ++i)
    {
        currentByte = block[i];
        list<byte>::iterator pos = encoding.begin();
        byte counter = 0;
        for (; ;++pos, ++counter)
            if (*pos == currentByte)
                break;
        block[i] = counter;
        encoding.erase(pos);
        encoding.push_front(currentByte);
    }
}
