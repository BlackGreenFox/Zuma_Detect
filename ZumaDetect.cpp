#include <Windows.h>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "Ball.h";
#include <iostream>

cv::Scalar yellowLow = cv::Scalar(25, 130, 180);
cv::Scalar yellowHigh = cv::Scalar(45, 255, 255);
cv::Scalar greenLow = cv::Scalar(46, 40, 40);
cv::Scalar greenHigh = cv::Scalar(70, 255, 255);
cv::Scalar blueLow = cv::Scalar(100, 150, 150);
cv::Scalar blueHigh = cv::Scalar(140, 255, 255);
cv::Scalar purpleLow = cv::Scalar(148, 117, 89);
cv::Scalar purpleHigh = cv::Scalar(152, 255, 255);
cv::Scalar redLow = cv::Scalar(170, 140, 160);
cv::Scalar redHigh = cv::Scalar(180, 255, 255);


std::vector<Ball> balls;

void GetBalls(cv::Mat img, cv::Scalar low, cv::Scalar high, Color color) 
{
	cv::Mat mask;
	cv::inRange(img, low, high, mask);

	std::vector <std::vector <cv::Point>> contours;
	cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

	for (size_t i = 0; i < contours.size(); ++i)
	{
		cv::Rect boundRect = cv::boundingRect(contours[i]);
		if (boundRect.area() > 350 && (boundRect.width < 70 || boundRect.height < 70))
			balls.emplace_back(color, boundRect, boundRect.x + boundRect.width/2, boundRect.y + boundRect.height / 2);
			 
	}
}

void DrawBalls(cv::Mat img) {
	for (size_t i = 0; i < balls.size(); ++i)
	{
		switch (balls[i].color)
		{
		case RED:
			cv::rectangle(img, balls[i].rect.tl(), balls[i].rect.br(), CV_RGB(255, 0,0), 2);
			break;
		case BLUE:
			cv::rectangle(img, balls[i].rect.tl(), balls[i].rect.br(), CV_RGB(0, 0, 255), 2);
			break;
		case GREEN:
			cv::rectangle(img, balls[i].rect.tl(), balls[i].rect.br(), CV_RGB(0, 255, 0), 2);
			break;
		case YELLOW:
			cv::rectangle(img, balls[i].rect.tl(), balls[i].rect.br(), CV_RGB(255, 255, 0), 2);
			break;
		case PURPLE:
			cv::rectangle(img, balls[i].rect.tl(), balls[i].rect.br(), CV_RGB(128, 0, 128), 2);
			break;
		}
	}
}

cv::Mat GetMat(HWND hWND) 
{

	HDC deviceContext = GetDC(hWND);
	HDC memoryDeviceContext = CreateCompatibleDC(deviceContext);

	RECT windowRect;
	GetClientRect(hWND, &windowRect);

	int height = windowRect.bottom;
	int width = windowRect.right;


	HBITMAP bitmap = CreateCompatibleBitmap(deviceContext, width, height);

	SelectObject(memoryDeviceContext, bitmap);

	BitBlt(memoryDeviceContext, 0,0, width, height, deviceContext, 0,0, SRCCOPY);


	BITMAPINFOHEADER bi;
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = width;
	bi.biHeight = -height;
	bi.biPlanes = 1;
	bi.biBitCount = 32;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 1;
	bi.biYPelsPerMeter = 2;
	bi.biClrUsed = 3;
	bi.biClrImportant = 4;


	cv::Mat mat = cv::Mat(height, width, CV_8UC4);

	GetDIBits(memoryDeviceContext, bitmap, 0, height, mat.data,(BITMAPINFO*)&bi, DIB_RGB_COLORS);

	//Clear up
	DeleteObject(bitmap);
	DeleteDC(memoryDeviceContext);
	ReleaseDC(hWND, deviceContext);

	return mat;
}

int main()
{

	LPCWSTR windowTitle = L"Zuma Deluxe 1.0";
	HWND hWND = FindWindow(NULL, windowTitle);
	while (!hWND)
	{
		std::system("cls");
		std::cout << "Start the game" << std::endl;

		hWND = FindWindow(NULL, windowTitle);
		Sleep(100);
	}


	cv::namedWindow("Output", cv::WINDOW_NORMAL);


	while (true)
	{

	cv::Mat target = GetMat(hWND);

	cv::Mat background;
	target.copyTo(background);

	cv::cvtColor(target, target, cv::COLOR_BGR2HSV);

	cv::rectangle(target, cv::Point(0,0), cv::Point(640, 30), (0,0,0), cv::FILLED);

	balls.clear();
	GetBalls(target, yellowLow, yellowHigh, Color::YELLOW);
	GetBalls(target, blueLow, blueHigh, Color::BLUE);
	GetBalls(target, redLow, redHigh, Color::RED);
	GetBalls(target, greenLow, greenHigh, Color::GREEN);
	GetBalls(target, purpleLow, purpleHigh, Color::PURPLE);
	DrawBalls(background);

	
	cv::imshow("Output", background);
	cv::waitKey(30);
	}
}
