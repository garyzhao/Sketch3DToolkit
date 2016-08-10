/*
 * GridPatch.h
 * Overlapping grid image patch class to extract feature from images.
 */

#ifndef _GRIDPATCH_H_
#define _GRIDPATCH_H_

#include "Patch.h"

class GridPatch : public Patch {

protected:
	int _stepX, _stepY;
	int _iWinX, _iWinY;

public:
	int _nWinX, _nWinY, _winWidth, _winHeight;

	GridPatch(const cv::Mat& im, int nWinX, int nWinY, int winWidth, int winHeight);
	GridPatch(const cv::Mat& im, int nWin, int winSize);
	virtual ~GridPatch();

	virtual void begin() override;
	virtual bool next() override;
	virtual void get(int iWinX, int iWinY, cv::Rect& rect) const override;
	cv::Point index() const;
};

#endif /* _GRIDPATCH_H_ */