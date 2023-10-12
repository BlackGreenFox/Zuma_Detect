#include "Ball.h"

Ball::Ball()
{

}

Ball::Ball(Color color, cv::Rect rect, int x, int y)
{
	this->color = color;
	this->rect = rect;
	this->point = cv::Point(x,y);

}