/*
 * Definition of a BaseDrawer, which is a Drawer that draws basic geometry using Vertex Buffer Objects.
 * The final result will only be influenced by the OpenGL lighting conditions.
 */

#ifndef BASEDRAWER_H_
#define BASEDRAWER_H_

#include "Drawer.h"

namespace meshutil {

	class BaseDrawer : public Drawer
	{
	private:
		void draw_tstrips(const trimesh::TriMesh &mesh);
	public:
		BaseDrawer();
		virtual void draw(Model* m, trimesh::vec camera_position);
	};

};

#endif /* BASEDRAWER_H_ */
