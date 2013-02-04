#include <string.h>
#include <sys/stat.h>
#include "operation.h"
#include "bziperrors.h"
#include "definitions.h"
#include "bzipthread.h"
using namespace std;

Operation::Operation(const char* inFileName, const char* outFileName, int threadsQuantity)
{
	_inFileName = inFileName;
	_outFileName = outFileName;
	_threadsQuantity = threadsQuantity;
	_threads = new thread[_threadsQuantity];
	_vars = new threadVariables[_threadsQuantity];
	_args = new bzipArguments[_threadsQuantity];
	_end = false;
}

Operation::~Operation()
{
	delete [] _threads;
	delete [] _vars;
	delete [] _args;
}

void Operation::performOperation()
{
	openFiles();

	afterOpeningFiles();

	_threads = new thread[_threadsQuantity];
	_vars = new threadVariables[_threadsQuantity];
	_args = new bzipArguments[_threadsQuantity];

	for (_threadNum=0; _threadNum<_threadsQuantity && !_end; ++_threadNum)
	{
		threadRun();
	}
	_threadsQuantity = _threadNum;
	
	for (int blockNumber=0, finishedThreadsQuantity=0; finishedThreadsQuantity<_threadsQuantity; ++blockNumber)
	{
		_threadNum = blockNumber % _threadsQuantity;
		unique_lock<std::mutex> lk(_vars[_threadNum].theMutex);
		while (_vars[_threadNum].dataReady!=DONE /*|| _vars[_threadNum].dataReady!=ERROR*/)
			_vars[_threadNum].cond.wait(lk);
		writeData();
		// Если inSize == 0, поток понимает, что должен завершить работу.
		_args[_threadNum].inSize = 0;
		if (!_end) readData();
		else finishedThreadsQuantity++;
		_vars[_threadNum].dataReady = READY;
		_vars[_threadNum].cond.notify_all();
	}

	for (_threadNum=0; _threadNum<_threadsQuantity; ++_threadNum)
	{
		_threads[_threadNum].join();
	}

	fclose(_outFile);
	fclose(_inFile);
}

void Operation::openFiles()
{
	if (!strcmp(_inFileName, _outFileName))
		identicalAddressesError(_inFileName, _outFileName);
	struct stat st;
	if (stat(_inFileName, &st)!=0)
		fileNotFoundError(_inFileName);
	_inFileSize = st.st_size;
	_inFile = fopen(_inFileName, "rb");
	if (!_inFile)
		fileNotOpenedError(_inFileName);
	_outFile = fopen(_outFileName, "wb");
	if (!_outFile)
	{
		fclose(_inFile);
		fileNotOpenedError(_outFileName);
	}
}