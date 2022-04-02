#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <iostream>
#include <Windows.h>

#include "LoadGiMatchResource.h"
#include "FlowWork.h"
#include "ErrorCode.h"
#include "Kalmanfilter.h"

#include "../Logger/LoggingFacility.h"
#include "../Logger/MatchLogger.h"

#include "../../GetPositionClass.h"
#include "../../GetTransformClass.h"
#include "../../GetDirectionClass.h"
#include "../../GetRotationClass.h"
#include "../../GetStarClass.h"
#include "../../GetStarJsonClass.h"
#include "../../GetUIDClass.h"
#include "../../GetInfoLoadPictureClass.h"
#include "../../GetInfoLoadVideoClass.h"

#include "../../util.h"



struct Transform {
	bool result;
	double x, y, a;
};
struct Position {
	bool result;
	double x, y;
};
struct Direction {
	bool result;
	double a;
};
struct Rotation {
	bool result;
	double a2;
};
struct Star {
	bool result;
	double x, y;
	bool isEnd;
};
struct StarJson {
	bool result;
	char* jsonBuff;
};
struct UID {
	bool result;
	int uid;
};
struct InfoLoadPicture {
	bool result;
	char* path;
	int uid;
	double x, y, a;
};
struct InfoLoadVideo {
	bool result;
	char* path;
	char* pathOutFile;
};

class AutoTrackCore {
private:
	Logger logger;
	Logger Matchlog = std::make_shared<MatchLogger>();

public:
	AutoTrackCore(void);
	~AutoTrackCore(void);

	bool asyncStartService();

	bool asyncStopService();


	bool asyncSetHandle(long long int handle);
	bool asyncSetWorldCenter(double x, double y);
	bool asyncSetWorldScale(double scale);

	bool asyncGetTransform(double& x, double& y, double& a);
	bool asyncGetPosition(double& x, double& y);
	bool asyncGetDirection(double& a);
	bool asyncGetRotation(double& a);
	bool asyncGetStar(double& x, double& y, bool& isEnd);
	bool asyncGetStarJson(char* jsonBuff);
	bool asyncGetUID(int& uid);
	bool asyncGetInfoLoadPicture(char* path, int& uid, double& x, double& y, double& a);
	bool asyncGetInfoLoadVideo(char* path, char* pathOutFile);

	GetTransformClass* _transform;
	GetPositionClass* _position;
	GetDirectionClass* _direction;
	GetRotationClass* _rotation;
	GetStarClass* _star;
	GetStarJsonClass* _starJson;
	GetUIDClass* _uid;
	GetInfoLoadPictureClass* _infoLoadPicture;
	GetInfoLoadVideoClass* _infoLoadVideo;
	util _u;
	
	bool init();
	bool uninit();

	bool SetHandle(long long int handle);
	bool SetWorldCenter(double x, double y);
	bool SetWorldScale(double scale);


	bool GetTransform(double& x, double& y, double& a);
	bool GetPosition(double& x, double& y);
	bool GetDirection(double& a);
	bool GetRotation(double& a2);
	bool GetStar(double& x, double& y, bool& isEnd);
	bool GetStarJson(char* jsonBuff);
	bool GetUID(int& uid);
	bool GetInfoLoadPicture(char* path, int& uid, double& x, double& y, double& a);
	bool GetInfoLoadVideo(char* path, char* pathOutFile);
	
	int GetLastError();
	const char* GetLastErrorStr();


public:
	Transform TransformData;
	Position PositionData;
	Direction DirectionData;
	Rotation RotationData;
	Star StarData;
	StarJson StarJsonData;
	UID UIDData;
	InfoLoadPicture InfoLoadPictureData;
	InfoLoadVideo InfoLoadVideoData;






	thread* pth_handle;
	std::atomic<bool> isLoopService;
	std::atomic<bool> isRunService;

private:
	ErrorCode& err = ErrorCode::getInstance();
private:
	bool is_init_end = false;

private:
	//用户定义映射关系参数
	double UserWorldOrigin_X = 0;
	double UserWorldOrigin_Y = 0;
	double UserWorldScale = 1.0;

	// 绝对世界中心 绝对世界缩放系数
	//World Center on AbsAllMap Coor
	double WorldCenter_X = 5352; //Abs
	double WorldCenter_Y = 9432; //Abs
	double WorldScale = 1.0; //Abs

	//相对绝对空间原点坐标
	//Map Origin Point on AbsAllMap Coor.
	double MapWorldAbsOrigin_X = 1703; //from diff Image
	double MapWorldAbsOrigin_Y = 1718; //from diff Image

	//忘了是啥了，记得明天推导
	//09.07 是个固定值，不用变
	cv::Point2d MapWorldOffset = cv::Point2d(3412, 2025); // ? forget
	double MapWorldScale = 1.0;
private:
	bool is_Auto_getHandle = true;
private:
	HWND giHandle;

};
