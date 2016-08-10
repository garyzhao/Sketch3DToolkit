/**
 * The implementation of several general-purpose methods to compute view-dependent per-vertex information.
 */

#include "vertex_info.h"

using namespace std;
using namespace trimesh;

namespace meshutil {

	/**
	 * Compute ndotv_ for a given mesh_
	 *
	 * @param *mesh: Pointer to a TriMesh
	 * @param camera: the current camera position, in 3-dimensional coordinates
	 * @param &ndotv: The vector where the results will be stored.
	 *
	 */
	void compute_ndotv(const TriMesh &mesh, const vec camera, vector<float> &ndotv)
	{
		ndotv.resize(mesh.vertices.size());

#pragma omp parallel for
		for (unsigned int i = 0; i < mesh.vertices.size(); i++)
		{
			vec view = camera - mesh.vertices[i];
			normalize(view);
			ndotv[i] = mesh.normals[i] DOT view;
		}
	}

	/**
	 * Compute view-dependent curvature information for a given mesh_
	 *
	 * @param *mesh: Pointer to a TriMesh
	 * @param camera: the current camera position, in 3-dimensional coordinates
	 * @param &kr: The vector where the results of the radial curvature computation will be stored
	 * @param &num: The vector where numerator of the directional derivative of the radial curvature computation will be stored
	 * @param &den: The vector where denominator of the directional derivative of the radial curvature computation will be stored
	 *
	 */
	void compute_CurvDerivatives(const TriMesh &mesh, const vec camera, vector<float> &kr, vector<float> &num, vector<float> &den, float sc_threshold)
	{
		kr.resize(mesh.vertices.size());
		num.resize(mesh.vertices.size());
		den.resize(mesh.vertices.size());

#pragma omp parallel for
		for (unsigned int i = 0; i < mesh.vertices.size(); i++)
		{
			// compute ndtov
			vec view = camera - mesh.vertices[i];
			float norm = 1.0f / len(view);
			view *= norm;
			float ndotv = mesh.normals[i] DOT view;
			// compute radial curvature (Euler's formula)
			float u = view DOT mesh.pdir1[i];
			float u2 = u*u;
			float t = view DOT mesh.pdir2[i];
			float v2 = t*t;
			kr[i] = ((mesh.curv1)[i] * u2 + (mesh.curv2)[i] * v2);
			// compute numerator and denominator of derivative of radial curvature
			// suggestive contours computation
			num[i] = u2 * (u*mesh.dcurv[i][0] + 3.0f*t*mesh.dcurv[i][1]) + v2 * (3.0f*u*mesh.dcurv[i][2] + t*mesh.dcurv[i][3]);
			float thetafix = (1.0f / (u2 + v2));
			num[i] *= thetafix;
			float tr = (mesh.curv2[i] - mesh.curv1[i]) * u * t *thetafix;
			num[i] -= 2.0f * ndotv *sqr(tr);
			den[i] = ndotv;
			// filtering of lines: see NPAR 2004 paper, page 5: other strategies for trimming suggestive contours
			num[i] -= sc_threshold * den[i];
		}
	}

};
