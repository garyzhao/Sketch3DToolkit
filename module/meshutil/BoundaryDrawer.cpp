#include "BoundaryDrawer.h"
#include "mesh_info.h"

using namespace std;
using namespace trimesh;
using namespace meshutil;

/**
* Construct a new linedrawer with given linecolor and linewidth
*
* @param: color : the desired linecolor
* @param: linewidth: the desired linewidth
*/
BoundaryDrawer::BoundaryDrawer(vec color = vec(0, 0, 0), float linewidth = 4.0) : LineDrawer(color, linewidth)
{
	// nothing left to do
}

BoundaryDrawer::~BoundaryDrawer() {
	// nothing to do in destructor
}

/**
* Draw the edge contours for a given model and camera position
*
* @param Model* : the model
* @param camera_position: the current camera position, given in 3d-coordinates
*/
void BoundaryDrawer::draw(Model* m, vec camera_position)
{
	if (isVisible()) {
		// setup OpenGL for nice linedrawing
		glPolygonOffset(5.0f, 30.0f);
		glEnable(GL_LINE_SMOOTH); // line anti-aliasing
		glDisable(GL_LIGHTING);
		glEnable(GL_POLYGON_OFFSET_FILL);
		// set color and linewidth_
		glLineWidth(linewidth_);
		glColor3f(linecolor_[0], linecolor_[1], linecolor_[2]);
	
		for (int i = 0; i < m->mesh_->faces.size(); i++) {
			for (int j = 0; j < 3; j++) {
				if (m->mesh_->across_edge[i][j] < 0) {
					drawbuffer_vertices_.push_back(m->mesh_->vertices[m->mesh_->faces[i][(j + 1) % 3]]);
					drawbuffer_vertices_.push_back(m->mesh_->vertices[m->mesh_->faces[i][(j + 2) % 3]]);
				}
			}
		}
	
		// flush draw buffer to draw found lines
		flushDrawBuffer();
	}
}