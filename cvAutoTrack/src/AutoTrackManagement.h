#pragma once
#include "Core/AutoTrackCore.h"

class AutoTrackManagement {
	AutoTrackCore ATC;
public:
	AutoTrackManagement(void);
	~AutoTrackManagement(void);
	
	/// <summary>
	/// 开启持久化服务，后台将持续解算，以及时获取最新状态数据
	/// </summary>
	/// <returns></returns>
	bool startServe();
	/// <summary>
	/// 停止持久化服务，单独调用获取最新状态数据
	/// </summary>
	/// <returns></returns>
	bool stopServe();

private:
	/// <summary>
	/// 持久化服务
	/// </summary>
	void server();

public:
	/// <summary>
	/// 初始化部分大资源
	/// </summary>
	/// <returns>是否成功</returns>
	bool init();
	bool uninit();
	bool SetHandle(long long int handle);
	bool SetWorldCenter(double x, double y);
	bool SetWorldScale(double scale);
	
	/// <summary>
	/// 获取角色当前坐标和角度
	/// </summary>
	/// <param name="x">角色所在天理坐标系下的X坐标</param>
	/// <param name="y">角色所在天理坐标系下的Y坐标</param>
	/// <param name="a">角色所朝向的角度</param>
	/// <returns>是否成功获取</returns>
	bool GetTransform(double &x, double&y, double&a);

	/// <summary>
	/// 获取角色当前坐标
	/// </summary>
	/// <param name="x">角色所在天理坐标系下的X坐标</param>
	/// <param name="y">角色所在天理坐标系下的Y坐标</param>
	/// <returns>是否成功获取</returns>
	bool GetPosition(double &x, double &y);
	bool GetDirection(double &a);
	bool GetRotation(double &a);
	//获取发现的神瞳坐标,isEnd为真则为当前画面中的最后一个神瞳
	bool GetStar(double &x, double &y, bool &isEnd);
	//获取发现的神瞳坐标，以json字符串格式
	bool GetStarJson(char *jsonBuff);
	bool GetUID(int &uid);
	bool GetInfoLoadPicture(char* path, int &uid, double &x, double &y, double &a);
	bool GetInfoLoadVideo(char * path, char * pathOutFile);

	/// <summary>
	/// 返回最后一个错误码
	/// </summary>
	/// <returns>错误码</returns>
	int GetLastError();
	/// <summary>
	/// 返回错误文字说明
	/// </summary>
	/// <returns>错误码对应文字说明</returns>
	const char* GetLastErrorStr();
};
