// cvAutoTrack.cpp : 定义 DLL 的导出函数。
//

#include "pch.h"
#include "cvAutoTrack.h"

#include "AutoTrackManagement.h"

static AutoTrackManagement _AutoTrackManagement;

bool __stdcall startServe()
{
	return _AutoTrackManagement.startServe();
}
bool __stdcall stopServe()
{
	return _AutoTrackManagement.stopServe();
}
bool __stdcall init()
{
	return _AutoTrackManagement.init();
}
bool __stdcall uninit()
{
	return _AutoTrackManagement.uninit();
}
bool __stdcall SetHandle(long long int handle)
{
	return _AutoTrackManagement.SetHandle(handle);
}
bool __stdcall SetWorldCenter(double x, double y)
{
	return _AutoTrackManagement.SetWorldCenter(x, y);
}
bool __stdcall SetWorldScale(double scale)
{
	return _AutoTrackManagement.SetWorldScale(scale);
}
bool __stdcall GetTransform(double &x, double&y, double&a)
{
	return _AutoTrackManagement.GetTransform(x, y, a);
}
bool __stdcall GetPosition(double & x, double & y)
{
	return _AutoTrackManagement.GetPosition(x, y);
}
bool __stdcall GetDirection(double & a)
{
	return _AutoTrackManagement.GetDirection(a);
}
bool __stdcall GetRotation(double & a)
{
	return _AutoTrackManagement.GetRotation(a);
}
bool __stdcall GetStar(double &x, double &y, bool &isEnd)
{
	return _AutoTrackManagement.GetStar(x, y, isEnd);
}
bool GetStarJson(char * jsonBuff)
{
	return _AutoTrackManagement.GetStarJson(jsonBuff);
}
bool __stdcall GetUID(int & uid)
{
	return _AutoTrackManagement.GetUID(uid);
}
bool __stdcall GetInfoLoadPicture(char * path, int & uid, double & x, double & y, double & a)
{
	return _AutoTrackManagement.GetInfoLoadPicture(path, uid, x, y, a);
}
bool __stdcall GetInfoLoadVideo(char * path, char * pathOutFile)
{
	return _AutoTrackManagement.GetInfoLoadVideo(path, pathOutFile);
}
int __stdcall GetLastErr()
{
	return _AutoTrackManagement.GetLastError();
}
const char * __stdcall GetLastErrStr()
{
	return _AutoTrackManagement.GetLastErrorStr();
}
