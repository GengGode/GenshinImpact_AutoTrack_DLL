#include "pch.h"
#include "AutoTrackManagement.h"
#include "ErrorCode.h"

#include <opencv2/xfeatures2d.hpp>
#include <opencv2/xfeatures2d/nonfree.hpp>

#include <Thread/ThreadBase.h>


AutoTrackManagement::AutoTrackManagement(void)
{
	// 创建任务循环线程，生命周期应该为整个运行期
	ATC.pth_handle = new thread(&AutoTrackManagement::server, this);
}

AutoTrackManagement::~AutoTrackManagement(void)
{
	ATC.uninit();

	//应该有一个设置标志位

	//释放线程，此时应该结束了整个生命周期
	ATC.pth_handle->join();
}

bool AutoTrackManagement::startServe()
{
	// 启动各个流程的线程循环
	return ATC.asyncStartService();
}

bool AutoTrackManagement::stopServe()
{
	return ATC.asyncStopService();
}

void AutoTrackManagement::server()
{
	while (ATC.isLoopService)
	{

		if (ATC.isRunService)
		{
			// 判断各个流程跳转的条件，使对应线程工作
			//ATC.screenTask = Task::Run;

			//setTaskCondition()

		}

		// 1.检测句柄
		// 
		// 2.截屏
		// 
		// 3.区域大块划分
		// 
		// 4.大块区域内识别待识别区域
		// 
		// 5.待识别区域划分
		// 
		// 6.识别待识别区域，计算特征
		// 
		// 7.其他操作


		// 观察者方案？（速度最快，高性能开销，需维护多个线程）
		// ：每个任务维护一个线程，时刻循环执行，维护对应变量的最新值，主线程需要时去取对应的最新值。
		// 
		// 轮询方案？（速度略低，但是开销小些）
		// ：每个任务在需要的时候开启一个线程，获取到最新值。
		// 
		// 单线程方案？（速度最慢，低性能开销）
		// 在这一个线程中循环调用每个步骤



	}
}


bool AutoTrackManagement::init()
{
	return ATC.init();
}

bool AutoTrackManagement::uninit()
{
	return ATC.uninit();
}

bool AutoTrackManagement::SetHandle(long long int handle)
{
	return ATC.asyncSetHandle(handle);
}

bool AutoTrackManagement::SetWorldCenter(double x, double y)
{
	return ATC.asyncSetWorldCenter(x, y);
}

bool AutoTrackManagement::SetWorldScale(double scale)
{
	return ATC.asyncSetWorldScale(scale);
}

bool AutoTrackManagement::GetTransform(double& x, double& y, double& a)
{
	return ATC.asyncGetTransform(x, y, a);
}

bool AutoTrackManagement::GetPosition(double& x, double& y)
{
	return ATC.GetPosition(x, y);
}

bool AutoTrackManagement::GetDirection(double& a)
{

	return ATC.GetDirection(a);
}

bool AutoTrackManagement::GetRotation(double& a2)
{
	return ATC.GetRotation(a2);
}

bool AutoTrackManagement::GetStar(double& x, double& y, bool& isEnd)
{
	return ATC.GetStar(x, y, isEnd);

}

bool AutoTrackManagement::GetStarJson(char* jsonBuff)
{
	return ATC.GetStarJson(jsonBuff);
}

bool AutoTrackManagement::GetUID(int& uid)
{
	return ATC.asyncGetUID(uid);
}

bool AutoTrackManagement::GetInfoLoadPicture(char* path, int& uid, double& x, double& y, double& a)
{
	return ATC.asyncGetInfoLoadPicture(path, uid, x, y, a);
}

bool AutoTrackManagement::GetInfoLoadVideo(char* path, char* pathOutFile)
{
	return ATC.asyncGetInfoLoadVideo(path, pathOutFile);
}

int AutoTrackManagement::GetLastError()
{
	return ATC.GetLastError();
}

const char* AutoTrackManagement::GetLastErrorStr()
{
	return ATC.GetLastErrorStr();
}

