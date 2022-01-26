#include "pch.h"
#include "GetPositionClass.h"
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/xfeatures2d/nonfree.hpp>

GetPositionClass::GetPositionClass(const Logger& loggerService)
{
	logger = loggerService;

	//wForAfter.append(this, &AutoTrackCore::getGengshinImpactWnd, 101);
	//wForAfter.append(this, &AutoTrackCore::getGengshinImpactRect, 102);
	//wForAfter.append(this, &AutoTrackCore::getGengshinImpactScreen, 103);
}

bool GetPositionClass::get(double& x, double& y)
{
	if (wForAfter.run() == false)
	{
		return false;
	}

	if (!is_init_end)
	{
		//err = 1;
		//logger.Error("1");
		return false;
	}


	getPaimonRefMat();

	cv::Mat paimonTemplate;

	cv::resize(giMatchResource.PaimonTemplate, paimonTemplate, giPaimonRef.size());

	cv::Mat tmp;


	giPaimonRef = giFrame(cv::Rect(0, 0, cvCeil(giFrame.cols / 10), cvCeil(giFrame.rows / 10)));



#ifdef _DEBUG
	cv::namedWindow("test", cv::WINDOW_FREERATIO);
	cv::imshow("test", giPaimonRef);
#endif


	std::vector<cv::Mat> lisT, lisR;
	cv::split(paimonTemplate, lisT);
	cv::split(giPaimonRef, lisR);

	cv::Mat Template, Ref;
	cv::cvtColor(paimonTemplate, Template, cv::COLOR_RGBA2GRAY);
	cv::cvtColor(giPaimonRef, Ref, cv::COLOR_RGBA2GRAY);

	//cv::matchTemplate(paimonTemplate, giPaimonRef, tmp, cv::TM_CCOEFF_NORMED);
	//cv::matchTemplate(Template, Ref, tmp, cv::TM_CCOEFF_NORMED);
	cv::matchTemplate(lisT[3], lisR[3], tmp, cv::TM_CCOEFF_NORMED);

	double minVal, maxVal;
	cv::Point minLoc, maxLoc;
	cv::minMaxLoc(tmp, &minVal, &maxVal, &minLoc, &maxLoc);

#ifdef _DEBUG
	cv::namedWindow("test2", cv::WINDOW_FREERATIO);
	cv::imshow("test2", tmp);
	//logger.log("Paimon Match: " + minVal + "," + maxVal);
#endif

	if (maxVal < 0.36 || maxVal == 1)
	{
		//err = 6;//未能匹配到派蒙
		return false;
	}

	getMiniMapRefMat();

	cv::Mat img_scene(giMatchResource.MapTemplate);
	cv::Mat img_object(giMiniMapRef(cv::Rect(30, 30, giMiniMapRef.cols - 60, giMiniMapRef.rows - 60)));

	cv::cvtColor(img_scene, img_scene, cv::COLOR_RGBA2RGB);

	if (img_object.empty())
	{
		//err = 5;//原神小地图区域为空或者区域长宽小于60px
		return false;
	}

	isContinuity = false;

	cv::Point2d* hisP = _TransformHistory;

	cv::Point2d pos;

	if (hisPisContinuity(hisP, img_scene))
	{
		isContinuity = true;
		if (isContinuity)
		{
			if (isOnCity == false)
			{
				Func_Match_OnCity(img_scene, hisP, img_object, pos);
			}
			else
			{
				Func_Match_NotOnCity(img_scene, hisP, img_object, pos);
			}
		}

	}
	else
	{
		isConveying = true;
	}

	if (!isContinuity)
	{

		bool retflag;
		bool retval = Func_Match_NotContinuity(img_object, img_scene, pos, retflag);

		if (retflag) return retval;
	}

	if (isConveying)
	{
		pos = posFilter.reinitfilterting(pos);
	}
	else
	{
		pos = posFilter.filterting(pos);
	}

	hisP[0] = hisP[1];
	hisP[1] = hisP[2];
	hisP[2] = pos;

	pos = TransferTianLiAxes(pos * MapAbsScale, MapWorldOffset, MapWorldScale);

	pos = TransferUserAxes(pos, UserWorldOrigin_X, UserWorldOrigin_Y, UserWorldScale);

	x = (pos.x);
	y = (pos.y);

	//err = 0;
	return true;
}

bool GetPositionClass::hisPisContinuity(cv::Point2d* hisP, cv::Mat& img_scene)
{
	return (dis(hisP[2] - hisP[1]) < 1000) && (hisP[2].x > someSizeR && hisP[2].x < img_scene.cols - someSizeR && hisP[2].y>someSizeR && hisP[2].y < img_scene.rows - someSizeR);
}

void GetPositionClass::Func_Match_OnCity(cv::Mat& img_scene, cv::Point2d* hisP, cv::Mat& img_object, cv::Point2d& pos)
{
	cv::Mat someMap(img_scene(cv::Rect(cvRound(hisP[2].x - someSizeR), cvRound(hisP[2].y - someSizeR), cvRound(someSizeR * 2), cvRound(someSizeR * 2))));
	cv::Mat minMap(img_object);

	cv::Ptr<cv::xfeatures2d::SURF>& detectorSomeMap = _detectorSomeMap;
	std::vector<cv::KeyPoint>& KeyPointSomeMap = _KeyPointSomeMap;
	cv::Mat& DataPointSomeMap = _DataPointSomeMap;
	std::vector<cv::KeyPoint>& KeyPointMiniMap = _KeyPointMiniMap;
	cv::Mat& DataPointMiniMap = _DataPointMiniMap;

	detectorSomeMap = cv::xfeatures2d::SURF::create(minHessian);
	detectorSomeMap->detectAndCompute(someMap, cv::noArray(), KeyPointSomeMap, DataPointSomeMap);
	detectorSomeMap->detectAndCompute(minMap, cv::noArray(), KeyPointMiniMap, DataPointMiniMap);

	std::vector<double> lisx;
	std::vector<double> lisy;
	double sumx = 0;
	double sumy = 0;

	if (KeyPointMiniMap.size() == 0 || KeyPointSomeMap.size() == 0)
	{
		isContinuity = false;
	}
	else
	{
		NewFunctionasd(DataPointMiniMap, DataPointSomeMap, KeyPointMiniMap, lisx, minMap, KeyPointSomeMap, lisy, sumx, sumy, someMap, img_object);


		if (min(lisx.size(), lisy.size()) <= 4)
		{
			NewFunctionasdasdasd(img_scene, hisP, someMap, detectorSomeMap, KeyPointSomeMap, DataPointSomeMap);
			if (KeyPointSomeMap.size() == 0 || KeyPointMiniMap.size() == 0)
			{
				isContinuity = false;
			}
			else
			{
				NewFunctionxzccxc(DataPointMiniMap, DataPointSomeMap, lisx, lisy, sumx, sumy, KeyPointMiniMap, minMap, KeyPointSomeMap, someMap, img_object);
				if (min(lisx.size(), lisy.size()) <= 4)
				{
					isContinuity = false;
				}
				else
				{
					if (min(lisx.size(), lisy.size()) >= 10)
					{
						isOnCity = true;
					}
					else
					{
						isOnCity = false;
					}

					double meanx = sumx / lisx.size(); //均值
					double meany = sumy / lisy.size(); //均值
					cv::Point2d p = SPC(lisx, sumx, lisy, sumy);

					//int x = (int)meanx;
					//int y = (int)meany;

					double x = (p.x - someMap.cols / 2.0) / 2.0;
					double y = (p.y - someMap.rows / 2.0) / 2.0;

					pos = cv::Point2d(x + hisP[2].x, y + hisP[2].y);
				}

			}
		}
		else
		{
			isOnCity = false;

			double meanx = sumx / lisx.size(); //均值
			double meany = sumy / lisy.size(); //均值
			cv::Point2d p = SPC(lisx, sumx, lisy, sumy);


			double x = p.x;
			double y = p.y;

			pos = cv::Point2d(x + hisP[2].x - someSizeR, y + hisP[2].y - someSizeR);
		}
	}
}

void GetPositionClass::NewFunctionasdasdasd(cv::Mat& img_scene, cv::Point2d* hisP, cv::Mat& someMap, cv::Ptr<cv::xfeatures2d::SURF>& detectorSomeMap, std::vector<cv::KeyPoint>& KeyPointSomeMap, cv::Mat& DataPointSomeMap)
{
	//有可能处于城镇中

	/***********************/
	//重新从完整中地图取出角色周围部分地图
	img_scene(cv::Rect(cvCeil(hisP[2].x - someSizeR), cvCeil(hisP[2].y - someSizeR), someSizeR * 2, someSizeR * 2)).copyTo(someMap);
	//Mat minMap(img_object);

	resize(someMap, someMap, cv::Size(someSizeR * 4, someSizeR * 4));
	//resize(minMap, minMap, Size(), MatchMatScale, MatchMatScale, 1);

	detectorSomeMap = cv::xfeatures2d::SURF::create(minHessian);
	detectorSomeMap->detectAndCompute(someMap, cv::noArray(), KeyPointSomeMap, DataPointSomeMap);
	//detectorSomeMap->detectAndCompute(minMap, noArray(), KeyPointMinMap, DataPointMinMap);
}

void GetPositionClass::NewFunctionxzccxc(cv::Mat& DataPointMiniMap, cv::Mat& DataPointSomeMap, std::vector<double>& lisx, std::vector<double>& lisy, double& sumx, double& sumy, std::vector<cv::KeyPoint>& KeyPointMiniMap, cv::Mat& minMap, std::vector<cv::KeyPoint>& KeyPointSomeMap, cv::Mat& someMap, cv::Mat& img_object)
{
	cv::Ptr<cv::DescriptorMatcher> matcherTmp = cv::DescriptorMatcher::create(cv::DescriptorMatcher::FLANNBASED);
	std::vector< std::vector<cv::DMatch> > KNN_mTmp;
#ifdef _DEBUG
	std::vector<cv::DMatch> good_matchesTmp;
#endif
	matcherTmp->knnMatch(DataPointMiniMap, DataPointSomeMap, KNN_mTmp, 2);
	//std::vector<double> lisx;
	//std::vector<double> lisy;
	lisx.clear();
	lisy.clear();
	//double sumx = 0;
	//double sumy = 0;
	sumx = 0;
	sumy = 0;

	Match_GetGoodMatches(KNN_mTmp, good_matchesTmp, KeyPointMiniMap, lisx, minMap, KeyPointSomeMap, lisy, sumx, sumy, 0.8667);
#ifdef _DEBUG
	cv::Mat img_matches, imgmap, imgminmap;
	ShowMatchKeyPoint(someMap, KeyPointSomeMap, imgmap, img_object, KeyPointMiniMap, imgminmap, good_matchesTmp, img_matches);
#endif
}

void GetPositionClass::NewFunctionasd(cv::Mat& DataPointMiniMap, cv::Mat& DataPointSomeMap, std::vector<cv::KeyPoint>& KeyPointMiniMap, std::vector<double>& lisx, cv::Mat& minMap, std::vector<cv::KeyPoint>& KeyPointSomeMap, std::vector<double>& lisy, double& sumx, double& sumy, cv::Mat& someMap, cv::Mat& img_object)
{
	cv::Ptr<cv::DescriptorMatcher> matcherTmp = cv::DescriptorMatcher::create(cv::DescriptorMatcher::FLANNBASED);
	std::vector< std::vector<cv::DMatch> > KNN_mTmp;
#ifdef _DEBUG
	std::vector<cv::DMatch> good_matchesTmp;
#endif
	matcherTmp->knnMatch(DataPointMiniMap, DataPointSomeMap, KNN_mTmp, 2);

	Match_GetGoodMatches(KNN_mTmp, good_matchesTmp, KeyPointMiniMap, lisx, minMap, KeyPointSomeMap, lisy, sumx, sumy, mapScale);
#ifdef _DEBUG
	cv::Mat img_matches, imgmap, imgminmap;
	ShowMatchKeyPoint(someMap, KeyPointSomeMap, imgmap, img_object, KeyPointMiniMap, imgminmap, good_matchesTmp, img_matches);
#endif
}

void GetPositionClass::Func_Match_NotOnCity(cv::Mat& img_scene, cv::Point2d* hisP, cv::Mat& img_object, cv::Point2d& pos)
{
	//在城镇中
	/***********************/
	//重新从完整中地图取出角色周围部分地图
	cv::Mat someMap(img_scene(cv::Rect(cvCeil(hisP[2].x - someSizeR), cvCeil(hisP[2].y - someSizeR), someSizeR * 2, someSizeR * 2)));
	cv::Mat minMap(img_object);

	cv::Ptr<cv::xfeatures2d::SURF>& detectorSomeMap = _detectorSomeMap;
	std::vector<cv::KeyPoint>& KeyPointSomeMap = _KeyPointSomeMap;
	cv::Mat& DataPointSomeMap = _DataPointSomeMap;
	std::vector<cv::KeyPoint>& KeyPointMiniMap = _KeyPointMiniMap;
	cv::Mat& DataPointMiniMap = _DataPointMiniMap;


	resize(someMap, someMap, cv::Size(someSizeR * 4, someSizeR * 4));
	//resize(minMap, minMap, Size(), MatchMatScale, MatchMatScale, 1);

	detectorSomeMap = cv::xfeatures2d::SURF::create(minHessian);
	detectorSomeMap->detectAndCompute(someMap, cv::noArray(), KeyPointSomeMap, DataPointSomeMap);
	detectorSomeMap->detectAndCompute(minMap, cv::noArray(), KeyPointMiniMap, DataPointMiniMap);

	if (KeyPointSomeMap.size() != 0 && KeyPointMiniMap.size() != 0)
	{
		cv::Ptr<cv::DescriptorMatcher> matcherTmp = cv::DescriptorMatcher::create(cv::DescriptorMatcher::FLANNBASED);
		std::vector< std::vector<cv::DMatch> > KNN_mTmp;
#ifdef _DEBUG
		std::vector<cv::DMatch> good_matchesTmp;
#endif
		KNN_mTmp.clear();
		matcherTmp->knnMatch(DataPointMiniMap, DataPointSomeMap, KNN_mTmp, 2);
		std::vector<double> lisx;
		std::vector<double> lisy;
		double sumx = 0;
		double sumy = 0;

		Match_GetGoodMatches(KNN_mTmp, good_matchesTmp, KeyPointMiniMap, lisx, minMap, KeyPointSomeMap, lisy, sumx, sumy, 0.8667);
#ifdef _DEBUG
		cv::Mat img_matches, imgmap, imgminmap;
		ShowMatchKeyPoint(someMap, KeyPointSomeMap, imgmap, img_object, KeyPointMiniMap, imgminmap, good_matchesTmp, img_matches);
#endif
		if (max(lisx.size(), lisy.size()) > 4)
		{
			NewFunction_B(lisx, lisy, sumx, sumy);

			cv::Point2d p = SPC(lisx, sumx, lisy, sumy);

			double x = (p.x - someMap.cols / 2.0) / 2.0;
			double y = (p.y - someMap.rows / 2.0) / 2.0;

			pos = cv::Point2d(x + hisP[2].x, y + hisP[2].y);
		}
		else
		{
			isContinuity = false;
		}//if (max(lisx.size(), lisy.size()) > 4)
	}
	else
	{
		isContinuity = false;
	}//if (KeyPointSomeMap.size() != 0 && KeyPointMinMap.size() != 0)
}

/// <summary>
/// 匹配，非连续情况，即位置位置
/// </summary>
/// <param name="img_object"></param>
/// <param name="img_scene"></param>
/// <param name="pos"></param>
/// <param name="retflag"></param>
/// <returns></returns>
bool GetPositionClass::Func_Match_NotContinuity(cv::Mat& img_object, cv::Mat& img_scene, cv::Point2d& pos, bool& retflag)
{
	retflag = true;
	cv::Ptr<cv::xfeatures2d::SURF>& detectorAllMap = _detectorAllMap;
	std::vector<cv::KeyPoint>& KeyPointAllMap = _KeyPointAllMap;
	cv::Mat& DataPointAllMap = _DataPointAllMap;
	std::vector<cv::KeyPoint>& KeyPointMiniMap = _KeyPointMiniMap;
	cv::Mat& DataPointMiniMap = _DataPointMiniMap;

	detectorAllMap->detectAndCompute(img_object, cv::noArray(), KeyPointMiniMap, DataPointMiniMap);

	if (KeyPointMiniMap.size() == 0)
	{
		//err = 4;//未能匹配到特征点
		return false;
	}
	else
	{
		cv::Ptr<cv::DescriptorMatcher> matcher = cv::DescriptorMatcher::create(cv::DescriptorMatcher::FLANNBASED);
		std::vector< std::vector<cv::DMatch> > KNN_m;
#ifdef _DEBUG
		std::vector<cv::DMatch> good_matches;
#endif
		matcher->knnMatch(DataPointMiniMap, DataPointAllMap, KNN_m, 2);

		std::vector<double> lisx;
		std::vector<double> lisy;
		double sumx = 0;
		double sumy = 0;

		Match_GetGoodMatches(KNN_m, good_matches, KeyPointMiniMap, lisx, img_object, KeyPointAllMap, lisy, sumx, sumy, mapScale);

#ifdef _DEBUG
		cv::Mat img_matches, imgmap, imgminmap;
		ShowMatchKeyPoint(img_scene, KeyPointAllMap, imgmap, img_object, KeyPointMiniMap, imgminmap, good_matches, img_matches);
#endif

		if (lisx.size() == 0 || lisy.size() == 0)
		{
			//err = 4;//未能匹配到特征点
			return false;
		}
		else
		{
			pos = SPC(lisx, sumx, lisy, sumy);
		}
	}
	retflag = false;
	return {};
}



void GetPositionClass::NewFunction_B(std::vector<double>& lisx, std::vector<double>& lisy, double sumx, double sumy)
{
	if (min(lisx.size(), lisy.size()) >= 10)
	{
		isOnCity = true;
	}
	else
	{
		isOnCity = false;
	}

	double meanx = sumx / lisx.size(); //均值
	double meany = sumy / lisy.size(); //均值
}

/// <summary>
/// 获取最佳的匹配点，以及计算坐标偏移
/// </summary>
/// <param name="KNN_m"></param>
/// <param name="good_matches"></param>
/// <param name="KeyPointMiniMap"></param>
/// <param name="lisx"></param>
/// <param name="minMap"></param>
/// <param name="KeyPointSomeMap"></param>
/// <param name="lisy"></param>
/// <param name="sumx"></param>
/// <param name="sumy"></param>
/// <param name="mapScaleTmp"></param>
void GetPositionClass::Match_GetGoodMatches(std::vector<std::vector<cv::DMatch>>& KNN_m, std::vector<cv::DMatch>& good_matches, std::vector<cv::KeyPoint>& KeyPointMiniMap, std::vector<double>& lisx, cv::Mat& minMap, std::vector<cv::KeyPoint>& KeyPointSomeMap, std::vector<double>& lisy, double& sumx, double& sumy, double mapScaleTmp)
{
	//长时间运行概率触发 有未经处理的异常: Microsoft C++ 异常: cv::Exception，位于内存位置 
	for (size_t i = 0; i < KNN_m.size(); i++)
	{
		if (KNN_m[i][0].distance < ratio_thresh * KNN_m[i][1].distance)
		{
#ifdef _DEBUG
			good_matches.push_back(KNN_m[i][0]);
#endif
			// 这里有个bug回卡进来，进入副本或者切换放大招时偶尔触发
			if (KNN_m[i][0].queryIdx >= KeyPointMiniMap.size())
			{
				continue;
			}

			Match_For_GetOffset(lisx, lisy, minMap,  KeyPointMiniMap[KNN_m[i][0].queryIdx].pt, KeyPointSomeMap[KNN_m[i][0].trainIdx].pt, mapScaleTmp);
			sumx += lisx.back();
			sumy += lisy.back();
		}
	}
}

/// <summary>
/// 计算偏移
/// </summary>
/// <param name="lisx">x偏移列表</param>
/// <param name="lisy">y偏移列表</param>
/// <param name="minMap"></param>
/// <param name="KeyPointMiniMapPt"></param>
/// <param name="KeyPointSomeMapPt"></param>
/// <param name="mapScaleTmp">缩放因子</param>
void GetPositionClass::Match_For_GetOffset(std::vector<double>& lisx, std::vector<double>& lisy, cv::Mat& minMap, cv::Point2f& KeyPointMiniMapPt, cv::Point2f& KeyPointSomeMapPt, double mapScaleTmp)
{
	lisx.push_back(((minMap.cols / 2.0 - KeyPointMiniMapPt.x) * mapScaleTmp + KeyPointSomeMapPt.x));
	lisy.push_back(((minMap.rows / 2.0 - KeyPointMiniMapPt.y) * mapScaleTmp + KeyPointSomeMapPt.y));
}


/// <summary>
/// 绘制显示关键点匹配图
/// </summary>
/// <param name="img_scene"></param>
/// <param name="KeyPointAllMap"></param>
/// <param name="imgmap"></param>
/// <param name="img_object"></param>
/// <param name="KeyPointMiniMap"></param>
/// <param name="imgminmap"></param>
/// <param name="good_matches"></param>
/// <param name="img_matches"></param>
void GetPositionClass::ShowMatchKeyPoint(cv::Mat& img_scene, std::vector<cv::KeyPoint>& KeyPointAllMap, cv::Mat& imgmap, cv::Mat& img_object, std::vector<cv::KeyPoint>& KeyPointMiniMap, cv::Mat& imgminmap, std::vector<cv::DMatch>& good_matches, cv::Mat& img_matches)
{
	drawKeypoints(img_scene, KeyPointAllMap, imgmap, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
	drawKeypoints(img_object, KeyPointMiniMap, imgminmap, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
	drawMatches(img_object, KeyPointMiniMap, img_scene, KeyPointAllMap, good_matches, img_matches, cv::Scalar::all(-1), cv::Scalar::all(-1), std::vector<char>(), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
}
