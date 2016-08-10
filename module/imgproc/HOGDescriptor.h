/*
 * HOGDescriptor.h
 * The class implements Histogram of Oriented Gradients ([Dalal2005]) for gray-level images.
 * This code was developed from the MATLAB code for the work: O. Ludwig, D. Delgado, V. Goncalves, and U. Nunes, "Trainable
 * Classifier-Fusion Schemes: An Application To Pedestrian Detection", In: 12th International IEEE Conference On Intelligent Transportation Systems, 2009.
 */

#ifndef _HOGDESCRIPTOR_H_
#define _HOGDESCRIPTOR_H_

#include <vector>
#include "Descriptor.h"

class HOGDescriptor : public Descriptor
{
protected:
	int _nWinX, _nWinY, _nBin;
	cv::Mat _imAng, _imMag;
	std::vector<cv::Mat> _imHOGIntegral;

	void computeImageGradient();
	void computeHOGIntegral();

public:
	HOGDescriptor(const cv::Mat& im, int nWinX = 3, int nWinY = 3, int nBin = 9);
	virtual ~HOGDescriptor();

	void compute(const cv::Rect& position, cv::Mat& HOG) const override;
};

#endif /* _HOGDESCRIPTOR_H_ */