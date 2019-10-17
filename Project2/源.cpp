#include<opencv2/opencv.hpp>
#include<vector>
using namespace cv;
using namespace std;

vector<Vec3f>circles;//圆
vector<vector<Point>>contours;
vector<Point>sanjiao;
vector<Point>approx;
vector<Point>squares;
Mat srcImg, tempImg, gray;

int main()
{

	srcImg = imread("e:\\99.jpg");
	if (!srcImg.data)
	{
		cout << "no Img" << endl;
		return -1;
	}
	resize(srcImg, tempImg, Size(srcImg.cols / 2, srcImg.rows / 2), 0, 0);
	cvtColor(tempImg, gray, COLOR_RGB2GRAY);
	medianBlur(gray, gray, 3);//中值滤波
	HoughCircles(gray, circles, HOUGH_GRADIENT, 1, srcImg.rows / 20, 100, 60, 0, 0);
	Mat thresh;
	threshold(gray, thresh, 0, 255, THRESH_OTSU);//二值化，大于0的都是黑色 ,0都是白色背景
	//imshow("二值化", thresh);
	Mat srcImg1;
	srcImg1 = thresh.clone();
	findContours(srcImg1, contours, RETR_TREE,CHAIN_APPROX_NONE);//找最外层轮廓，（点集）
	Mat dstImg(srcImg1.rows, srcImg1.cols, CV_8UC3, Scalar(255, 255, 255));

	//找圆
	for (size_t i = 0; i < circles.size(); i++)//画圆
	{
		Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
		int radius = cvRound(circles[i][2]);
		circle(dstImg, center, radius, Scalar(0, 255, 0), 5, 8, 0);
		circle(dstImg, center, 3, Scalar(0, 255, 0), -1);
		cout << "圆心" << i + 1 << center << endl;
		double length = arcLength(contours[i], true);
		drawContours(dstImg, contours, i, Scalar(0, 0, 255), 2, 8, Mat(), 0, Point());
		cout << "周长: " << length << endl;
		double area = contourArea(contours[i]);
		cout << " 面积: " << area << endl;
	}

	//???找轮廓
	for (size_t i = 0; i < contours.size(); i++)
	{
		approxPolyDP(contours[i], approx, arcLength(Mat(contours[i]), true)*0.02, true);
		if (approx.size() == 4 && fabs(contourArea(Mat(approx))) > 1000 && isContourConvex(Mat(approx)))
		{
			double minDist = 1e10;
			for (int i = 0; i < 4; i++)
			{
				Point side = approx[i] - approx[(i + 1) % 4];
				double squaredSideLength = side.dot(side);
				minDist = min(minDist, squaredSideLength);
			}
			if (minDist < 50)
				break;
			for (int i = 0; i < 4; i++)
				squares.push_back(Point(approx[i].x, approx[i].y));
			drawContours(dstImg, contours, i, Scalar(0, 0, 255), 4);
		}
		approxPolyDP(contours[i], approx, arcLength(Mat(contours[i]), true)*0.1, true);
		if (approx.size() == 3 && fabs(contourArea(Mat(approx))) > 1000 && isContourConvex(Mat(approx)))
		{
			double minDist = 1e10;
			for (int i = 0; i < 3; i++)
			{
				Point side = approx[i] - approx[(i + 1) % 3];
				double squaredSideLength = side.dot(side);
				minDist = min(minDist, squaredSideLength);
			}
			if (minDist < 50)
				break;
			for (int i = 0; i < 3; i++)
				sanjiao.push_back(Point(approx[i].x, approx[i].y));
		}
		drawContours(dstImg, contours, i, Scalar(0, 0, 255), 3);
	}

//找三角形
for (size_t i = 0; i < sanjiao.size(); i += 3)
	{
		Point center;
		center.x = (sanjiao[i].x + sanjiao[i + 1].x + sanjiao[i + 2].x) / 3;
		center.y = (sanjiao[i].y + sanjiao[i + 1].y + sanjiao[i + 2].y) / 3;
		line(dstImg, sanjiao[i], sanjiao[i + 1], Scalar(255, 0, 0), 4);
		line(dstImg, sanjiao[i + 1], sanjiao[i + 2], Scalar(255, 0, 0), 4);
		line(dstImg, sanjiao[i], sanjiao[i + 2], Scalar(255, 0, 0), 4);
		cout << "三角形中心" << (i + 4) / 3 << center << endl;
		circle(dstImg, center, 3, Scalar(255, 0, 0), -1);
		double length = arcLength(contours[i], true);
		drawContours(dstImg, contours, i, Scalar(0, 0, 255), 2, 8, Mat(), 0, Point());//画轮廓
		cout << "周长: " << length << endl;
		double area = contourArea(contours[i]);
		cout << " 面积: " << area << endl;
	}

//找矩形,不能识别超过4边
for (size_t i = 0; i < squares.size(); i += 4)
	{
		Point center;
		center.x = (squares[i].x + squares[i + 2].x) / 2;
		center.y = (squares[i].y + squares[i + 2].y) / 2;
		line(dstImg, squares[i], squares[i + 1], Scalar(0, 0, 255), 4);
		line(dstImg, squares[i + 1], squares[i + 2], Scalar(0, 0, 255), 4);
		line(dstImg, squares[i + 2], squares[i + 3], Scalar(0, 0, 255), 4);
		line(dstImg, squares[i + 3], squares[i], Scalar(0, 0, 255), 4);
		cout << "矩形中心" << (i + 5) / 4 << center << endl;
		circle(dstImg, center, 3, Scalar(0, 0, 255), -1);
		double length = arcLength(contours[i/2], true);
		Mat img = dstImg.clone();
	   drawContours(dstImg, contours, i/2, Scalar(0,0 , 255), 2, 8, Mat(), 0, Point());//test
		cout << "周长: " << length << endl;
		double area = contourArea(contours[i/2]);
		cout << " 面积: " << area << endl;
	}

	

	imshow("123", dstImg);
	waitKey(0);
	return 0;
}
