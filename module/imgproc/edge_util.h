/*
 * edge_util.h
 */

#ifndef _EDGE_UTIL_H_
#define _EDGE_UTIL_H_

#include <cassert>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

void edgeThinningZhangSuen(cv::Mat& im);
void edgeThinningGuoHall(cv::Mat& im);

void computeSobelOrientation(const cv::Mat& im_gray, cv::Mat& im_dst);
std::vector<cv::Point2f> detectHarrisKeyPoints(const cv::Mat& im_gray, int max_points,
	double quality_level = 0.01, double min_distance = 10, int block_size = 3, double k = 0.04);

void clusterImageEdge(
	const cv::Mat& E,
	const cv::Mat& O,
	int& _segCnt,					// total segment count
	cv::Mat& _segIds,				// segment ids (-1/0 means no segment)
	std::vector<float>& meanX,
	std::vector<float>& meanY,
	std::vector<float>& meanO,
	std::vector<float>& _segMag,	// segment edge magnitude sums
	std::vector<int>& _segR, std::vector<int>& _segC,	// segment lower-right pixel
	std::vector<std::vector<float>>& _segAff,			// segment affinities 
	std::vector<std::vector<int>>& _segAffIdx,			// segment neighbors 

	float _edgeMinMag = 0.1,
	float _edgeMergeThr = 0.5,
	float _clusterMinMag = 0.5,
	float _gamma = 2);

template<typename T>
cv::Rect computeBoundingBox(const cv::Mat& mask) {

	int w = mask.cols, h = mask.rows;
	int xmin = w, ymin = h, xmax = 0, ymax = 0;

	for (int c = 0; c < w; c++) {
		for (int r = 0; r < h; r++) {
			if (mask.at<T>(r, c) > 0) {
				xmin = std::min(xmin, c);
				xmax = std::max(xmax, c);
				ymin = std::min(ymin, r);
				ymax = std::max(ymax, r);
			}
		}
	}
	assert(xmin <= xmax);
	assert(ymin <= ymax);
	return cv::Rect(xmin, ymin, xmax - xmin + 1, ymax - ymin + 1);
}

cv::Mat paddingImage(const cv::Mat& src, int borderLen = 0);

#endif /* _EDGE_UTIL_H_ */