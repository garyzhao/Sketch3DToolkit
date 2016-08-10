/*
 * SaliencyDescriptor.cpp
 */

#include <cassert>
#include "SaliencyDescriptor.h"

using namespace cv;
using namespace std;

SaliencyDescriptor::SaliencyDescriptor(const Mat& im) : Descriptor(im)
{
	// check if the input image is gray-level
	assert(_im.channels() == 1);
	assert(_im.depth() == CV_8U);

	computeSaliencyIntegral();
}

SaliencyDescriptor::~SaliencyDescriptor()
{
	// do nothing here
}

void SaliencyDescriptor::computeSaliencyIntegral()
{
	vector<Point2f> pointVector;
	double imScale = sqrt(_imHeight * _imWidth);
	goodFeaturesToTrack(_im, pointVector, imScale, 0.01, 0, Mat(), 3, true, 0.04);

	_imSaliencyIntegral = Mat::zeros(_imHeight, _imWidth, CV_64F);
	for (const auto& point : pointVector)
		_imSaliencyIntegral.at<double>(point) = 1.0;

	integral(_imSaliencyIntegral, _imSaliencyIntegral, CV_64F);
}

void SaliencyDescriptor::compute(const Rect& position, Mat& saliency) const
{
	saliency.create(1, 1, CV_64F);

	int xMin = position.x;
	int yMin = position.y;
	int xWidth = position.width;
	int yHeight = position.height;

	const Mat& I(_imSaliencyIntegral);
	double N = I.at<double>(yMin + yHeight, xMin + xWidth) - I.at<double>(yMin, xMin + xWidth) - I.at<double>(yMin + yHeight, xMin) + I.at<double>(yMin, xMin);

	saliency.at<double>(0, 0) = N / position.width;
}