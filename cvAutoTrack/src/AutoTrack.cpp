#include "pch.h"
#include "AutoTrack.h"
#include "ErrorCode.h"

#include <opencv2/xfeatures2d.hpp>
#include <opencv2/xfeatures2d/nonfree.hpp>

AutoTrack::AutoTrack()
{

	MapWorldOffset.x = MapWorldAbsOrigin_X - WorldCenter_X;
	MapWorldOffset.y = MapWorldAbsOrigin_Y - WorldCenter_Y;
	MapWorldScale = WorldScale;

	wForAfter.append(this, &AutoTrack::getGengshinImpactWnd, 101);
	wForAfter.append(this, &AutoTrack::getGengshinImpactRect, 102);
	wForAfter.append(this, &AutoTrack::getGengshinImpactScreen, 103);

	wPaimon.append(this, &AutoTrack::getGengshinImpactWnd, 104);
	wPaimon.append(this, &AutoTrack::getGengshinImpactRect, 105);
	wPaimon.append(this, &AutoTrack::getGengshinImpactScreen, 106);
	wPaimon.append(this, &AutoTrack::getPaimonRefMat, 107);

	wMiniMap.append(this, &AutoTrack::getAutoTrackIsInit, 108);
	wMiniMap.append(this, &AutoTrack::getGengshinImpactWnd, 109);
	wMiniMap.append(this, &AutoTrack::getGengshinImpactRect, 110);
	wMiniMap.append(this, &AutoTrack::getGengshinImpactScreen, 111);
	wMiniMap.append(this, &AutoTrack::getMiniMapRefMat, 112);

	wAvatar.append(this, &AutoTrack::getGengshinImpactWnd, 113);
	wAvatar.append(this, &AutoTrack::getGengshinImpactRect, 114);
	wAvatar.append(this, &AutoTrack::getGengshinImpactScreen, 115);
	wAvatar.append(this, &AutoTrack::getAvatarRefMat, 116);

	
	wRotating.append(this, &AutoTrack::getGengshinImpactWnd, 117);
	wRotating.append(this, &AutoTrack::getGengshinImpactRect, 118);
	wRotating.append(this, &AutoTrack::getGengshinImpactScreen, 119);
	wRotating.append(this, &AutoTrack::getMiniMapRefMat, 120);

	wStar.append(this, &AutoTrack::getGengshinImpactWnd, 121);
	wStar.append(this, &AutoTrack::getGengshinImpactRect, 122);
	wStar.append(this, &AutoTrack::getGengshinImpactScreen, 123);
	wStar.append(this, &AutoTrack::getMiniMapRefMat, 124);

	wUID.append(this, &AutoTrack::getGengshinImpactWnd, 125);
	wUID.append(this, &AutoTrack::getGengshinImpactRect, 126);
	wUID.append(this, &AutoTrack::getGengshinImpactScreen, 127);
	wUID.append(this, &AutoTrack::getUIDRefMat, 128);

}

AutoTrack::~AutoTrack(void)
{
}

bool AutoTrack::init()
{
	if (!is_init_end)
	{
		giMatchResource.install();

		_detectorAllMap = cv::xfeatures2d::SURF::create(minHessian);
		_detectorAllMap->detectAndCompute(giMatchResource.MapTemplate, cv::noArray(), _KeyPointAllMap, _DataPointAllMap);

		is_init_end = true;
	}
	return is_init_end;
}

bool AutoTrack::uninit()
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


bool AutoTrack::SetHandle(long long int handle)
{
	if (handle == 0)
	{
		is_Auto_getHandle = true;
		return true;
	}
	else
	{
		is_Auto_getHandle = false;
		giHandle = (HWND)handle;
	}

	// uwp
	return true;// IsWindow(giHandle);
}

bool AutoTrack::SetWorldCenter(double x, double y)
{
	UserWorldOrigin_X = x;
	UserWorldOrigin_Y = y;
	return true;
}

bool AutoTrack::SetWorldScale(double scale)
{
	UserWorldScale = scale;
	return true;
}

bool AutoTrack::startServe()
{
	return false;
}

bool AutoTrack::stopServe()
{
	return false;
}

bool AutoTrack::GetTransform(float & x, float & y, float & a)
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
	if (!GetPosition(x2, y2))
	{
		return false;
	}
	if (!GetDirection(a2))
	{
		return false;
	}
	x = (float)x2;
	y = (float)y2;
	a = (float)a2;
	return true;
}

bool AutoTrack::GetPosition(double & x, double & y)
{

	if (wForAfter.run() == false)
	{
		return false;
	}
	if (!is_init_end)
	{
		err = 1;
		return false;
	}


	getPaimonRefMat();

	cv::Mat paimonTemplate;

	cv::resize(giMatchResource.PaimonTemplate, paimonTemplate, giPaimonRef.size());

	cv::Mat tmp;

#define Mode1

#ifdef Mode1
	giPaimonRef = giFrame(cv::Rect(0, 0, cvCeil(giFrame.cols / 10), cvCeil(giFrame.rows / 10)));
#endif // Mode1

#ifdef _DEBUG

#ifdef Mode2
	cv::Ptr<cv::xfeatures2d::SURF> detectorPaimon = cv::xfeatures2d::SURF::create(minHessian);
	std::vector<cv::KeyPoint> KeyPointPaimonTemplate, KeyPointPaimonRef;
	cv::Mat DataPointPaimonTemplate, DataPointPaimonRef;

	detectorPaimon->detectAndCompute(giPaimonRef, cv::noArray(), KeyPointPaimonRef, DataPointPaimonRef);
	detectorPaimon->detectAndCompute(paimonTemplate, cv::noArray(), KeyPointPaimonTemplate, DataPointPaimonTemplate);
	cv::Ptr<cv::DescriptorMatcher> matcherPaimonTmp = cv::DescriptorMatcher::create(cv::DescriptorMatcher::FLANNBASED);
	std::vector< std::vector<cv::DMatch> > KNN_PaimonmTmp;
	std::vector<cv::DMatch> good_matchesPaimonTmp;

	for (size_t i = 0; i < KNN_PaimonmTmp.size(); i++)
	{
		if (KNN_PaimonmTmp[i][0].distance < ratio_thresh * KNN_PaimonmTmp[i][1].distance)
		{
			good_matchesPaimonTmp.push_back(KNN_PaimonmTmp[i][0]);
		}
	}

	cv::Mat img_matchesA, imgmapA, imgminmapA;
	drawKeypoints(giPaimonRef, KeyPointPaimonRef, imgmapA, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
	drawKeypoints(paimonTemplate, KeyPointPaimonTemplate, imgminmapA, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
	drawMatches(paimonTemplate, KeyPointPaimonTemplate, giPaimonRef, KeyPointPaimonRef, good_matchesPaimonTmp, img_matchesA, cv::Scalar::all(-1), cv::Scalar::all(-1), std::vector<char>(), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

	cv::imshow("asda", img_matchesA);

	if (good_matchesPaimonTmp.size() < 7)
	{
		err = 6;//未能匹配到派蒙
		return false;
	}
#endif // Mode2

#endif

#ifdef _DEBUG
	cv::namedWindow("test", cv::WINDOW_FREERATIO);
	cv::imshow("test", giPaimonRef);
#endif

#ifdef Mode1

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
	std::cout << "Paimon Match: " << minVal << "," << maxVal << std::endl;
#endif

	if (maxVal < 0.36 || maxVal == 1)
	{
		err = 6;//未能匹配到派蒙
		return false;
	}
#endif

	getMiniMapRefMat();

	cv::Mat img_scene(giMatchResource.MapTemplate);
	cv::Mat img_object(giMiniMapRef(cv::Rect(30, 30, giMiniMapRef.cols - 60, giMiniMapRef.rows - 60)));

	cv::cvtColor(img_scene, img_scene, cv::COLOR_RGBA2RGB);

	if (img_object.empty())
	{
		err = 5;//原神小地图区域为空或者区域长宽小于60px
		return false;
	}

	isContinuity = false;

	cv::Point2d *hisP = _TransformHistory;

	cv::Point2d pos;

	if (dis(hisP[2] - hisP[1]) < 1000)
	{
		if (hisP[2].x > someSizeR && hisP[2].x < img_scene.cols - someSizeR && hisP[2].y>someSizeR && hisP[2].y < img_scene.rows - someSizeR)
		{
			isContinuity = true;
			if (isContinuity)
			{
				if (isOnCity == false)
				{
					cv::Mat someMap(img_scene(cv::Rect(cvRound(hisP[2].x - someSizeR), cvRound(hisP[2].y - someSizeR), cvRound(someSizeR * 2), cvRound(someSizeR * 2))));
					cv::Mat minMap(img_object);

					//resize(someMap, someMap, Size(), MatchMatScale, MatchMatScale, 1);
					//resize(minMap, minMap, Size(), MatchMatScale, MatchMatScale, 1);

					//cv::Ptr<cv::xfeatures2d::SURF>& detectorSomeMap = *(cv::Ptr<cv::xfeatures2d::SURF>*)_detectorSomeMap;
					//std::vector<cv::KeyPoint>& KeyPointSomeMap = *(std::vector<cv::KeyPoint>*)_KeyPointSomeMap;
					//cv::Mat& DataPointSomeMap = *(cv::Mat*)_DataPointSomeMap;
					//std::vector<cv::KeyPoint>& KeyPointMiniMap = *(std::vector<cv::KeyPoint>*)_KeyPointMiniMap;
					//cv::Mat& DataPointMiniMap = *(cv::Mat*)_DataPointMiniMap;

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
						cv::Ptr<cv::DescriptorMatcher> matcherTmp = cv::DescriptorMatcher::create(cv::DescriptorMatcher::FLANNBASED);
						std::vector< std::vector<cv::DMatch> > KNN_mTmp;
#ifdef _DEBUG
						std::vector<cv::DMatch> good_matchesTmp;
#endif
						matcherTmp->knnMatch(DataPointMiniMap, DataPointSomeMap, KNN_mTmp, 2);

						//长时间运行概率触发 有未经处理的异常: Microsoft C++ 异常: cv::Exception，位于内存位置 
						for (size_t i = 0; i < KNN_mTmp.size(); i++)
						{
							if (KNN_mTmp[i][0].distance < ratio_thresh * KNN_mTmp[i][1].distance)
							{
#ifdef _DEBUG
								good_matchesTmp.push_back(KNN_mTmp[i][0]);
#endif
								if (KNN_mTmp[i][0].queryIdx >= KeyPointMiniMap.size())
								{
									continue;
								}
								// 这里有个bug回卡进来，进入副本或者切换放大招时偶尔触发
								lisx.push_back(((minMap.cols / 2.0 - KeyPointMiniMap[KNN_mTmp[i][0].queryIdx].pt.x)*mapScale + KeyPointSomeMap[KNN_mTmp[i][0].trainIdx].pt.x));
								lisy.push_back(((minMap.rows / 2.0 - KeyPointMiniMap[KNN_mTmp[i][0].queryIdx].pt.y)*mapScale + KeyPointSomeMap[KNN_mTmp[i][0].trainIdx].pt.y));

								sumx += lisx.back();
								sumy += lisy.back();
							}
						}
#ifdef _DEBUG
						cv::Mat img_matches, imgmap, imgminmap;
						drawKeypoints(someMap, KeyPointSomeMap, imgmap, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
						drawKeypoints(img_object, KeyPointMiniMap, imgminmap, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
						drawMatches(img_object, KeyPointMiniMap, someMap, KeyPointSomeMap, good_matchesTmp, img_matches, cv::Scalar::all(-1), cv::Scalar::all(-1), std::vector<char>(), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
#endif
						if (min(lisx.size(), lisy.size()) <= 4)
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
							if (KeyPointSomeMap.size() == 0 || KeyPointMiniMap.size() == 0)
							{
								isContinuity = false;
							}
							else
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

								for (size_t i = 0; i < KNN_mTmp.size(); i++)
								{
									if (KNN_mTmp[i][0].distance < ratio_thresh * KNN_mTmp[i][1].distance)
									{
#ifdef _DEBUG
										good_matchesTmp.push_back(KNN_mTmp[i][0]);
#endif
										// 这里有个bug回卡进来，进入副本或者切换放大招时偶尔触发
										if (KNN_mTmp[i][0].queryIdx >= KeyPointMiniMap.size())
										{
											continue;
										}

										lisx.push_back(((minMap.cols / 2.0 - KeyPointMiniMap[KNN_mTmp[i][0].queryIdx].pt.x)*0.8667 + KeyPointSomeMap[KNN_mTmp[i][0].trainIdx].pt.x));
										lisy.push_back(((minMap.rows / 2.0 - KeyPointMiniMap[KNN_mTmp[i][0].queryIdx].pt.y)*0.8667 + KeyPointSomeMap[KNN_mTmp[i][0].trainIdx].pt.y));
										sumx += lisx.back();
										sumy += lisy.back();
									}
								}
#ifdef _DEBUG
								//Mat img_matches, imgmap, imgminmap;
								drawKeypoints(someMap, KeyPointSomeMap, imgmap, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
								drawKeypoints(img_object, KeyPointMiniMap, imgminmap, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

								drawMatches(img_object, KeyPointMiniMap, someMap, KeyPointSomeMap, good_matchesTmp, img_matches, cv::Scalar::all(-1), cv::Scalar::all(-1), std::vector<char>(), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
#endif
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
				else
				{
					//在城镇中
						/***********************/
						//重新从完整中地图取出角色周围部分地图
					cv::Mat someMap(img_scene(cv::Rect(cvCeil(hisP[2].x - someSizeR), cvCeil(hisP[2].y - someSizeR), someSizeR * 2, someSizeR * 2)));
					cv::Mat minMap(img_object);

					//cv::Ptr<cv::xfeatures2d::SURF>& detectorSomeMap = *(cv::Ptr<cv::xfeatures2d::SURF>*)_detectorSomeMap;
					//std::vector<cv::KeyPoint>& KeyPointSomeMap = *(std::vector<cv::KeyPoint>*)_KeyPointSomeMap;
					//cv::Mat& DataPointSomeMap = *(cv::Mat*)_DataPointSomeMap;
					//std::vector<cv::KeyPoint>& KeyPointMiniMap = *(std::vector<cv::KeyPoint>*)_KeyPointMiniMap;
					//cv::Mat& DataPointMiniMap = *(cv::Mat*)_DataPointMiniMap;

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

						for (size_t i = 0; i < KNN_mTmp.size(); i++)
						{
							if (KNN_mTmp[i][0].distance < ratio_thresh * KNN_mTmp[i][1].distance)
							{
#ifdef _DEBUG
								good_matchesTmp.push_back(KNN_mTmp[i][0]);
#endif
								// 这里有个bug回卡进来，进入副本或者切换放大招时偶尔触发
								if (KNN_mTmp[i][0].queryIdx >= KeyPointMiniMap.size())
								{
									continue;
								}

								lisx.push_back(((minMap.cols / 2.0 - KeyPointMiniMap[KNN_mTmp[i][0].queryIdx].pt.x)*0.8667 + KeyPointSomeMap[KNN_mTmp[i][0].trainIdx].pt.x));
								lisy.push_back(((minMap.rows / 2.0 - KeyPointMiniMap[KNN_mTmp[i][0].queryIdx].pt.y)*0.8667 + KeyPointSomeMap[KNN_mTmp[i][0].trainIdx].pt.y));
								sumx += lisx.back();
								sumy += lisy.back();
							}
						}
#ifdef _DEBUG
						cv::Mat img_matches, imgmap, imgminmap;
						drawKeypoints(someMap, KeyPointSomeMap, imgmap, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
						drawKeypoints(img_object, KeyPointMiniMap, imgminmap, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

						drawMatches(img_object, KeyPointMiniMap, someMap, KeyPointSomeMap, good_matchesTmp, img_matches, cv::Scalar::all(-1), cv::Scalar::all(-1), std::vector<char>(), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
#endif
						if (max(lisx.size(), lisy.size()) > 4)
						{
							if (min(lisx.size(), lisy.size()) >= 10)
							{
								isOnCity = true;
							}
							else
							{
								isOnCity = false;
							}
							//if(max(lisx.size(), lisy.size()) <=10 )
							//{
							//	isOnCity = false;
							//}

							double meanx = sumx / lisx.size(); //均值
							double meany = sumy / lisy.size(); //均值
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
			}
		}
	}
	else
	{
		isConveying = true;
	}
	if (!isContinuity)
	{
		//cv::Ptr<cv::xfeatures2d::SURF>& detectorAllMap = *(cv::Ptr<cv::xfeatures2d::SURF>*)_detectorAllMap;
		//std::vector<cv::KeyPoint>& KeyPointAllMap = *(std::vector<cv::KeyPoint>*)_KeyPointAllMap;
		//cv::Mat& DataPointAllMap = *(cv::Mat*)_DataPointAllMap;
		//std::vector<cv::KeyPoint>& KeyPointMiniMap = *(std::vector<cv::KeyPoint>*)_KeyPointMiniMap;
		//cv::Mat& DataPointMiniMap = *(cv::Mat*)_DataPointMiniMap;

		cv::Ptr<cv::xfeatures2d::SURF>& detectorAllMap = _detectorAllMap;
		std::vector<cv::KeyPoint>& KeyPointAllMap = _KeyPointAllMap;
		cv::Mat& DataPointAllMap = _DataPointAllMap;
		std::vector<cv::KeyPoint>& KeyPointMiniMap = _KeyPointMiniMap;
		cv::Mat& DataPointMiniMap = _DataPointMiniMap;

		detectorAllMap->detectAndCompute(img_object, cv::noArray(), KeyPointMiniMap, DataPointMiniMap);
		
		if (KeyPointMiniMap.size() == 0)
		{
			err = 4;//未能匹配到特征点
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
			for (size_t i = 0; i < KNN_m.size(); i++)
			{
				if (KNN_m[i][0].distance < ratio_thresh * KNN_m[i][1].distance)
				{
#ifdef _DEBUG
					good_matches.push_back(KNN_m[i][0]);
#endif
					if (KNN_m[i][0].queryIdx >= KeyPointMiniMap.size())
					{
						continue;
					}
					lisx.push_back(((img_object.cols / 2.0 - KeyPointMiniMap[KNN_m[i][0].queryIdx].pt.x)*mapScale + KeyPointAllMap[KNN_m[i][0].trainIdx].pt.x));
					lisy.push_back(((img_object.rows / 2.0 - KeyPointMiniMap[KNN_m[i][0].queryIdx].pt.y)*mapScale + KeyPointAllMap[KNN_m[i][0].trainIdx].pt.y));
					sumx += lisx.back();
					sumy += lisy.back();
				}
			}
#ifdef _DEBUG
			cv::Mat img_matches, imgmap, imgminmap;
			drawKeypoints(img_scene, KeyPointAllMap, imgmap, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
			drawKeypoints(img_object, KeyPointMiniMap, imgminmap, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

			drawMatches(img_object, KeyPointMiniMap, img_scene, KeyPointAllMap, good_matches, img_matches, cv::Scalar::all(-1), cv::Scalar::all(-1), std::vector<char>(), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
#endif

			if (lisx.size() == 0 || lisy.size() == 0)
			{
				err = 4;//未能匹配到特征点
				return false;
			}
			else
			{
				pos = SPC(lisx, sumx, lisy, sumy);
			}
		}
	}
	
	if (isConveying)
	{
		pos = posFilter.reinitfilterting(pos);
		//isConveying = false;
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

	x = (float)(pos.x);
	y = (float)(pos.y);

	err = 0;
	return true;
}

bool AutoTrack::GetDirection(double & a)
{
	if (wAvatar.run() == false)
	{
		return false;
	}

	getPaimonRefMat();

	cv::Mat paimonTemplate;

	cv::resize(giMatchResource.PaimonTemplate, paimonTemplate, giPaimonRef.size());

	cv::Mat tmp;

	giPaimonRef = giFrame(cv::Rect(0, 0, cvCeil(giFrame.cols / 20), cvCeil(giFrame.rows / 10)));

	cv::matchTemplate(paimonTemplate, giPaimonRef, tmp, cv::TM_CCOEFF_NORMED);

	double minVal, maxVal;
	cv::Point minLoc, maxLoc;
	cv::minMaxLoc(tmp, &minVal, &maxVal, &minLoc, &maxLoc);

	if (maxVal < 0.36 || maxVal == 1)
	{
		err = 6;//未能匹配到派蒙
		return false;
	}

	getMiniMapRefMat();

	if (giMiniMapRef.empty())
	{
		err = 5;//原神小地图区域为空或者区域长宽小于60px
		return false;
	}

	getAvatarRefMat();

	if (giAvatarRef.empty())
	{
		err = 11;//未能取到小箭头区域
		return false;
	}

	cv::resize(giAvatarRef, giAvatarRef, cv::Size(), 2, 2);
	std::vector<cv::Mat> lis;
	cv::split(giAvatarRef, lis);

	cv::Mat gray0;
	cv::Mat gray1;
	cv::Mat gray2;

	cv::threshold(lis[0], gray0, 240, 255, cv::THRESH_BINARY);
	cv::threshold(lis[1], gray1, 212, 255, cv::THRESH_BINARY);
	cv::threshold(lis[2], gray2, 25, 255, cv::THRESH_BINARY_INV);

	cv::Mat and12;
	cv::bitwise_and(gray1, gray2, and12, gray0);
	cv::resize(and12, and12, cv::Size(), 2, 2, 3);
	cv::Canny(and12, and12, 20, 3 * 20, 3);
	cv::circle(and12, cv::Point(cvCeil(and12.cols / 2), cvCeil(and12.rows / 2)), cvCeil(and12.cols / 4.5), cv::Scalar(0, 0, 0), -1);
	cv::Mat dilate_element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2, 2));
	cv::dilate(and12, and12, dilate_element);
	cv::Mat erode_element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2, 2));
	cv::erode(and12, and12, erode_element);

	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarcy;

	cv::findContours(and12, contours, hierarcy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);

	std::vector<cv::Rect> boundRect(contours.size());  //定义外接矩形集合
	cv::Point2f rect[4];

	std::vector<cv::Point2d> AvatarKeyPoint;
	double AvatarKeyPointLine[3] = { 0 };
	std::vector<cv::Point2f> AvatarKeyLine;
	cv::Point2f KeyLine;

	if (contours.size() != 3)
	{
		err = 9;//提取小箭头特征误差过大
		return false;
	}

	for (int i = 0; i < 3; i++)
	{
		boundRect[i] = cv::boundingRect(cv::Mat(contours[i]));
		AvatarKeyPoint.push_back(cv::Point(cvRound(boundRect[i].x + boundRect[i].width / 2), cvRound(boundRect[i].y + boundRect[i].height / 2)));
	}

	AvatarKeyPointLine[0] = dis(AvatarKeyPoint[2] - AvatarKeyPoint[1]);
	AvatarKeyPointLine[1] = dis(AvatarKeyPoint[2] - AvatarKeyPoint[0]);
	AvatarKeyPointLine[2] = dis(AvatarKeyPoint[1] - AvatarKeyPoint[0]);

	if (AvatarKeyPointLine[0] >= AvatarKeyPointLine[2] && AvatarKeyPointLine[1] >= AvatarKeyPointLine[2])
	{
		AvatarKeyLine.push_back(AvatarKeyPoint[2] - AvatarKeyPoint[1]);
		AvatarKeyLine.push_back(AvatarKeyPoint[2] - AvatarKeyPoint[0]);
	}
	if (AvatarKeyPointLine[0] >= AvatarKeyPointLine[1] && AvatarKeyPointLine[2] >= AvatarKeyPointLine[1])
	{
		AvatarKeyLine.push_back(AvatarKeyPoint[1] - AvatarKeyPoint[0]);
		AvatarKeyLine.push_back(AvatarKeyPoint[1] - AvatarKeyPoint[2]);
	}
	if (AvatarKeyPointLine[1] >= AvatarKeyPointLine[0] && AvatarKeyPointLine[2] >= AvatarKeyPointLine[0])
	{
		AvatarKeyLine.push_back(AvatarKeyPoint[0] - AvatarKeyPoint[1]);
		AvatarKeyLine.push_back(AvatarKeyPoint[0] - AvatarKeyPoint[2]);
	}

	AvatarKeyLine = Vector2UnitVector(AvatarKeyLine);
	KeyLine = AvatarKeyLine[0] + AvatarKeyLine[1];

	a = Line2Angle(KeyLine);

	err = 0;
	return true;
}

bool AutoTrack::GetRotation(double & a)
{
	if (wForAfter.run() == false)
	{
		return false;
	}
	getPaimonRefMat();

	cv::Mat paimonTemplate;

	cv::resize(giMatchResource.PaimonTemplate, paimonTemplate, giPaimonRef.size());

	cv::Mat tmp;

#define Mode1

#ifdef Mode1
	giPaimonRef = giFrame(cv::Rect(0, 0, cvCeil(giFrame.cols / 10), cvCeil(giFrame.rows / 10)));
#endif // Mode1

#ifdef _DEBUG

#ifdef Mode2
	cv::Ptr<cv::xfeatures2d::SURF> detectorPaimon = cv::xfeatures2d::SURF::create(minHessian);
	std::vector<cv::KeyPoint> KeyPointPaimonTemplate, KeyPointPaimonRef;
	cv::Mat DataPointPaimonTemplate, DataPointPaimonRef;

	detectorPaimon->detectAndCompute(giPaimonRef, cv::noArray(), KeyPointPaimonRef, DataPointPaimonRef);
	detectorPaimon->detectAndCompute(paimonTemplate, cv::noArray(), KeyPointPaimonTemplate, DataPointPaimonTemplate);
	cv::Ptr<cv::DescriptorMatcher> matcherPaimonTmp = cv::DescriptorMatcher::create(cv::DescriptorMatcher::FLANNBASED);
	std::vector< std::vector<cv::DMatch> > KNN_PaimonmTmp;
	std::vector<cv::DMatch> good_matchesPaimonTmp;

	for (size_t i = 0; i < KNN_PaimonmTmp.size(); i++)
	{
		if (KNN_PaimonmTmp[i][0].distance < ratio_thresh * KNN_PaimonmTmp[i][1].distance)
		{
			good_matchesPaimonTmp.push_back(KNN_PaimonmTmp[i][0]);
		}
	}

	cv::Mat img_matchesA, imgmapA, imgminmapA;
	drawKeypoints(giPaimonRef, KeyPointPaimonRef, imgmapA, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
	drawKeypoints(paimonTemplate, KeyPointPaimonTemplate, imgminmapA, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
	drawMatches(paimonTemplate, KeyPointPaimonTemplate, giPaimonRef, KeyPointPaimonRef, good_matchesPaimonTmp, img_matchesA, cv::Scalar::all(-1), cv::Scalar::all(-1), std::vector<char>(), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

	cv::imshow("asda", img_matchesA);

	if (good_matchesPaimonTmp.size() < 7)
	{
		err = 6;//未能匹配到派蒙
		return false;
	}
#endif // Mode2

#endif

#ifdef _DEBUG
	cv::namedWindow("test", cv::WINDOW_FREERATIO);
	cv::imshow("test", giPaimonRef);
#endif

#ifdef Mode1

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
	std::cout << "Paimon Match: " << minVal << "," << maxVal << std::endl;
#endif

	if (maxVal < 0.36 || maxVal == 1)
	{
		err = 6;//未能匹配到派蒙
		return false;
	}
#endif

	getMiniMapRefMat();

	//cv::Mat img_scene(giMatchResource.MapTemplate);
	cv::Mat img_object(giMiniMapRef(cv::Rect(40, 40, giMiniMapRef.cols - 80, giMiniMapRef.rows - 80)));

	if (img_object.channels() != 4)
	{
		err = 401;
		return false;
	}

	std::vector<cv::Mat>scr_channels;

	split(img_object, scr_channels);

	cv::Mat Alpha = scr_channels[3];

	Alpha = 255.0 - Alpha;

	Alpha = Alpha * 2;

	cv::threshold(Alpha, Alpha, 150, 0, cv::THRESH_TOZERO_INV);
	cv::threshold(Alpha, Alpha, 50, 0, cv::THRESH_TOZERO);
	cv::threshold(Alpha, Alpha, 50, 255, cv::THRESH_BINARY);

	cv::circle(Alpha, cv::Point(Alpha.cols / 2, Alpha.rows / 2), min(Alpha.cols / 2, Alpha.rows / 2)*1.21, cv::Scalar(0, 0, 0), min(Alpha.cols / 2, Alpha.rows / 2)*0.42);
	cv::circle(Alpha, cv::Point(Alpha.cols / 2, Alpha.rows / 2), min(Alpha.cols / 2, Alpha.rows / 2)*0.3, cv::Scalar(0, 0, 0),-1);


	cv::Mat dilate_element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(4, 4));
	cv::dilate(Alpha, Alpha, dilate_element);
	cv::Mat erode_element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(4, 4));
	cv::erode(Alpha, Alpha, erode_element);

	erode_element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(4, 4));
	cv::erode(Alpha, Alpha, erode_element);
	dilate_element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(4, 4));
	cv::dilate(Alpha, Alpha, dilate_element);


	//传入黑白图
	//根据白块部分计算视角中心坐标
	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarcy;

	cv::findContours(Alpha, contours, hierarcy, 0, 1);

	std::vector<cv::Rect> boundRect(contours.size());  //定义外接矩形集合

	if (contours.size() == 0)
	{
		err = 402;//
		return false;
	}

	cv::Point p;
	int maxBlack = 0;
	int maxId = 0;

	for (int i = 0; i < contours.size(); i++)
	{
		if (contours[i].size() > maxBlack)
		{
			maxBlack = contours[i].size();
			maxId = i;
		}
		boundRect[i] = cv::boundingRect(cv::Mat(contours[i]));
		
	}

	p = cv::Point(boundRect[maxId].x + boundRect[maxId].width / 2, boundRect[maxId].y + boundRect[maxId].height / 2);



	double res;
#ifdef _DEBUG
	circle(Alpha, p, 3, cv::Scalar(255, 0, 0));
	line(Alpha, p, cv::Point(img_object.cols / 2, img_object.rows / 2), cv::Scalar(0, 255, 0));
	cv::imshow("Img", Alpha);
#endif
	p = p - cv::Point(img_object.cols / 2, img_object.rows / 2);

	a = Line2Angle(p);

	return true;
}

bool AutoTrack::GetStar(double & x, double & y, bool & isEnd)
{
	static bool isNotSee = false;
	static vector<cv::Point2d> pos;
	static int seeId=0;

	int MAXLOOP = 0;
	bool isLoopMatch = false;
	cv::Mat tmp;
	double minVal, maxVal;
	cv::Point minLoc, maxLoc;
	double scale = 1.3;

	if(isNotSee)
	{
		if (isOnCity)
		{
			scale = 0.8667;
		}
		x = pos[seeId].x*scale;
		y = pos[seeId].y*scale;
		seeId++;
		if (seeId == pos.size())
		{
			isEnd = true;
			isNotSee=false;
		}
		else
		{
			isEnd=false;
			isNotSee = true;
		}
		return true;
	}
	else
	{
		pos.clear();
		seeId = 0;

		if (wStar.run() == false)
		{
			return false;
		}

		getPaimonRefMat();

		cv::cvtColor(giMiniMapRef(cv::Rect(36, 36, giMiniMapRef.cols - 72, giMiniMapRef.rows - 72)),
			giStarRef, cv::COLOR_RGBA2GRAY);


		matchTemplate(giMatchResource.StarTemplate, giStarRef, tmp, cv::TM_CCOEFF_NORMED);
		minMaxLoc(tmp, &minVal, &maxVal, &minLoc, &maxLoc);
#ifdef _DEBUG
		cout << "Match Star MinVal & MaxVal : " << minVal << " , " << maxVal << endl;
#endif
		if (maxVal < 0.66)
		{
			isStarVisible = false;
		}
		else
		{
			isLoopMatch = true;
			isStarVisible = true;
			pos.push_back(cv::Point2d(maxLoc) -
				cv::Point2d(giStarRef.cols / 2, giStarRef.rows / 2) +
				cv::Point2d(giMatchResource.StarTemplate.cols / 2, giMatchResource.StarTemplate.rows / 2));
		}

		while (isLoopMatch)
		{
			giStarRef(cv::Rect(maxLoc.x, maxLoc.y, giMatchResource.StarTemplate.cols, giMatchResource.StarTemplate.rows)) = cv::Scalar(0, 0, 0);
			matchTemplate(giMatchResource.StarTemplate, giStarRef, tmp, cv::TM_CCOEFF_NORMED);
			minMaxLoc(tmp, &minVal, &maxVal, &minLoc, &maxLoc);
#ifdef _DEBUG
			cout << "Match Star MinVal & MaxVal : " << minVal << " , " << maxVal << endl;
#endif

			if (maxVal < 0.66)
			{
				isLoopMatch = false;
			}
			else
			{
				pos.push_back(cv::Point2d(maxLoc) -
					cv::Point2d(giStarRef.cols / 2, giStarRef.rows / 2) +
					cv::Point2d(giMatchResource.StarTemplate.cols / 2, giMatchResource.StarTemplate.rows / 2));
			}

			MAXLOOP > 10 ? isLoopMatch = false : MAXLOOP++;
		}
		

		if (isStarVisible == true)
		{
			if (isOnCity)
			{
				scale = 0.8667;
			}
			x = pos[seeId].x*scale;
			y = pos[seeId].y*scale;
			seeId++;
			if (seeId == pos.size())
			{
				isEnd = true;
				isNotSee = false;
			}
			else
			{
				isEnd = false;
				isNotSee=true;
			}
			return true;
		}
		err = 601;
		return false;
	}

	return false;
}

bool AutoTrack::GetStarJson(char * jsonBuff)
{
	int MAXLOOP = 0;
	bool isLoopMatch = false;
	cv::Mat tmp;
	double minVal, maxVal;
	cv::Point minLoc, maxLoc;
	vector<cv::Point2d> pos;
	double scale = 1.3;


	if (wStar.run() == false)
	{
		return false;
	}

	getPaimonRefMat();

	//一个bug 未开游戏而先开应用，开游戏时触发
	cv::cvtColor(giMiniMapRef(cv::Rect(36, 36, giMiniMapRef.cols - 72, giMiniMapRef.rows - 72)),
		giStarRef, cv::COLOR_RGBA2GRAY);


	matchTemplate(giMatchResource.StarTemplate, giStarRef, tmp, cv::TM_CCOEFF_NORMED);
	minMaxLoc(tmp, &minVal, &maxVal, &minLoc, &maxLoc);
#ifdef _DEBUG
	cout << "Match Star MinVal & MaxVal : " << minVal << " , " << maxVal << endl;
#endif
	if (maxVal < 0.66)
	{
		isStarVisible = false;
	}
	else
	{
		isLoopMatch = true;
		isStarVisible = true;
		pos.push_back(cv::Point2d(maxLoc) - 
			cv::Point2d(giStarRef.cols / 2, giStarRef.rows / 2) + 
			cv::Point2d(giMatchResource.StarTemplate.cols / 2, giMatchResource.StarTemplate.rows / 2));
	}

	while (isLoopMatch)
	{
		giStarRef(cv::Rect(maxLoc.x, maxLoc.y, giMatchResource.StarTemplate.cols, giMatchResource.StarTemplate.rows)) = cv::Scalar(0, 0, 0);
		matchTemplate(giMatchResource.StarTemplate, giStarRef, tmp, cv::TM_CCOEFF_NORMED);
		minMaxLoc(tmp, &minVal, &maxVal, &minLoc, &maxLoc);
#ifdef _DEBUG
		cout << "Match Star MinVal & MaxVal : " << minVal << " , " << maxVal << endl;
#endif
		if (maxVal < 0.66)
		{
			isLoopMatch = false;
		}
		else
		{
			pos.push_back(cv::Point2d(maxLoc) - 
				cv::Point2d(giStarRef.cols / 2, giStarRef.rows / 2) + 
				cv::Point2d(giMatchResource.StarTemplate.cols / 2, giMatchResource.StarTemplate.rows / 2));
		}

		MAXLOOP > 10 ? isLoopMatch = false : MAXLOOP++;
	}

	if (isOnCity)
	{
		scale = 0.8667;
	}

	if (isStarVisible == true)
	{
		
		sprintf_s(jsonBuff,1024,"{\"n\": %d ,\"list\":[",static_cast<int>(pos.size()));//[123,12],[123,53]]}")
		for (int i = 0; i < pos.size(); i++)
		{
			char buff[99];
			if (i == 0)
			{
				sprintf_s(buff, 99, "[ %lf , %lf ]", pos[i].x * scale, pos[i].y * scale);
			}
			else
			{
				sprintf_s(buff, 99, ",[ %lf , %lf ]", pos[i].x*scale, pos[i].y*scale);
			}
			strncat_s(jsonBuff,1024, buff,99);
		}
		strncat_s(jsonBuff,1024, "]}",3);
		return true;
	}
	sprintf_s(jsonBuff, 99, "{\"n\": 0 ,\"list\":[]}");
	return true;
	//err = 501;
	//return false;
}

bool AutoTrack::GetUID(int &uid)
{
	if (wForAfter.run() == false)
	{
		err = 300;
		return false;
	}
	if (getUIDRefMat() == false)
	{
		return false;
	}

	std::vector<cv::Mat> channels;

	split(giUIDRef, channels);
	giUIDRef = channels[3];

	int _uid = 0;
	int _NumBit[9] = { 0 };

	int bitCount = 1;
	cv::Mat matchTmp;
	cv::Mat Roi;
	cv::Mat checkUID = giMatchResource.UID;

#ifdef _DEBUG
	//if (checkUID.rows > Roi.rows)
	//{
	//	cv::resize(checkUID, checkUID, cv::Size(), Roi.rows/ checkUID.rows);
	//}
#endif
	giUIDRef.copyTo(Roi);

	if (checkUID.rows > Roi.rows)
	{
		cv::resize(Roi, Roi, cv::Size(cvRound(1.0 * checkUID.rows / Roi.rows * Roi.cols), checkUID.rows),0);
	}

	cv::matchTemplate(Roi, checkUID, matchTmp, cv::TM_CCOEFF_NORMED);

	double minVal, maxVal;
	cv::Point minLoc, maxLoc;
	//寻找最佳匹配位置
	cv::minMaxLoc(matchTmp, &minVal, &maxVal, &minLoc, &maxLoc);
	if (maxVal > 0.75)
	{
		int x = maxLoc.x + checkUID.cols + 7;
		int y = 0;
		double tmplis[10] = { 0 };
		int tmplisx[10] = { 0 };
		for (int p = 8; p >= 0; p--)
		{
			_NumBit[p] = 0;
			for (int i = 0; i < 10; i++)
			{
				cv::Rect r(x, y, giMatchResource.UIDnumber[i].cols + 2, giUIDRef.rows);//180-46/9->140/9->16->16*9=90+54=144
				if (x + r.width > giUIDRef.cols)
				{
					r = cv::Rect(giUIDRef.cols - giMatchResource.UIDnumber[i].cols - 2, y, giMatchResource.UIDnumber[i].cols + 2, giUIDRef.rows);
				}

				cv::Mat numCheckUID = giMatchResource.UIDnumber[i];
				Roi = giUIDRef(r);

				cv::matchTemplate(Roi, numCheckUID, matchTmp, cv::TM_CCOEFF_NORMED);

				double minVali, maxVali;
				cv::Point minLoci, maxLoci;
				//寻找最佳匹配位置
				cv::minMaxLoc(matchTmp, &minVali, &maxVali, &minLoci, &maxLoci);

				tmplis[i] = maxVali;
				tmplisx[i] = maxLoci.x + numCheckUID.cols - 1;
				if (maxVali > 0.91)
				{
					_NumBit[p] = i;
					x = x + maxLoci.x + numCheckUID.cols - 1;
					break;
				}
				if (i == 10 - 1)
				{
					_NumBit[p] = getMaxID(tmplis, 10);
					x = x + tmplisx[_NumBit[p]];
				}
			}
		}
	}
	_uid = 0;
	for (int i = 0; i < 9; i++)
	{
		_uid += _NumBit[i] * bitCount;
		bitCount = bitCount * 10;
	}
	if (_uid == 0)
	{
		err = 8;//未能在UID区域检测到有效UID
		return false;
	}
	uid = _uid;
	err = 0;
	return true;

}

bool AutoTrack::GetInfoLoadPicture(char * path, int & uid, double & x, double & y, double & a)
{
	std::string pathStr(path);

	giFrame = cv::imread(pathStr);

	if (giFrame.empty())
	{
		err = 201;
		return false;
	}

	getPaimonRefMat();

	cv::Mat paimonTemplate;

	cv::resize(giMatchResource.PaimonTemplate, paimonTemplate, giPaimonRef.size());

	cv::Mat tmp;
//	giPaimonRef = giFrame(cv::Rect(0, 0, cvCeil(giFrame.cols / 20), cvCeil(giFrame.rows / 10)));
//
//#ifdef _DEBUG
//	cv::namedWindow("test", cv::WINDOW_FREERATIO);
//	cv::imshow("test", giPaimonRef);
//#endif

	if (giPaimonRef.channels() != 4)
	{
		cv::cvtColor(paimonTemplate, paimonTemplate, cv::COLOR_RGBA2GRAY);
		cv::cvtColor(giPaimonRef, giPaimonRef, cv::COLOR_RGB2GRAY);
	}

	cv::matchTemplate(paimonTemplate, giPaimonRef, tmp, cv::TM_CCOEFF_NORMED);

	double minVal, maxVal;
	cv::Point minLoc, maxLoc;
	cv::minMaxLoc(tmp, &minVal, &maxVal, &minLoc, &maxLoc);
#ifdef _DEBUG
	cv::namedWindow("test2", cv::WINDOW_FREERATIO);
	cv::imshow("test2", tmp);
	std::cout << "Paimon Match: " << minVal << "," << maxVal << std::endl;
#endif
	if (maxVal < 0.34 || maxVal == 1)
	{
		err=202;
		return false;
	}

	getMiniMapRefMat();

	cv::Mat img_scene(giMatchResource.MapTemplate);
	cv::Mat img_object(giMiniMapRef(cv::Rect(30, 30, giMiniMapRef.cols - 60, giMiniMapRef.rows - 60)));

	cv::cvtColor(img_scene, img_scene, cv::COLOR_RGBA2RGB);

	if (img_object.empty())
	{
		err = 203;
		return false;
	}
	
	getAvatarRefMat();

	if (giAvatarRef.empty())
	{
		err = 204;//未能取到小箭头区域
		return false;
	}

	cv::resize(giAvatarRef, giAvatarRef, cv::Size(), 2, 2);
	std::vector<cv::Mat> lis;
	cv::split(giAvatarRef, lis);

	cv::Mat gray0;
	cv::Mat gray1;
	cv::Mat gray2;

	cv::threshold(lis[0], gray0, 240, 255, cv::THRESH_BINARY);
	cv::threshold(lis[1], gray1, 212, 255, cv::THRESH_BINARY);
	cv::threshold(lis[2], gray2, 25, 255, cv::THRESH_BINARY_INV);

	cv::Mat and12;
	cv::bitwise_and(gray1, gray2, and12);// , gray0);
	cv::resize(and12, and12, cv::Size(), 2, 2, 3);
	cv::Canny(and12, and12, 20, 3 * 20, 3);
	cv::circle(and12, cv::Point(cvCeil(and12.cols / 2), cvCeil(and12.rows / 2)), cvCeil(and12.cols / 4.5), cv::Scalar(0, 0, 0), -1);
	cv::Mat dilate_element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2, 2));
	cv::dilate(and12, and12, dilate_element);
	cv::Mat erode_element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2, 2));
	cv::erode(and12, and12, erode_element);

	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarcy;

	cv::findContours(and12, contours, hierarcy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);

	std::vector<cv::Rect> boundRect(contours.size());  //定义外接矩形集合
	cv::Point2f rect[4];

	std::vector<cv::Point2d> AvatarKeyPoint;
	double AvatarKeyPointLine[3] = { 0 };
	std::vector<cv::Point2f> AvatarKeyLine;
	cv::Point2f KeyLine;

	if (contours.size() != 3)
	{
		err = 205;//提取小箭头特征误差过大
		//return false;
	}
	else
	{
		for (int i = 0; i < 3; i++)
		{
			boundRect[i] = cv::boundingRect(cv::Mat(contours[i]));
			AvatarKeyPoint.push_back(cv::Point(cvRound(boundRect[i].x + boundRect[i].width / 2), cvRound(boundRect[i].y + boundRect[i].height / 2)));
		}

		AvatarKeyPointLine[0] = dis(AvatarKeyPoint[2] - AvatarKeyPoint[1]);
		AvatarKeyPointLine[1] = dis(AvatarKeyPoint[2] - AvatarKeyPoint[0]);
		AvatarKeyPointLine[2] = dis(AvatarKeyPoint[1] - AvatarKeyPoint[0]);

		if (AvatarKeyPointLine[0] >= AvatarKeyPointLine[2] && AvatarKeyPointLine[1] >= AvatarKeyPointLine[2])
		{
			AvatarKeyLine.push_back(AvatarKeyPoint[2] - AvatarKeyPoint[1]);
			AvatarKeyLine.push_back(AvatarKeyPoint[2] - AvatarKeyPoint[0]);
		}
		if (AvatarKeyPointLine[0] >= AvatarKeyPointLine[1] && AvatarKeyPointLine[2] >= AvatarKeyPointLine[1])
		{
			AvatarKeyLine.push_back(AvatarKeyPoint[1] - AvatarKeyPoint[0]);
			AvatarKeyLine.push_back(AvatarKeyPoint[1] - AvatarKeyPoint[2]);
		}
		if (AvatarKeyPointLine[1] >= AvatarKeyPointLine[0] && AvatarKeyPointLine[2] >= AvatarKeyPointLine[0])
		{
			AvatarKeyLine.push_back(AvatarKeyPoint[0] - AvatarKeyPoint[1]);
			AvatarKeyLine.push_back(AvatarKeyPoint[0] - AvatarKeyPoint[2]);
		}

		AvatarKeyLine = Vector2UnitVector(AvatarKeyLine);
		KeyLine = AvatarKeyLine[0] + AvatarKeyLine[1];

		a = Line2Angle(KeyLine);
	}

	

	if (getUIDRefMat() == false)
	{
		err = 206;
		//return false;
	}
	else
	{
		std::vector<cv::Mat> channels;

		split(giUIDRef, channels);
		giUIDRef = channels[0];

		int _uid = 0;
		int _NumBit[9] = { 0 };

		int bitCount = 1;
		cv::Mat matchTmp;
		cv::Mat Roi;
		cv::Mat checkUID = giMatchResource.UID;

#ifdef _DEBUG
		//if (checkUID.rows > Roi.rows)
		//{
		//	cv::resize(checkUID, checkUID, cv::Size(), Roi.rows/ checkUID.rows);
		//}
#endif
		giUIDRef.copyTo(Roi);

		if (checkUID.rows > Roi.rows)
		{
			cv::resize(Roi, Roi, cv::Size(cvRound(1.0 * checkUID.rows / Roi.rows * Roi.cols), checkUID.rows), 0);
		}

		cv::matchTemplate(Roi, checkUID, matchTmp, cv::TM_CCOEFF_NORMED);

		minVal = 0;
		maxVal = 0;
		minLoc = cv::Point(0, 0);
		maxLoc = cv::Point(0, 0);
		//寻找最佳匹配位置
		cv::minMaxLoc(matchTmp, &minVal, &maxVal, &minLoc, &maxLoc);
		if (maxVal > 0.75)
		{
			int x = maxLoc.x + checkUID.cols + 7;
			int y = 0;
			double tmplis[10] = { 0 };
			int tmplisx[10] = { 0 };
			for (int p = 8; p >= 0; p--)
			{
				_NumBit[p] = 0;
				for (int i = 0; i < 10; i++)
				{
					cv::Rect r(x, y, giMatchResource.UIDnumber[i].cols + 2, giUIDRef.rows);//180-46/9->140/9->16->16*9=90+54=144
					if (x + r.width > giUIDRef.cols)
					{
						r = cv::Rect(giUIDRef.cols - giMatchResource.UIDnumber[i].cols - 2, y, giMatchResource.UIDnumber[i].cols + 2, giUIDRef.rows);
					}

					cv::Mat numCheckUID = giMatchResource.UIDnumber[i];
					Roi = giUIDRef(r);

					cv::matchTemplate(Roi, numCheckUID, matchTmp, cv::TM_CCOEFF_NORMED);

					double minVali, maxVali;
					cv::Point minLoci, maxLoci;
					//寻找最佳匹配位置
					cv::minMaxLoc(matchTmp, &minVali, &maxVali, &minLoci, &maxLoci);

					tmplis[i] = maxVali;
					tmplisx[i] = maxLoci.x + numCheckUID.cols - 1;
					if (maxVali > 0.91)
					{
						_NumBit[p] = i;
						x = x + maxLoci.x + numCheckUID.cols - 1;
						break;
					}
					if (i == 10 - 1)
					{
						_NumBit[p] = getMaxID(tmplis, 10);
						x = x + tmplisx[_NumBit[p]];
					}
				}
			}
		}

		_uid = 0;
		for (int i = 0; i < 9; i++)
		{
			_uid += _NumBit[i] * bitCount;
			bitCount = bitCount * 10;
		}
		if (_uid == 0)
		{
			err = 207;//未能在UID区域检测到有效UID
			//return false;
		}
		uid = _uid;
	}




	if (!is_init_end)
	{
		init();//初始化
	}
	if (img_object.empty())
	{
		err = 208;//原神小地图区域为空或者区域长宽小于60px
		return false;
	}

	isContinuity = false;

	cv::Point2d *hisP = _TransformHistory;

	cv::Point2d pos;

	if ((dis(hisP[1] - hisP[0]) + dis(hisP[2] - hisP[1])) < 2000)
	{
		if (hisP[2].x > someSizeR && hisP[2].x < img_scene.cols - someSizeR && hisP[2].y>someSizeR && hisP[2].y < img_scene.rows - someSizeR)
		{
			isContinuity = true;
			if (isContinuity)
			{
				if (isOnCity == false)
				{
					cv::Mat someMap(img_scene(cv::Rect(cvRound(hisP[2].x - someSizeR), cvRound(hisP[2].y - someSizeR), cvRound(someSizeR * 2), cvRound(someSizeR * 2))));
					cv::Mat minMap(img_object);

					//resize(someMap, someMap, Size(), MatchMatScale, MatchMatScale, 1);
					//resize(minMap, minMap, Size(), MatchMatScale, MatchMatScale, 1);

					//cv::Ptr<cv::xfeatures2d::SURF>& detectorSomeMap = *(cv::Ptr<cv::xfeatures2d::SURF>*)_detectorSomeMap;
					//std::vector<cv::KeyPoint>& KeyPointSomeMap = *(std::vector<cv::KeyPoint>*)_KeyPointSomeMap;
					//cv::Mat& DataPointSomeMap = *(cv::Mat*)_DataPointSomeMap;
					//std::vector<cv::KeyPoint>& KeyPointMiniMap = *(std::vector<cv::KeyPoint>*)_KeyPointMiniMap;
					//cv::Mat& DataPointMiniMap = *(cv::Mat*)_DataPointMiniMap;

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
						cv::Ptr<cv::DescriptorMatcher> matcherTmp = cv::DescriptorMatcher::create(cv::DescriptorMatcher::FLANNBASED);
						std::vector< std::vector<cv::DMatch> > KNN_mTmp;
#ifdef _DEBUG
						std::vector<cv::DMatch> good_matchesTmp;
#endif
						matcherTmp->knnMatch(DataPointMiniMap, DataPointSomeMap, KNN_mTmp, 2);


						for (size_t i = 0; i < KNN_mTmp.size(); i++)
						{
							if (KNN_mTmp[i][0].distance < ratio_thresh * KNN_mTmp[i][1].distance)
							{
#ifdef _DEBUG
								good_matchesTmp.push_back(KNN_mTmp[i][0]);
#endif
								if (KNN_mTmp[i][0].queryIdx >= KeyPointMiniMap.size())
								{
									continue;
								}
								// 这里有个bug回卡进来，进入副本或者切换放大招时偶尔触发
								lisx.push_back(((minMap.cols / 2 - KeyPointMiniMap[KNN_mTmp[i][0].queryIdx].pt.x)*mapScale + KeyPointSomeMap[KNN_mTmp[i][0].trainIdx].pt.x));
								lisy.push_back(((minMap.rows / 2 - KeyPointMiniMap[KNN_mTmp[i][0].queryIdx].pt.y)*mapScale + KeyPointSomeMap[KNN_mTmp[i][0].trainIdx].pt.y));

								sumx += lisx.back();
								sumy += lisy.back();
							}
						}
#ifdef _DEBUG
						cv::Mat img_matches, imgmap, imgminmap;
						drawKeypoints(someMap, KeyPointSomeMap, imgmap, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
						drawKeypoints(img_object, KeyPointMiniMap, imgminmap, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
						drawMatches(img_object, KeyPointMiniMap, someMap, KeyPointSomeMap, good_matchesTmp, img_matches, cv::Scalar::all(-1), cv::Scalar::all(-1), std::vector<char>(), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
#endif
						if (min(lisx.size(), lisy.size()) <= 4)
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
							if (KeyPointSomeMap.size() == 0 || KeyPointMiniMap.size() == 0)
							{
								isContinuity = false;
							}
							else
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

								for (size_t i = 0; i < KNN_mTmp.size(); i++)
								{
									if (KNN_mTmp[i][0].distance < ratio_thresh * KNN_mTmp[i][1].distance)
									{
#ifdef _DEBUG
										good_matchesTmp.push_back(KNN_mTmp[i][0]);
#endif
										// 这里有个bug回卡进来，进入副本或者切换放大招时偶尔触发
										if (KNN_mTmp[i][0].queryIdx >= KeyPointMiniMap.size())
										{
											continue;
										}

										lisx.push_back(((minMap.cols / 2 - KeyPointMiniMap[KNN_mTmp[i][0].queryIdx].pt.x)*0.8667 + KeyPointSomeMap[KNN_mTmp[i][0].trainIdx].pt.x));
										lisy.push_back(((minMap.rows / 2 - KeyPointMiniMap[KNN_mTmp[i][0].queryIdx].pt.y)*0.8667 + KeyPointSomeMap[KNN_mTmp[i][0].trainIdx].pt.y));
										sumx += lisx.back();
										sumy += lisy.back();
									}
								}
#ifdef _DEBUG
								//Mat img_matches, imgmap, imgminmap;
								drawKeypoints(someMap, KeyPointSomeMap, imgmap, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
								drawKeypoints(img_object, KeyPointMiniMap, imgminmap, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

								drawMatches(img_object, KeyPointMiniMap, someMap, KeyPointSomeMap, good_matchesTmp, img_matches, cv::Scalar::all(-1), cv::Scalar::all(-1), std::vector<char>(), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
#endif
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

									int x = cvRound((p.x - someMap.cols / 2) / 2);
									int y = cvRound((p.y - someMap.rows / 2) / 2);

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
				else
				{
					//在城镇中
						/***********************/
						//重新从完整中地图取出角色周围部分地图
					cv::Mat someMap(img_scene(cv::Rect(cvCeil(hisP[2].x - someSizeR), cvCeil(hisP[2].y - someSizeR), someSizeR * 2, someSizeR * 2)));
					cv::Mat minMap(img_object);

					//cv::Ptr<cv::xfeatures2d::SURF>& detectorSomeMap = *(cv::Ptr<cv::xfeatures2d::SURF>*)_detectorSomeMap;
					//std::vector<cv::KeyPoint>& KeyPointSomeMap = *(std::vector<cv::KeyPoint>*)_KeyPointSomeMap;
					//cv::Mat& DataPointSomeMap = *(cv::Mat*)_DataPointSomeMap;
					//std::vector<cv::KeyPoint>& KeyPointMiniMap = *(std::vector<cv::KeyPoint>*)_KeyPointMiniMap;
					//cv::Mat& DataPointMiniMap = *(cv::Mat*)_DataPointMiniMap;

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
						matcherTmp->knnMatch(DataPointMiniMap, DataPointSomeMap, KNN_mTmp, 2);
						std::vector<double> lisx;
						std::vector<double> lisy;
						double sumx = 0;
						double sumy = 0;

						for (size_t i = 0; i < KNN_mTmp.size(); i++)
						{
							if (KNN_mTmp[i][0].distance < ratio_thresh * KNN_mTmp[i][1].distance)
							{
#ifdef _DEBUG
								good_matchesTmp.push_back(KNN_mTmp[i][0]);
#endif
								// 这里有个bug回卡进来，进入副本或者切换放大招时偶尔触发
								if (KNN_mTmp[i][0].queryIdx >= KeyPointMiniMap.size())
								{
									continue;
								}

								lisx.push_back(((minMap.cols / 2 - KeyPointMiniMap[KNN_mTmp[i][0].queryIdx].pt.x)*0.8667 + KeyPointSomeMap[KNN_mTmp[i][0].trainIdx].pt.x));
								lisy.push_back(((minMap.rows / 2 - KeyPointMiniMap[KNN_mTmp[i][0].queryIdx].pt.y)*0.8667 + KeyPointSomeMap[KNN_mTmp[i][0].trainIdx].pt.y));
								sumx += lisx.back();
								sumy += lisy.back();
							}
						}
#ifdef _DEBUG
						cv::Mat img_matches, imgmap, imgminmap;
						drawKeypoints(someMap, KeyPointSomeMap, imgmap, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
						drawKeypoints(img_object, KeyPointMiniMap, imgminmap, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

						drawMatches(img_object, KeyPointMiniMap, someMap, KeyPointSomeMap, good_matchesTmp, img_matches, cv::Scalar::all(-1), cv::Scalar::all(-1), std::vector<char>(), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
#endif
						if (max(lisx.size(), lisy.size()) > 4)
						{
							if (min(lisx.size(), lisy.size()) >= 10)
							{
								isOnCity = true;
							}
							else
							{
								isOnCity = false;
							}
							//if(max(lisx.size(), lisy.size()) <=10 )
							//{
							//	isOnCity = false;
							//}

							double meanx = sumx / lisx.size(); //均值
							double meany = sumy / lisy.size(); //均值
							cv::Point2d p = SPC(lisx, sumx, lisy, sumy);

							int x = cvRound((p.x - someMap.cols / 2) / 2);
							int y = cvRound((p.y - someMap.rows / 2) / 2);

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
			}
		}
	}
	if (!isContinuity)
	{
		//cv::Ptr<cv::xfeatures2d::SURF>& detectorAllMap = *(cv::Ptr<cv::xfeatures2d::SURF>*)_detectorAllMap;
		//std::vector<cv::KeyPoint>& KeyPointAllMap = *(std::vector<cv::KeyPoint>*)_KeyPointAllMap;
		//cv::Mat& DataPointAllMap = *(cv::Mat*)_DataPointAllMap;
		//std::vector<cv::KeyPoint>& KeyPointMiniMap = *(std::vector<cv::KeyPoint>*)_KeyPointMiniMap;
		//cv::Mat& DataPointMiniMap = *(cv::Mat*)_DataPointMiniMap;

		cv::Ptr<cv::xfeatures2d::SURF>& detectorAllMap = _detectorAllMap;
		std::vector<cv::KeyPoint>& KeyPointAllMap = _KeyPointAllMap;
		cv::Mat& DataPointAllMap = _DataPointAllMap;
		std::vector<cv::KeyPoint>& KeyPointMiniMap = _KeyPointMiniMap;
		cv::Mat& DataPointMiniMap = _DataPointMiniMap;

		detectorAllMap->detectAndCompute(img_object, cv::noArray(), KeyPointMiniMap, DataPointMiniMap);

		if (KeyPointMiniMap.size() == 0)
		{
			err = 209;//未能匹配到特征点
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
			for (size_t i = 0; i < KNN_m.size(); i++)
			{
				if (KNN_m[i][0].distance < ratio_thresh * KNN_m[i][1].distance)
				{
#ifdef _DEBUG
					good_matches.push_back(KNN_m[i][0]);
#endif
					if (KNN_m[i][0].queryIdx >= KeyPointMiniMap.size())
					{
						continue;
					}
					lisx.push_back(((img_object.cols / 2 - KeyPointMiniMap[KNN_m[i][0].queryIdx].pt.x)*mapScale + KeyPointAllMap[KNN_m[i][0].trainIdx].pt.x));
					lisy.push_back(((img_object.rows / 2 - KeyPointMiniMap[KNN_m[i][0].queryIdx].pt.y)*mapScale + KeyPointAllMap[KNN_m[i][0].trainIdx].pt.y));
					sumx += lisx.back();
					sumy += lisy.back();
				}
			}
#ifdef _DEBUG
			cv::Mat img_matches, imgmap, imgminmap;
			drawKeypoints(img_scene, KeyPointAllMap, imgmap, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
			drawKeypoints(img_object, KeyPointMiniMap, imgminmap, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

			drawMatches(img_object, KeyPointMiniMap, img_scene, KeyPointAllMap, good_matches, img_matches, cv::Scalar::all(-1), cv::Scalar::all(-1), std::vector<char>(), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
#endif

			if (lisx.size() == 0 || lisy.size() == 0)
			{
				err = 210;//未能匹配到特征点
				return false;
			}
			else
			{
				pos = SPC(lisx, sumx, lisy, sumy);
			}
		}
	}

	hisP[0] = hisP[1];
	hisP[1] = hisP[2];
	hisP[2] = pos;

	pos = TransferTianLiAxes(pos * MapAbsScale, MapWorldOffset, MapWorldScale);

	pos = TransferUserAxes(pos, UserWorldOrigin_X, UserWorldOrigin_Y, UserWorldScale);

	x = (float)(pos.x);
	y = (float)(pos.y);

	err = 0;
	return true;

}

bool AutoTrack::GetInfoLoadVideo(char * path, char * pathOutFile)
{
	std::string pathStr(path);
	std::string pathOutFileStr(pathOutFile);

	cv::VideoCapture Video;
	Video.open(pathStr);

	if (!Video.isOpened())
	{
		err = 301;
		return false;
	}

	FILE *fpSave;
	errno_t errOpenFile;
	errOpenFile = fopen_s(&fpSave, pathOutFileStr.c_str(), "w");
	if (errOpenFile != 0)
	{
		err = 302;
		return false;
	}
#define TestJson
#ifdef TestJson

#else
	fprintf_s(fpSave, "{\"curveName\":\"新的画布\",\"curve_list\":[{\"lineName\":\"新的线条\",\"curve_poi\":[");
#endif

	int uid = 0;
	double x = 0;
	double y = 0;
	double a = 0;

	int timeNumber = 0;

	bool isCycle = true;
	while(isCycle)
	{
		Video >> giFrame;
	
		if (giFrame.empty())
		{
			isCycle=false;
			break;
		}

		getPaimonRefMat();

		cv::Mat paimonTemplate;

		cv::resize(giMatchResource.PaimonTemplate, paimonTemplate, giPaimonRef.size());

		cv::Mat tmp;
		giPaimonRef = giFrame(cv::Rect(0, 0, cvCeil(giFrame.cols / 20), cvCeil(giFrame.rows / 10)));

#ifdef _DEBUG
		cv::namedWindow("test", cv::WINDOW_FREERATIO);
		cv::imshow("test", giPaimonRef);
#endif

		if (giPaimonRef.channels() != 4)
		{
			cv::cvtColor(paimonTemplate, paimonTemplate, cv::COLOR_RGBA2GRAY);
			cv::cvtColor(giPaimonRef, giPaimonRef, cv::COLOR_RGB2GRAY);
		}

		cv::matchTemplate(paimonTemplate, giPaimonRef, tmp, cv::TM_CCOEFF_NORMED);

		double minVal, maxVal;
		cv::Point minLoc, maxLoc;
		cv::minMaxLoc(tmp, &minVal, &maxVal, &minLoc, &maxLoc);
#ifdef _DEBUG
		cv::namedWindow("test2", cv::WINDOW_FREERATIO);
		cv::imshow("test2", tmp);
		std::cout << "Paimon Match: " << minVal << "," << maxVal << std::endl;
#endif
		if (maxVal < 0.36 || maxVal == 1)
		{
			continue;
		}

		getMiniMapRefMat();

		cv::Mat img_scene(giMatchResource.MapTemplate);
		cv::Mat img_object(giMiniMapRef(cv::Rect(30, 30, giMiniMapRef.cols - 60, giMiniMapRef.rows - 60)));

		cv::cvtColor(img_scene, img_scene, cv::COLOR_RGBA2RGB);

		if (img_object.empty())
		{
			continue;
		}

		getAvatarRefMat();

		if (giAvatarRef.empty())
		{
			//err = 204;//未能取到小箭头区域
			//break;// return false;
		}
		else
		{
			cv::resize(giAvatarRef, giAvatarRef, cv::Size(), 2, 2);
			std::vector<cv::Mat> lis;
			cv::split(giAvatarRef, lis);

			cv::Mat gray0;
			cv::Mat gray1;
			cv::Mat gray2;

			cv::threshold(lis[0], gray0, 240, 255, cv::THRESH_BINARY);
			cv::threshold(lis[1], gray1, 212, 255, cv::THRESH_BINARY);
			cv::threshold(lis[2], gray2, 25, 255, cv::THRESH_BINARY_INV);

			cv::Mat and12;
			cv::bitwise_and(gray1, gray2, and12);// , gray0);
			cv::resize(and12, and12, cv::Size(), 2, 2, 3);
			cv::Canny(and12, and12, 20, 3 * 20, 3);
			cv::circle(and12, cv::Point(cvCeil(and12.cols / 2), cvCeil(and12.rows / 2)), cvCeil(and12.cols / 4.5), cv::Scalar(0, 0, 0), -1);
			cv::Mat dilate_element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2, 2));
			cv::dilate(and12, and12, dilate_element);
			cv::Mat erode_element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2, 2));
			cv::erode(and12, and12, erode_element);

			std::vector<std::vector<cv::Point>> contours;
			std::vector<cv::Vec4i> hierarcy;

			cv::findContours(and12, contours, hierarcy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);

			std::vector<cv::Rect> boundRect(contours.size());  //定义外接矩形集合
			cv::Point2f rect[4];

			std::vector<cv::Point2d> AvatarKeyPoint;
			double AvatarKeyPointLine[3] = { 0 };
			std::vector<cv::Point2f> AvatarKeyLine;
			cv::Point2f KeyLine;

			if (contours.size() != 3)
			{
				//err = 205;//提取小箭头特征误差过大
				//break;//return false;
			}
			else
			{
				for (int i = 0; i < 3; i++)
				{
					boundRect[i] = cv::boundingRect(cv::Mat(contours[i]));
					AvatarKeyPoint.push_back(cv::Point(cvRound(boundRect[i].x + boundRect[i].width / 2), cvRound(boundRect[i].y + boundRect[i].height / 2)));
				}

				AvatarKeyPointLine[0] = dis(AvatarKeyPoint[2] - AvatarKeyPoint[1]);
				AvatarKeyPointLine[1] = dis(AvatarKeyPoint[2] - AvatarKeyPoint[0]);
				AvatarKeyPointLine[2] = dis(AvatarKeyPoint[1] - AvatarKeyPoint[0]);

				if (AvatarKeyPointLine[0] >= AvatarKeyPointLine[2] && AvatarKeyPointLine[1] >= AvatarKeyPointLine[2])
				{
					AvatarKeyLine.push_back(AvatarKeyPoint[2] - AvatarKeyPoint[1]);
					AvatarKeyLine.push_back(AvatarKeyPoint[2] - AvatarKeyPoint[0]);
				}
				if (AvatarKeyPointLine[0] >= AvatarKeyPointLine[1] && AvatarKeyPointLine[2] >= AvatarKeyPointLine[1])
				{
					AvatarKeyLine.push_back(AvatarKeyPoint[1] - AvatarKeyPoint[0]);
					AvatarKeyLine.push_back(AvatarKeyPoint[1] - AvatarKeyPoint[2]);
				}
				if (AvatarKeyPointLine[1] >= AvatarKeyPointLine[0] && AvatarKeyPointLine[2] >= AvatarKeyPointLine[0])
				{
					AvatarKeyLine.push_back(AvatarKeyPoint[0] - AvatarKeyPoint[1]);
					AvatarKeyLine.push_back(AvatarKeyPoint[0] - AvatarKeyPoint[2]);
				}

				AvatarKeyLine = Vector2UnitVector(AvatarKeyLine);
				KeyLine = AvatarKeyLine[0] + AvatarKeyLine[1];

				a = Line2Angle(KeyLine);
			}
		}

		



		if (getUIDRefMat() == false)
		{
			continue;
		}
		else
		{
			std::vector<cv::Mat> channels;

			split(giUIDRef, channels);
			giUIDRef = channels[0];

			int _uid = 0;
			int _NumBit[9] = { 0 };

			int bitCount = 1;
			cv::Mat matchTmp;
			cv::Mat Roi;
			cv::Mat checkUID = giMatchResource.UID;

#ifdef _DEBUG
			//if (checkUID.rows > Roi.rows)
			//{
			//	cv::resize(checkUID, checkUID, cv::Size(), Roi.rows/ checkUID.rows);
			//}
#endif
			giUIDRef.copyTo(Roi);

			if (checkUID.rows > Roi.rows)
			{
				cv::resize(Roi, Roi, cv::Size(cvRound(1.0 * checkUID.rows / Roi.rows * Roi.cols), checkUID.rows), 0);
			}

			cv::matchTemplate(Roi, checkUID, matchTmp, cv::TM_CCOEFF_NORMED);

			minVal = 0;
			maxVal = 0;
			minLoc = cv::Point(0, 0);
			maxLoc = cv::Point(0, 0);
			//寻找最佳匹配位置
			cv::minMaxLoc(matchTmp, &minVal, &maxVal, &minLoc, &maxLoc);
			if (maxVal > 0.75)
			{
				int x = maxLoc.x + checkUID.cols + 7;
				int y = 0;
				double tmplis[10] = { 0 };
				int tmplisx[10] = { 0 };
				for (int p = 8; p >= 0; p--)
				{
					_NumBit[p] = 0;
					for (int i = 0; i < 10; i++)
					{
						cv::Rect r(x, y, giMatchResource.UIDnumber[i].cols + 2, giUIDRef.rows);//180-46/9->140/9->16->16*9=90+54=144
						if (x + r.width > giUIDRef.cols)
						{
							r = cv::Rect(giUIDRef.cols - giMatchResource.UIDnumber[i].cols - 2, y, giMatchResource.UIDnumber[i].cols + 2, giUIDRef.rows);
						}

						cv::Mat numCheckUID = giMatchResource.UIDnumber[i];
						Roi = giUIDRef(r);

						cv::matchTemplate(Roi, numCheckUID, matchTmp, cv::TM_CCOEFF_NORMED);

						double minVali, maxVali;
						cv::Point minLoci, maxLoci;
						//寻找最佳匹配位置
						cv::minMaxLoc(matchTmp, &minVali, &maxVali, &minLoci, &maxLoci);

						tmplis[i] = maxVali;
						tmplisx[i] = maxLoci.x + numCheckUID.cols - 1;
						if (maxVali > 0.91)
						{
							_NumBit[p] = i;
							x = x + maxLoci.x + numCheckUID.cols - 1;
							break;
						}
						if (i == 10 - 1)
						{
							_NumBit[p] = getMaxID(tmplis, 10);
							x = x + tmplisx[_NumBit[p]];
						}
					}
				}
			}

			_uid = 0;
			for (int i = 0; i < 9; i++)
			{
				_uid += _NumBit[i] * bitCount;
				bitCount = bitCount * 10;
			}
			if (_uid == 0)
			{
				//err = 207;//未能在UID区域检测到有效UID
				//return false;
			}
			uid = _uid;
		}

		



		if (!is_init_end)
		{
			init();//初始化
		}
		if (img_object.empty())
		{
			continue;
		}

		isContinuity = false;

		cv::Point2d *hisP = _TransformHistory;

		cv::Point2d pos;

		if ((dis(hisP[1] - hisP[0]) + dis(hisP[2] - hisP[1])) < 2000)
		{
			if (hisP[2].x > someSizeR && hisP[2].x < img_scene.cols - someSizeR && hisP[2].y>someSizeR && hisP[2].y < img_scene.rows - someSizeR)
			{
				isContinuity = true;
				if (isContinuity)
				{
					if (isOnCity == false)
					{
						cv::Mat someMap(img_scene(cv::Rect(cvRound(hisP[2].x - someSizeR), cvRound(hisP[2].y - someSizeR), cvRound(someSizeR * 2), cvRound(someSizeR * 2))));
						cv::Mat minMap(img_object);

						//resize(someMap, someMap, Size(), MatchMatScale, MatchMatScale, 1);
						//resize(minMap, minMap, Size(), MatchMatScale, MatchMatScale, 1);

						//cv::Ptr<cv::xfeatures2d::SURF>& detectorSomeMap = *(cv::Ptr<cv::xfeatures2d::SURF>*)_detectorSomeMap;
						//std::vector<cv::KeyPoint>& KeyPointSomeMap = *(std::vector<cv::KeyPoint>*)_KeyPointSomeMap;
						//cv::Mat& DataPointSomeMap = *(cv::Mat*)_DataPointSomeMap;
						//std::vector<cv::KeyPoint>& KeyPointMiniMap = *(std::vector<cv::KeyPoint>*)_KeyPointMiniMap;
						//cv::Mat& DataPointMiniMap = *(cv::Mat*)_DataPointMiniMap;

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
							cv::Ptr<cv::DescriptorMatcher> matcherTmp = cv::DescriptorMatcher::create(cv::DescriptorMatcher::FLANNBASED);
							std::vector< std::vector<cv::DMatch> > KNN_mTmp;
#ifdef _DEBUG
							std::vector<cv::DMatch> good_matchesTmp;
#endif
							matcherTmp->knnMatch(DataPointMiniMap, DataPointSomeMap, KNN_mTmp, 2);


							for (size_t i = 0; i < KNN_mTmp.size(); i++)
							{
								if (KNN_mTmp[i][0].distance < ratio_thresh * KNN_mTmp[i][1].distance)
								{
#ifdef _DEBUG
									good_matchesTmp.push_back(KNN_mTmp[i][0]);
#endif
									if (KNN_mTmp[i][0].queryIdx >= KeyPointMiniMap.size())
									{
										continue;
									}
									// 这里有个bug回卡进来，进入副本或者切换放大招时偶尔触发
									lisx.push_back(((minMap.cols / 2 - KeyPointMiniMap[KNN_mTmp[i][0].queryIdx].pt.x)*mapScale + KeyPointSomeMap[KNN_mTmp[i][0].trainIdx].pt.x));
									lisy.push_back(((minMap.rows / 2 - KeyPointMiniMap[KNN_mTmp[i][0].queryIdx].pt.y)*mapScale + KeyPointSomeMap[KNN_mTmp[i][0].trainIdx].pt.y));

									sumx += lisx.back();
									sumy += lisy.back();
								}
							}
#ifdef _DEBUG
							cv::Mat img_matches, imgmap, imgminmap;
							drawKeypoints(someMap, KeyPointSomeMap, imgmap, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
							drawKeypoints(img_object, KeyPointMiniMap, imgminmap, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
							drawMatches(img_object, KeyPointMiniMap, someMap, KeyPointSomeMap, good_matchesTmp, img_matches, cv::Scalar::all(-1), cv::Scalar::all(-1), std::vector<char>(), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
#endif
							if (min(lisx.size(), lisy.size()) <= 4)
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
								if (KeyPointSomeMap.size() == 0 || KeyPointMiniMap.size() == 0)
								{
									isContinuity = false;
								}
								else
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

									for (size_t i = 0; i < KNN_mTmp.size(); i++)
									{
										if (KNN_mTmp[i][0].distance < ratio_thresh * KNN_mTmp[i][1].distance)
										{
#ifdef _DEBUG
											good_matchesTmp.push_back(KNN_mTmp[i][0]);
#endif
											// 这里有个bug回卡进来，进入副本或者切换放大招时偶尔触发
											if (KNN_mTmp[i][0].queryIdx >= KeyPointMiniMap.size())
											{
												continue;
											}

											lisx.push_back(((minMap.cols / 2 - KeyPointMiniMap[KNN_mTmp[i][0].queryIdx].pt.x)*0.8667 + KeyPointSomeMap[KNN_mTmp[i][0].trainIdx].pt.x));
											lisy.push_back(((minMap.rows / 2 - KeyPointMiniMap[KNN_mTmp[i][0].queryIdx].pt.y)*0.8667 + KeyPointSomeMap[KNN_mTmp[i][0].trainIdx].pt.y));
											sumx += lisx.back();
											sumy += lisy.back();
										}
									}
#ifdef _DEBUG
									//Mat img_matches, imgmap, imgminmap;
									drawKeypoints(someMap, KeyPointSomeMap, imgmap, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
									drawKeypoints(img_object, KeyPointMiniMap, imgminmap, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

									drawMatches(img_object, KeyPointMiniMap, someMap, KeyPointSomeMap, good_matchesTmp, img_matches, cv::Scalar::all(-1), cv::Scalar::all(-1), std::vector<char>(), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
#endif
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

										double x = (p.x - someMap.cols / 2) / 2;
										double y = (p.y - someMap.rows / 2) / 2;

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
					else
					{
						//在城镇中
							/***********************/
							//重新从完整中地图取出角色周围部分地图
						cv::Mat someMap(img_scene(cv::Rect(cvCeil(hisP[2].x - someSizeR), cvCeil(hisP[2].y - someSizeR), someSizeR * 2, someSizeR * 2)));
						cv::Mat minMap(img_object);

						//cv::Ptr<cv::xfeatures2d::SURF>& detectorSomeMap = *(cv::Ptr<cv::xfeatures2d::SURF>*)_detectorSomeMap;
						//std::vector<cv::KeyPoint>& KeyPointSomeMap = *(std::vector<cv::KeyPoint>*)_KeyPointSomeMap;
						//cv::Mat& DataPointSomeMap = *(cv::Mat*)_DataPointSomeMap;
						//std::vector<cv::KeyPoint>& KeyPointMiniMap = *(std::vector<cv::KeyPoint>*)_KeyPointMiniMap;
						//cv::Mat& DataPointMiniMap = *(cv::Mat*)_DataPointMiniMap;

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
							matcherTmp->knnMatch(DataPointMiniMap, DataPointSomeMap, KNN_mTmp, 2);
							std::vector<double> lisx;
							std::vector<double> lisy;
							double sumx = 0;
							double sumy = 0;

							for (size_t i = 0; i < KNN_mTmp.size(); i++)
							{
								if (KNN_mTmp[i][0].distance < ratio_thresh * KNN_mTmp[i][1].distance)
								{
#ifdef _DEBUG
									good_matchesTmp.push_back(KNN_mTmp[i][0]);
#endif
									// 这里有个bug回卡进来，进入副本或者切换放大招时偶尔触发
									if (KNN_mTmp[i][0].queryIdx >= KeyPointMiniMap.size())
									{
										continue;
									}

									lisx.push_back(((minMap.cols / 2 - KeyPointMiniMap[KNN_mTmp[i][0].queryIdx].pt.x)*0.8667 + KeyPointSomeMap[KNN_mTmp[i][0].trainIdx].pt.x));
									lisy.push_back(((minMap.rows / 2 - KeyPointMiniMap[KNN_mTmp[i][0].queryIdx].pt.y)*0.8667 + KeyPointSomeMap[KNN_mTmp[i][0].trainIdx].pt.y));
									sumx += lisx.back();
									sumy += lisy.back();
								}
							}
#ifdef _DEBUG
							cv::Mat img_matches, imgmap, imgminmap;
							drawKeypoints(someMap, KeyPointSomeMap, imgmap, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
							drawKeypoints(img_object, KeyPointMiniMap, imgminmap, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

							drawMatches(img_object, KeyPointMiniMap, someMap, KeyPointSomeMap, good_matchesTmp, img_matches, cv::Scalar::all(-1), cv::Scalar::all(-1), std::vector<char>(), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
#endif
							if (max(lisx.size(), lisy.size()) > 4)
							{
								if (min(lisx.size(), lisy.size()) >= 10)
								{
									isOnCity = true;
								}
								else
								{
									isOnCity = false;
								}
								//if(max(lisx.size(), lisy.size()) <=10 )
								//{
								//	isOnCity = false;
								//}

								double meanx = sumx / lisx.size(); //均值
								double meany = sumy / lisy.size(); //均值
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
				}
			}
		}
		if (!isContinuity)
		{
			//cv::Ptr<cv::xfeatures2d::SURF>& detectorAllMap = *(cv::Ptr<cv::xfeatures2d::SURF>*)_detectorAllMap;
			//std::vector<cv::KeyPoint>& KeyPointAllMap = *(std::vector<cv::KeyPoint>*)_KeyPointAllMap;
			//cv::Mat& DataPointAllMap = *(cv::Mat*)_DataPointAllMap;
			//std::vector<cv::KeyPoint>& KeyPointMiniMap = *(std::vector<cv::KeyPoint>*)_KeyPointMiniMap;
			//cv::Mat& DataPointMiniMap = *(cv::Mat*)_DataPointMiniMap;

			cv::Ptr<cv::xfeatures2d::SURF>& detectorAllMap = _detectorAllMap;
			std::vector<cv::KeyPoint>& KeyPointAllMap = _KeyPointAllMap;
			cv::Mat& DataPointAllMap = _DataPointAllMap;
			std::vector<cv::KeyPoint>& KeyPointMiniMap = _KeyPointMiniMap;
			cv::Mat& DataPointMiniMap = _DataPointMiniMap;

			detectorAllMap->detectAndCompute(img_object, cv::noArray(), KeyPointMiniMap, DataPointMiniMap);

			if (KeyPointMiniMap.size() == 0)
			{
				continue;
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
				for (size_t i = 0; i < KNN_m.size(); i++)
				{
					if (KNN_m[i][0].distance < ratio_thresh * KNN_m[i][1].distance)
					{
#ifdef _DEBUG
						good_matches.push_back(KNN_m[i][0]);
#endif
						if (KNN_m[i][0].queryIdx >= KeyPointMiniMap.size())
						{
							continue;
						}
						lisx.push_back(((img_object.cols / 2.0 - KeyPointMiniMap[KNN_m[i][0].queryIdx].pt.x)*mapScale + KeyPointAllMap[KNN_m[i][0].trainIdx].pt.x));
						lisy.push_back(((img_object.rows / 2.0 - KeyPointMiniMap[KNN_m[i][0].queryIdx].pt.y)*mapScale + KeyPointAllMap[KNN_m[i][0].trainIdx].pt.y));
						sumx += lisx.back();
						sumy += lisy.back();
					}
				}
#ifdef _DEBUG
				cv::Mat img_matches, imgmap, imgminmap;
				drawKeypoints(img_scene, KeyPointAllMap, imgmap, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
				drawKeypoints(img_object, KeyPointMiniMap, imgminmap, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

				drawMatches(img_object, KeyPointMiniMap, img_scene, KeyPointAllMap, good_matches, img_matches, cv::Scalar::all(-1), cv::Scalar::all(-1), std::vector<char>(), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
#endif

				if (lisx.size() == 0 || lisy.size() == 0)
				{
					continue;
				}
				else
				{
					pos = SPC(lisx, sumx, lisy, sumy);
				}
			}
		}

		hisP[0] = hisP[1];
		hisP[1] = hisP[2];
		hisP[2] = pos;

		pos = TransferTianLiAxes(pos * MapAbsScale, MapWorldOffset, MapWorldScale);

		pos = TransferUserAxes(pos, UserWorldOrigin_X, UserWorldOrigin_Y, UserWorldScale);

		x = (float)(pos.x);
		y = (float)(pos.y);

#ifdef TestJson
		fprintf_s(fpSave, "%d %d %lf %lf %lf\n", timeNumber, uid, x, y, a);
#else
		if (timeNumber != 0)
		{
			fprintf_s(fpSave, ",\n");
		}
		else
		{
			fprintf_s(fpSave, "\n");
		}
		fprintf_s(fpSave, "{\"x\": %lf,\"y\": %lf,\"z\":0.0}", x, y);
#endif
		timeNumber++;
	}


#ifdef TestJson

#else
	fprintf_s(fpSave, "]}]}");
#endif

	std::fclose(fpSave);
	Video.release();
	err = 0;
	return true;
}

int AutoTrack::GetLastError()
{
#ifdef _DEBUG
	std::cout << err;
#endif
	return err;
}

bool AutoTrack::getAutoTrackIsInit()
{
	if (is_init_end)
	{
		err = 1;
		return false;
	}
	else
	{
		return true;
	}
}

const char * AutoTrack::GetLastErrorStr()
{
	return nullptr;
}

bool AutoTrack::getGengshinImpactWnd()
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
		std::cout << "GI 原神 Genshin Impact 원신 Window Name Find is " << giWindowName << std::endl;
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

bool AutoTrack::getGengshinImpactRect()
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

bool AutoTrack::getGengshinImpactScale()
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


bool AutoTrack::getGengshinImpactScreen()
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

	GetBitmapBits(hBmp, bmp.bmHeight*bmp.bmWidth*nChannels, giFrame.data);

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

bool AutoTrack::getPaimonRefMat()
{
	int Paimon_Rect_x = cvCeil(giFrame.cols*0.0135);
	int Paimon_Rect_y = cvCeil(giFrame.cols*0.006075);
	int Paimon_Rect_w = cvCeil(giFrame.cols*0.035);
	int Paimon_Rect_h = cvCeil(giFrame.cols*0.0406);

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

bool AutoTrack::getMiniMapRefMat()
{
	int MiniMap_Rect_x = cvRound(giFrame.cols*0.033);
	int MiniMap_Rect_y = cvRound(giFrame.cols*0.01);
	int MiniMap_Rect_w = cvRound(giFrame.cols*0.11);
	int MiniMap_Rect_h = cvRound(giFrame.cols*0.11);

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

bool AutoTrack::getUIDRefMat()
{
	int UID_Rect_x = cvCeil(giFrame.cols*0.875);
	int UID_Rect_y = cvCeil(giFrame.rows*0.9755);
	int UID_Rect_w = cvCeil(giFrame.cols* 0.0938);
	int UID_Rect_h = cvCeil(UID_Rect_w*0.11);

	if (giFrame.cols == 3440 && giFrame.rows == 1440)
	{
		cv::resize(giFrame, giFrame, cv::Size(2560, 1080));
	}

	if (giFrame.cols == 2560 && giFrame.rows == 1080)
	{
		UID_Rect_x = cvCeil(1920 * 0.875 + 550);
		UID_Rect_y = cvCeil(1080 * 0.9755);
		UID_Rect_w = cvCeil(1920 * 0.0938);
		UID_Rect_h = cvCeil(UID_Rect_w*0.11);
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

bool AutoTrack::getAvatarRefMat()
{
	if (giMiniMapRef.empty())
	{
		err = 21;
		return false;
	}
	int Avatar_Rect_x = cvRound(giMiniMapRef.cols*0.4);
	int Avatar_Rect_y = cvRound(giMiniMapRef.rows*0.4);
	int Avatar_Rect_w = cvRound(giMiniMapRef.cols*0.2);
	int Avatar_Rect_h = cvRound(giMiniMapRef.rows*0.2);

	giAvatarRef = giMiniMapRef(cv::Rect(Avatar_Rect_x, Avatar_Rect_y, Avatar_Rect_w, Avatar_Rect_h));

#ifdef _DEBUG
	cv::namedWindow("Avatar", cv::WINDOW_FREERATIO);
	cv::imshow("Avatar", giAvatarRef);
	cv::waitKey(1);
	std::cout << "Show Avatar" << std::endl;
#endif
	return true;
}

