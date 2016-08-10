/*
 * vertex_info.h
 *
 * 	The definition of several general-purpose methods to compute view-dependent per-vertex information.
 */

#ifndef VERTEX_INFO_H_
#define VERTEX_INFO_H_

#include "TriMesh.h"
#include "Model.h"
#include <vector>

namespace meshutil {

	void compute_ndotv(const trimesh::TriMesh &mesh, const trimesh::vec camera, std::vector<float> &ndtov);
	void compute_CurvDerivatives(const trimesh::TriMesh &mesh, const trimesh::vec camera, std::vector<float> &kr, std::vector<float> &num, std::vector<float> &den, float sc_threshold);

};

#endif /* VERTEX_INFO_H_ */
