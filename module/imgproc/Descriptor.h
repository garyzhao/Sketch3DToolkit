/*
 * Descriptor.h
 * Abstract feature descriptor class.
 */

#ifndef _DESCRIPTOR_H_
#define _DESCRIPTOR_H_

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

class Descriptor abstract {

public:
	const cv::Mat& _im;
	int _imHeight, _imWidth;

public:
	Descriptor(const cv::Mat& im);
	virtual ~Descriptor();

	virtual void compute(const cv::Rect& position, cv::Mat& H) const = 0;
};

#endif /* _DESCRIPTOR_H_ */