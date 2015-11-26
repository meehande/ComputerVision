#pragma once
#include "ColourHistogram.h"


class SampleImage
{
public:
	Mat* original_image;
	Mat* back_project_image;
	Mat* back_project_image_display;
	Point2f page_corners[4];
	Mat* perspective_transform_image;
	Mat* eroded_image;
	SampleImage();
	Mat* backProject(Mat* image);
	Mat* geometricTransform(Mat* image);


};