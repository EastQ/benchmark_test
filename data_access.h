#ifndef __DATA_ACCESS_H__
#define __DATA_ACCESS_H__
#include <stdio.h>
#include <mysql.h>
#include <stdlib.h>
#include <string>
#include <map>
#include <iostream>
#include <unistd.h>

class CDataAccess
{
public:
	virtual ~CDataAccess() {}
	virtual	int Init(int flag, const char *host = NULL, 
			const char *uid = NULL, 
			const char *passwd = NULL, 
			const char *database = NULL)=0;

	virtual int Reinit()=0;
	virtual void Uninit()=0;
	virtual	int Run()=0;		

	static void *ThreadProc(void *arg);
protected:
	std::string m_strHost;
	std::string m_strUid;
	std::string m_strPasswd;
	std::string m_strDb;
	int		m_iFlag;
};
#endif
