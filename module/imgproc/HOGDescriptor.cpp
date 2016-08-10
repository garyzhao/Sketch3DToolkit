/*
 * HOGDescriptor.cpp
 */

#include <cmath>
#include <cassert>
#include "HOGDescriptor.h"

using namespace cv;
using namespace std;

::HOGDescriptor::HOGDescriptor(const Mat& im, int nWinX, int nWinY, int nBin) : Descriptor(im), _nWinX(nWinX), _nWinY(nWinY), _nBin(nBin)
{	
	// check if the input image is gray-level
	assert(_im.channels() == 1);
	assert(_im.depth() == CV_8U);

	computeImageGradient();
	computeHOGIntegral();
}

::HOGDescriptor::~HOGDescriptor()
{
	// do nothing here
}

void ::HOGDescriptor::computeImageGradient()
{
	Mat gradX, gradY;

	// Sobel(_im, gradX, CV_32F, 1, 0, 3);
	// Sobel(_im, gradY, CV_32F, 0, 1, 3);

	float k[] = { -1, 0, 1 };
	Mat kernel(1, 3, CV_32F, k);
	filter2D(_im, gradX, CV_32F, kernel);
	filter2D(_im, gradY, CV_32F, -kernel.t());

	phase(gradX, gradY, _imAng);
	magnitude(gradX, gradY, _imMag);
}

void ::HOGDescriptor::computeHOGIntegral()
{
	assert(_imAng.depth() == CV_32F);
	assert(_imMag.depth() == CV_32F);

	_imHOGIntegral.resize(_nBin);

	for (int i = 0; i < _imHOGIntegral.size(); i++)
		_imHOGIntegral[i] = Mat::zeros(_imHeight, _imWidth, CV_64F);

	for (int c = 0; c < _imWidth; c++) {
		for (int r = 0; r < _imHeight; r++) {
			float ang = _imAng.at<float>(r, c);
			float mag = _imMag.at<float>(r, c);
			
			int idx = ang / (2.0 * CV_PI / _nBin);
			_imHOGIntegral[idx].at<double>(r, c) = double(mag);
		}
	}

	// compute integral images for HOG bins
	for (int i = 0; i < _imHOGIntegral.size(); i++)
		integral(_imHOGIntegral[i], _imHOGIntegral[i], CV_64F);
}

void ::HOGDescriptor::compute(const cv::Rect& position, cv::Mat& HOG) const
{
	constexpr static double EPS = numeric_limits<double>::epsilon();

	HOG.create(1, _nWinX * _nWinY * _nBin, CV_64F);
	double *HPtr = HOG.ptr<double>(0);

	int iWin = 0;
	int stepX = floor(position.width / (_nWinX + 1.0));
	int stepY = floor(position.height / (_nWinY + 1.0));

	for (int iY = 0; iY < _nWinY; iY++) {
		for (int iX = 0; iX < _nWinX; iX++) {
		
			int xMin = iX * stepX + position.x;
			int yMin = iY * stepY + position.y;
			int xWidth = 2 * stepX;
			int yHeight = 2 * stepY;

			Mat bH(1, _nBin, CV_64F);
			double *bHPtr = bH.ptr<double>(0);

			// compute histogram using integral images
			for (int iB = 0; iB < _nBin; iB++) {
				const Mat& I(_imHOGIntegral[iB]);
				bHPtr[iB] = I.at<double>(yMin + yHeight, xMin + xWidth) - I.at<double>(yMin, xMin + xWidth) - I.at<double>(yMin + yHeight, xMin) + I.at<double>(yMin, xMin);
			}

			// normalize
			double N = norm(bH) + EPS;
			for (int iB = 0; iB < _nBin; iB++)
				HPtr[iWin * _nBin + iB] = bHPtr[iB] / N;

			iWin++;
		}
	}
}