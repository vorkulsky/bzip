#include <string.h>
#include <sys/stat.h>
#include <stdexcept>
#include "operation.h"
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
    _error = false;
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

    for (_threadNum=0; _threadNum<_threadsQuantity && !_end && !_error; ++_threadNum)
    {
        readData();
        if (_error) break;
        threadRun();
    }
    _threadsQuantity = _threadNum;
    
    for (int blockNumber=0, finishedThreadsQuantity=0; finishedThreadsQuantity<_threadsQuantity; ++blockNumber)
    {
        _threadNum = blockNumber % _threadsQuantity;
        unique_lock<std::mutex> lk(_vars[_threadNum].theMutex);
        while (_vars[_threadNum].dataReady!=DONE && _vars[_threadNum].dataReady!=ERROR)
            _vars[_threadNum].cond.wait(lk);
        if (_vars[_threadNum].dataReady!=ERROR)
            writeData();
        else
        {
            _error = true;
            _pExc = _vars[_threadNum].pExc;
            deleteData();
        }
        if (!_end && !_error)
        {
            readData();
            if (!_error) _vars[_threadNum].dataReady = READY;
            else _vars[_threadNum].dataReady = END;
        }
        else 
        {
            finishedThreadsQuantity++;
            _vars[_threadNum].dataReady = END;
        }
        _vars[_threadNum].cond.notify_all();
    }

    for (_threadNum=0; _threadNum<_threadsQuantity; ++_threadNum)
    {
        _threads[_threadNum].join();
    }

    fclose(_outFile);
    fclose(_inFile);

    if (_error)
        rethrow_exception(_pExc);
}

void Operation::openFiles()
{
    if (!strcmp(_inFileName, _outFileName))
        throw invalid_argument("Addresses of infile and outfile are identical.");
    struct stat st;
    if (stat(_inFileName, &st)!=0)
        throw invalid_argument("File not found: infile.");
    _inFileSize = st.st_size;
    _inFile = fopen(_inFileName, "rb");
    if (!_inFile)
        logic_error("File not opened: infile.");
    _outFile = fopen(_outFileName, "wb");
    if (!_outFile)
    {
        fclose(_inFile);
        logic_error("File not opened: outfile.");
    }
}

void Operation::deleteData()
{
    delete [] _args[_threadNum].in;
    _args[_threadNum].in = NULL;
    _args[_threadNum].inSize = 0;
    delete [] _args[_threadNum].out;
    _args[_threadNum].out = NULL;
    _args[_threadNum].outSize = 0;
    delete [] _args[_threadNum].codesLengths;
    _args[_threadNum].codesLengths = NULL;
}
