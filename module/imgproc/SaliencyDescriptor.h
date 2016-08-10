/*
 * SaliencyDescriptor.h
 */

#ifndef _SALIENCYDESCRIPTOR_H_
#define _SALIENCYDESCRIPTOR_H_

#include "Descriptor.h"

class SaliencyDescriptor : public Descriptor
{
protected:
	cv::Mat _imSaliencyIntegral;

	void computeSaliencyIntegral();

public:
	SaliencyDescriptor(const cv::Mat& im);
	virtual ~SaliencyDescriptor();

	void compute(const cv::Rect& position, cv::Mat& saliency) const override;
};

#endif /* _SALIENCYDESCRIPTOR_H_ */