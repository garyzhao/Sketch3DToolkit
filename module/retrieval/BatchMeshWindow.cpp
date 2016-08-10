/*
 * BatchMeshWindow.h
 */

#include <cassert>
#include "retrieval.h"
#include "BatchMeshWindow.h"

using namespace cv;
using namespace std;
using namespace trimesh;
using namespace meshutil;

void BatchMeshWindow::init()
{
	_auto_swap_buffers = false;

	//  framebuffer
	glGenFramebuffers(1, &_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, _fbo);

	// color renderbuffer
	glGenRenderbuffers(1, &_rbo_color);
	glBindRenderbuffer(GL_RENDERBUFFER, _rbo_color);
	
	// Storage must be one of: GL_RGBA4, GL_RGB565, GL_RGB5_A1, GL_DEPTH_COMPONENT16, GL_STENCIL_INDEX8.
	glRenderbufferStorage(GL_RENDERBUFFER, GL_RGB565, _width, _height);
	glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, _rbo_color);

	// depth renderbuffer
	glGenRenderbuffers(1, &_rbo_depth);
	glBindRenderbuffer(GL_RENDERBUFFER, _rbo_depth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, _width, _height);
	glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _rbo_depth);

	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	glReadBuffer(GL_COLOR_ATTACHMENT0);

	// sanity check
	int V;
	glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, &V);
	assert(glCheckFramebufferStatus(GL_FRAMEBUFFER));
	assert(_width < V && _height < V);
}

void BatchMeshWindow::destroy()
{
	glDeleteFramebuffers(1, &_fbo);
	glDeleteRenderbuffers(1, &_rbo_color);
	glDeleteRenderbuffers(1, &_rbo_depth);
}

void BatchMeshWindow::draw_contour() 
{
	MeshWindow::reshape(_width, _height);
	MeshWindow::draw();
	glFlush();
}

ViewInfo BatchMeshWindow::collect_viewpoint(const tuple<point, vec2d>& viewpoint_info, double angle)
{
	_mesh_ptr->look_at(get<0>(viewpoint_info), _mesh_ptr->bsphere().center, vec3(0.0, 1.0, 0.0), *_xf_ptr);
	*_xf_ptr = xform::rot(angle, 0, 0, 1) * (*_xf_ptr);
	this->draw_contour();

	Mat viewImage = glDumpRenderbuffer();
	Mat depthImage = glDumpDepthbuffer();
	depthImage = formatDepthImage(1 - depthImage);
	threshold(depthImage, depthImage, 0, 1, THRESH_BINARY);

	ViewInfo view_info;
	view_info.view_image = move(viewImage);
	view_info.depth_image = move(depthImage);
	view_info.xf = *_xf_ptr;
	view_info.eye_angle = angle;
	view_info.position = get<1>(viewpoint_info);

	return move(view_info);
}

vector<ViewInfo> BatchMeshWindow::sample_mesh_viewpoints(int num)
{
	vector<tuple<point, vec2d>> viewpoint_info_vector = _mesh_ptr->sample_uniform_viewpoints(num, num);
	vector<ViewInfo> view_info_vector;
	view_info_vector.reserve(viewpoint_info_vector.size() * 4);

	for (const auto& viewpoint_info : viewpoint_info_vector)
	{
		view_info_vector.push_back(move(collect_viewpoint(viewpoint_info, 0)));
		view_info_vector.push_back(move(collect_viewpoint(viewpoint_info, M_PI / 2)));
		view_info_vector.push_back(move(collect_viewpoint(viewpoint_info, M_PI)));
		view_info_vector.push_back(move(collect_viewpoint(viewpoint_info, -M_PI / 2)));
	}
	return move(view_info_vector);
}