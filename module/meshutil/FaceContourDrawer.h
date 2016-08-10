/*
 * Definition of a FaceContourDrawer, which is a LineDrawer that draws contour lines on the faces of a given model,
 * using lineair interpolation between the zero points of n dot v.
 */

#ifndef FACECONTOURDRAWER_H_
#define FACECONTOURDRAWER_H_

#include "LineDrawer.h"

namespace meshutil {

	class FaceContourDrawer : public LineDrawer{
	private:
		void construct_faceline(Model* m, int v0, int v1, int v2);
		void find_facelines(Model* m, trimesh::vec camera_position);
	public:
		FaceContourDrawer(trimesh::vec color, float linewidth);
		virtual void draw(Model* m, trimesh::vec camera_position);
	};

};

#endif /* FACECONTOURDRAWER_H_ */
