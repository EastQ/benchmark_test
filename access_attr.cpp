#include <stdio.h>
#include <string.h>
#include <stdio.h>
#include <mysql.h>
#include <stdlib.h>
#include <string>
#include <map>
#include <iostream>
#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>
#include "access_attr.h"
#include "mysql_test.h"

int CAccessAttr::s_iReportDue = 1;
bool CAccessAttr::s_bIgnore = true;
std::map<int, unsigned long> CAccessAttr::s_mapCountStatistics;
std::map<int, unsigned long long> CAccessAttr::s_mapMaxTimeStatistics;
std::map<int, unsigned long long> CAccessAttr::s_mapMinTimeStatistics;
std::map<int, unsigned long long> CAccessAttr::s_mapPreTime;
CProcessLock CAccessAttr::s_cProcessLock;
bool CAccessAttr::s_bStopFlag = false;

void CAccessAttr::SetThreadNum(int iThreadNum)
{
	for (int i = 0; i < iThreadNum; ++i)
	{
		s_mapCountStatistics[i] = 0; 	
		s_mapMaxTimeStatistics[i] = 0;
		s_mapMinTimeStatistics[i] = 1000000000000000;
	}

	s_cProcessLock.Init();
}


void CAccessAttr::Report(int m_iFlag)
{
	struct timeval start;
    	gettimeofday(&start, NULL);
	unsigned long long iPreTime = s_mapPreTime[m_iFlag];
	s_mapPreTime[m_iFlag] = start.tv_sec*1000000 + start.tv_usec; 
	
	iPreTime = s_mapPreTime[m_iFlag] - iPreTime;	
	
	if (s_mapMaxTimeStatistics[m_iFlag] < iPreTime)
	{
		s_mapMaxTimeStatistics[m_iFlag] = iPreTime;
	}

	if (s_mapMaxTimeStatistics[m_iFlag] > iPreTime)
	{
		s_mapMinTimeStatistics[m_iFlag] = iPreTime;
	}

	s_mapCountStatistics[m_iFlag] += REPORT_DUE;	
}

void CAccessAttr::SetReportDue(int iDue)
{
	s_iReportDue = iDue;
}

int CAccessAttr::GetReportDUe()
{
	return s_iReportDue;
}

void CAccessAttr::SetIgnoreErr(bool bIgnore)
{
	s_bIgnore = bIgnore;	
}

bool CAccessAttr::GetIgnoreErr()
{
	return s_bIgnore;
}

int CAccessAttr::PrintfStatistics()
{
	static std::map<int, unsigned long> report_switch_map1;
	static std::map<int, unsigned long> report_switch_map2;
	static int index = 0;

	report_switch_map1 = report_switch_map2;
	report_switch_map2 = s_mapCountStatistics;
	index ++;
	printf("\n\nindex = %d\n", index);

	for (int i = 0; i < s_mapCountStatistics.size(); ++i)
	{
		printf("flag = %d, exec_num = %ld\n", 
			i, report_switch_map2[i] - report_switch_map1[i]);
	}

	return index;
}

void CAccessAttr::PrintfCountStatistics(int iCountTime)
{	
	unsigned long long  lTotal = 0;

	printf("=====================================================================\n");
	for (int i = 0; i < s_mapCountStatistics.size(); ++i)
	{
		printf("flag = %d, total = %ld, avg = %ld\n",
			 i, s_mapCountStatistics[i], 
			s_mapCountStatistics[i]/iCountTime);		
		lTotal += s_mapCountStatistics[i];
	}	

	printf("=====================================================================\n");
	printf("all total = %lld, avg = %lld, thread_avg = %lld\n", 
		lTotal, lTotal/iCountTime, lTotal/iCountTime/s_mapCountStatistics.size()); 
	
	unsigned long long  iMaxTime = 0;
	int iIndex = -1;

	for (int i = 0; i < s_mapMaxTimeStatistics.size(); ++i)
	{
		if (s_mapMaxTimeStatistics[i] > iMaxTime)
		{
			iMaxTime = s_mapMaxTimeStatistics[i];
			iIndex = i;
		}		
	}	

	printf("=====================================================================\n");
	printf("MaxTime = %lld us, flag = %d\n", iMaxTime, iIndex);
	printf("=====================================================================\n");

	unsigned long long iMinTime = 1000000000000000000; 
	iIndex = -1;

	for (int i = 0; i < s_mapMinTimeStatistics.size(); ++i)
	{
		if (s_mapMinTimeStatistics[i] < iMinTime)
		{
			iMinTime = s_mapMinTimeStatistics[i];
			iIndex = i;
		}	
	}

	printf("MinTime = %lld us, flag = %d\n", iMinTime, iIndex);
	printf("=====================================================================\n");
}

bool CAccessAttr::SetStopFlag(bool bStop)
{
	s_bStopFlag = bStop;
}

bool CAccessAttr::GetStopFlag()
{
	return s_bStopFlag;
}

const char * CAccessAttr::GetQueryStr(int m_iFlag)
{
	struct timeval start;
    	gettimeofday(&start, NULL);
	s_mapPreTime[m_iFlag] = start.tv_sec*1000000 + start.tv_usec; 
	//设置要查询的sql语句
	return EXEC_SQL;
}


void *start_test(void *arg)
{
        srand(time(NULL));
      	CAccessAttr::SetThreadNum(THREAD_NUM);
	CMySqlAccess access[THREAD_NUM];
	pthread_t pthreads[THREAD_NUM];	

        for (int i = 0; i < THREAD_NUM; ++i)
        {
                access[i].Init(i, HOST, USER, PASSWD, DB);
                pthread_create(&pthreads[i], NULL, CDataAccess::ThreadProc, &access[i]);
        }

        int index = 0;

        while(!CAccessAttr::GetStopFlag())
        {
                sleep(1);
		index = CAccessAttr::PrintfStatistics();		
        }

	for (int i = 0; i < THREAD_NUM; ++i)
	{
		pthread_join(pthreads[i], NULL);
	}
	
	CAccessAttr::PrintfCountStatistics(index);
}

int main()
{
        pthread_t pid;
        pthread_create(&pid, NULL, start_test, NULL);

        getchar();
	CAccessAttr::SetStopFlag(true);

        pthread_join(pid, NULL);
}
