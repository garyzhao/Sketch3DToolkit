/*
 * Patch.h
 * Abstract image patch class for feature extraction.
 */

#ifndef _PATCH_H_
#define _PATCH_H_

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

class Patch abstract {

protected:
	int _imHeight, _imWidth;
	cv::Rect _rect;

public:
	Patch(const cv::Mat& im);
	virtual ~Patch();

	virtual const cv::Rect& rect() const;
	virtual bool isOverflow(const cv::Rect& rect) const;

	virtual void begin() = 0;
	virtual bool next() = 0;
	virtual void get(int iWinX, int iWinY, cv::Rect& rect) const = 0;
};

#endif /* _PATCH_H_ */