/*
 * Patch.cpp
 */

#include "Patch.h"
#include <cassert>

using namespace cv;

Patch::Patch(const Mat& im) : _imHeight(im.rows), _imWidth(im.cols) {

	// do nothing here
}

Patch::~Patch() {

	// do nothing here
}

bool Patch::isOverflow(const cv::Rect& rect) const {

	return (rect.x < 0 || rect.x >= _imWidth || rect.y < 0 || rect.y >= _imHeight || 
		rect.width < 1 || rect.x + rect.width > _imWidth || rect.height < 1 || rect.y + rect.height > _imHeight);
}

const Rect& Patch::rect() const {

	assert(!isOverflow(_rect));
	return _rect;
}