#include "Histogram.h"

class ColourHistogram : public Histogram
{
private:
	MatND mHistogram;
public:
	ColourHistogram( Mat image, int number_of_bins ) :
	  Histogram( image, number_of_bins )
	{
		ComputeHistogram();
	}
	void ComputeHistogram()
	{
		const float* channel_ranges[] = { mChannelRange, mChannelRange, mChannelRange };
		calcHist(&mImage, 1, mChannelNumbers, Mat(), mHistogram, mNumberChannels, mNumberBins, channel_ranges);
	}
	void NormaliseHistogram()
	{
		normalize(mHistogram,mHistogram,1.0);
	}
	Mat BackProject( Mat& image )
	{
		Mat& result = image.clone();
		const float* channel_ranges[] = { mChannelRange, mChannelRange, mChannelRange };
		calcBackProject(&image,1,mChannelNumbers,mHistogram,result,channel_ranges,255.0);
		return result;
	}
	MatND getHistogram()
	{
		return mHistogram;
	}
};

