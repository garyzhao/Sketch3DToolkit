/*
 * Implementation of an EdgeContourDrawer, which is a LineDrawer that draws contour lines on the edges between vertices.
 * This results in fast computation and rendering, but the generated contour lines can contain loops and may suffer
 * from bad temporal coherence between several frames.
 */

#include "EdgeContourDrawer.h"
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
EdgeContourDrawer::EdgeContourDrawer(vec color = vec(0, 0, 0), float linewidth = 4.0) : LineDrawer(color, linewidth)
{
	// nothing left to do
}

/**
 * Draw the edge contours for a given model and camera position
 *
 * @param Model* : the model
 * @param camera_position: the current camera position, given in 3d-coordinates
 */
void EdgeContourDrawer::draw(Model* m, vec camera_position)
{
	if (isVisible()){
		// setup OpenGL for nice linedrawing
		glPolygonOffset(5.0f, 30.0f);
		glEnable(GL_LINE_SMOOTH); // line anti-aliasing
		glDisable(GL_LIGHTING);
		glEnable(GL_POLYGON_OFFSET_FILL);
		// set color and linewidth_
		glLineWidth(linewidth_);
		glColor3f(linecolor_[0], linecolor_[1], linecolor_[2]);
		// find contour edges
		find_edges(m, camera_position);
		// flush draw buffer to draw found lines
		flushDrawBuffer();
	}
}

/**
 * Finds the contour edges for a given model and camera position and buffers them
 *
 * @param Model* : the model
 * @param camera_position: the current camera position, given in 3d-coordinates
 */
void EdgeContourDrawer::find_edges(Model* m, vec camera_position)
{
	// some aliases to write readable code
	const vector<TriMesh::Face> &faces = m->mesh_->faces;
	const vector<point> &vertices = m->mesh_->vertices;
	const vector<TriMesh::Face> &edge = m->mesh_->across_edge;

	// for every face
	for (unsigned int i = 0; i < faces.size(); i++){
		if (to_camera(m, i, camera_position)){
			// check for broken edge map (=holes in the mesh, faces without neighbour)
			if (unlikely((edge[i][0] < 0) | (edge[i][1] < 0) | (edge[i][3] < 0))){
				continue; // edge map broken -> skip this face
			}
			// if edge map is not broken, add edges which are facing away
			if (!to_camera(m, edge[i][0], camera_position)){
				drawbuffer_vertices_.push_back(vertices[faces[i][1]]);
				drawbuffer_vertices_.push_back(vertices[faces[i][2]]);
			}
			if (!to_camera(m, edge[i][1], camera_position)){
				drawbuffer_vertices_.push_back(vertices[faces[i][0]]);
				drawbuffer_vertices_.push_back(vertices[faces[i][2]]);
			}
			if (!to_camera(m, edge[i][2], camera_position)){
				drawbuffer_vertices_.push_back(vertices[faces[i][0]]);
				drawbuffer_vertices_.push_back(vertices[faces[i][1]]);
			}
		}
	}
}

EdgeContourDrawer::~EdgeContourDrawer(){
	// nothing to do in destructor
}