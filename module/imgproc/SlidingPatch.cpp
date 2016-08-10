/*
 * SlidingPatch.cpp
 */

#include "SlidingPatch.h"

using namespace cv;

SlidingPatch::SlidingPatch(const Descriptor& descriptor, int nWinX, int nWinY, int winWidth, int winHeight, int stepX, int stepY, double K_mean, double K_std) :
	GridPatch(descriptor._im, nWinX, nWinY, winWidth, winHeight), _imDescriptor(descriptor), _stepX(stepX), _stepY(stepY), _K_mean(K_mean), _K_std(K_std)
{
	// compute "mean" and "std" of image globel feature
	computeMeanStdDev(Rect(0, 0, _imWidth, _imHeight), _imMean, _imStd);
}

SlidingPatch::SlidingPatch(const Descriptor& descriptor, int nWin, int winSize, int step, double K_mean, double K_std) : 
	SlidingPatch(descriptor, nWin, nWin, winSize, winSize, step, step, K_mean, K_std) 
{
	// do nothing here
}

SlidingPatch::~SlidingPatch() 
{	
	// do nothing here
}

void SlidingPatch::computeMeanStdDev(const Rect& rect, double& m, double& s) const 
{
	Mat H, M, S;
	_imDescriptor.compute(rect, H);
	meanStdDev(H, M, S);

	m = M.at<double>(0, 0);
	s = S.at<double>(0, 0);
}

void SlidingPatch::get(int iWinX, int iWinY, cv::Rect& rect) const 
{
	// static const int imArea = _imHeight * _imWidth / 4;

	GridPatch::get(iWinX, iWinY, rect);
	Rect rect_new = rect;
	double win_mean, win_std, win_new_mean, win_new_std;
	computeMeanStdDev(rect_new, win_mean, win_std);

	while (true) {

		rect_new.x -= _stepX;
		rect_new.y -= _stepY;
		rect_new.width += _stepX * 2;
		rect_new.height += _stepY * 2;

		// the patch is overflow, use the previous patch instead
		if (isOverflow(rect_new))
			break;
		/*
		// the patch is too big (> 1/4 image), use the previous patch instead
		if (rect_new.area() > imArea)
			break;
		*/
		computeMeanStdDev(rect_new, win_new_mean, win_new_std);
		if (win_new_mean >= (_K_mean * _imMean) && win_new_std <= (_K_std * _imStd)) {	// find the good patch
			rect = rect_new;
			break;
		}

		if (win_new_mean > win_mean) {	// this patch is better than the previous one
			rect = rect_new;
			win_mean = win_new_mean;
		}
	}
}