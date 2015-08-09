#include "data_access.h"

void *CDataAccess::ThreadProc(void *arg)
{
	CDataAccess *pAccess = (CDataAccess *)arg;
	pAccess->Run();
}
