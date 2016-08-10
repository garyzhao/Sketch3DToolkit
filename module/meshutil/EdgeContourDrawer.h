/*
 * Definition of an EdgeContourDrawer, which is a LineDrawer that draws contour lines on the edges between vertices.
 * This results in fast computation and rendering, but the generated contour lines can contain loops and may suffer
 * from bad temporal coherence between several frames.
 */

#ifndef EDGECONTOURDRAWER_H_
#define EDGECONTOURDRAWER_H_

#include "Drawer.h"
#include "LineDrawer.h"

namespace meshutil {

	class EdgeContourDrawer : public LineDrawer{
	private:
		void find_edges(Model* m, trimesh::vec camera_position);
	public:
		EdgeContourDrawer(trimesh::vec color, float linewidth);
		virtual ~EdgeContourDrawer();
		virtual void draw(Model* m, trimesh::vec camera_position);
	};
}

#endif /* EDGECONTOURDRAWER_H_ */
