/*
 * Descriptor.cpp
 */

#include "Descriptor.h"

using namespace cv;

Descriptor::Descriptor(const Mat& im) : _im(im) {
	
	// compute image height & width 
	_imHeight = _im.rows;
	_imWidth = _im.cols;
}

Descriptor::~Descriptor() {

	// do nothing here
}