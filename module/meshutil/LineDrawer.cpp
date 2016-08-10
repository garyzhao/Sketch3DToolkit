/*
 * Implementation of the protected/public functions of the abstract class of LineDrawers, which
 * represents a subset of Drawers that solely draw lines.
 */

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <GL/glui.h>
#include "LineDrawer.h"

using namespace meshutil;
using namespace trimesh;

/**
 * Protected LineDrawer constructor
 */
LineDrawer::LineDrawer(vec color, float linewidth) : Drawer(true), linecolor_(color), linewidth_(linewidth)
{

}

/**
 * Flush the drawbuffers to the OpenGL Draw buffer to display the computed lines, then clear them.
 */
void LineDrawer::flushDrawBuffer()
{
	// if we've got some lines to draw ...
	if (!drawbuffer_vertices_.empty()) {
		glEnableClientState(GL_VERTEX_ARRAY);

		// flush draw buffer to avoid overflow
		size_t N = 0;
		while (drawbuffer_vertices_.size() > N)
		{
			glVertexPointer(3, GL_FLOAT, sizeof(drawbuffer_vertices_[N]), &drawbuffer_vertices_[N][0]);
			// if per-line colors were defined
			if (!drawbuffer_colors_.empty()) {
				glEnableClientState(GL_COLOR_ARRAY);
				glColorPointer(4, GL_FLOAT, sizeof(drawbuffer_colors_[N]), &drawbuffer_colors_[N][0]);
			}
			// push lines to GPU
			glDrawArrays(GL_LINES, 0, std::min(drawbuffer_limit, drawbuffer_vertices_.size() - N));
			N += drawbuffer_limit;
		}

		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);
		// clear the buffers
		drawbuffer_vertices_.clear();
		drawbuffer_colors_.clear();
	}
}

/**
 * Returns the current line color
 */
vec LineDrawer::getLineColor(){
	return linecolor_;
}

/**
 * Returns the current line width
 */
float LineDrawer::getLineWidth(){
	return linewidth_;
}

/**
 * Sets the current line color
 */
void LineDrawer::setLineColor(vec color){
	this->linecolor_ = color;
}

/**
 * Sets the current line width
 */
void LineDrawer::setLineWidth(float width){
	this->linewidth_ = width;
}