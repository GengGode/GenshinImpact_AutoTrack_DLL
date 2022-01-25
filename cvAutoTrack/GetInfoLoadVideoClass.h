#pragma once
#include "src/Logger/LoggingFacility.h"

#include "LoadGiMatchResource.h"
#include "FlowWork.h"
#include "ErrorCode.h"
#include "Kalmanfilter.h"

#include <opencv2/xfeatures2d.hpp>
#include <opencv2/xfeatures2d/nonfree.hpp>

class GetInfoLoadVideoClass
{
	Logger logger;
public:
	GetInfoLoadVideoClass() = delete;
	GetInfoLoadVideoClass(const Logger& loggerService);

	bool get(char* path, char* pathOutFile);

private:
	bool is_init_end = false;

	LoadGiMatchResource giMatchResource;
	ErrorCode& err = ErrorCode::getInstance();

	cv::Mat giFrame;
	cv::Mat giPaimonRef;
	cv::Mat giMiniMapRef;
	cv::Mat giUIDRef;
	cv::Mat giAvatarRef;

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
	//�û�����ӳ���ϵ����
	double UserWorldOrigin_X = 0;
	double UserWorldOrigin_Y = 0;
	double UserWorldScale = 1.0;

	// ������������ ������������ϵ��
	//World Center on AbsAllMap Coor
	double WorldCenter_X = 5352; //Abs
	double WorldCenter_Y = 9432; //Abs
	double WorldScale = 1.0; //Abs


	//��Ծ��Կռ�����ϵ��
	//Map and AbsAllMap Scale Value, Map * MapAbsScale = AbsAllMap.
	double MapAbsScale = 2.557; //from diff Image 67.40%

	//������ɶ�ˣ��ǵ������Ƶ�
	//09.07 �Ǹ��̶�ֵ�����ñ�
	cv::Point2d MapWorldOffset = cv::Point2d(3412, 2025); // ? forget
	//double MapWorldOffset_X = 3412;
	//double MapWorldOffset_Y = 2025;

	//Ҳ����
	double MapWorldScale = 1.0;

	// ???
	double screen_scale = 1;
private:
	void init() {}
	bool getPaimonRefMat() {}
	bool getMiniMapRefMat(){}
	bool getUIDRefMat() {}
	bool getAvatarRefMat() {}
};


