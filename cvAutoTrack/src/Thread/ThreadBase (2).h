#pragma once
#include <thread>
#include <functional>
//#include <Windows.h>
using namespace std;

class Mat
{

};
template<class cOut, class cIn>
class ThreadBase
{
	thread* ptrThread = nullptr;

	Mat workInput;
	function<cOut(cIn)> ptr= nullptr;
	bool isExitThread = false;
	bool isExistFunction = false;
	bool isRunWork = false;
	bool isEndWork = false;

	void run();

public:
	bool isInit = false;
	ThreadBase();
	~ThreadBase();
	ThreadBase(void* (*funPtr)(void* inMat));
	void setFunction(void* (*funPtr)(void*));

	void start(Mat& inMat);
	bool isEnd();

};


