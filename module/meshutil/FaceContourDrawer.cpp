/*
 * Implementation of a FaceContourDrawer, which is a LineDrawer that draws contour lines on the faces of a given model,
 * using lineair interpolation between the zero points of n dot v.
 *
 * This results in correct rendering with a high temporal coherence, but it is slower.
 */

#include "FaceContourDrawer.h"

using namespace std;
using namespace meshutil;
using namespace trimesh;

/**
 * Constructs a new FaceContourDrawer
 *
 * @param color : the linecolor in which the Drawer will draw its lines
 * @param linewidth: the linewidth in which the Drawer will draw its lines
 */
FaceContourDrawer::FaceContourDrawer(vec color, float linewidth) : LineDrawer(color, linewidth){
	// nothing left to do
}

/**
 * Draws the face contours for a given model and camera position
 *
 * @param Model* : the model
 * @param camera_position: the current camera position, given in 3d-coordinates
 */
void FaceContourDrawer::draw(Model* m, vec camera_position){
	if (isVisible()){
		// configure OpenGL to draw nice lines
		glPolygonOffset(5.0f, 30.0f);
		glEnable(GL_LINE_SMOOTH);
		glDisable(GL_LIGHTING);
		glEnable(GL_POLYGON_OFFSET_FILL);
		// set color and linewidth_
		glLineWidth(linewidth_);
		glColor3f(linecolor_[0], linecolor_[1], linecolor_[2]);
		// we need ndotv_ information
		m->needNdotV(camera_position);
		// find the contour lines on the faces
		find_facelines(m, camera_position);
		// flush the drawbuffer to draw found lines
		flushDrawBuffer();
	}
}

/**
 * Finds the contour lines on the faces of the model and buffer them
 *
 * @param: Model
 */
void FaceContourDrawer::find_facelines(Model* m, vec camera_position)
{
	// aliases for easy coding
	const vector<float> &ndotv = m->ndotv_;
	const vector<TriMesh::Face> &faces = m->mesh_->faces;
	// for every face
	for (unsigned int i = 0; i < m->mesh_->faces.size(); i++){
		// vector point aliases
		const int &v0 = faces[i][0];
		const int &v1 = faces[i][1];
		const int &v2 = faces[i][2];
		// at least one corner should have different sign of ndotv_
		if (unlikely((ndotv[v0] > 0.0f || ndotv[v1] > 0.0f || ndotv[v2] > 0.0f) &&
			(ndotv[v0] <= 0.0f || ndotv[v1] <= 0.0f || ndotv[v2] <= 0.0f))){
			// which corner has the different sign?
			if ((ndotv[v0] > 0.0f && ndotv[v1] <= 0.0f && ndotv[v2] <= 0.0f) ||
				(ndotv[v0] < 0.0f && ndotv[v1] >= 0.0f && ndotv[v2] >= 0.0f)){
				construct_faceline(m, v0, v1, v2);
			}
			else if ((ndotv[v1] > 0.0f && ndotv[v0] <= 0.0f && ndotv[v2] <= 0.0f) ||
				(ndotv[v1] < 0.0f && ndotv[v0] >= 0.0f && ndotv[v2] >= 0.0f)){
				construct_faceline(m, v1, v0, v2);
			}
			else if ((ndotv[v2] > 0.0f && ndotv[v0] <= 0.0f && ndotv[v1] <= 0.0f) ||
				(ndotv[v2] < 0.0f && ndotv[v0] >= 0.0f && ndotv[v1] >= 0.0f)){
				construct_faceline(m, v2, v0, v1);
			}
		}
	}
}
/**
 * Constructs a face contour line on the face defined by 3 given vertex indices.
 * The first given vertex index should contain the point which has a different value of NdotV
 *
 * @param Model: the model to which the vertices belong
 * @param v0,v1,v2: the vertex indices
 */
void FaceContourDrawer::construct_faceline(Model* m, int v0, int v1, int v2)
{
	float w10 = m->ndotv_[v0] / (m->ndotv_[v0] - m->ndotv_[v1]); // linear interpolation
	float w01 = 1.0 - w10;
	float w20 = m->ndotv_[v0] / (m->ndotv_[v0] - m->ndotv_[v2]);
	float w02 = 1.0 - w20;
	vec p1 = w01 * m->mesh_->vertices[v0] + w10 * m->mesh_->vertices[v1];
	vec p2 = w02 * m->mesh_->vertices[v0] + w20 * m->mesh_->vertices[v2];
	drawbuffer_vertices_.push_back(p1);
	drawbuffer_vertices_.push_back(p2);
}