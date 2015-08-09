#ifndef __MYSQL_ACCESS_H__
#define __MYSQL_ACCESS_H__
#include <stdio.h>
#include <mysql.h>
#include <stdlib.h>
#include <string>
#include <map>
#include <iostream>
#include <unistd.h>
#include "data_access.h"

class CMySqlAccess:public CDataAccess
{
public:
	int Init(int flag, const char *host = NULL, 
			const char *uid = NULL, 
			const char *passwd = NULL,
			const char *database = NULL);
	void Uninit();
	int Reinit();
	int Run();
	int HandleResult();
	~CMySqlAccess()
	{
		Uninit();
	}

private:
	MYSQL *m_mysql;
	MYSQL_RES *m_res_ptr;
};

#endif
