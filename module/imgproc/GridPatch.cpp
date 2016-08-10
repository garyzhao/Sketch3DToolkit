/*
 * GridPatch.cpp
 */

#include "GridPatch.h"

using namespace cv;

GridPatch::GridPatch(const Mat& im, int nWinX, int nWinY, int winWidth, int winHeight) : Patch(im), _nWinX(nWinX), _nWinY(nWinY), _winWidth(winWidth), _winHeight(winHeight) {

	_stepX = (_imWidth - _winWidth) / double(_nWinX - 1.0);
	_stepY = (_imHeight - _winHeight) / double(_nWinY - 1.0);

	assert(_stepX > 0 && _stepX < _imWidth);
	assert(_stepY > 0 && _stepY < _imHeight);

	begin();
}

GridPatch::GridPatch(const Mat& im, int nWin, int winSize) : GridPatch(im, nWin, nWin, winSize, winSize) {

	// do nothing here
}

GridPatch::~GridPatch() {

	// do nothing here
}

void GridPatch::begin() {

	_iWinX = 0;
	_iWinY = 1;
	_rect = Rect(0, 0, _winWidth, _winHeight);
}

bool GridPatch::next() {

	if (_iWinX >= _nWinX && _iWinY >= _nWinY)
		return false;

	if (_iWinX < _nWinX) {			// next column
		_iWinX++;
	}
	else if (_iWinX == _nWinX) {	// next row
		_iWinX = 1;
		_iWinY++;
	}

	Point idx = index();
	get(idx.x, idx.y, _rect);
	return true;
}

void GridPatch::get(int iWinX, int iWinY, cv::Rect& rect) const {
	
	assert(iWinX >= 0 && iWinX < _nWinX);
	assert(iWinY >= 0 && iWinY < _nWinY);

	rect.x = iWinX * _stepX;
	rect.y = iWinY * _stepY;
	rect.width = _winWidth;
	rect.height = _winHeight;
}

Point GridPatch::index() const {

	return Point(_iWinX - 1, _iWinY - 1);
}