/*
 * Several general-purpose methods for computing mesh info.
 */

#ifndef MESH_INFO_H_
#define MESH_INFO_H_

#include "TriMesh.h"
#include "Model.h"
#include <vector>

namespace meshutil {

	void computeFaceNormals(const trimesh::TriMesh &mesh, std::vector<trimesh::vec> &facenormals);
	float computeFeatureSize(const trimesh::TriMesh &mesh);
	bool to_camera(Model* m, int face, trimesh::vec camera_position);

};

#endif /* MESH_INFO_H_ */
