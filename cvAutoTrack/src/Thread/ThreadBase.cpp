#include "pch.h"
#include "ThreadBase.h"

ThreadBase::ThreadBase()
{
	t_ptr = new std::thread(&ThreadBase::Run, this);
	//t_ptr.get()->detach();
}

ThreadBase::ThreadBase(std::function<void(void)> functionPtr)
{
	f_tpr = functionPtr;
	isExistFptr = true;
	//t_ptr =new std::thread(Run, this);
}

ThreadBase::~ThreadBase()
{
	isExit = true;

	t_ptr->join();
}

void ThreadBase::Run()
{
	bool isCycle = true;
	while (isCycle)
	{
		if (isExit)
		{
			break;
		}

		//������ڹ�������
		if (isExistFptr)
		{
			isRunning = true;
			/// ���й�������
			f_tpr();
			isRunning = false;
		}
		else
		{
#ifdef _WIN32
			Sleep(1);
#endif // _WIN32
		}
	}
}

bool ThreadBase::setFunctionPtr(std::function<void(void)> functionPtr)
{
	if (isRunning == false || isExistFptr == false)
	{
		f_tpr = functionPtr;
		isExistFptr = true;
	}
	else
	{
		return false;
	}
	return true;
}

bool ThreadBase::getRunState()
{
	return isRunning;
}