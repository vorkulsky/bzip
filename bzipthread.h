#pragma once
#ifndef bzipthread
#define bzipthread

#include <thread>
#include <mutex>
#include <condition_variable>
#include "definitions.h"
using namespace std;

// READY - данные подготовлены для потока.
// DONE - данные вычислены потоком.
// NO - не то, ни другое.
// ERROR - ошибка в потоке.
// END - сигнализирует потоку, что ему нужно заветшиться.
enum readiness {READY, DONE, NO, ERROR, END};

struct threadVariables
{
	mutex theMutex;
	condition_variable cond;
	readiness dataReady;
	exception_ptr pExc;
	threadVariables(): dataReady(NO) {};
};

struct bzipArguments
{
	byte *in;
	byte *out;
	byte *codesLengths;
	int inSize;
	int outSize;
	int lastBytePosition;
	bzipArguments(): in(NULL), out(NULL), codesLengths(NULL), inSize(0), outSize(0), lastBytePosition(0) {};
	~bzipArguments()
	{
		delete [] in;
		delete [] out;
		delete [] codesLengths;
	}
};

void BZIPEncodeThread(bzipArguments &args, threadVariables &vars);

void BZIPDecodeThread(bzipArguments &args, threadVariables &vars);

#endif