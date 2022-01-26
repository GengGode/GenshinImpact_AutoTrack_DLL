#pragma once
#include "src/Logger/LoggingFacility.h"

#include "LoadGiMatchResource.h"
#include "FlowWork.h"
#include "ErrorCode.h"
#include "Kalmanfilter.h"

#include <opencv2/xfeatures2d.hpp>
#include <opencv2/xfeatures2d/nonfree.hpp>

#include "GetPositionClass.h"
#include "GetDirectionClass.h"
class GetTransformClass
{
	Logger logger;
public:
	GetTransformClass()=delete;
	GetTransformClass(const Logger& loggerService);


	bool get(double& x, double& y, double& a);

	GetPositionClass* position;
	GetDirectionClass* direction;
private:
	bool is_init_end = false;
private:
	bool init() { return false; }
};

