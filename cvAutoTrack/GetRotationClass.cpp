#include "pch.h"
#include "GetRotationClass.h"

GetRotationClass::GetRotationClass(const Logger& loggerService)
{
	logger = loggerService;

	//wForAfter.append(this, &AutoTrackCore::getGengshinImpactWnd, 101);
	//wForAfter.append(this, &AutoTrackCore::getGengshinImpactRect, 102);
	//wForAfter.append(this, &AutoTrackCore::getGengshinImpactScreen, 103);
}

bool GetRotationClass::get( double& a)
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

	cv::circle(Alpha, cv::Point(Alpha.cols / 2, Alpha.rows / 2), min(Alpha.cols / 2, Alpha.rows / 2) * 1.21, cv::Scalar(0, 0, 0), min(Alpha.cols / 2, Alpha.rows / 2) * 0.42);
	cv::circle(Alpha, cv::Point(Alpha.cols / 2, Alpha.rows / 2), min(Alpha.cols / 2, Alpha.rows / 2) * 0.3, cv::Scalar(0, 0, 0), -1);


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
