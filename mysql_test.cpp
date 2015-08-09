#include <pthread.h>
#include <unistd.h>
#include "mysql_test.h"
#include "access_attr.h"

int CMySqlAccess::Init(int flag, const char *host, const char *uid, 
			const char *passwd, const char *database)
{
	printf(">>> init begin\n");
	m_iFlag = flag;
	m_strHost = host;
	m_strUid = uid;
	m_strPasswd = passwd;
	m_strDb = database;
	m_res_ptr = NULL;

         if (mysql_library_init(0, NULL, NULL))
        {
                printf("mysql_library_init return failed\n");
                return -1;
        }

        if ((m_mysql = mysql_init(NULL)) == NULL)
        {
                printf("mysql_init return failed, errinfo = %s\n", mysql_error(m_mysql));
                return -1;
        }

        if (mysql_real_connect(m_mysql, host, uid, passwd, database, 0, NULL, 0) == NULL)
        {
                printf("mysql_real_connect return failed, errinfo = %s\n", mysql_error(m_mysql));
                mysql_close(m_mysql);
                return -1;
        }

	printf(">>> init end\n");
	return 0;
}

void CMySqlAccess::Uninit()
{
//	printf(">>>uinit begin\n");

	if (m_res_ptr != NULL)
	{
		mysql_free_result(m_res_ptr);
		m_res_ptr = NULL;
	}

	mysql_close(m_mysql);
	mysql_library_end();
//	printf(">>>uinit end\n");
}

int CMySqlAccess::Reinit()
{
	Uninit();
	mysql_init(m_mysql);
	m_res_ptr = NULL;

        if (mysql_library_init(0, NULL, NULL))
        {
                printf("mysql_library_init return failed\n");
                return -1;
        }

        if ((m_mysql = mysql_init(NULL)) == NULL)
        {
                printf("mysql_init return failed, errinfo = %s\n", mysql_error(m_mysql));
                return -1;
        }

	 MYSQL *connect = mysql_real_connect(m_mysql, m_strHost.c_str(), 
					m_strUid.c_str(), m_strPasswd.c_str(), 
					m_strDb.c_str(), 0, NULL, 0);

	if (connect ==  NULL)
	{
		printf("%s\n", mysql_error(m_mysql));
		mysql_close(m_mysql);
		return -1;
	}

//	printf(">>> init end\n");
	return 0;
}

int CMySqlAccess::HandleResult()
{
        try
        {
                MYSQL_RES *m_res_ptr;
                MYSQL_ROW m_res_row;
                m_res_ptr = mysql_store_result(m_mysql);
                int ret_num = mysql_num_rows(m_res_ptr);

                while (ret_num != 0)
                {
                        m_res_row = mysql_fetch_row(m_res_ptr);
                        std::string struid = m_res_row[0];              
                        ret_num --;
                }       

                mysql_free_result(m_res_ptr);
		m_res_ptr = NULL;
                return 0;
        }
        catch(...)
        {
		m_res_ptr = NULL;
		return -1;
        }
	
	return 0;
}

int CMySqlAccess::Run()
{
        int index = 0;
        std::string strCache;

        while(!CAccessAttr::GetStopFlag())
        {
                strCache = CAccessAttr::GetQueryStr(m_iFlag);
                int ret = mysql_query(m_mysql, strCache.c_str());

                if (ret != 0 )
                {
                        printf("mysql_query return failed , errinfo = %d\n",
				 mysql_errno(m_mysql));
			
			if (!CAccessAttr::GetIgnoreErr())
			{
				return -1;
			}
                }

                index++;

                if (index % CAccessAttr::GetReportDUe() == 0)
                {
                        CAccessAttr::Report(m_iFlag);
                }

                if (HandleResult() != 0 && !CAccessAttr::GetIgnoreErr())
                {
                        printf("handle_result return failed\n  ");
                        return -1;
                }
        }

        printf("run end, index = %d, flag = %d\n", index, m_iFlag);
	return 0;
}
