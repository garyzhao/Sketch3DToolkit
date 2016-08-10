/*
 * Implementation of a BaseDrawer, which is a Drawer that draws basic geometry using Vertex Buffer Objects.
 * The final result will only be influenced by the OpenGL lighting conditions.
 */

#include "BaseDrawer.h"

using namespace trimesh;
using namespace meshutil;

/**
 * Constructor
 */
BaseDrawer::BaseDrawer() : Drawer(true) {
	// nothing to do
}

/**
 * Draw the given model, given the current camera position
 *
 * @param: Model* : the model to be drawn
 * @param: camera_position: the current camera position, in 3d-coordinates
 */
void BaseDrawer::draw(Model* m, vec camera_position)
{
	if (isVisible()) {
		// setup vertex and array pointers
		glBindBufferARB(GL_ARRAY_BUFFER_ARB, m->vbo_positions_);
		glEnableClientState(GL_VERTEX_ARRAY); // enable vertices
		glVertexPointer(3, GL_FLOAT, 0, 0);
		glBindBufferARB(GL_ARRAY_BUFFER_ARB, m->vbo_normals_);
		glEnableClientState(GL_NORMAL_ARRAY); // enable normals
		glNormalPointer(GL_FLOAT, 0, 0);
		glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
		
		// draw the mesh_ using triangle strips
		glColor3f(1, 1, 1);
		draw_tstrips(*(m->mesh_));

		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
	}
}

/**
 * Draw the given mesh using the Triangle strips method (which is the fastest).
 *
 * @param: themesh: the mesh to be drawn.
 */
void BaseDrawer::draw_tstrips(const TriMesh &themesh)
{
	const int *t = &themesh.tstrips[0];
	const int *end = t + themesh.tstrips.size();
	while (likely(t < end)) { // as long as we haven't reached the TSTRIPS-end
		int striplen = *t++;
		glDrawElements(GL_TRIANGLE_STRIP, striplen, GL_UNSIGNED_INT, t);
		t += striplen;
	}
}