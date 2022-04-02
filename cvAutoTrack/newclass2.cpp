#include "pch.h"
#include "newclass2.h"

bool newclass2::get_value1(cv::Mat& img_scene, cv::Mat& img_object, double& x, double& y, bool& retflag)
{
	retflag = true;

	isContinuity = false;

	cv::Point2d* hisP = _TransformHistory;

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
								lisx.push_back(((minMap.cols / 2 - KeyPointMiniMap[KNN_mTmp[i][0].queryIdx].pt.x) * mapScale + KeyPointSomeMap[KNN_mTmp[i][0].trainIdx].pt.x));
								lisy.push_back(((minMap.rows / 2 - KeyPointMiniMap[KNN_mTmp[i][0].queryIdx].pt.y) * mapScale + KeyPointSomeMap[KNN_mTmp[i][0].trainIdx].pt.y));

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

										lisx.push_back(((minMap.cols / 2 - KeyPointMiniMap[KNN_mTmp[i][0].queryIdx].pt.x) * 0.8667 + KeyPointSomeMap[KNN_mTmp[i][0].trainIdx].pt.x));
										lisy.push_back(((minMap.rows / 2 - KeyPointMiniMap[KNN_mTmp[i][0].queryIdx].pt.y) * 0.8667 + KeyPointSomeMap[KNN_mTmp[i][0].trainIdx].pt.y));
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

								lisx.push_back(((minMap.cols / 2 - KeyPointMiniMap[KNN_mTmp[i][0].queryIdx].pt.x) * 0.8667 + KeyPointSomeMap[KNN_mTmp[i][0].trainIdx].pt.x));
								lisy.push_back(((minMap.rows / 2 - KeyPointMiniMap[KNN_mTmp[i][0].queryIdx].pt.y) * 0.8667 + KeyPointSomeMap[KNN_mTmp[i][0].trainIdx].pt.y));
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
			return true;
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
					lisx.push_back(((img_object.cols / 2.0 - KeyPointMiniMap[KNN_m[i][0].queryIdx].pt.x) * mapScale + KeyPointAllMap[KNN_m[i][0].trainIdx].pt.x));
					lisy.push_back(((img_object.rows / 2.0 - KeyPointMiniMap[KNN_m[i][0].queryIdx].pt.y) * mapScale + KeyPointAllMap[KNN_m[i][0].trainIdx].pt.y));
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
				return true;
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
	retflag = false;
	return {};
}
