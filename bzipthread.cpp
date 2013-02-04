#include "bzipthread.h"
#include "bzip.h"
using namespace std;

void BZIPEncodeThread(bzipArguments &args, threadVariables &vars)
{
	while(args.inSize)
	{
		unique_lock<std::mutex> lk(vars.theMutex);
		args.out = BZIPEncode(args.in, args.inSize, args.outSize, args.lastBytePosition, args.codesLengths);
		vars.dataReady = DONE;
		vars.cond.notify_all();
		while (vars.dataReady != READY)
			vars.cond.wait(lk);
	}
}

void BZIPDecodeThread(bzipArguments &args, threadVariables &vars)
{
	while(args.inSize)
	{
		unique_lock<std::mutex> lk(vars.theMutex);
		try
		{
			BZIPDecode(args.in, args.inSize, args.out, args.outSize, args.lastBytePosition, args.codesLengths);
		}
		catch(...)
		{
			vars.pExc = current_exception();
			vars.dataReady = ERROR;
			vars.cond.notify_all();
		}
		vars.dataReady = DONE;
		vars.cond.notify_all();
		while (vars.dataReady != READY)
			vars.cond.wait(lk);
	}
}