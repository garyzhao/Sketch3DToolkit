/*
* edge_util.cc
*/

#include "edge_util.h"
#include <iostream>

using namespace cv;
using namespace std;

/**
* Code for thinning a binary image using Zhang-Suen algorithm.
*
* Perform one thinning iteration.
* Normally you wouldn't call this function directly from your code.
*
* @param  im    Binary image with range = 0-1
* @param  iter  0 = even, 1 = odd
*/
void thinningZhangSuenIteration(cv::Mat& im, int iter)
{
	cv::Mat marker = cv::Mat::zeros(im.size(), CV_8UC1);

	for (int i = 1; i < im.rows - 1; i++)
	{
		for (int j = 1; j < im.cols - 1; j++)
		{
			uchar p2 = im.at<uchar>(i - 1, j);
			uchar p3 = im.at<uchar>(i - 1, j + 1);
			uchar p4 = im.at<uchar>(i, j + 1);
			uchar p5 = im.at<uchar>(i + 1, j + 1);
			uchar p6 = im.at<uchar>(i + 1, j);
			uchar p7 = im.at<uchar>(i + 1, j - 1);
			uchar p8 = im.at<uchar>(i, j - 1);
			uchar p9 = im.at<uchar>(i - 1, j - 1);

			int A = (p2 == 0 && p3 == 1) + (p3 == 0 && p4 == 1) +
				(p4 == 0 && p5 == 1) + (p5 == 0 && p6 == 1) +
				(p6 == 0 && p7 == 1) + (p7 == 0 && p8 == 1) +
				(p8 == 0 && p9 == 1) + (p9 == 0 && p2 == 1);
			int B = p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9;
			int m1 = iter == 0 ? (p2 * p4 * p6) : (p2 * p4 * p8);
			int m2 = iter == 0 ? (p4 * p6 * p8) : (p2 * p6 * p8);

			if (A == 1 && (B >= 2 && B <= 6) && m1 == 0 && m2 == 0)
				marker.at<uchar>(i, j) = 1;
		}
	}

	im &= ~marker;
}

/**
* Function for thinning the given binary image
*
* @param  im  Binary image with range = 0-255
*/
void edgeThinningZhangSuen(cv::Mat& im)
{
	im /= 255;

	cv::Mat prev = cv::Mat::zeros(im.size(), CV_8UC1);
	cv::Mat diff;

	static int i = 0;

	do {
		thinningZhangSuenIteration(im, 0);
		thinningZhangSuenIteration(im, 1);
		cv::absdiff(im, prev, diff);
		im.copyTo(prev);

		cout << "Edge thinning ZhangSuen iter#" << ++i * 2 << endl;
	} while (cv::countNonZero(diff) > 0);

	im *= 255;
}

/**
* Code for thinning a binary image using Guo-Hall algorithm.
*
* Perform one thinning iteration.
* Normally you wouldn't call this function directly from your code.
*
* @param  im    Binary image with range = 0-1
* @param  iter  0 = even, 1 = odd
*/
void thinningGuoHallIteration(cv::Mat& im, int iter)
{
	cv::Mat marker = cv::Mat::zeros(im.size(), CV_8UC1);

	for (int i = 1; i < im.rows - 1; i++)
	{
		for (int j = 1; j < im.cols - 1; j++)
		{
			uchar p2 = im.at<uchar>(i - 1, j);
			uchar p3 = im.at<uchar>(i - 1, j + 1);
			uchar p4 = im.at<uchar>(i, j + 1);
			uchar p5 = im.at<uchar>(i + 1, j + 1);
			uchar p6 = im.at<uchar>(i + 1, j);
			uchar p7 = im.at<uchar>(i + 1, j - 1);
			uchar p8 = im.at<uchar>(i, j - 1);
			uchar p9 = im.at<uchar>(i - 1, j - 1);

			int C = (!p2 & (p3 | p4)) + (!p4 & (p5 | p6)) +
				(!p6 & (p7 | p8)) + (!p8 & (p9 | p2));
			int N1 = (p9 | p2) + (p3 | p4) + (p5 | p6) + (p7 | p8);
			int N2 = (p2 | p3) + (p4 | p5) + (p6 | p7) + (p8 | p9);
			int N = N1 < N2 ? N1 : N2;
			int m = iter == 0 ? ((p6 | p7 | !p9) & p8) : ((p2 | p3 | !p5) & p4);

			if (C == 1 && (N >= 2 && N <= 3) && m == 0)
				marker.at<uchar>(i, j) = 1;
		}
	}

	im &= ~marker;
}

/**
* Function for thinning the given binary image
*
* @param  im  Binary image with range = 0-255
*/
void edgeThinningGuoHall(cv::Mat& im)
{
	im /= 255;

	cv::Mat prev = cv::Mat::zeros(im.size(), CV_8UC1);
	cv::Mat diff;

	static int i = 0;

	do {
		thinningGuoHallIteration(im, 0);
		thinningGuoHallIteration(im, 1);
		cv::absdiff(im, prev, diff);
		im.copyTo(prev);

		cout << "Edge thinning GuoHall iter#" << ++i * 2 << endl;
	} while (cv::countNonZero(diff) > 0);

	im *= 255;
}

Mat paddingImage(const Mat& src, int borderLen) {

	Mat dst = src;
	int w = src.cols, h = src.rows;
	int top = 0, bottom = 0, left = 0, right = 0;

	int d = w - h;
	if (d > 0) {
		top = d / 2;
		bottom = d - top;
	} if (d < 0) {
		left = -d / 2;
		right = -d - left;
	}

	top += borderLen;
	bottom += borderLen;
	left += borderLen;
	right += borderLen;

	copyMakeBorder(src, dst, top, bottom, left, right, BORDER_CONSTANT);
	assert(dst.cols == dst.rows);
	return dst;
}

void computeSobelOrientation(const Mat& im_gray, Mat& im_dst) {

	Mat grad_x, grad_y;
	Sobel(im_gray, grad_x, CV_32F, 1, 0, 3);
	Sobel(im_gray, grad_y, CV_32F, 0, 1, 3);
	phase(grad_x, grad_y, im_dst);
}

void clusterImageEdge(
	const Mat& E, const Mat& O,
	int& _segCnt, Mat& _segIds, 
	vector<float>& meanX, vector<float>& meanY, vector<float>& meanO,
	vector<float>& _segMag,
	vector<int>& _segR, vector<int>& _segC,
	vector<vector<float>>& _segAff, vector<vector<int>>& _segAffIdx,
	float _edgeMinMag, float _edgeMergeThr, float _clusterMinMag, float _gamma) {

	int c, r, cd, rd, i, j;
	int h = E.rows, w = E.cols;

	_segIds.create(h, w, CV_32SC1);
	_segCnt = 1;

	// greedily merge connected edge pixels into clusters
	for (c = 0; c < w; c++) {
		for (r = 0; r < h; r++) {
			if (c == 0 || r == 0 || c == w - 1 || r == h - 1 || E.at<float>(r, c) <= _edgeMinMag)
				_segIds.at<int>(r, c) = -1;
			else
				_segIds.at<int>(r, c) = 0;
		}
	}

	for (c = 1; c < w - 1; c++) {
		for (r = 1; r < h - 1; r++) {
			if (_segIds.at<int>(r, c) != 0) continue;

			float sumv = 0; int c0 = c, r0 = r;
			vector<float> vs;
			vector<int> cs, rs;

			while (sumv < _edgeMergeThr) {
				_segIds.at<int>(r0, c0) = _segCnt;

				float o0 = O.at<float>(r0, c0), o1, v;
				bool found;

				for (cd = -1; cd <= 1; cd++) {
					for (rd = -1; rd <= 1; rd++) {
						if (_segIds.at<int>(r0 + rd, c0 + cd) != 0) continue; found = false;
						for (i = 0; i < cs.size(); i++)
						if (cs[i] == c0 + cd && rs[i] == r0 + rd) { found = true; break; }
						if (found) continue;
						o1 = O.at<float>(r0 + rd, c0 + cd);
						v = fabs(o1 - o0) / CV_PI;
						if (v > .5) v = 1 - v;
						vs.push_back(v); cs.push_back(c0 + cd); rs.push_back(r0 + rd);
					}
				}

				float minv = 1000; j = 0;
				for (i = 0; i < vs.size(); i++) {
					if (vs[i] < minv) {
						minv = vs[i]; c0 = cs[i]; r0 = rs[i]; j = i;
					}
				}
				sumv += minv; if (minv < 1000) vs[j] = 1000;
			}
			_segCnt++;
		}
	}

	// merge or remove small segments
	_segMag.resize(_segCnt, 0);
	for (c = 1; c < w - 1; c++) {
		for (r = 1; r< h - 1; r++) {
			if ((j = _segIds.at<int>(r, c)) > 0) _segMag[j] += E.at<float>(r, c);
		}
	}

	for (c = 1; c < w - 1; c++) {
		for (r = 1; r < h - 1; r++) {
			if ((j = _segIds.at<int>(r, c)) > 0 && _segMag[j] <= _clusterMinMag) _segIds.at<int>(r, c) = 0;
		}
	}

	i = 1;
	while (i > 0) {
		i = 0;
		for (c = 1; c < w - 1; c++) {
			for (r = 1; r< h - 1; r++) {
				if (_segIds.at<int>(r, c) != 0) continue;

				float o0 = O.at<float>(r, c), o1, v, minv = 1000;
				j = 0;
				for (cd = -1; cd <= 1; cd++) {
					for (rd = -1; rd <= 1; rd++) {
						if (_segIds.at<int>(r + rd, c + cd) <= 0) continue;
						o1 = O.at<float>(r + rd, c + cd);
						v = fabs(o1 - o0) / CV_PI;
						if (v > .5) v = 1 - v;
						if (v < minv) { minv = v; j = _segIds.at<int>(r + rd, c + cd); }
					}
				}
				_segIds.at<int>(r, c) = j;
				if (j > 0) i++;
			}
		}
	}

	// compactify representation
	_segMag.assign(_segCnt, 0);
	vector<int> map(_segCnt, 0); _segCnt = 1;
	for (c = 1; c < w - 1; c++) for (r = 1; r < h - 1; r++) if ((j = _segIds.at<int>(r, c)) > 0) _segMag[j] += E.at<float>(r, c);
	for (i = 0; i < _segMag.size(); i++) if (_segMag[i] > 0) map[i] = _segCnt++;
	for (c = 1; c < w - 1; c++) for (r = 1; r < h - 1; r++) if ((j = _segIds.at<int>(r, c)) > 0) _segIds.at<int>(r, c) = map[j];

	// compute positional means and recompute _segMag
	_segMag.assign(_segCnt, 0);
	vector<float> meanOx(_segCnt, 0), meanOy(_segCnt, 0);
	meanX.assign(_segCnt, 0), meanY.assign(_segCnt, 0);
	meanO.assign(_segCnt, 0);
	for (c = 1; c < w - 1; c++) {
		for (r = 1; r < h - 1; r++) {
			j = _segIds.at<int>(r, c); if (j <= 0) continue;
			float m = E.at<float>(r, c), o = O.at<float>(r, c); _segMag[j] += m;
			meanOx[j] += m * cos(2 * o); meanOy[j] += m * sin(2 * o);
			meanX[j] += m * c; meanY[j] += m * r;
		}
	}
	for (i = 0; i < _segCnt; i++) {
		if (_segMag[i] > 0) {
			float m = _segMag[i]; meanX[i] /= m; meanY[i] /= m;
			meanO[i] = atan2(meanOy[i] / m, meanOx[i] / m) / 2;
		}
	}

	// compute segment affinities
	_segAff.resize(_segCnt); _segAffIdx.resize(_segCnt);
	for (i = 0; i < _segCnt; i++) _segAff[i].resize(0);
	for (i = 0; i < _segCnt; i++) _segAffIdx[i].resize(0);

	const int rad = 2;
	for (c = rad; c < w - rad; c++) {
		for (r = rad; r < h - rad; r++) {
			int s0 = _segIds.at<int>(r, c);
			if (s0 <= 0) continue;
			for (cd = -rad; cd <= rad; cd++) {
				for (rd = -rad; rd <= rad; rd++) {
					int s1 = _segIds.at<int>(r + rd, c + cd);
					if (s1 <= s0) continue;
					bool found = false;
					for (i = 0; i < _segAffIdx[s0].size(); i++) if (_segAffIdx[s0][i] == s1) { found = true; break; }
					if (found) continue;
					float o = atan2(meanY[s0] - meanY[s1], meanX[s0] - meanX[s1]) + CV_PI / 2;
					float a = fabs(cos(meanO[s0] - o) * cos(meanO[s1] - o)); a = pow(a, _gamma);
					_segAff[s0].push_back(a); _segAffIdx[s0].push_back(s1);
					_segAff[s1].push_back(a); _segAffIdx[s1].push_back(s0);
				}
			}
		}
	}

	// compute _segC and _segR
	_segC.resize(_segCnt); _segR.resize(_segCnt);
	for (c = 1; c < w - 1; c++) for (r = 1; r < h - 1; r++) if ((j = _segIds.at<int>(r, c)) > 0) { _segC[j] = c; _segR[j] = r; }
}

vector<Point2f> detectHarrisKeyPoints(const Mat& im_gray, int max_points,
	double quality_level, double min_distance, int block_size, double k) {

	vector<Point2f> points;
	goodFeaturesToTrack(im_gray, points, max_points, quality_level, min_distance, Mat(), block_size, true, k);

	return points;
}