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
Point2f find_top_left(Mat test_image, Point2f initial_xy);
Point2f find_top_right(Mat test_image, Point2f initial_xy);
Point2f find_bottom_right(Mat test_image, Point2f initial_xy);
Point2f find_bottom_left(Mat test_image, Point2f initial_xy);

#define SCENE_IMAGES_INDEX  0
#define PAGE_IMAGES_INDEX  50
int main(int argc, const char** argv)
{

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
	char* back_project_sample_file = "BlueBookPixels.png";

	

	// Load images to test 
	int number_of_images = sizeof(image_files)/sizeof(image_files[0]);
	cout << "N " << number_of_images << endl;
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
	//imshow(to_string(0), *images[0].original_image);
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
	for(int i = number_of_images; i < (number_of_images + number_of_pages); i++){
		back_projection_probabilities_display[i] = back_projection(page_images[i-number_of_images].original_image, blue_sample);
		page_images[i-number_of_images].back_project_image_display = &back_projection_probabilities_display[i];
		//imshow(to_string(i), *page_images[i-number_of_images].back_project_image_display);
	}
	
//FINDING CORNER POINTS FOR GEOMETRIC TRANSFORMATION
#pragma region CORNER_POINTS
	for(int i =0; i<number_of_images ; i++){
		Mat test_image = *images[i].back_project_image_display;
		int current[3];
	//top left
		/*cout << "rows: " << test_image.rows << endl;
		cout << "cols: " << test_image.cols << endl;
		cout << "test function 1 " << i << " " <<  find_top_left(*images[i].back_project_image_display, images[i].page_corners[0])<< endl;*/
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
		/*cout << "test function 2 " << i << " " <<  find_top_left(*images[i].back_project_image_display, images[i].page_corners[0])<< endl;
		cout << "original loop " << i << " " << images[i].page_corners[0]<< endl << endl;*/
			//top right
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
	//pages
	for(int i =0; i<number_of_pages ; i++){
		Mat test_image = *page_images[i].back_project_image_display;
		int current[3];
	//top left
		/*cout << "rows: " << test_image.rows << endl;
		cout << "cols: " << test_image.cols << endl;
		cout << "test function 1 " << i << " " <<  find_top_left(*images[i].back_project_image_display, images[i].page_corners[0])<< endl;*/
		for (int row=0; row < test_image.rows; row++){
			for (int col = 320; col < test_image.cols; col++){//choose pixel
				if(page_images[i].page_corners[0].x == 0 && page_images[i].page_corners[0].y ==0) {
					current[0] = test_image.at<Vec3b>(row,col)[0];
					current[1] = test_image.at<Vec3b>(row,col)[1];
					current[2] = test_image.at<Vec3b>(row,col)[2];
					if(current[0] == 255 && current[1] == 255 && current[2] == 255){ //if white					
						page_images[i].page_corners[0].x = col;
						page_images[i].page_corners[0].y = row;
					}
				}
			}
		}
		/*cout << "test function 2 " << i << " " <<  find_top_left(*images[i].back_project_image_display, images[i].page_corners[0])<< endl;
		cout << "original loop " << i << " " << images[i].page_corners[0]<< endl << endl;*/
			//top right
		for (int col = test_image.cols-1; col > 0; col--){
			for (int row=0; row < test_image.rows; row++){
			//choose pixel
				if(page_images[i].page_corners[1].x == 0 && page_images[i].page_corners[1].y ==0) {
					current[0] = test_image.at<Vec3b>(row,col)[0];
					current[1] = test_image.at<Vec3b>(row,col)[1];
					current[2] = test_image.at<Vec3b>(row,col)[2];
					if(current[0] == 255 && current[1] == 255 && current[2] == 255){
						page_images[i].page_corners[1].x = col;
						page_images[i].page_corners[1].y = row;
					}
				}
			}
		}
		//bottom right
		for (int row=test_image.rows-1; row >0; row--){
			for (int col = test_image.cols-1; col > 300; col--){//choose pixel
				if(page_images[i].page_corners[2].x == 0 && page_images[i].page_corners[2].y ==0) {
					current[0] = test_image.at<Vec3b>(row,col)[0];
					current[1] = test_image.at<Vec3b>(row,col)[1];
					current[2] = test_image.at<Vec3b>(row,col)[2];		
					if(current[0] == 255 && current[1] == 255 && current[2] == 255){
						page_images[i].page_corners[2].x = col;
						page_images[i].page_corners[2].y = row;
					}
				}
			}
		}
		//bottom left
		for (int col = 300; col < test_image.cols; col++){
			for (int row=test_image.rows-1; row>0; row--){//choose pixel
				if(page_images[i].page_corners[3].x == 0 && page_images[i].page_corners[3].y ==0) {
					current[0] = test_image.at<Vec3b>(row,col)[0];
					current[1] = test_image.at<Vec3b>(row,col)[1];
					current[2] = test_image.at<Vec3b>(row,col)[2];
					if(current[0] == 255 && current[1] == 255 && current[2] == 255){
						page_images[i].page_corners[3].x = col;
						page_images[i].page_corners[3].y = row;
					}
				}
			}
		}
}
#pragma endregion

//PERSPECTIVE GEOMETRIC TRANSFORMATION

    Mat perspective_matrix( 2, 4, CV_32FC1 );
	Mat temp_image;
	Point2f destination_points[4];
	Mat* perspective_warped_image = new Mat[number_of_images+number_of_pages+1];
	Mat perspective_image;
	for (int i = 0; i<number_of_images; i++){	
		temp_image = *images[i].original_image;
		perspective_matrix = Mat::zeros( temp_image.rows, temp_image.cols, temp_image.type() );	
		destination_points[0] = Point2f( 0,0 );
		destination_points[1] = Point2f(  temp_image.cols-1,0);
		destination_points[2] = Point2f(  temp_image.cols-1, temp_image.rows-1);
		destination_points[3] = Point2f( 0, temp_image.rows-1  );
	
		//Input and Output Image;
		
		Point2f source_points[] = {images[i].page_corners[0], images[i].page_corners[1], images[i].page_corners[2], images[i].page_corners[3]};
		// Get the Perspective Transform Matrix 
		perspective_matrix = getPerspectiveTransform( source_points, destination_points);
		// Apply the Perspective Transform just found to the src image
		warpPerspective(temp_image,perspective_warped_image[i],perspective_matrix,perspective_warped_image[i].size() );
		resize(perspective_warped_image[i], perspective_warped_image[i], pages[0].size());

		images[i].perspective_transform_image = &perspective_warped_image[i];
	}

#pragma region EROSION
//EROSION
	Mat* eroded_images = new Mat[number_of_images + number_of_pages+1];
	for (int i = 0; i < number_of_images; i++){
		Mat eroded_image, eroded5_image;
		temp_image = *images[i].perspective_transform_image;
		Mat binary_image;
		int current_threshold = 128;
		int max_threshold = 255;
		threshold(temp_image,binary_image,current_threshold,max_threshold,THRESH_BINARY);
		Mat gray_pcb_image;
		cvtColor(temp_image, gray_pcb_image, CV_BGR2GRAY);
		threshold(gray_pcb_image,binary_image,current_threshold,max_threshold,
			THRESH_BINARY | THRESH_OTSU);
		erode(binary_image,eroded_images[i],Mat());
		Mat five_by_five_element(5,5,CV_8U,Scalar(1));
		//erode(binary_image,eroded_images[i],five_by_five_element);
		Mat original_display, eroded3_display, eroded5_display;
		cvtColor(gray_pcb_image, original_display, CV_GRAY2BGR);
		cvtColor(eroded_image, eroded3_display, CV_GRAY2BGR);
		cvtColor(eroded_images[i], eroded5_display, CV_GRAY2BGR);
		//Mat output1 = JoinImagesHorizontally( original_display, "Original Image", eroded3_display, "Eroded Image (3x3)", 4 );
		//Mat output2 = JoinImagesHorizontally( output1, "", eroded_images[i], "Eroded Image (5x5)", 4 );
		//Mat output3 = JoinImagesHorizontally( otsu_display, "Otsu Thresholded Image", dilated3_display, "Dilated Image (3x3)", 4 );
		//imshow("Erosion... ", output2);
		images[i].eroded_image = &eroded_images[i];
		//imshow(to_string(i), *images[i].eroded_image );
	}
	//ERODE TEMPLATE IMAGES
	for (int i = number_of_images; i < (number_of_images+number_of_pages ); i++){
		Mat eroded_image, eroded5_image;
 		temp_image = *page_images[i - number_of_images].original_image;
		Mat binary_image;
		int current_threshold = 128;
		int max_threshold = 255;
		threshold(temp_image,binary_image,current_threshold,max_threshold,THRESH_BINARY);
		Mat gray_pcb_image;
		cvtColor(temp_image, gray_pcb_image, CV_BGR2GRAY);
		threshold(gray_pcb_image,binary_image,current_threshold,max_threshold,
			THRESH_BINARY | THRESH_OTSU);
		erode(binary_image,eroded_images[i],Mat());
		Mat five_by_five_element(5,5,CV_8U,Scalar(1));
		//erode(binary_image,eroded_images[i],five_by_five_element);
		Mat original_display, eroded3_display, eroded5_display;
		cvtColor(gray_pcb_image, original_display, CV_GRAY2BGR);
		cvtColor(eroded_image, eroded3_display, CV_GRAY2BGR);
		cvtColor(eroded_images[i], eroded5_display, CV_GRAY2BGR);
		Mat output1 = JoinImagesHorizontally( original_display, "Original Image", eroded3_display, "Eroded Image (3x3)", 4 );
		Mat output2 = JoinImagesHorizontally( output1, "", eroded_images[i], "Eroded Image (5x5)", 4 );
		//Mat output3 = JoinImagesHorizontally( otsu_display, "Otsu Thresholded Image", dilated3_display, "Dilated Image (3x3)", 4 );
		//imshow("Erosion... ", output2);
		page_images[i- number_of_images].eroded_image = &eroded_images[i];
		//imshow(to_string(i), *page_images[i].eroded_image );
	}
#pragma endregion

	//RESIZE PAGE IMAGE
	for(int i = 0; i<number_of_pages; i++){

		resize(*page_images[i].eroded_image,*page_images[i].eroded_image, pages[0].size());
		//imshow(to_string(i), *page_images[i].eroded_image);
	}

	//OTSU PAGES
	// Otsu thresholding
	int current_threshold = 128;
	int max_threshold = 255;
	Mat* otsu_images = new Mat[number_of_pages];
	for (int i=0; i< number_of_pages; i++){
		Mat otsu_binary_image;
		temp_image = *page_images[i].eroded_image;
		threshold(temp_image,otsu_binary_image,current_threshold,max_threshold,
			THRESH_BINARY | THRESH_OTSU);
		Mat otsu_binary_image_display;
		cvtColor(otsu_binary_image, otsu_images[i], CV_GRAY2BGR);
		page_images[i].otsu_image = &otsu_images[i];
		imshow("otsu", otsu_binary_image);
		imshow("otsu display", otsu_images[i]);
		//imshow(to_string(i), *page_images[i].otsu_image);
	}



//ADD NOISE
	Mat* noisey_images = new Mat[number_of_pages];
	for (int i = 0; i< number_of_pages; i++){
		noisey_images[i] = *page_images[i].otsu_image;
		addGaussianNoise(noisey_images[i]);
		page_images[i].noisey_image = &noisey_images[i];
		//imshow(to_string(i),*page_images[i].noisey_image);
	}

	imshow("Test o", *page_images[number_of_pages-1].original_image);
	imshow("Test e", *page_images[number_of_pages-1].eroded_image);
	imshow("Test otsu", *page_images[number_of_pages-1].otsu_image);
	imshow("Test n", *page_images[number_of_pages-1].noisey_image);

//COMPARE TO CLASSIFY
	Mat* comparison = new Mat() ;
	double most_similar_page = 0;
	int most_like;
	for(int j = 0; j<number_of_images; j++){
	//int j = 0;
		most_like = 0;
		most_similar_page = 0;
		for(int i = 0; i<number_of_pages; i++){
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
		cout << "choice " << j << " " <<  most_similar_page << endl;
	}

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
	/*images[i].back_project_image_display = &back_projection_probabilities_display[i];
	Mat output1 = JoinImagesHorizontally(image_backproject,"Original Image",blue_sample,"Blue Samples",4);
	Mat output2 = JoinImagesHorizontally(output1,"",back_projection_probabilities_display[i],"Blue Back Projection",4);*/
	return back_projection_probabilities_display;
}

