/*
 * LineDrawer.h
 */

#ifndef LINEDRAWER_H_
#define LINEDRAWER_H_

#include "Drawer.h"

const size_t drawbuffer_limit = 4096;

namespace meshutil {

	class LineDrawer : public Drawer {

	protected:
		// line properties
		trimesh::vec linecolor_;
		float linewidth_;
		// buffers
		std::vector<trimesh::vec> drawbuffer_vertices_;
		std::vector<trimesh::vec4> drawbuffer_colors_;

		LineDrawer(trimesh::vec color, float linewidth);
		void flushDrawBuffer();

	public:
		trimesh::vec getLineColor();
		float getLineWidth();
		void setLineColor(trimesh::vec color);
		void setLineWidth(float width);
	};

};

#endif /* LINEDRAWER_H_ */
