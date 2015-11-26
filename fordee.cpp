#include "Utilities.h"
#include <iostream>
#include <fstream>
#include <vector>
#include "ColourHistogram.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;

//Function Definitions
float Recall(int TP, int FN);
float Precision(int TP, int FP);
float Accuracy(int TP, int TN, int TotalSamples);
float Specificity(int TN, int FP);
float F1(float Precision, float Recall);
float Fhalf(float Precision, float Recall);
float F2(float Precision, float Recall);

//Assuming Positive means a label was found
int true_positives = 0;
int false_positives = 0;
int true_negatives = 0;
int false_negatives = 0;
#define BLUE_SAMPLE_IMAGE_INDEX 0
#define SCENE_IMAGES_INDEX  1
#define PAGE_IMAGES_INDEX  50


int main( int argc, char** argv )
{
	char* file_location = "Media/Books/";
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
		"BookView25.jpg",
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
		"BookView50.jpg"	
    };

	char* sample_pages[] = {
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

	int number_of_images = sizeof(image_files)/sizeof(image_files[0]);
	int number_of_samples = sizeof(sample_pages)/sizeof(sample_pages[0]);
	Mat* image = new Mat[number_of_images];
	Mat* sample = new Mat[number_of_samples];

	for (int file_no=0; (file_no < number_of_images); file_no++)
	{
		string filename(file_location);
		filename.append(image_files[file_no]);
		image[file_no] = imread(filename, -1);
		if (image[file_no].empty())
		{
			cout << "Could not open " << image[file_no] << endl;
			return -1;
		}
	}
	for (int file_no=0; (file_no < number_of_samples); file_no++)
	{
		string filename(file_location);
		filename.append(sample_pages[file_no]);
		sample[file_no] = imread(filename, -1);
		if (sample[file_no].empty())
		{
			cout << "Could not open " << sample[file_no] << endl;
			return -1;
		}
	}
	//BACK PROJECTION (TAKEN FROM HISTOGRAMSDEMO FUNCTION)
	Mat blue_sample = imread("Media/Blue10.jpg"); //sample of blue for back projection
	//imshow("blue", blue_sample);
	Mat hls_image;
	Mat image_backproject = image[0];
	imshow("first image", image_backproject);
	cvtColor(blue_sample, hls_image, CV_BGR2HLS); //convert blue sample to hls and store in hls
	//imshow("hls image 1", hls_image);
	ColourHistogram histogram3D(hls_image,8);
	histogram3D.NormaliseHistogram();
	cvtColor(image_backproject, hls_image, CV_BGR2HLS);
	//imshow("hls image 2", hls_image);
	Mat back_projection_probabilities = histogram3D.BackProject(hls_image);
	//imshow("back p 1", back_projection_probabilities);
	back_projection_probabilities = StretchImage( back_projection_probabilities );
	Mat back_projection_probabilities_display;
	cvtColor(back_projection_probabilities, back_projection_probabilities_display, CV_GRAY2BGR);
	imshow("back p 1", back_projection_probabilities_display);
	Mat output1 = JoinImagesHorizontally(image_backproject,"Original Image",blue_sample,"Blue Samples",4);
	Mat output2 = JoinImagesHorizontally(output1,"",back_projection_probabilities_display,"Blue Back Projection",4);
	//imshow( "Back Projection", output2 );
	int prev[3];	
	Point2f Top_Left(1024, 1024);
	Point2f Bottom_Left(1024, 1024);
	Point2f Top_Right(0, 1024);
	Point2f Bottom_Right(1024, 0);
	bool loop = true;							//Allows us to break if we find a label
	for(int row=0; row<back_projection_probabilities_display.rows; row++)
	{
		for(int col=0; col<back_projection_probabilities_display.cols; col++)
		{
			int currval[3];
			//Take value fo each channel
			currval[0] = back_projection_probabilities_display.at<Vec3b>(row,col)[0];
			currval[1] = back_projection_probabilities_display.at<Vec3b>(row,col)[1];
			currval[2] = back_projection_probabilities_display.at<Vec3b>(row,col)[2];
			if(currval[0] == 255 && currval[1] == 255 && currval[2] == 255){
				if(row < Top_Left.y)
				{
					Top_Left.x = col;
					Top_Left.y = row;
				}
				if(col < Bottom_Left.x)
				{
					Bottom_Left.x = col;
					Bottom_Left.y = row;
				}
				if(col > Top_Right.x){
					Top_Right.x = col;
					Top_Right.y = row;
				}
				if(row > Bottom_Right.y)
				{
					Bottom_Right.x = col;
					Bottom_Right.y= row;
				}
			}
		}
	}
	// Lambda Matrix
    Mat lambda( 2, 4, CV_32FC1 );
	lambda = Mat::zeros( image_backproject.rows, image_backproject.cols, image_backproject.type() );
	Point2f outputQuad[4];
	outputQuad[0] = Point2f( 0,0 );
    outputQuad[1] = Point2f(  image_backproject.cols-1,0);
    outputQuad[2] = Point2f(  image_backproject.cols-1, image_backproject.rows-1);
    outputQuad[3] = Point2f( 0, image_backproject.rows-1  );
	
    //Input and Output Image;
    Mat output;
	Mat perspective_image;
	Point2f input_array[] = {Top_Left, Top_Right, Bottom_Right, Bottom_Left};
	// Get the Perspective Transform Matrix i.e. lambda 
    lambda = getPerspectiveTransform( input_array, outputQuad );
    // Apply the Perspective Transform just found to the src image
    warpPerspective(image_backproject,output,lambda,output.size() );
	resize(output, output, sample[0].size());
	//Display input and output
    //imshow("Input",image_backproject);
    imshow("Output",output);
	imshow("sample", sample[0]);
	/*cout << Top_Left[0] << ", " << Top_Left[1] << endl;
	cout << Top_Right[0] << ", " << Top_Right[1] << endl;
	cout << Bottom_Left[0] << ", " << Bottom_Left[1] << endl;
	cout << Bottom_Right[0] << ", " << Bottom_Right[1] << endl;*/
	/*vector< vector<Point> >  co_ordinates;
	co_ordinates.push_back(vector<Point>());

	co_ordinates[0].push_back(Top_Left);
	co_ordinates[0].push_back(Top_Right);
	co_ordinates[0].push_back(Bottom_Right);
	co_ordinates[0].push_back(Bottom_Left);
	drawContours( output2,co_ordinates,0, Scalar(255),CV_FILLED, 8 );
	imshow("output", output2);/*

	/* above this from histogram demo - set up for back projection
	need to figure out how to include histogram
	how to get HLS image... */

	//imshow("test", image[0]);
	int choice;
	while (true)
	{

		
		choice = cvWaitKey();
	
	} 
	return 0;
}


float Recall(int TP, int FN)
{
	//Recall is the percentage of the objects being sought which have been successfully located.
	return(TP/(TP+FN));
}

float Precision(int TP, int FP)
{
	//Precision is the percentage of the positive classifications which are correct (i.e. are not false alarms).
	return (TP/(TP+FP));
}

float Accuracy(int TP, int TN, int TotalSamples)
{
	//Accuracy is the percentage of the total samples which are correct.
	return((TP+TN)/TotalSamples);
}

float Specificity(int TN, int FP)
{
	//Specificity, also referred to as the true negative rate, is the percentage of the negative classifications which are correct.
	return (TN/(FP+TN));
}

//The Fx measure is a weighted measure of precision and recall which can be used to put a different emphasis on precision and recall.
float F1(float Precision, float Recall)
{
	//Even Weight
	return ((1+(1^2))*((Precision*Recall)/(((1^2)*Precision)+Recall)));
}

float Fhalf(float Precision, float Recall)
{
	//More weight on Precision
	return ((1+((1/2)^2))*((Precision*Recall)/((((1/2)^2)*Precision)+Recall)));
}

float F2(float Precision, float Recall)
{
	//More Weight on Recall
	return ((1+(2^2))*((Precision*Recall)/(((2^2)*Precision)+Recall)));
}