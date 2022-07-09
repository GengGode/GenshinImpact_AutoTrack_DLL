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


// 此类是不导出的
class AutoTrack {
public:
	AutoTrack(void);
	~AutoTrack(void);


	bool init();
	bool uninit();
	int  GetGpuCount();
	bool SetGpuDevice(int deviceId = 0);
	bool SetHandle(long long int handle = 0);
	bool SetWorldCenter(double x, double y);
	bool SetWorldScale(double scale);
	bool GetTransform(float &x, float &y, float &a);
	bool GetPosition(double &x, double &y);
	bool GetPositionOfMap(double& x, double& y, int& mapId);
	bool GetDirection(double &a);
	bool GetRotation(double &a);
	//获取发现的神瞳坐标,isEnd为真则为当前画面中的最后一个神瞳
	bool GetStar(double &x, double &y, bool &isEnd);
	//获取发现的神瞳坐标，以json字符串格式
	bool GetStarJson(char *jsonBuff);
	bool GetUID(int &uid);
	bool GetInfoLoadPicture(char* path, int &uid, double &x, double &y, double &a);
	bool GetInfoLoadVideo(char * path, char * pathOutFile);
	int GetLastError();
	const char* GetLastErrorStr();
	bool startServe();
	bool stopServe();

private:
	LoadGiMatchResource giMatchResource;
	ErrorCode& err = ErrorCode::getInstance();
	FlowWork wPaimon;
	FlowWork wMiniMap;
	FlowWork wAvatar;
	FlowWork wRotating;
	FlowWork wStar;

	FlowWork wUID;

	FlowWork wForAfter;

	Kalmanfilter posFilter;
private:
	int error_code = 0;

private:
	bool is_init_end = false;

private:

#define MAP_2_1

#ifdef MAP_1_7_1
	int minHessian = 400;
	float ratio_thresh = 0.66f;
	float mapScale = 1.68f;//1.3;
	int someSizeR = 106;
	float MatchMatScale = 2.0;
#endif

#ifdef MAP_1_7_2
	int minHessian = 400;
	float ratio_thresh = 0.66f;
	float mapScale = 1.68f;//1.3;
	int someSizeR = 106;
	float MatchMatScale = 2.0;


	double UserWorldOrigin_X = 0;
	double UserWorldOrigin_Y = 0;
	double UserWorldScale = 1.0;

	//World Center on AbsAllMap Coor
	double WorldCenter_X = 5452;
	double WorldCenter_Y = 9432;
	double WorldScale = 1.0;

	//Map Origin Point on AbsAllMap Coor.
	double MapWorldAbsOrigin_X = 8866;
	double MapWorldAbsOrigin_Y = 11457;

	//Map and AbsAllMap Scale Value, Map * MapAbsScale = AbsAllMap.
	double MapAbsScale = 2;

	cv::Point2d MapWorldOffset = cv::Point2d(3412, 2025);
	//double MapWorldOffset_X = 3412;
	//double MapWorldOffset_Y = 2025;

	double MapWorldScale = 1.0;

#endif

#ifdef MAP_2_0
	int minHessian = 400;
	double ratio_thresh = 0.66;
	double mapScale = 1.3;//1.3;
	int someSizeR = 106;
	double MatchMatScale = 2.0;

	double UserWorldOrigin_X = 0;
	double UserWorldOrigin_Y = 0;
	double UserWorldScale = 1.0;


	//World Center on AbsAllMap Coor
	double WorldCenter_X = 5352;
	double WorldCenter_Y = 9432;
	double WorldScale = 1.0;

	//Map Origin Point on AbsAllMap Coor.
	double MapWorldAbsOrigin_X = 1703;// 1690;
	double MapWorldAbsOrigin_Y = 1924;// 1911;

	//Map and AbsAllMap Scale Value, Map * MapAbsScale = AbsAllMap.
	double MapAbsScale = 2.557;

	cv::Point2d MapWorldOffset = cv::Point2d(3412, 2025);
	//double MapWorldOffset_X = 3412;
	//double MapWorldOffset_Y = 2025;

	double MapWorldScale = 1.0;

#endif

#ifdef MAP_2_1
	int minHessian = 400;
	double ratio_thresh = 0.66;
	double mapScale = 1.3;//1.3;
	int someSizeR = 106;
	double MatchMatScale = 2.0;

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

	//相对绝对空间缩放系数
	//Map and AbsAllMap Scale Value, Map * MapAbsScale = AbsAllMap.
	double MapAbsScale = 2.557; //from diff Image 67.40%

	//忘了是啥了，记得明天推导
	//09.07 是个固定值，不用变
	cv::Point2d MapWorldOffset = cv::Point2d(3412, 2025); // ? forget
	//double MapWorldOffset_X = 3412;
	//double MapWorldOffset_Y = 2025;

	//也忘了
	double MapWorldScale = 1.0;

#endif

	// ???
	double screen_scale = 1;


private:
#ifdef oldValue
	//cv::Ptr<cv::xfeatures2d::SURF>
	void* _detectorAllMap = nullptr;
	//cv::Ptr<cv::xfeatures2d::SURF>
	void* _detectorSomeMap = nullptr;
	//std::vector<cv::KeyPoint>
	void* _KeyPointAllMap = nullptr;
	//std::vector<cv::KeyPoint>
	void* _KeyPointSomeMap = nullptr;
	//std::vector<cv::KeyPoint>
	void* _KeyPointMiniMap = nullptr;
	//cv::Mat
	void* _DataPointAllMap = nullptr;
	//cv::Mat
	void* _DataPointSomeMap = nullptr;
	//cv::Mat
	void* _DataPointMiniMap = nullptr;
#else
	//
	cv::Ptr<cv::xfeatures2d::SURF> _detectorAllMap;
	//
	cv::Ptr<cv::xfeatures2d::SURF> _detectorSomeMap ;
	//
	std::vector<cv::KeyPoint> _KeyPointAllMap;
	//std::vector<cv::KeyPoint>
	std::vector<cv::KeyPoint> _KeyPointSomeMap;
	//std::vector<cv::KeyPoint>
	std::vector<cv::KeyPoint> _KeyPointMiniMap;
	//cv::Mat
	cv::Mat _DataPointAllMap;
	//cv::Mat
	cv::Mat _DataPointSomeMap;
	//cv::Mat
	cv::Mat _DataPointMiniMap;
#endif

private:
	bool isOnCity = false;
	bool isContinuity = false;
	bool isConveying = false;
	cv::Point2d _TransformHistory[3];
	bool is_Auto_getHandle = true;
	bool isStarVisible = false;


private:
	HWND giHandle;
	RECT giRect;
	RECT giClientRect;
	cv::Size giClientSize;
	cv::Mat giFrame;
	cv::Mat giPaimonRef;
	cv::Mat giMiniMapRef;
	cv::Mat giUIDRef;
	cv::Mat giAvatarRef;
	cv::Mat giStarRef;
	
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
