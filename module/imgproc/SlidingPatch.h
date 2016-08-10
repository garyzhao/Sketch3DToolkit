/*
 * SlidingPatch.h
 * Sliding image patch class to extract feature from images.
 */

#ifndef _SLIDINGPATCH_H_
#define _SLIDINGPATCH_H_

#include "GridPatch.h"
#include "Descriptor.h"

class SlidingPatch : public GridPatch 
{
	
protected:
	const Descriptor& _imDescriptor;
	int _stepX, _stepY;
	double _K_mean, _K_std;

	double _imMean, _imStd;

	void computeMeanStdDev(const cv::Rect& rect, double& m, double& s) const;

public:
	SlidingPatch(const Descriptor& descriptor, int nWinX, int nWinY, int winWidth, int winHeight, int stepX, int stepY, double K_mean = 1.0, double K_std = 1.0);
	SlidingPatch(const Descriptor& descriptor, int nWin, int winSize, int step, double K_mean = 1.0, double K_std = 1.0);
	virtual ~SlidingPatch();

	// virtual bool next() override;
	virtual void get(int iWinX, int iWinY, cv::Rect& rect) const override;
};

#endif /* _SLIDINGPATCH_H_ */