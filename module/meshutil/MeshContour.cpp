/*
 * MeshContour.cpp
 */

#include <iostream>
#include <GL/glew.h>
#include "MeshContour.h"
#include "edge_util.h"

using namespace cv;
using namespace std;
using namespace meshutil;

inline bool is_point_valid(const Mat& m, int x, int y) { return (x >= 0 && x < m.cols && y >= 0 && y < m.rows); }

shared_ptr<MeshContour> MeshContour::dump_glbuffer() {

	GLint V[4];
	glGetIntegerv(GL_VIEWPORT, V);
	GLint width = V[2], height = V[3];

	Mat img(height, width, CV_8UC3);

	glPixelStorei(GL_PACK_ALIGNMENT, (img.step & 3) ? 1 : 4);
	glPixelStorei(GL_PACK_ROW_LENGTH, img.step / img.elemSize());
	glReadPixels(V[0], V[1], width, height, GL_BGR, GL_UNSIGNED_BYTE, img.data);
	flip(img, img, 0);
	
	return load_image(img);
}

shared_ptr<MeshContour> MeshContour::load_image(Mat& img) {

	Mat bw, ori;
	
	// to gray image
	cvtColor(img, bw, CV_BGR2GRAY);
	bw = 255 - bw;
	cv::threshold(bw, bw, 128, 255, CV_THRESH_BINARY);

	bw = Mat(bw, computeBoundingBox<uchar>(bw));
	edgeThinningZhangSuen(bw);
	computeSobelOrientation(bw, ori);

	shared_ptr<MeshContour> contour_ptr = make_shared<MeshContour>();
	contour_ptr->_E_data = move(bw);
	contour_ptr->_O_data = move(ori);
	return contour_ptr;
}

/*
double minVal, maxVal;
minMaxLoc(sobelx, &minVal, &maxVal); // find minimum and maximum intensities
Mat draw;
sobelx.convertTo(draw, CV_8U, 255.0/(maxVal - minVal), -minVal * 255.0/(maxVal - minVal));

double min, max;
Mat E;
_E_data.convertTo(E, CV_32F);
E /= 255;

cv::minMaxLoc(E, &min, &max);
cout << min << endl << max << endl;
*/

void MeshContour::cluster() {

	Mat E, O;
	_E_data.convertTo(E, CV_32F);
	_O_data.convertTo(O, CV_32F);
	
	E /= 255.0;
	for (int c = 0; c < O.cols; c++) {
		for (int r = 0; r < O.rows; r++) {
			if (O.at<float>(r, c) > CV_PI) O.at<float>(r, c) -= CV_PI;
		}
	}

	vector<float> segMag, segMeanX, segMeanY, segMeanO;
	vector<int> segR, segC;
	vector<vector<float>> segAff;

	int h = E.rows, w = E.cols;
	clusterImageEdge(E, O, _segNum, _segIdx, segMeanX, segMeanY, segMeanO, segMag, segR, segC, segAff, _segAffIdx);
	
	_segNodes.reserve(_segNum);
	for (int i = 0; i < _segNum; i++) {
		Node n;
		n.i = i;
		n.x = segMeanX[i] / w;
		n.y = segMeanY[i] / h;
		n.o = segMeanO[i];
		n.m = segMag[i] / (w + h);
		_segNodes.push_back(n);
	}

	cout << "Generated " << _segNum << " edge segments." << endl;
}

Mat MeshContour::visualize_segIdx() const {

	int h = _segIdx.rows, w = _segIdx.cols;
	int r, c, i;
	Mat V(h, w, CV_8UC3);

	for (c = 0; c < w; c++) {
		for (r = 0; r < h; r++) {
			i = _segIdx.at<int>(r, c);
			V.at<Vec3b>(r, c)[0] = i <= 0 ? 255 : ((174 * i + 80) % 255);
			V.at<Vec3b>(r, c)[1] = i <= 0 ? 255 : ((7 * i + 3) % 255);
			V.at<Vec3b>(r, c)[2] = i <= 0 ? 255 : ((123 * i + 128) % 255);
		}
	}
	return V;
}

Mat MeshContour::visualize_keySegIdx(const vector<int>& key_idx) const {
	
	int h = _segIdx.rows, w = _segIdx.cols;
	int r, c, i, j;
	Mat V(h, w, CV_8UC3);

	for (c = 0; c < w; c++) {
		for (r = 0; r < h; r++) {
			i = _segIdx.at<int>(r, c);
			V.at<Vec3b>(r, c)[0] = i <= 0 ? 255 : 0;
			V.at<Vec3b>(r, c)[1] = i <= 0 ? 255 : 0;
			V.at<Vec3b>(r, c)[2] = i <= 0 ? 255 : 0;

			bool found = false;
			for (j = 0; j < key_idx.size(); j++) {
				if (key_idx[j] == i) {
					found = true;
					break;
				}
			}

			if (found) V.at<Vec3b>(r, c)[2] = 255;
		}
	}
	return V;
}

void MeshContour::collect_node_walks(vector<vector<int>>& walks, const std::vector<int>& w, int l) {

	int idx = w.back();
	int nadj = _segAffIdx[idx].size();

	if (l > 0 && nadj > 0) {
		
		int nfound = 0;
		for (int i = 0; i < nadj; i++) {
			
			int adj_idx = _segAffIdx[idx][i];
			bool found = false;
			for (int j = 0; j < w.size(); j++) {
				if (w[j] == adj_idx) {
					found = true;
					nfound++;
					break;
				}
			}

			if (found) {
				continue;
			} else {
				vector<int> new_w = w;
				new_w.push_back(adj_idx);
				collect_node_walks(walks, new_w, l - 1);
			}
		}

		if (nfound == nadj) walks.push_back(w);
	} else {
		walks.push_back(w);
		return;
	}
}

vector<vector<int>> MeshContour::collect_harris_nodes_walks(int l) {

	assert(l >= 0);

	vector<Point2f> points = detectHarrisKeyPoints(_E_data, 100);
	vector<int> map(_segNum, 0);
	for (int i = 0; i < points.size(); i++) {
		int idx = find_closet_segIdx(points[i].x, points[i].y);
		map[idx]++;
	}

	vector<int> nodes_idx;
	for (int i = 0; i < map.size(); i++) {
		if (map[i] > 0) nodes_idx.push_back(i);
	}
	cout << "Collected " << nodes_idx.size() << " harris segments." << endl;

	vector<vector<int>> walks;
	for (int i = 0; i < nodes_idx.size(); i++) {
		vector<int> walk;
		walk.push_back(nodes_idx[i]);
		collect_node_walks(walks, walk, l);
	}

	cout << "Collected " << walks.size() << " harris segment walks." << endl;
	return walks;
}

vector<vector<int>> MeshContour::collect_all_nodes_walks(int l) {

	assert(l >= 0);

	vector<int> nodes_idx;
	for (int i = 0; i < _segNum; i++) nodes_idx.push_back(i);

	vector<vector<int>> walks;
	for (int i = 0; i < nodes_idx.size(); i++) {
		vector<int> walk;
		walk.push_back(nodes_idx[i]);
		collect_node_walks(walks, walk, l);
	}

	cout << "Collected " << walks.size() << " all segment walks." << endl;
	return walks;
}

int MeshContour::find_closet_segIdx(int x, int y) {

	int idx = _segIdx.at<int>(y, x);
	if (idx > 0) return idx;

	int d = 1;
	while (d < _segIdx.rows || d < _segIdx.cols) {
		for (int r = y - d; r < y + d; r++) {
			for (int c = x - d; c < x + d; c++) {
				if (is_point_valid(_segIdx, c, r)) {
					idx = _segIdx.at<int>(r, c);
					if (idx > 0) return idx;
				}
			}
		}
		d++;
	}

	assert(true);
}