#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/xfeatures2d.hpp>
//#include <opencv2/xfeatures2d/nonfree.hpp>
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

struct Transform {
	bool result;
	double x, y, a;
};
struct Position {
	bool result;
	double x, y;
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

	GetTransformClass _transform;
	GetPositionClass _position;
	GetDirectionClass _direction;
	GetRotationClass _rotation;
	GetStarClass _star;
	GetStarJsonClass _starJson;
	GetUIDClass _uid;
	GetInfoLoadPictureClass _infoLoadPicture;
	GetInfoLoadVideoClass _infoLoadVideo;


	bool init();
	bool uninit();
	bool SetHandle(long long int handle);
	bool SetWorldCenter(double x, double y);
	bool SetWorldScale(double scale);


	bool GetTransform(double& x, double& y, double& a);
	bool GetPosition(double& x, double& y);
	bool GetDirection(double& a);
	bool GetRotation(double& a);
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

	thread* pth_handle;
	std::atomic<bool> isLoopService;
	std::atomic<bool> isRunService;


//
//private:
//	LoadGiMatchResource giMatchResource;
//	ErrorCode& err = ErrorCode::getInstance();
//
//	FlowWork wPaimon;
//	FlowWork wMiniMap;
//	FlowWork wAvatar;
//	FlowWork wRotating;
//	FlowWork wStar;
//
//	FlowWork wUID;
//
//	FlowWork wForAfter;
//
//	Kalmanfilter posFilter;
//private:
//	int error_code = 0;
//
//private:
//	bool is_init_end = false;
//
//private:
//	int minHessian = 400;
//	double ratio_thresh = 0.66;
//	double mapScale = 1.3;//1.3;
//	int someSizeR = 106;
//	double MatchMatScale = 2.0;
//
//	//用户定义映射关系参数
//	double UserWorldOrigin_X = 0;
//	double UserWorldOrigin_Y = 0;
//	double UserWorldScale = 1.0;
//
	// 绝对世界中心 绝对世界缩放系数
	//World Center on AbsAllMap Coor
	double WorldCenter_X = 5352; //Abs
	double WorldCenter_Y = 9432; //Abs
	double WorldScale = 1.0; //Abs

	//相对绝对空间原点坐标
	//Map Origin Point on AbsAllMap Coor.
	double MapWorldAbsOrigin_X = 1703; //from diff Image
	double MapWorldAbsOrigin_Y = 1718; //from diff Image

	////相对绝对空间缩放系数
	////Map and AbsAllMap Scale Value, Map * MapAbsScale = AbsAllMap.
	//double MapAbsScale = 2.557; //from diff Image 67.40%

	//忘了是啥了，记得明天推导
	//09.07 是个固定值，不用变
	cv::Point2d MapWorldOffset = cv::Point2d(3412, 2025); // ? forget
//	//double MapWorldOffset_X = 3412;
//	//double MapWorldOffset_Y = 2025;

	//也忘了
	double MapWorldScale = 1.0;
//
//	// ???
//	double screen_scale = 1;
//
//
//private:
//	//
//	cv::Ptr<cv::xfeatures2d::SURF> _detectorAllMap;
//	//
//	cv::Ptr<cv::xfeatures2d::SURF> _detectorSomeMap;
//	//
//	std::vector<cv::KeyPoint> _KeyPointAllMap;
//	//std::vector<cv::KeyPoint>
//	std::vector<cv::KeyPoint> _KeyPointSomeMap;
//	//std::vector<cv::KeyPoint>
//	std::vector<cv::KeyPoint> _KeyPointMiniMap;
//	//cv::Mat
//	cv::Mat _DataPointAllMap;
//	//cv::Mat
//	cv::Mat _DataPointSomeMap;
//	//cv::Mat
//	cv::Mat _DataPointMiniMap;
//
//private:
//	bool isOnCity = false;
//	bool isContinuity = false;
//	bool isConveying = false;
//	cv::Point2d _TransformHistory[3];
//	bool is_Auto_getHandle = true;
//	bool isStarVisible = false;
//
//
//private:
//	HWND giHandle;
//	RECT giRect;
//	RECT giClientRect;
//	cv::Size giClientSize;
//	cv::Mat giFrame;
//	cv::Mat giPaimonRef;
//	cv::Mat giMiniMapRef;
//	cv::Mat giUIDRef;
//	cv::Mat giAvatarRef;
//	cv::Mat giStarRef;

private:
	bool getAutoTrackIsInit();
	bool getGengshinImpactWnd();
	bool getGengshinImpactRect();
	bool getGengshinImpactScale();
	bool getGengshinImpactScreen();

	bool getPaimonRefMat();
	bool getMiniMapRefMat();
	bool getUIDRefMat();
	bool getAvatarRefMat();


private:

};
