/*
 * This code is provided as part of "A Practical Introduction to Computer Vision with OpenCV"
 * by Kenneth Dawson-Howe � Wiley & Sons Inc. 2014.  All rights reserved.
 */
//#include "Utilities.h"
//#include "ColourHistogram.h"
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
		"BookView25.jpg"/*,
		"BookView26.jpg",
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
	//Mat* image = new Mat[number_of_images];
	SampleImage* image = new SampleImage[number_of_images];
	for (int file_no=0; (file_no < number_of_images); file_no++)
	{
		string filename(file_location);
		filename.append(image_files[file_no]);
		image[file_no].original_image = &imread(filename, -1);
		if (image[file_no].original_image->empty())
		{
			cout << "Could not open " << image[file_no].original_image << endl;
			return -1;
		}
	}
	// Load pages to recognise against
	int number_of_pages = sizeof(page_files)/sizeof(page_files[0]);
	Mat* pages = new Mat[number_of_pages];
	for (int file_no=0; (file_no < number_of_pages); file_no++)
	{
		string filename(file_location);
		filename.append(page_files[file_no]);
		pages[file_no] = imread(filename, -1);
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
	//imshow("blue", blue_sample);
	Mat hls_image;
	Mat back_projection_probabilities; 
	for(int i = 0; i < 1 ; i ++){
		Mat image_backproject = *image[i].original_image;
		imshow("first image", image_backproject);
		cvtColor(blue_sample, hls_image, CV_BGR2HLS); //convert blue sample to hls and store in hls
		//imshow("hls image 1", hls_image);
		ColourHistogram histogram3D(hls_image,8);
		histogram3D.NormaliseHistogram();
		cvtColor(image_backproject, hls_image, CV_BGR2HLS);
		//imshow("hls image 2", hls_image);

		back_projection_probabilities = histogram3D.BackProject(hls_image);
		*image[i].back_project_image = histogram3D.BackProject(hls_image);

		imshow("back p 1", back_projection_probabilities);
		imshow("back p 2", *image[i].back_project_image);
		
		//back_projection_probabilities = StretchImage( back_projection_probabilities );		
		*image[i].back_project_image_display = StretchImage(back_projection_probabilities);

		cvtColor(back_projection_probabilities, *image[i].back_project_image_display, CV_GRAY2BGR);
		
		imshow(to_string(i), *image[i].back_project_image_display);
	}
	//imshow(image_files[number_of_images-1], image[number_of_images-1]);

//FINDING CORNER POINTS FOR GEOMETRIC TRANSFORMATION
	Point2f x0y0(1024, 1024); //top left
	Point2f x1y1(1024, 0); //bottom left
	Point2f x2y2(1024,0); //top right
	Point2f x3y3(1024, 0); //bottom right
	int i = 0;
	Mat test_image = back_projection_probabilities_display[i];
	for (int row=0; row < test_image.rows; row++){
		for (int col=0; col < test_image.cols; col++){//choose pixel
			int current[3];
			current[0] = test_image.at<Vec3b>(row,col)[0];
			current[1] = test_image.at<Vec3b>(row,col)[1];
			current[2] = test_image.at<Vec3b>(row,col)[2];
			if(current[0] == 255 && current[1] == 255 && current[2] == 255){ //if white
				if(row < x0y0.y && col < x0y0.x) 
				{ //top left: lowest row with lowest column
					x0y0.x = col;
					x0y0.y = row;
				}
				if(col < x1y1.x && row > x1y1.y) 
				{ //bottom left: highest row with lowest column
					x1y1.x = col;
					x1y1.y = row;
				}
				if(col > x2y2.x && row > x2y2.y) 
				{ //top right: lowest row with highest column
					x2y2.x = col;
					x2y2.y = row;
				}
				//TO DO: SECOND CONDITION FOR THIS!!
				if(row > x3y3.y )//&& col > x3y3.x)
				{ //bottom right: highest row with highest column
					x3y3.x = col;
					x3y3.y = row;
				}
			}
		}
	}

//PERSPECTIVE GEOMETRIC TRANSFORMATION
	/*
	// Perspective Lambda Matrix
    Mat perspective_matrix( 2, 4, CV_32FC1 );
	Mat image_backproject = *image[i].original_image;
	perspective_matrix = Mat::zeros( image_backproject.rows, image_backproject.cols, image_backproject.type() );
	Point2f destination_points[4];
	destination_points[0] = Point2f( 0,0 );
    destination_points[1] = Point2f(  image_backproject.cols-1,0);
    destination_points[2] = Point2f(  image_backproject.cols-1, image_backproject.rows-1);
    destination_points[3] = Point2f( 0, image_backproject.rows-1  );
	
    //Input and Output Image;
    Mat perspective_warped_image;
	Mat perspective_image;
	Point2f source_points[] = {x0y0, x2y2, x3y3, x1y1};
	// Get the Perspective Transform Matrix 
    perspective_matrix = getPerspectiveTransform( source_points, destination_points);
    // Apply the Perspective Transform just found to the src image
    warpPerspective(image_backproject,perspective_warped_image,perspective_matrix,perspective_warped_image.size() );
	resize(perspective_warped_image, perspective_warped_image, pages[0].size());
    imshow("Output",perspective_warped_image);

	cout << "x0y0" << x0y0 << endl;
	cout << "x1y1" << x1y1 << endl;
	cout << "x2y2" << x2y2 << endl;
	cout << "x3y3" << x3y3 << endl;*/

	
	

	//imshow("test", image[0]);
	int choice;
	while (true)
	{

		//imshow("test1", image[0]);
		choice = cvWaitKey();
	
	} 
	return 0;
}
