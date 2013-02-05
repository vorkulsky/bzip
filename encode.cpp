#include <stdexcept>
#include "encode.h"
#include "bzipthread.h"
#include "definitions.h"
using namespace std;

void Encode::afterOpeningFiles()
{
    // Если при архивации применять заданный размер блока, то может случиться, что последний блок
    // окажется очень маленьким, что плохо скажется на сжатии. Поэтому в этом случае
    // мы изменим немого размер блока так, чтобы все блоки стали примерно одинаковой длины.
    int delta = _inFileSize % _blockSize;
    if (delta < _blockSize / 2 && _inFileSize > _blockSize)
        _blockSize += delta / (int)(_inFileSize / (double)_blockSize) + 1;
}

void Encode::threadRun()
{
    _threads[_threadNum] = thread(BZIPEncodeThread, ref(_args[_threadNum]), ref(_vars[_threadNum]));
}

void Encode::readData()
{
    if (!_inFileSize) {_end = true; return;}
    try
    {
        if (_inFileSize < _blockSize)
            _blockSize = _inFileSize;

        _args[_threadNum].in = new byte[_blockSize];
        size_t readResult;
        readResult = fread(_args[_threadNum].in, sizeof(byte), _blockSize, _inFile);
        if (readResult != (size_t)_blockSize) throw length_error(_inFileName);

        _args[_threadNum].inSize = _blockSize;

        _args[_threadNum].codesLengths = new byte[ALPHABET];

        _inFileSize -= _blockSize;
        if (!_inFileSize) _end = true;
    }
    catch(...)
    {
        _error = true;
        _pExc = current_exception();
        deleteData();
    }
}

void Encode::writeData()
{
    if (_args[_threadNum].outSize)
    {
        fwrite(&_args[_threadNum].outSize, sizeof _args[_threadNum].outSize, 1, _outFile);
        fwrite(&_args[_threadNum].inSize, sizeof _args[_threadNum].inSize, 1, _outFile);
        fwrite(&_args[_threadNum].lastBytePosition, sizeof _args[_threadNum].lastBytePosition, 1, _outFile);
        fwrite(_args[_threadNum].codesLengths, sizeof(byte), ALPHABET, _outFile);
        fwrite(_args[_threadNum].out, sizeof(byte), _args[_threadNum].outSize, _outFile);
    }

    deleteData();
}

Encode::~Encode() {}
