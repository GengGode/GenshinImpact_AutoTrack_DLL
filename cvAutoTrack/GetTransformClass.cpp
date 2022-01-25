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
		init();//��ʼ��
	}

	/*
	�ֱ��ж��Ƿ�ɹ���ȡ������ǰһ��error_code����һ��error_code����
	�����±���������false����ʾʧ�ܣ���error_codeΪ0����ʾ�ɹ�����
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

