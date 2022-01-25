#include "pch.h"
#include "GetTransformClass.h"

#include <opencv2/xfeatures2d.hpp>
#include <opencv2/xfeatures2d/nonfree.hpp>

GetTransformClass::GetTransformClass(const Logger& loggerService)
{
	logger = loggerService;
}

bool GetTransformClass::get(double& x, double& y, double& a)
{
	double x2 = 0, y2 = 0, a2 = 0;
	if (!is_init_end)
	{
		init();//初始化
	}

	/*
	分别判断是否成功获取，避免前一个error_code被后一个error_code覆盖
	而导致本函数返回false（表示失败）但error_code为0（表示成功）。
	*/
	if (!position->get(x2, y2))
	{
		return false;
	}
	if (!direction->get(a2))
	{
		return false;
	}
	x = x2;
	y = y2;
	a = a2;
	return true;
}

