#ifndef __ACCESS_ATTR_H__
#define __ACCESS_ATTR_H__
#include <stdio.h>
#include <mysql.h>
#include <stdlib.h>
#include <string>
#include <map>
#include <iostream>
#include <unistd.h>
#include "define.h"
#include "tool.h"

class CAccessAttr
{
public:
	static void SetThreadNum(int iThreadNum);
	static const char * GetQueryStr(int m_iFlag);
	static void Report(int m_iFlag);
	static void SetReportDue(int iDue);
	static int GetReportDUe();
	static void SetIgnoreErr(bool bIgnore);
	static bool GetIgnoreErr();
	static int PrintfStatistics();
	static void PrintfCountStatistics(int iCountTime);
	static bool SetStopFlag(bool bStop);
	static bool GetStopFlag();
public:
	static  int s_iReportDue;
	static bool s_bIgnore;
	static std::map<int, unsigned long> s_mapCountStatistics;
	static std::map<int, unsigned long long> s_mapMaxTimeStatistics;
	static std::map<int, unsigned long long> s_mapMinTimeStatistics;
	static std::map<int, unsigned long long> s_mapPreTime;
	static bool s_bStopFlag;
	static CProcessLock s_cProcessLock;
};

#endif
