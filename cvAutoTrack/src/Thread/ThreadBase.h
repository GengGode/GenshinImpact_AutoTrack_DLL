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
	/// 线程实体智能指针
	/// </summary>
	std::thread* t_ptr;
	/// <summary>
	/// 线程是否正在运行
	/// </summary>
	std::atomic<bool> isRunning=false;
	/// <summary>
	/// 线程内循环执行的工作函数
	/// </summary>
	std::function<void(void)> f_tpr;
	/// <summary>
	/// 是否存在工作函数
	/// </summary>
	std::atomic<bool> isExistFptr = false;

private:
	///// <summary>
	///// 线程内工作函数是否循环执行
	///// </summary>
	//std::atomic<bool> isLoop;

	/// <summary>
	/// 线程是否结束并退出（并非立即退出）
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