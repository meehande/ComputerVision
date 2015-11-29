/*
 * This code is provided as part of "A Practical Introduction to Computer Vision with OpenCV"
 * by Kenneth Dawson-Howe � Wiley & Sons Inc. 2014.  All rights reserved.
 */
//#include "Utilities.h"
#include "SampleImage.h"
#include <iostream>
#include <fstream>
using namespace std;
 
Mat back_projection(Mat* image, Mat sample_image);
double recall(double TP, double FN);
double precision(double TP, double FP);
double accuracy(double TP, double TN, double nSamples);
double specificity(double TN, double FP);
double f1(double TP, double FP, double FN);


#define SCENE_IMAGES_INDEX  0
#define PAGE_IMAGES_INDEX  50
int main(int argc, const char** argv)
{
//LOADING SAMPLEIMAGE ARRAYS
#pragma region INIT

	int ground_truth_pageNums[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,2,3,5,4,7,9,8,7,11,13,12,2};

	char* file_location = "Books/";
	char* image_files[] = {
		"BookView01.jpg",
		"BookView02.jpg",
		"BookView03.jpg",
		"BookView04.jpg",
		"BookView05.jpg",
		"BookView06.jpg",
		"BookView07.jpg",
		"BookView08.jpg",
		"BookView09.jpg",
		"BookView10.jpg",
		"BookView11.jpg",
		"BookView12.jpg",
		"BookView13.jpg",
		"BookView14.jpg",
		"BookView15.jpg",
		"BookView16.jpg",
		"BookView17.jpg",
		"BookView18.jpg",
		"BookView19.jpg",
		"BookView20.jpg",
		"BookView21.jpg",
		"BookView22.jpg",
		"BookView23.jpg",
		"BookView24.jpg",
		"BookView25.jpg"
    };
	char* page_files[] = {
		"Page01.jpg",
		"Page02.jpg",
		"Page03.jpg",
		"Page04.jpg",
		"Page05.jpg",
		"Page06.jpg",
		"Page07.jpg",
		"Page08.jpg",
		"Page09.jpg",
		"Page10.jpg",
		"Page11.jpg",
		"Page12.jpg",
		"Page13.jpg"
	};
	char* back_project_sample_file = "BlueBoxes.png";

	

	// Load images to test 
	int number_of_images = sizeof(image_files)/sizeof(image_files[0]);
	//cout << "N " << number_of_images << endl;
	SampleImage* images = new SampleImage[number_of_images];
	Mat* image = new Mat[number_of_images];
	for (int file_no=0; (file_no < number_of_images); file_no++)
	{
		string filename(file_location);
		filename.append(image_files[file_no]);
		image[file_no] = imread(filename, -1);
		images[file_no].original_image = &image[file_no];
		images[file_no].ground_truth_pageNum = ground_truth_pageNums[file_no];
		//imshow(to_string(file_no), *images[0].original_image);
		if (image[file_no].empty())
		{
			cout << "Could not open " << image[file_no] << endl;
			return -1;
		}
	}

	// Load pages to recognise against
	int number_of_pages = sizeof(page_files)/sizeof(page_files[0]);
	Mat* pages = new Mat[number_of_pages];
	SampleImage* page_images = new SampleImage[number_of_pages];
	for (int file_no=0; (file_no < number_of_pages); file_no++)
	{
		string filename(file_location);
		filename.append(page_files[file_no]);
		pages[file_no] = imread(filename, -1);
		page_images[file_no].original_image = &pages[file_no];
		if (pages[file_no].empty())
		{
			cout << "Could not open " << pages[file_no] << endl;
			return -1;
		}
	}

	// Load sample of blue for back projection
	Mat blue_sample;
	string filename(file_location);
	filename.append(back_project_sample_file);
	blue_sample = imread(filename, -1);
	if (blue_sample.empty())
	{
		cout << "Could not open " << back_project_sample_file<< endl;
		return -1;
	}

#pragma endregion

//BACK PROJECTION (TAKEN FROM HISTOGRAMSDEMO FUNCTION)
	Mat* back_projection_probabilities_display = new Mat[number_of_images + number_of_pages + 1];
	for(int i = 0; i < number_of_images ; i ++){
		back_projection_probabilities_display[i] = back_projection(images[i].original_image, blue_sample);
		images[i].back_project_image_display = &back_projection_probabilities_display[i];
		//imshow(to_string(i), *images[i].back_project_image_display);
	}

//OTSU THRESHOLDING IMAGES
	int current_threshold = 128;
	int max_threshold = 255;
	Mat temp_image;
	Mat* otsu_images = new Mat[number_of_images];
	for (int i=0; i< number_of_images; i++){
		Mat otsu_binary_image;
		temp_image = *images[i].back_project_image_display;
		cvtColor(temp_image,temp_image, CV_BGR2GRAY);
		threshold(temp_image,otsu_binary_image,current_threshold,max_threshold,
			THRESH_BINARY | THRESH_OTSU);
		Mat otsu_binary_image_display;
		cvtColor(otsu_binary_image, otsu_images[i], CV_GRAY2BGR);
		images[i].otsu_image = &otsu_images[i];
		//imshow("otsu", otsu_binary_image);
		//imshow("otsu display", otsu_images[i]);
		//imshow(to_string(i), *images[i].otsu_image);
	}
	
//FINDING CORNER POINTS FOR GEOMETRIC TRANSFORMATION
#pragma region CORNER_POINTS
	for(int i =0; i<number_of_images ; i++){
		Mat test_image = *images[i].otsu_image;
		int current[3];
	//top left 
		//search row-by-row from (0,0) - first white pixel found
		for (int row=0; row < test_image.rows; row++){
			for (int col = 320; col < test_image.cols; col++){//choose pixel
				if(images[i].page_corners[0].x == 0 && images[i].page_corners[0].y ==0) {
					current[0] = test_image.at<Vec3b>(row,col)[0];
					current[1] = test_image.at<Vec3b>(row,col)[1];
					current[2] = test_image.at<Vec3b>(row,col)[2];
					if(current[0] == 255 && current[1] == 255 && current[2] == 255){ //if white					
						images[i].page_corners[0].x = col;
						images[i].page_corners[0].y = row;
					}
				}
			}
		}
		//top right
		//search column-by-column from (0,0) - first pixel found
		for (int col = test_image.cols-1; col > 0; col--){
			for (int row=0; row < test_image.rows; row++){
			//choose pixel
				if(images[i].page_corners[1].x == 0 && images[i].page_corners[1].y ==0) {
					current[0] = test_image.at<Vec3b>(row,col)[0];
					current[1] = test_image.at<Vec3b>(row,col)[1];
					current[2] = test_image.at<Vec3b>(row,col)[2];
					if(current[0] == 255 && current[1] == 255 && current[2] == 255){
						images[i].page_corners[1].x = col;
						images[i].page_corners[1].y = row;
					}
				}
			}
		}
		//bottom right
		//search row-by-row from (max_col, max_row) - first pixel found
		for (int row=test_image.rows-1; row >0; row--){
			for (int col = test_image.cols-1; col > 300; col--){//choose pixel
				if(images[i].page_corners[2].x == 0 && images[i].page_corners[2].y ==0) {
					current[0] = test_image.at<Vec3b>(row,col)[0];
					current[1] = test_image.at<Vec3b>(row,col)[1];
					current[2] = test_image.at<Vec3b>(row,col)[2];		
					if(current[0] == 255 && current[1] == 255 && current[2] == 255){
						images[i].page_corners[2].x = col;
						images[i].page_corners[2].y = row;
					}
				}
			}
		}
		//bottom left
		//search column-by-column from (0,max_row) - first pixel found
		for (int col = 300; col < test_image.cols; col++){
			for (int row=test_image.rows-1; row>0; row--){//choose pixel
				if(images[i].page_corners[3].x == 0 && images[i].page_corners[3].y ==0) {
					current[0] = test_image.at<Vec3b>(row,col)[0];
					current[1] = test_image.at<Vec3b>(row,col)[1];
					current[2] = test_image.at<Vec3b>(row,col)[2];
					if(current[0] == 255 && current[1] == 255 && current[2] == 255){
						images[i].page_corners[3].x = col;
						images[i].page_corners[3].y = row;
					}
				}
			}
		}
	}
	
#pragma endregion

//PERSPECTIVE GEOMETRIC TRANSFORMATION
    Mat perspective_matrix( 2, 4, CV_32FC1 );
	Point2f destination_points[4];
	Mat* perspective_warped_image = new Mat[number_of_images+number_of_pages+1];
	for (int i = 0; i<number_of_images; i++){	
		temp_image = *images[i].original_image;
		perspective_matrix = Mat::zeros(temp_image.rows, temp_image.cols, temp_image.type() );	
		destination_points[0] = Point2f(0,0);
		destination_points[1] = Point2f(temp_image.cols-1,0);
		destination_points[2] = Point2f(temp_image.cols-1, temp_image.rows-1);
		destination_points[3] = Point2f(0, temp_image.rows-1);	
		Point2f source_points[] = {images[i].page_corners[0], images[i].page_corners[1], images[i].page_corners[2], images[i].page_corners[3]};
		// Get the Perspective Transform Matrix 
		perspective_matrix = getPerspectiveTransform( source_points, destination_points);
		// Apply the Perspective Transform just found to the src image
		warpPerspective(temp_image,perspective_warped_image[i],perspective_matrix,perspective_warped_image[i].size() );
		resize(perspective_warped_image[i], perspective_warped_image[i], pages[0].size());
		images[i].perspective_transform_image = &perspective_warped_image[i];
	}

#pragma region EROSION
//ERODE IMAGES
	Mat* eroded_images = new Mat[number_of_images + number_of_pages+1];
	for (int i = 0; i < number_of_images; i++){
		temp_image = *images[i].perspective_transform_image;
		Mat binary_image;
		int current_threshold = 128;
		int max_threshold = 255;
		threshold(temp_image,binary_image,current_threshold,max_threshold,THRESH_BINARY);
		Mat gray_pcb_image;
		cvtColor(temp_image, gray_pcb_image, CV_BGR2GRAY);
		threshold(gray_pcb_image,binary_image,current_threshold,max_threshold,
			THRESH_BINARY | THRESH_OTSU);
		Mat ten_by_ten_element(10,10,CV_8U,Scalar(1));
		erode(binary_image,eroded_images[i],ten_by_ten_element);
		images[i].eroded_image = &eroded_images[i];
		//imshow(to_string(i), *images[i].eroded_image );
	}
//ERODE PAGES
	for (int i = number_of_images; i < (number_of_images+number_of_pages ); i++){
 		temp_image = *page_images[i - number_of_images].original_image;
		Mat binary_image;
		int current_threshold = 128;
		int max_threshold = 255;
		threshold(temp_image,binary_image,current_threshold,max_threshold,THRESH_BINARY);
		Mat gray_pcb_image;
		cvtColor(temp_image, gray_pcb_image, CV_BGR2GRAY);
		threshold(gray_pcb_image,binary_image,current_threshold,max_threshold,
			THRESH_BINARY | THRESH_OTSU);
		Mat ten_by_ten_element(10,10,CV_8U,Scalar(1));
		erode(binary_image,eroded_images[i],ten_by_ten_element);
		page_images[i- number_of_images].eroded_image = &eroded_images[i];
		//imshow(to_string(i), *page_images[i].eroded_image );
	}
#pragma endregion

//RESIZE PAGE IMAGE
	for(int i = 0; i<number_of_pages; i++){
		resize(*page_images[i].eroded_image,*page_images[i].eroded_image, pages[0].size());
		//imshow(to_string(i), *page_images[i].eroded_image);
	}

//COMPARE TO CLASSIFY
	Mat* comparison = new Mat() ;
	double most_similar_page = 0;
	int most_like;//keep track of current max similarity 
	for(int j = 0; j<number_of_images; j++){ //pick image
		most_like = 0;
		most_similar_page = 0;
		for(int i = 0; i<number_of_pages; i++){ //compare image to every page
			images[0].eroded_image->copyTo(*comparison);
			compare(*images[j].eroded_image, *page_images[i].eroded_image, *comparison, CMP_EQ);
			Mat vComparison;
			reduce(*comparison, vComparison, 1, CV_REDUCE_AVG);
			Mat sComparison;
			reduce(vComparison, sComparison, 0, CV_REDUCE_AVG);
			//reduce(*comparison, vComparison, 0, CV_REDUCE_AVG);
			int greater_than;
			if((int)sComparison.data[0] > most_like){
				most_like = (int)sComparison.data[0];
				most_similar_page = i+1;
			}
		}
		images[j].detected_pageNum = most_similar_page;
		//cout << "choice " << j << " " <<  most_similar_page << endl;
	}

	//True Positive =a page is visible and recognised correctly
	//False Positive = an incorrectly recognised page, where EITHER no page  was visible OR a different page was visible
	//True Negative = no page visible and no page identified
	//False Negative = a page is visible but no page was found
	double TP=0,FP=0,TN=0,FN=0;
	for (int i = 0; i<number_of_images; i++){
		if(images[i].ground_truth_pageNum == images[i].detected_pageNum)
			TP++;
		else{
			FP++;
			cout << "FP: " << i << endl;
		}
	}
	cout << "Correct: " << TP  << "/25\n" << endl;
	cout << "Incorrect: " << FP << "/25\n" << endl;
	cout << "Precision: " << precision(TP, FP) << endl;
	cout << "Recall: " << recall(TP,FN) << endl;
	cout << "Specificity: " << specificity(TN,FP) << endl;
	cout << "Accuracy: " << accuracy(TP,TN,number_of_images) <<endl;
	cout << "F1: " << f1(TP,FP,FN) << endl;

	
	int choice;
	while (true)
	{

		//imshow("test1", image[0]);
		choice = cvWaitKey();
	
	} 
	return 0;
}

Mat back_projection(Mat* image, Mat sample_image){
	Mat hls_image;
	Mat image_backproject;
	Mat back_projection_probabilities;
	Mat back_projection_probabilities_display;
	image_backproject = *image;
	cvtColor(sample_image, hls_image, CV_BGR2HLS); //convert blue sample to hls and store in hls
	ColourHistogram histogram3D(hls_image,8);
	histogram3D.NormaliseHistogram();
	cvtColor(image_backproject, hls_image, CV_BGR2HLS);
	back_projection_probabilities = histogram3D.BackProject(hls_image);
	back_projection_probabilities = StretchImage(back_projection_probabilities);		
	cvtColor(back_projection_probabilities, back_projection_probabilities_display, CV_GRAY2BGR);
	return back_projection_probabilities_display;
}

/*
* Recall = TP/(TP+FN)
* Precision = TP/(TP+FP)
* Accuracy = (TP +TN)/(Total Samples)
* Specificity = TN/(FP+TN)
* Fβ = (1+β2)* [(Precision*Recall)] / [( β2*Precision)+Recall]
*/

double recall(double TP, double FN){
	return TP/(TP + FN) *100;
}
double precision(double TP, double FP){
	return TP/(TP + FP) *100;
}
double accuracy(double TP, double TN, double nSamples){
	return (TP + TN)/nSamples *100;
}
double specificity(double TN, double FP){
	return TN/(FP + TN) *100;
}
double f1(double TP, double FP, double FN){
	return ( 2*precision(TP,FP)*recall(TP,FN) ) / (precision(TP,FP) + recall(TP,FN)) ;
}