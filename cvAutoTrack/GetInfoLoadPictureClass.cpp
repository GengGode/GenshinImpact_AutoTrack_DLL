#include "pch.h"
#include "GetInfoLoadPictureClass.h"

GetInfoLoadPictureClass::GetInfoLoadPictureClass(const Logger& loggerService)
{
	logger = loggerService;
}

bool GetInfoLoadPictureClass::get(char* path, int& uid, double& x, double& y, double& a)
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
		err = 202;
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
					lisx.push_back(((img_object.cols / 2 - KeyPointMiniMap[KNN_m[i][0].queryIdx].pt.x) * mapScale + KeyPointAllMap[KNN_m[i][0].trainIdx].pt.x));
					lisy.push_back(((img_object.rows / 2 - KeyPointMiniMap[KNN_m[i][0].queryIdx].pt.y) * mapScale + KeyPointAllMap[KNN_m[i][0].trainIdx].pt.y));
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
