#pragma once

#include "Colors.h"
#include <opencv2/core/types.hpp>

class Ball
{
public:
	Color color;
	cv::Point point;
	cv::Rect rect;

	Ball();
	Ball(Color color, cv::Rect rect, int x, int y);
};

