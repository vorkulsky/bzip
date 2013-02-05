#include <stdexcept>
#include "decode.h"
#include "bzipthread.h"
#include "definitions.h"
using namespace std;

void Decode::afterOpeningFiles()
{
    // Ничего не делает.
}

void Decode::threadRun()
{
    _threads[_threadNum] = thread(BZIPDecodeThread, ref(_args[_threadNum]), ref(_vars[_threadNum]));
}

void Decode::readData()
{
    if (!_inFileSize) {_end = true; return;}
    try
    {
        size_t readResult;
        readResult = fread(&_args[_threadNum].inSize, sizeof _args[_threadNum].inSize, 1, _inFile);
        if (readResult != 1) throw length_error(_inFileName);
        readResult = fread(&_args[_threadNum].outSize, sizeof _args[_threadNum].outSize, 1, _inFile);
        if (readResult != 1) throw length_error(_inFileName);
        readResult = fread(&_args[_threadNum].lastBytePosition, sizeof _args[_threadNum].lastBytePosition, 1, _inFile);
        if (readResult != 1) throw length_error(_inFileName);

        if (_args[_threadNum].inSize && _args[_threadNum].outSize)
        {
            _args[_threadNum].codesLengths = new byte[ALPHABET];
            readResult = fread(_args[_threadNum].codesLengths, sizeof(byte), ALPHABET, _inFile);
            if (readResult != ALPHABET) throw length_error(_inFileName);

            _args[_threadNum].in = new byte[_args[_threadNum].inSize];
            readResult = fread(_args[_threadNum].in, sizeof(byte), _args[_threadNum].inSize, _inFile);
            if (readResult != (size_t)_args[_threadNum].inSize) throw length_error(_inFileName);

            _args[_threadNum].out = new byte[_args[_threadNum].outSize];
        }

        _inFileSize -= sizeof _args[_threadNum].inSize + sizeof _args[_threadNum].outSize
            + sizeof _args[_threadNum].lastBytePosition + ALPHABET*sizeof(byte) + _args[_threadNum].inSize*sizeof(byte);

        if (!_inFileSize) _end = true;
    }
    catch(...)
    {
        _error = true;
        _pExc = current_exception();
        deleteData();
    }
}

void Decode::writeData()
{
    if (_args[_threadNum].outSize)
    {
        fwrite(_args[_threadNum].out, sizeof(byte), _args[_threadNum].outSize, _outFile);
    }

    deleteData();
}

Decode::~Decode() {}
