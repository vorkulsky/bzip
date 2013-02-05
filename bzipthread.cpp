#include "bzipthread.h"
#include "bzip.h"
using namespace std;

void BZIPEncodeThread(bzipArguments &args, threadVariables &vars)
{
	do {
		unique_lock<std::mutex> lk(vars.theMutex);
		try
		{
			args.out = BZIPEncode(args.in, args.inSize, args.outSize, args.lastBytePosition, args.codesLengths);
			vars.dataReady = DONE;
		}
		catch(...)
		{
			vars.pExc = current_exception();
			vars.dataReady = ERROR;
		}
		vars.cond.notify_all();
		while (vars.dataReady != READY && vars.dataReady != END)
			vars.cond.wait(lk);
	} while(vars.dataReady != END);
}

void BZIPDecodeThread(bzipArguments &args, threadVariables &vars)
{
	
	do {
		unique_lock<std::mutex> lk(vars.theMutex);
		try
		{
			BZIPDecode(args.in, args.inSize, args.out, args.outSize, args.lastBytePosition, args.codesLengths);
			vars.dataReady = DONE;
		}
		catch(...)
		{
			vars.pExc = current_exception();
			vars.dataReady = ERROR;
		}
		vars.cond.notify_all();
		while (vars.dataReady != READY && vars.dataReady != END)
			vars.cond.wait(lk);
	} while(vars.dataReady != END);
}