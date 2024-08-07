#pragma once

#include "Colors.h"
#include <opencv2/core/types.hpp>

class Ball
{
public:
	Color color;
	cv::Point position;
	cv::Rect rect;

	Ball();
	Ball(Color _color, cv::Rect _rect, int _x, int _y);
};

