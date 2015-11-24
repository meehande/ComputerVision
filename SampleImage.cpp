#include "SampleImage.h"

SampleImage::SampleImage(){
	page_corners[0] = Point2f(1024,1024); //top left
	page_corners[1] = Point2f(1024,1024); //bottom left
	page_corners[2] = Point2f(0,1024); //top right
	page_corners[3] = Point2f(1024,0); //bottomw right

}

Mat* SampleImage::backProject(Mat* image){
	return NULL;
}


Mat* SampleImage::geometricTransform(Mat* image){
	return NULL;
}