#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <iostream>
#include <Windows.h>

#include "LoadGiMatchResource.h"
#include "ErrorCode.h"

class util
{
public:
	util(){}
	cv::Ptr<cv::xfeatures2d::SURF> _detectorAllMap;
	std::vector<cv::KeyPoint> _KeyPointAllMap;


private:
	ErrorCode& err = ErrorCode::getInstance();

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

	bool is_Auto_getHandle = true;
	double screen_scale = 1;

	bool is_init_end = false;
	LoadGiMatchResource giMatchResource;
	int minHessian = 400;
	cv::Mat _DataPointAllMap;

public:

	bool init()
	{


		if (!is_init_end)
		{
			giMatchResource.install();

			_detectorAllMap = cv::xfeatures2d::SURF::create(minHessian);
			_detectorAllMap->detectAndCompute(giMatchResource.MapTemplate, cv::noArray(), _KeyPointAllMap, _DataPointAllMap);
			//cv::FileStorage test("", cv::FileStorage::WRITE);


			is_init_end = true;
		}
		return is_init_end;
	}

	bool uninit()
	{


		if (is_init_end)
		{
			_detectorAllMap.release();
			_KeyPointAllMap.resize(0);
			_KeyPointAllMap.reserve(0);
			_DataPointAllMap.release();

			giMatchResource.release();

			is_init_end = false;
		}
		return !is_init_end;
	}

	bool getGengshinImpactWnd()
	{
		if (is_Auto_getHandle)
		{
			LPCWSTR giWindowName = { L"原神" };
			/* 对原神窗口的操作 */
			giWindowName = L"原神";
			giHandle = FindWindowW(L"UnityWndClass", giWindowName);
			if (giHandle == NULL)
			{
				giWindowName = L"Genshin Impact";
				giHandle = FindWindowW(L"UnityWndClass", giWindowName); /* 匹配名称：原神 */
			}
			if (giHandle == NULL)
			{
				giWindowName = L"원신";
				giHandle = FindWindowW(L"UnityWndClass", giWindowName); /* 匹配名称：원신 */
			}
			if (giHandle == NULL)
			{
				giWindowName = L"\u539F\u795E";
				giHandle = FindWindowW(L"UnityWndClass", giWindowName); /* 匹配名称：原神 */
			}
			if (giHandle == NULL)
			{
				giWindowName = L"\uC6D0\uC2E0";
				giHandle = FindWindowW(L"UnityWndClass", giWindowName); /* 匹配名称：원신 */
			}
			//std::cout << "GI 原神 Genshin Impact 원신 Window Name Find is " << L"原神" <<" "<< L"原神" << std::endl;
#ifdef _DEBUG
			//std::cout << "GI 原神 Genshin Impact 원신 Window Name Find is " << giWindowName << std::endl;
			std::cout << "GI 原神 Genshin Impact 원신 Window Handle Find is " << giHandle << std::endl;
#endif
			if (giHandle == NULL)
			{
				err = 10; //无效句柄或指定句柄所指向窗口不存在
				return false;
			}
		}
		else
		{
			if (IsWindow(giHandle))
			{
				return true;
			}
			else
			{
				err = 10; //无效句柄或指定句柄所指向窗口不存在
				return false;
			}
		}

		return (giHandle != NULL ? true : false);
	}

	bool getGengshinImpactRect()
	{

		if (!GetWindowRect(giHandle, &giRect))
		{
			err = 12;//窗口句柄失效
			return false;
		}
		if (!GetClientRect(giHandle, &giClientRect))
		{
			err = 12;//窗口句柄失效
			return false;
		}

		//获取屏幕缩放比例
		getGengshinImpactScale();

		giClientSize.width = (int)(screen_scale * (giClientRect.right - giClientRect.left));
		giClientSize.height = (int)(screen_scale * (giClientRect.bottom - giClientRect.top));


#ifdef _DEBUG
		std::cout << "GI Windows Size: " << giClientSize.width << "," << giClientSize.height << std::endl;
		std::cout << "GI Windows Scale: " << screen_scale << std::endl;
#endif

		return true;
	}

	bool getGengshinImpactScale()
	{
#ifdef _DEBUG
		std::cout << "-> getGengshinImpactScale()" << std::endl;
#endif
		HWND hWnd = GetDesktopWindow();
		HMONITOR hMonitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);

		// 获取监视器逻辑宽度与高度
		MONITORINFOEX miex;
		miex.cbSize = sizeof(miex);
		GetMonitorInfo(hMonitor, &miex);
		int cxLogical = (miex.rcMonitor.right - miex.rcMonitor.left);
		int cyLogical = (miex.rcMonitor.bottom - miex.rcMonitor.top);

		// 获取监视器物理宽度与高度
		DEVMODE dm;
		dm.dmSize = sizeof(dm);
		dm.dmDriverExtra = 0;
		EnumDisplaySettings(miex.szDevice, ENUM_CURRENT_SETTINGS, &dm);
		int cxPhysical = dm.dmPelsWidth;
		int cyPhysical = dm.dmPelsHeight;

		double horzScale = ((double)cxPhysical / (double)cxLogical);
		screen_scale = horzScale;

		return true;
	}


	bool getGengshinImpactScreen()
	{
		static HBITMAP	hBmp;
		BITMAP bmp;

		DeleteObject(hBmp);

		if (giHandle == NULL)
		{
			err = 12;//窗口句柄失效
			return false;
		}
		if (!IsWindow(giHandle))
		{
			err = 12;//窗口句柄失效
			return false;
		}
		//获取目标句柄的窗口大小RECT
		GetWindowRect(giHandle, &giRect);/* 对原神窗口的操作 */

		//获取目标句柄的DC
		HDC hScreen = GetDC(giHandle);/* 对原神窗口的操作 */
		HDC hCompDC = CreateCompatibleDC(hScreen);

		//获取目标句柄的宽度和高度
		int	nWidth = (int)((screen_scale) * (giRect.right - giRect.left));
		int	nHeight = (int)((screen_scale) * (giRect.bottom - giRect.top));

		//创建Bitmap对象
		hBmp = CreateCompatibleBitmap(hScreen, nWidth, nHeight);//得到位图

		SelectObject(hCompDC, hBmp); //不写就全黑
		BitBlt(hCompDC, 0, 0, nWidth, nHeight, hScreen, 0, 0, SRCCOPY);

		//释放对象
		DeleteDC(hScreen);
		DeleteDC(hCompDC);

		//类型转换
		//这里获取位图的大小信息,事实上也是兼容DC绘图输出的范围
		GetObject(hBmp, sizeof(BITMAP), &bmp);

		int nChannels = bmp.bmBitsPixel == 1 ? 1 : bmp.bmBitsPixel / 8;
		int depth = bmp.bmBitsPixel == 1 ? 1 : 8;

		//mat操作
		giFrame.create(cv::Size(bmp.bmWidth, bmp.bmHeight), CV_MAKETYPE(CV_8U, nChannels));

		GetBitmapBits(hBmp, bmp.bmHeight * bmp.bmWidth * nChannels, giFrame.data);

		giFrame = giFrame(cv::Rect(giClientRect.left, giClientRect.top, giClientSize.width, giClientSize.height));


		if (giFrame.empty())
		{
			err = 3;
			return false;
		}

		if (giFrame.cols < 480 || giFrame.rows < 360)
		{
			err = 13;
			return false;
		}
		return true;
	}

	bool getPaimonRefMat()
	{
		int Paimon_Rect_x = cvCeil(giFrame.cols * 0.0135);
		int Paimon_Rect_y = cvCeil(giFrame.cols * 0.006075);
		int Paimon_Rect_w = cvCeil(giFrame.cols * 0.035);
		int Paimon_Rect_h = cvCeil(giFrame.cols * 0.0406);

		if (giFrame.cols == 3440 && giFrame.rows == 1440)
		{
			cv::resize(giFrame, giFrame, cv::Size(2560, 1080));
		}

		if (giFrame.cols == 2560 && giFrame.rows == 1080)
		{
			Paimon_Rect_x = cvCeil(1920 * 0.0135 + 72);
			Paimon_Rect_y = cvCeil(1920 * 0.006075);
			Paimon_Rect_w = cvCeil(1920 * 0.035);
			Paimon_Rect_h = cvCeil(1920 * 0.0406);
		}

		giPaimonRef = giFrame(cv::Rect(Paimon_Rect_x, Paimon_Rect_y, Paimon_Rect_w, Paimon_Rect_h));

#ifdef _DEBUG
		cv::namedWindow("Paimon", cv::WINDOW_FREERATIO);
		cv::imshow("Paimon", giPaimonRef);
		cv::waitKey(1);
		std::cout << "Show Paimon" << std::endl;
#endif
		return true;
	}

	bool getMiniMapRefMat()
	{
		int MiniMap_Rect_x = cvRound(giFrame.cols * 0.033);
		int MiniMap_Rect_y = cvRound(giFrame.cols * 0.01);
		int MiniMap_Rect_w = cvRound(giFrame.cols * 0.11);
		int MiniMap_Rect_h = cvRound(giFrame.cols * 0.11);

		if (giFrame.cols == 3440 && giFrame.rows == 1440)
		{
			cv::resize(giFrame, giFrame, cv::Size(2560, 1080));
		}

		if (giFrame.cols == 2560 && giFrame.rows == 1080)
		{
			MiniMap_Rect_x = cvRound(1920 * 0.033 + 72);
			MiniMap_Rect_y = cvRound(1920 * 0.01);
			MiniMap_Rect_w = cvRound(1920 * 0.11);
			MiniMap_Rect_h = cvRound(1920 * 0.11);
		}

		giMiniMapRef = giFrame(cv::Rect(MiniMap_Rect_x, MiniMap_Rect_y, MiniMap_Rect_w, MiniMap_Rect_h));

#ifdef _DEBUG
		cv::namedWindow("MiniMap", cv::WINDOW_FREERATIO);
		cv::imshow("MiniMap", giMiniMapRef);
		cv::waitKey(1);
		std::cout << "Show MiniMap" << std::endl;
#endif
		return true;
	}

	bool getUIDRefMat()
	{
		int UID_Rect_x = cvCeil(giFrame.cols * 0.875);
		int UID_Rect_y = cvCeil(giFrame.rows * 0.9755);
		int UID_Rect_w = cvCeil(giFrame.cols * 0.0938);
		int UID_Rect_h = cvCeil(UID_Rect_w * 0.11);

		if (giFrame.cols == 3440 && giFrame.rows == 1440)
		{
			cv::resize(giFrame, giFrame, cv::Size(2560, 1080));
		}

		if (giFrame.cols == 2560 && giFrame.rows == 1080)
		{
			UID_Rect_x = cvCeil(1920 * 0.875 + 550);
			UID_Rect_y = cvCeil(1080 * 0.9755);
			UID_Rect_w = cvCeil(1920 * 0.0938);
			UID_Rect_h = cvCeil(UID_Rect_w * 0.11);
		}

		giUIDRef = giFrame(cv::Rect(UID_Rect_x, UID_Rect_y, UID_Rect_w, UID_Rect_h));

#ifdef _DEBUG
		cv::namedWindow("UID", cv::WINDOW_FREERATIO);
		cv::imshow("UID", giUIDRef);
		cv::waitKey(1);
		std::cout << "Show UID" << std::endl;
#endif
		return true;
	}

	bool getAvatarRefMat()
	{
		if (giMiniMapRef.empty())
		{
			err = 21;
			return false;
		}
		int Avatar_Rect_x = cvRound(giMiniMapRef.cols * 0.4);
		int Avatar_Rect_y = cvRound(giMiniMapRef.rows * 0.4);
		int Avatar_Rect_w = cvRound(giMiniMapRef.cols * 0.2);
		int Avatar_Rect_h = cvRound(giMiniMapRef.rows * 0.2);

		giAvatarRef = giMiniMapRef(cv::Rect(Avatar_Rect_x, Avatar_Rect_y, Avatar_Rect_w, Avatar_Rect_h));

#ifdef _DEBUG
		cv::namedWindow("Avatar", cv::WINDOW_FREERATIO);
		cv::imshow("Avatar", giAvatarRef);
		cv::waitKey(1);
		std::cout << "Show Avatar" << std::endl;
#endif
		return true;
	}

};