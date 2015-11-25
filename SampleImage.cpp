#include "SampleImage.h"

SampleImage::SampleImage(){
	page_corners[0] = Point2f(0,0); //top left
	page_corners[1] = Point2f(0,0); //top right
	page_corners[2] = Point2f(0,0); //bottom right
	page_corners[3] = Point2f(0,0); //bottomw left

}

Mat* SampleImage::backProject(Mat* image){
	return NULL;
}


Mat* SampleImage::geometricTransform(Mat* image){
	return NULL;
}