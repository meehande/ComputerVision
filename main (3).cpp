/*
 * This code is provided as part of "A Practical Introduction to Computer Vision with OpenCV"
 * by Kenneth Dawson-Howe � Wiley & Sons Inc. 2014.  All rights reserved.
 */
//#include "Utilities.h"
#include "SampleImage.h"
#include <iostream>
#include <fstream>
using namespace std;
 

#define SCENE_IMAGES_INDEX  0
#define PAGE_IMAGES_INDEX  50
int main(int argc, const char** argv)
{
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
	/*	"BookView26.jpg",
		"BookView27.jpg",
		"BookView28.jpg",
		"BookView29.jpg",
		"BookView30.jpg",
		"BookView31.jpg",
		"BookView32.jpg",
		"BookView33.jpg",
		"BookView34.jpg",
		"BookView35.jpg",
		"BookView36.jpg",
		"BookView37.jpg",
		"BookView38.jpg",
		"BookView39.jpg",
		"BookView40.jpg",
		"BookView41.jpg",
		"BookView42.jpg",
		"BookView43.jpg",
		"BookView44.jpg",
		"BookView45.jpg",
		"BookView46.jpg",
		"BookView47.jpg",
		"BookView48.jpg",
		"BookView49.jpg",
		"BookView50.jpg"*/
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

	//BACK PROJECTION (TAKEN FROM HISTOGRAMSDEMO FUNCTION)
	//Mat blue_sample = image[BLUE_SAMPLE_IMAGE_INDEX]; //sample of blue for back projection
	Mat* back_projection_probabilities_display = new Mat[number_of_images]; //black&white array of back projected blue results

	Mat hls_image;
	Mat image_backproject;
	Mat back_projection_probabilities;
	for(int i = 0; i < number_of_images ; i ++){
		image_backproject = *images[i].original_image;
		cvtColor(blue_sample, hls_image, CV_BGR2HLS); //convert blue sample to hls and store in hls
		ColourHistogram histogram3D(hls_image,8);
		histogram3D.NormaliseHistogram();
		cvtColor(image_backproject, hls_image, CV_BGR2HLS);
		back_projection_probabilities = histogram3D.BackProject(hls_image);
		back_projection_probabilities = StretchImage(back_projection_probabilities);		
		cvtColor(back_projection_probabilities, back_projection_probabilities_display[i], CV_GRAY2BGR);
		images[i].back_project_image_display = &back_projection_probabilities_display[i];
		Mat output1 = JoinImagesHorizontally(image_backproject,"Original Image",blue_sample,"Blue Samples",4);
		Mat output2 = JoinImagesHorizontally(output1,"",back_projection_probabilities_display[i],"Blue Back Projection",4);
		//imshow(to_string(i), *images[i].back_project_image_display);
	}
	
	
//FINDING CORNER POINTS FOR GEOMETRIC TRANSFORMATION
	for(int i =0; i<number_of_images ; i++){
		Mat test_image = *images[i].back_project_image_display;
		//top left
		//for (int row=0; row < test_image.rows; row++){
		//	for (int col = 320; col < test_image.cols; col++){//choose pixel
				/*int current[3];
				current[0] = test_image.at<Vec3b>(row,col)[0];
				current[1] = test_image.at<Vec3b>(row,col)[1];
				current[2] = test_image.at<Vec3b>(row,col)[2];
				if(current[0] == 255 && current[1] == 255 && current[2] == 255){
*/

		//			
		//			if(row < images[i].page_corners[0].y )//&& col < images[i].page_corners[0].x) 
		//			{ //top left: lowest row with lowest column
		//			/*	x0y0.x = col;
		//				x0y0.y = row;*/
		//				images[i].page_corners[0].x = col;
		//				images[i].page_corners[0].y = row;
		//			}
		//			if(col < images[i].page_corners[1].x)// && row > images[i].page_corners[1].y) 
		//			{ //bottom left: highest row with lowest column
		//				/*x1y1.x = col;
		//				x1y1.y = row;*/
		//				images[i].page_corners[1].x = col;
		//				images[i].page_corners[1].y = row;
		//			}
		//			if(col > images[i].page_corners[2].x )//&& row > images[i].page_corners[2].y) 
		//			{ //top right: lowest row with highest column
		//				/*x2y2.x = col;
		//				x2y2.y = row;*/
		//				images[i].page_corners[2].x = col;
		//				images[i].page_corners[2].y = row;
		//			}
		//			//TO DO: SECOND CONDITION FOR THIS!!
		//			if(row > images[i].page_corners[3].y )//&& col > x3y3.x)
		//			{ //bottom right: highest row with highest column
		//		/*		x3y3.x = col;
		//				x3y3.y = row;*/
		//				images[i].page_corners[3].x = col;
		//				images[i].page_corners[3].y = row;
		//			}

		//		}
		//	}
		//}
		int current[3];
	//top left
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



//PERSPECTIVE GEOMETRIC TRANSFORMATION

    Mat perspective_matrix( 2, 4, CV_32FC1 );
	Mat temp_image;
	Point2f destination_points[4];
	Mat* perspective_warped_image = new Mat[number_of_images];
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
	//	imshow(to_string(i), image_backproject);
	//	imshow(to_string(i+25),perspective_warped_image);
		images[i].perspective_transform_image = &perspective_warped_image[i];
	//	imshow(to_string(i), *images[i].perspective_transform_image);
		/*cout << i << endl;
		cout << "point 0" << images[i].page_corners[0] << endl;
		cout << "point 1" << images[i].page_corners[1] << endl;
		cout << "point 2" << images[i].page_corners[2] << endl;
		cout << "point 3" << images[i].page_corners[3] << endl << endl;*/
	}


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
		erode(binary_image,eroded_image,Mat());
		Mat five_by_five_element(5,5,CV_8U,Scalar(1));
		erode(binary_image,eroded_images[i],five_by_five_element);
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
		erode(binary_image,eroded_image,Mat());
		Mat five_by_five_element(5,5,CV_8U,Scalar(1));
		erode(binary_image,eroded_images[i],five_by_five_element);
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

//COMPARE TO CLASSIFY
	cout << "pages object array size " << sizeof(page_images)/sizeof(page_images[0]) << endl;
	cout <<  "eroded images temp size " << sizeof(eroded_images)/sizeof(eroded_images[0]) << endl;
	cout << "number of pages: " << number_of_pages << endl;

	imshow("Test ", *page_images[number_of_pages-1].original_image);
	imshow("Test ", *page_images[number_of_pages-1].eroded_image);
	Mat* comparison = new Mat();
	images[0].eroded_image->copySize(*comparison);
	compare(*images[2].eroded_image, *page_images[2].eroded_image, *comparison, CMP_EQ);
	Mat vComparison;
	reduce(*comparison, vComparison, 0, CV_REDUCE_AVG);
	Mat sComparison;
	reduce(vComparison, sComparison, 1, CV_REDUCE_AVG);
	cout << "Comparison: " << sComparison << endl;
	imshow("image: ", *images[2].eroded_image);
	imshow("page: ", *page_images[2].eroded_image);
	imshow("image: ", *images[2].original_image);
	imshow("page: ", *page_images[2].original_image);
	/*for (int i = 0; i < number_of_pages; i++){
		imshow(to_string(i), *page_images[i].eroded_image );
	}*/

	int choice;
	while (true)
	{

		//imshow("test1", image[0]);
		choice = cvWaitKey();
	
	} 
	return 0;
}