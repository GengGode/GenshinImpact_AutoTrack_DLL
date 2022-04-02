#pragma once
#include "src/Logger/LoggingFacility.h"

#include "LoadGiMatchResource.h"
#include "FlowWork.h"
#include "ErrorCode.h"
#include "Kalmanfilter.h"

#include "newclass1.h"


class GetInfoLoadPictureClass
{
	Logger logger;
public:
	GetInfoLoadPictureClass() = delete;
	GetInfoLoadPictureClass(const Logger& loggerService);

	bool get(char* path, int& uid, double& x, double& y, double& a);

private:

	newclass1 test;

	bool is_init_end = false;

	LoadGiMatchResource giMatchResource;
	ErrorCode& err = ErrorCode::getInstance();

	cv::Mat giFrame;
	cv::Mat giPaimonRef;
	cv::Mat giMiniMapRef;
	cv::Mat giUIDRef;
	cv::Mat giAvatarRef;

private:
	bool init() { return false; }
	bool getPaimonRefMat() { return false; }
	bool getMiniMapRefMat() { return false; }
	bool getUIDRefMat() { return false; }
	bool getAvatarRefMat() { return false; }
};

