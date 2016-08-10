/*
 * Implementation of the Suggestive Contour Drawer class, which can draw suggestive contours for a given model.
 */

#include "SuggestiveContourDrawer.h"
#include "mesh_info.h"

using namespace meshutil;
using namespace std;
using namespace trimesh;

/**
 * Constructor: construct a new Suggestive Contour Drawer with given color, linewidth, fading scheme and threshold
 *
 * @param color: the line color
 * @param linewidth: the line width
 * @param fade: use an alpha value fading scheme or not
 * @param sc_thresh: the threshold for trimming contours with low stability
 */
SuggestiveContourDrawer::SuggestiveContourDrawer(vec color = vec(0, 0, 0), float linewidth = 3.0, bool fade = true, float sc_thresh = 0.01)
	: LineDrawer(color, linewidth), fading_(fade), sc_thresh_(sc_thresh){
	// nothing left to do
}

/**
 * Toggle alpha fading scheme on or off
 */
void SuggestiveContourDrawer::toggleFading(){
	fading_ = !fading_;
}

/**
 * Return the current status of the fading scheme
 */
bool SuggestiveContourDrawer::isFaded(){
	return fading_;
}

/**
 * Draw the suggestive contours for a given Model, viewed from a given camera position
 *
 * @param Model* : the model
 * @param camera_position: the current camera position, given in 3d-coordinates
 */
void SuggestiveContourDrawer::draw(Model* m, vec camera_position){
	if (isVisible()){
		// Setup OpenGL to draw nice lines
		glPolygonOffset(5.0f, 30.0f);
		glEnable(GL_LINE_SMOOTH); // line anti-aliasing
		glDisable(GL_LIGHTING);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// set color and linewidth_
		glLineWidth(linewidth_);
		glColor3f(linecolor_[0], linecolor_[1], linecolor_[2]);

		// if we use fading, set the fade parameter to something different than 0.0
		float fade = 0.0f;
		if (isFaded()){
			fade = 0.03f / sqr(m->feature_size_);
		}

		// we need model curvature info
		m->needCurvDerivatives(camera_position, sc_thresh_);
		find_sc_segments(m, camera_position, fade);
		// draw
		flushDrawBuffer();
	}
}

/**
 * Construct the suggestive contour segments between three given vertices defining a face
 *
 * @param *m : the model
 * @param vec0, vec1, vec2: the vertex indices of the cornerpoints of a mesh face
 * @param fade_factor : the alpha blending scheme for the fading
 */
void SuggestiveContourDrawer::construct_sc_segments(Model *m, int vec0, int vec1, int vec2, float fade_factor)
{
	// aliases
	const vector<point> &vertices = m->mesh_->vertices;
	const vector<float> &kr = m->kr_;
	const vector<float> &num = m->num_;
	const vector<float> &den = m->den_;
	// weights between vec0 and vec1/vec2
	float w10 = kr[vec0] / (kr[vec0] - kr[vec1]); float w01 = 1.0f - w10;
	float w20 = kr[vec0] / (kr[vec0] - kr[vec2]); float w02 = 1.0f - w20;
	// this results in these zero points
	point p1 = w01 * vertices[vec0] + w10 * vertices[vec1];
	point p2 = w02 * vertices[vec0] + w20 * vertices[vec2];
	// let's test num_ en den_ : interpolate values
	float num1 = w01 * num[vec0] + w10 * num[vec1];
	float num2 = w02 * num[vec0] + w20 * num[vec2];
	float den1 = w01 * den[vec0] + w10 * den[vec1];
	float den2 = w02 * den[vec0] + w20 * den[vec2];
	// is the direction derivative positive in the first point?
	bool valid_p1 = ((num1 >= 0.0f) == (den1 >= 0.0f)); // so one point is valid
	// find possible zero points (maximum 2 within a triangle)
	float zero_num = 0.0f;
	float zero_den = 0.0f;
	// if both num_'s don't have the same sign
	if ((num1 >= 0.0f) != (num2 >= 0.0f)){
		zero_num = num1 / (num1 - num2);
	}
	// if both den_'s don't have the same sign
	if ((den1 >= 0.0f) != (den2 >= 0.0f)){
		zero_den = den1 / (den1 - den2);
	}
	// reorder them
	if (zero_num == 0.0f){ // no num_ zero point
		zero_num = zero_den;
		zero_den = 0.0f;
	}
	else if (zero_den > zero_num){
		swap(zero_num, zero_den);
	}
	// count the number of vertex segment points we've drawn
	int nb_points_drawn = 0;
	// no or just 1 valid point: no suggestive contour segment can be drawn
	if (!valid_p1 && !zero_num && !zero_den){
		return;
	}
	if (valid_p1){ // first point is valid: it's on a segment
		drawbuffer_colors_.push_back(Vec<4, float>(linecolor_[0], linecolor_[1], linecolor_[2], num1 / (den1 * fade_factor + num1)));
		drawbuffer_vertices_.push_back(p1);
		nb_points_drawn++;
	}
	if (zero_num){ // if the dwKr dips below zero, first segment ends here. or vice versa, it starts here
		float num = (1.0f - zero_num) * num1 + zero_num * num2;
		float den = (1.0f - zero_num) * den1 + zero_num * den2;
		drawbuffer_colors_.push_back(Vec<4, float>(linecolor_[0], linecolor_[1], linecolor_[2], num / (den * fade_factor + num)));
		drawbuffer_vertices_.push_back((1.0f - zero_num)*p1 + zero_num*p2);
		nb_points_drawn++;
	}
	if (zero_den){ // it starts again here, or vice versa, it ends here
		float num = (1.0f - zero_den) * num1 + zero_den * num2;
		float den = (1.0f - zero_den) * den1 + zero_den * den2;
		drawbuffer_colors_.push_back(Vec<4, float>(linecolor_[0], linecolor_[1], linecolor_[2], num / (den * fade_factor + num)));
		drawbuffer_vertices_.push_back((1.0f - zero_den)*p1 + zero_den*p2);
		nb_points_drawn++;
	}
	if (nb_points_drawn != 2){ // when we need another point (no dwKr dips!). Complete 1st or 2nd segment.
		drawbuffer_colors_.push_back(Vec<4, float>(linecolor_[0], linecolor_[1], linecolor_[2], num2 / (den2 * fade_factor + num2)));
		drawbuffer_vertices_.push_back(p2);
	}
}

/**
 * Compute the suggestive contour lines for a given model and camera position
 *
 * @param Model* : the model
 * @param camera_position: the current camera position, given in 3d-coordinates
 * @param fade_factor: the alpha blending scheme for the fading
 */
void SuggestiveContourDrawer::find_sc_segments(Model* m, vec camera_position, float fade_factor)
{
	// some aliases to write readable code
	const vector<TriMesh::Face> &faces = m->mesh_->faces;
	const vector<float> &kr = m->kr_;

	// for every face in the filtered set
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		// find vertex points
		const int &v0 = faces[i][0];
		const int &v1 = faces[i][1];
		const int &v2 = faces[i][2];
		// does this face have a zero crossing for its radial curvature KR?
		if ((kr[v0] >= 0.0f || kr[v1] >= 0.0f || kr[v2] >= 0.0f) && (kr[v0] <= 0.0f || kr[v1] <= 0.0f || kr[v2] <= 0.0f)){
			// is this face turned to the camera?
			if (to_camera(m, i, camera_position)){
				// which polygon corner has the different sign of kr_ ?
				if ((kr[v0] > 0.0f && kr[v1] <= 0.0f && kr[v2] <= 0.0f) ||
					(kr[v0] < 0.0f && kr[v1] >= 0.0f && kr[v2] >= 0.0f)){
					construct_sc_segments(m, v0, v1, v2, fade_factor);
				}
				else if ((kr[v1] > 0.0f && kr[v2] <= 0.0f && kr[v0] <= 0.0f) ||
					(kr[v1] < 0.0f && kr[v2] >= 0.0f && kr[v0] >= 0.0f)){
					construct_sc_segments(m, v1, v0, v2, fade_factor);
				}
				else if ((kr[v2] > 0.0f && kr[v1] <= 0.0f && kr[v0] <= 0.0f) ||
					(kr[v2] < 0.0f && kr[v1] >= 0.0f && kr[v0] >= 0.0f)){
					construct_sc_segments(m, v2, v0, v1, fade_factor);
				}

			}
		}
	}
}
