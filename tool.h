#ifndef __TOOL_H__
#define __TOOL_H__

class CProcessLock
{
public:
	void Lock()
	{
		pthread_mutex_lock(&m_mutex);	
		m_bLock = 1;
	}

	void Unlock()
	{
		m_bLock = 0;
		pthread_mutex_unlock(&m_mutex);
	}

	void WaitProcess()
	{
		if (m_bLock)
		{
			Lock();
			Unlock();
		}
	}

	void Init()
	{
		pthread_mutex_init(&m_mutex, NULL);
		m_bLock = 0;
	}
private:
	int m_bLock;
	pthread_mutex_t m_mutex;
};

#endif

