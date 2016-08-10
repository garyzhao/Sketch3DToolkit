/*
 * retrieval.h
 */

#ifndef _RETRIEVAL_H_
#define _RETRIEVAL_H_

#include "MinHash.h"
#include "SlidingPatch.h"
#include "HOGDescriptor.h"
#include "SaliencyDescriptor.h"
#include "HashIndexer.h"

#include "Mesh.h"
#include "MeshWindow.h"
#include "BatchMeshWindow.h"

#define _USE_MATH_DEFINES

#include <math.h>
#include <functional>
#include <iostream>

#define HOG_NUM_CELL		3 // 8
#define HOG_NUM_BIN			9 // 8
#define IMAGE_SCALE			200
#define IMAGE_PATCH_NUM		17
#define IMAGE_PATCH_SIZE	40

#define MESH_VIEW_NUM		20
#define MESH_WIN_SIZE		600

typedef ::MinHash64						MinHash;
typedef MinHash::Tuple					Tuple;
typedef MinHash::TupleVector			TupleVector;
typedef ::HashIndexer<MinHash>			Indexer;
typedef ::HashIndexer<MinHash, size_t>	ViewIndexer;
typedef ::HOGDescriptor					PatchDescriptor;
typedef ::SlidingPatch					ImagePatch;

cv::Mat load_image(const std::string& imagePath);
void process_image(const cv::Mat& image, const std::function<void(const ImagePatch&, const cv::Mat&, TupleVector&)>& l);

template<typename idx_type>
void insert_image(const cv::Mat& image, const idx_type& imageIdx, HashIndexer<MinHash, idx_type>& hashIndexer)
{
	cv::Mat	image_bw = formatViewImage(image);
	process_image(image_bw, [&imageIdx, &hashIndexer](const ImagePatch& imagePatch, const cv::Mat& featureMat, const TupleVector& tupleVector) {
		hashIndexer.insert(imageIdx, PatchInfo(0, imagePatch.index()), tupleVector);
	});
}

template<typename idx_type>
void insert_image(const std::string& imagePath, const idx_type& imageIdx, HashIndexer<MinHash, idx_type>& hashIndexer)
{
	cv::Mat image = load_image(imagePath);
	insert_image(image, imageIdx, hashIndexer);
}

template<typename idx_type>
std::vector<std::pair<idx_type, double>> retrieve_image(const std::string& imagePath, HashIndexer<MinHash, idx_type>& hashIndexer)
{
	cv::Mat image = formatViewImage(load_image(imagePath));
	std::vector<std::pair<PatchInfo, TupleVector>> patchVector;
	patchVector.reserve(IMAGE_PATCH_NUM * IMAGE_PATCH_NUM);

	SaliencyDescriptor Sal(image);

	process_image(image, [&patchVector, &Sal](const ImagePatch& imagePatch, const cv::Mat& featureMat, TupleVector& tupleVector) 
	{
		PatchInfo info(0, imagePatch.index());
		cv::Mat saliency;
		Sal.compute(imagePatch.rect(), saliency);
		info.iW = 1 - 0.65 * saliency.at<double>(0, 0);

		patchVector.push_back(make_pair<PatchInfo, TupleVector>(move(info), move(tupleVector)));
	});

	std::vector<std::pair<idx_type, double>> resultVector = hashIndexer.retrieve(patchVector);
	return resultVector;
}

template<typename idx_type, typename lambda_type>
std::vector<std::pair<idx_type, double>> retrieve_mesh(const std::string& imagePath, HashIndexer<MinHash, idx_type>& hashIndexer, lambda_type& l)
{
	std::vector<std::pair<idx_type, double>> resultVector = retrieve_image(imagePath, hashIndexer);
	std::vector<std::pair<idx_type, double>> meshVector;
	std::unordered_map<idx_type, bool> meshMap;

	for (const auto& image_info : resultVector)
	{
		idx_type mesh_idx = l(image_info.first);
		if (!meshMap[mesh_idx])
		{
			meshMap[mesh_idx] = true;
			meshVector.emplace_back(mesh_idx, image_info.second);
		}
	}

	return meshVector;
}

cv::Mat formatViewImage(const cv::Mat& image);
cv::Mat formatDepthImage(const cv::Mat& image);
double computeDepthIoU(const cv::Mat& image1, const cv::Mat& image2);
std::unique_ptr<BatchMeshWindow> initGLContext();
void clearGLContext(std::unique_ptr<BatchMeshWindow>& main_window_ptr);
std::vector<ViewInfo> sampleMeshViewpoints(const std::string& mesh_path, BatchMeshWindow& main_window, int num = MESH_VIEW_NUM);
std::vector<size_t> rankMeshViews(const std::vector<ViewInfo>& view_info_vector, std::vector<std::pair<size_t, double>>& view_idx_vector, int max_num = 14, double th = 0.85);

cv::Mat glDumpRenderbuffer();
cv::Mat glDumpDepthbuffer();

vec3d euler_angle(const trimesh::xform& xf);
double rot_angle(const trimesh::xform& xf);
vec3d rot_vector(const trimesh::xform& xf, double angle);
double central_angle_dist(const vec2d& v1, const vec2d& v2);

#endif  // _RETRIEVAL_H_