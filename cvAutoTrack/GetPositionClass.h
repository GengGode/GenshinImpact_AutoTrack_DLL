#pragma once
#include "src/Logger/LoggingFacility.h"

#include "LoadGiMatchResource.h"
#include "FlowWork.h"
#include "ErrorCode.h"
#include "Kalmanfilter.h"

#include <opencv2/xfeatures2d.hpp>
#include <opencv2/xfeatures2d/nonfree.hpp>

class GetPositionClass
{
	Logger logger;
public:
	GetPositionClass() = delete;
	GetPositionClass(const Logger& loggerService);

	bool get(double &x,double &y);

	bool hisPisContinuity(cv::Point2d* hisP, cv::Mat& img_scene);

	void Func_Match_OnCity(cv::Mat& img_scene, cv::Point2d* hisP, cv::Mat& img_object, cv::Point2d& pos);

	void NewFunctionasdasdasd(cv::Mat& img_scene, cv::Point2d* hisP, cv::Mat& someMap, cv::Ptr<cv::xfeatures2d::SURF>& detectorSomeMap, std::vector<cv::KeyPoint>& KeyPointSomeMap, cv::Mat& DataPointSomeMap);

	void NewFunctionxzccxc(cv::Mat& DataPointMiniMap, cv::Mat& DataPointSomeMap, std::vector<double>& lisx, std::vector<double>& lisy, double& sumx, double& sumy, std::vector<cv::KeyPoint>& KeyPointMiniMap, cv::Mat& minMap, std::vector<cv::KeyPoint>& KeyPointSomeMap, cv::Mat& someMap, cv::Mat& img_object);

	void NewFunctionasd(cv::Mat& DataPointMiniMap, cv::Mat& DataPointSomeMap, std::vector<cv::KeyPoint>& KeyPointMiniMap, std::vector<double>& lisx, cv::Mat& minMap, std::vector<cv::KeyPoint>& KeyPointSomeMap, std::vector<double>& lisy, double& sumx, double& sumy, cv::Mat& someMap, cv::Mat& img_object);

	void Func_Match_NotOnCity(cv::Mat& img_scene, cv::Point2d* hisP, cv::Mat& img_object, cv::Point2d& pos);

	bool Func_Match_NotContinuity(cv::Mat& img_object, cv::Mat& img_scene, cv::Point2d& pos, bool& retflag);

	void ShowMatchKeyPoint(cv::Mat& img_scene, std::vector<cv::KeyPoint>& KeyPointAllMap, cv::Mat& imgmap, cv::Mat& img_object, std::vector<cv::KeyPoint>& KeyPointMiniMap, cv::Mat& imgminmap, std::vector<cv::DMatch>& good_matches, cv::Mat& img_matches);

	void Match_GetGoodMatches(std::vector<std::vector<cv::DMatch>>& KNN_mTmp, std::vector<cv::DMatch>& good_matchesTmp, std::vector<cv::KeyPoint>& KeyPointMiniMap, std::vector<double>& lisx, cv::Mat& minMap, std::vector<cv::KeyPoint>& KeyPointSomeMap, std::vector<double>& lisy, double& sumx, double& sumy, double mapScaleTmp);

	void Match_For_GetOffset(std::vector<double>& lisx, std::vector<double>& lisy, cv::Mat& minMap, cv::Point2f& KeyPointMiniMapPt, cv::Point2f& KeyPointSomeMapPt, double mapScaleTmp);

	void NewFunction_B(std::vector<double>& lisx, std::vector<double>& lisy, double sumx, double sumy);



private:
	FlowWork wForAfter;

	bool is_init_end = false;

	LoadGiMatchResource giMatchResource;

	cv::Mat giFrame;
	cv::Mat giPaimonRef;
	cv::Mat giMiniMapRef;

	bool isOnCity = false;
	bool isContinuity = false;
	bool isConveying = false;
	cv::Point2d _TransformHistory[3];

	int minHessian = 400;
	double ratio_thresh = 0.66;
	double mapScale = 1.3;//1.3;
	int someSizeR = 106;
	//double MatchMatScale = 2.0;

	cv::Ptr<cv::xfeatures2d::SURF> _detectorAllMap;
	cv::Ptr<cv::xfeatures2d::SURF> _detectorSomeMap;
	std::vector<cv::KeyPoint> _KeyPointAllMap;
	std::vector<cv::KeyPoint> _KeyPointSomeMap;
	std::vector<cv::KeyPoint> _KeyPointMiniMap;
	cv::Mat _DataPointAllMap;
	cv::Mat _DataPointSomeMap;
	cv::Mat _DataPointMiniMap;

	Kalmanfilter posFilter;
	//用户定义映射关系参数
	double UserWorldOrigin_X = 0;
	double UserWorldOrigin_Y = 0;
	double UserWorldScale = 1.0;

	// 绝对世界中心 绝对世界缩放系数
	//World Center on AbsAllMap Coor
	double WorldCenter_X = 5352; //Abs
	double WorldCenter_Y = 9432; //Abs
	double WorldScale = 1.0; //Abs


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

	// ???
	double screen_scale = 1;
private:
	bool getPaimonRefMat() { return false; }
	bool getMiniMapRefMat() { return false; }
};

