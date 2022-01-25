#ifdef _WIN32
#include <Windows.h>
#include <functional>
#include <thread>
#include <mutex>
#else

#endif // _WiN32


class ThreadBase 
{
	/// <summary>
	/// �߳�ʵ������ָ��
	/// </summary>
	std::thread* t_ptr;
	/// <summary>
	/// �߳��Ƿ���������
	/// </summary>
	std::atomic<bool> isRunning=false;
	/// <summary>
	/// �߳���ѭ��ִ�еĹ�������
	/// </summary>
	std::function<void(void)> f_tpr;
	/// <summary>
	/// �Ƿ���ڹ�������
	/// </summary>
	std::atomic<bool> isExistFptr = false;

private:
	///// <summary>
	///// �߳��ڹ��������Ƿ�ѭ��ִ��
	///// </summary>
	//std::atomic<bool> isLoop;

	/// <summary>
	/// �߳��Ƿ�������˳������������˳���
	/// </summary>
	std::atomic<bool> isExit = false;
public:
	ThreadBase();
	ThreadBase(std::function<void(void)> functionPtr);
	~ThreadBase();
	
 void Run();

	bool setFunctionPtr(std::function<void(void)> functionPtr);
	bool getRunState();
};