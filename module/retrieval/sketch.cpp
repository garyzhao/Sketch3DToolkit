/*
 * sketch.cpp
 */

#include "retrieval.h"
#include "edge_util.h"
#include <fstream>
#include <iomanip>

using namespace cv;
using namespace std;

//
// Helper functions.
//
bool is_feature_valid(const Mat& mat)
{
	double minVal, maxVal;
	minMaxIdx(mat, &minVal, &maxVal);
	return !(minVal == 0 && maxVal == 0);
}

Mat formatViewImage(const Mat& image_)
{
	Mat image = image_;

	if (image.channels() == 3) // to gray image
		cvtColor(image, image, CV_BGR2GRAY);

	assert(image.channels() == 1);
	assert(image.depth() == CV_8U);

	Mat image_bw(image, computeBoundingBox<uchar>(255 - image));	// compute bbox
	image_bw = paddingImage(image_bw, 5);							// padding border
	resize(image_bw, image_bw, Size(IMAGE_SCALE, IMAGE_SCALE));		// resize image

	return image_bw;
}

//
// Main functions.
//
Mat load_image(const string& imagePath)
{
	Mat image;
	image = imread(imagePath, CV_LOAD_IMAGE_GRAYSCALE);

	if (!image.data)
	{
		cerr << "Error: No image data. [" << imagePath << "]" << endl;
		abort();
	}
	
	return image;
}

void process_image(const Mat& image, const function<void(const ImagePatch&, const Mat&, TupleVector&)>& l)
{
	PatchDescriptor HOG(image, HOG_NUM_CELL, HOG_NUM_CELL, HOG_NUM_BIN);
	ImagePatch imagePatch(HOG, IMAGE_PATCH_NUM, IMAGE_PATCH_SIZE, 5);

	// extract feature
	while (imagePatch.next())
	{
		Mat featureMat;
		HOG.compute(imagePatch.rect(), featureMat);
		if (is_feature_valid(featureMat))
		{
			TupleVector tupleVector = MinHash::Hash(featureMat.ptr<double>(), int_cast_<uint16_t>(featureMat.cols), 0.2);
			l(imagePatch, featureMat, tupleVector);
		}
	}
}