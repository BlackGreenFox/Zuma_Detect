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

cv::Scalar frogLow = cv::Scalar(11, 10, 200);
cv::Scalar frogHigh = cv::Scalar(24, 40, 255);

cv::Point okBTNLocation = cv::Point(365, 389);
cv::Vec3b okBTNColour = cv::Vec3b(48, 142, 169);

cv::Point okBTN2Location = cv::Point(363, 205);
cv::Vec3b okBTN2Colour = cv::Vec3b(6, 11, 227);

cv::Point continueBTNLocation = cv::Point(557, 416);
cv::Vec3b continueBTNColour = cv::Vec3b(50, 143, 178);

cv::Point playBTNLocation = cv::Point(594, 460);
cv::Vec3b playBTNColour = cv::Vec3b(20, 207, 161);

std::vector<Ball> balls;
Ball myBall;
cv::Point me;

static bool linesIntersect(cv::Point a, cv::Point b, cv::Point c, cv::Point d)
{
	cv::Point cmP = cv::Point(c.x - a.x, c.y - a.y); //vector from A to C
	cv::Point r = cv::Point(b.x - a.x, b.y - a.y);
	cv::Point s = cv::Point(d.x - c.x, d.y - c.y);

	float CmPxr = cmP.x * r.y - cmP.y * r.x;
	float CmPxs = cmP.x * s.y - cmP.y * s.x;
	float rxs = r.x * s.y - r.y * s.x;

	if (CmPxr == 0.0)
	{
		// Lines are collinear, and so intersect if they have any overlap

		return ((c.x - a.x < 0.0) != (c.x - b.x < 0.0))
			|| ((c.y - a.y < 0.0) != (c.y - b.y < 0.0));
	}

	if (rxs == 0)
		return false; // Lines are parallel.

	float rxsr = 1.0 / rxs;
	float t = CmPxs * rxsr;
	float u = CmPxr * rxsr;

	return (t >= 0.0) && (t <= 1.0) && (u >= 0.0) && (u <= 1.0);
}

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

void Click(cv::Point position, HWND hWND) {
	POINT point;
	point.x = position.x; point.y = position.y;
	if (ClientToScreen(hWND, &point)) {
		SetCursorPos(point.x, point.y);
		//std::cout << "Target at: " << point.x << " " << point.y << std::endl;
		//std::cin.clear();
		//std::cin.get();
		INPUT iNPUT = { 0 };
		iNPUT.type = INPUT_MOUSE;
		iNPUT.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
		SendInput(1, &iNPUT, sizeof(iNPUT));
		//ZeroMemory(&iNPUT, sizeof(iNPUT));
		iNPUT.type = INPUT_MOUSE;
		iNPUT.mi.dwFlags = MOUSEEVENTF_LEFTUP;
		SendInput(1, &iNPUT, sizeof(iNPUT));
	}
}

void GetFrog(cv::Mat img, cv::Scalar low, cv::Scalar high) {
	cv::Mat mask;
	cv::inRange(img, low, high, mask);
	std::vector<std::vector<cv::Point> > contours;
	cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
	std::vector<std::vector<cv::Point> > contours_poly(contours.size());
	std::vector<cv::Rect> boundRect(contours.size());
	for (size_t i = 0; i < contours.size(); i++)
	{
		approxPolyDP(contours[i], contours_poly[i], 3, true);
		boundRect[i] = boundingRect(contours_poly[i]);
		//minEnclosingCircle(contours_poly[i], centers[i], radius[i]);
	}

	for (size_t i = 0; i < contours.size(); i++)
	{
		if (boundRect[i].area() > 100) {
			me = cv::Point(boundRect[i].x, boundRect[i].y);
			return;
		}
	}

	//if (me.x == 0 && me.y == 0) {
	//	Sleep(100);
	//	GetFrog(img, low, high);
	//}

	//cv::waitKey();
}

int main()
{

	LPCWSTR windowTitle = L"Zuma Deluxe 1.0";
	HWND hWND = FindWindow(NULL, windowTitle);
	cv::namedWindow("Zuma Bot", cv::WINDOW_NORMAL);

	bool running = false;
	int key = 0;


	while (key != 27)
	{
		if (GetAsyncKeyState(VK_NUMPAD0)) {
			running = !running;
		}

		HWND temp = GetForegroundWindow();

		if (temp != hWND) {
			Sleep(10);
			continue;
		}

		cv::Mat target = GetMat(hWND);
		cv::Mat background;
		target.copyTo(background);
		cv::cvtColor(target, target, cv::COLOR_BGR2HSV);  
		cv::rectangle(target, cv::Point(0, 0), cv::Point(640, 30), CV_RGB(0, 0, 0), cv::FILLED); 
	
	
		if (running) {
			cv::Vec3b colour = target.at<cv::Vec3b>(okBTNLocation);
			if (colour.val[0] == okBTNColour.val[0] && colour.val[1] == okBTNColour.val[1] && colour.val[2] == okBTNColour.val[2]) {
				Click(okBTNLocation, hWND);
				Sleep(1000);
			}

			colour = target.at<cv::Vec3b>(okBTN2Location);
			if (colour.val[0] == okBTN2Colour.val[0] && colour.val[1] == okBTN2Colour.val[1] && colour.val[2] == okBTN2Colour.val[2]) {
				Click(okBTN2Location, hWND);
				Sleep(1000);
			}

			colour = target.at<cv::Vec3b>(continueBTNLocation);
			if (colour.val[0] == continueBTNColour.val[0] && colour.val[1] == continueBTNColour.val[1] && colour.val[2] == continueBTNColour.val[2]) {
				Click(continueBTNLocation, hWND);
				Sleep(1000);
			}

			colour = target.at<cv::Vec3b>(playBTNLocation);

			if (colour.val[0] == playBTNColour.val[0] && colour.val[1] == playBTNColour.val[1] && colour.val[2] == playBTNColour.val[2]) {
				Click(playBTNLocation, hWND);
				Sleep(1000);
			}
		
		}
	
	
		bool foundFrog = false;
		GetFrog(target, frogLow, frogHigh); //get my/frogs location
		if (me.x != 0 && me.y != 0)
			foundFrog = true;


		balls.clear();
		GetBalls(target, yellowLow, yellowHigh, Color::YELLOW); //find yellow balls
		GetBalls(target, blueLow, blueHigh, Color::BLUE); //find blue balls
		GetBalls(target, redLow, redHigh, Color::RED); //find red balls
		GetBalls(target, greenLow, greenHigh, Color::GREEN); //find green balls
		GetBalls(target, purpleLow, purpleHigh, Color::PURPLE); // find purple balls
		DrawBalls(background);

		for (size_t i = 0; i < balls.size() && foundFrog; ++i) {
			if (balls[i].color == myBall.color) {
				bool collision = false;
				for (size_t j = 0; j < balls.size(); ++j) {
					if (i == j)
						continue;
					cv::Point topLeft = balls[j].rect.tl();
					cv::Point bottomRight = balls[j].rect.br();
					cv::Point topRight = cv::Point(bottomRight.x, topLeft.y);
					cv::Point bottomLeft = cv::Point(topLeft.x, bottomRight.y);
					if (linesIntersect(balls[i].position, me, topLeft, topRight)) {
						collision = true;
					}
					else if (linesIntersect(balls[i].position, me, topRight, bottomRight)) {
						collision = true;
					}
					else if (linesIntersect(balls[i].position, me, bottomRight, bottomLeft)) {
						collision = true;
					}
					else if (linesIntersect(balls[i].position, me, bottomLeft, topRight)) {
						collision = true;
					}

				}
				if (!collision && running) {
					cv::line(background, balls[i].position, me, CV_RGB(0, 0, 0), 3);
					Click(balls[i].position, hWND);
					Sleep(500);
					break;
				}
			}
		}

		cv::imshow("Zuma Bot", background);
		key = cv::waitKey(5);
	}
}
