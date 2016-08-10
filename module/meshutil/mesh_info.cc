/*
 * Several general-purpose methods for computing mesh info.
 */

#include "mesh_info.h"

using namespace std;
using namespace trimesh;

namespace meshutil {

	/**
	 * Pre-compute the face-normals for every face in a given mesh
	 *
	 * @param *mesh : A pointer to the mesh
	 * @param &facenormals: The vector in which the results will be stored, indexed by vertex number
	 */
	void computeFaceNormals(const TriMesh &mesh, vector<vec> &facenormals) {
		int n = mesh.faces.size();
		facenormals.resize(n);
		for (int i = 0; i < n; i++) {
			vec p0 = mesh.vertices[(mesh.faces)[i][0]];
			vec p1 = mesh.vertices[(mesh.faces)[i][1]];
			vec p2 = mesh.vertices[(mesh.faces)[i][2]];
			// compute vector, normalize it
			vec facenormal = (p0 - p1) CROSS(p1 - p2);
			normalize(facenormal);
			// put it in result vector
			facenormals[i] = facenormal;
		}
	}

	/**
	 * Compute the feature size for a given mesh, using random sampling
	 *
	 * @param *mesh : A pointer to the mesh
	 */
	float computeFeatureSize(const TriMesh &mesh) {
		int nv = mesh.curv1.size();
		// get samples
		int nsamp = min(nv, 500);
		vector<float> samples;
		samples.reserve(nsamp * 2);
		for (int i = 0; i < nsamp; i++) {
			// Quick 'n dirty portable random number generator
			static unsigned randq = 0;
			randq = unsigned(1664525) * randq + unsigned(1013904223);
			int ind = randq % nv;
			samples.push_back(fabs(mesh.curv1[ind]));
			samples.push_back(fabs(mesh.curv2[ind]));
		}
		const float frac = 0.1f;
		const float mult = 0.01f;
		float max_feature_size = 0.05f * mesh.bsphere.r;
		int which = int(frac * samples.size());
		// test on samples
		nth_element(samples.begin(), samples.begin() + which, samples.end());
		return min(mult / samples[which], max_feature_size);
	}

	/**
	 * Check wether or not a given face is turned to the camera
	 *
	 * @param *m: the model this face is in
	 * @param face: the face number
	 * @param camera_position: the current position of the camera
	 */
	bool to_camera(Model* m, int face, vec camera_position) {
		if (face < 0) return 0;

		// vector to point on face
		vec view = camera_position - (m->mesh_->vertices[(m->mesh_->faces[face])[0]]);
		normalize(view);
		vec facenormal = m->facenormals_[face];
		// do dot product to get camera facing test
		return ((view^facenormal) > 0.0f);
	}

};