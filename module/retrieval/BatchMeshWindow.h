/*
 * BatchMeshWindow.h
 */

#ifndef _BATCH_MESH_WINDOW_H_
#define _BATCH_MESH_WINDOW_H_

#include "MeshWindow.h"
#include <opencv2/imgproc/imgproc.hpp>

struct ViewInfo
{
	cv::Mat view_image;
	cv::Mat depth_image;
	trimesh::xform xf;
	double eye_angle;
	vec2d position;
};

class BatchMeshWindow : public MeshWindow
{
	using MeshWindow::MeshWindow;

protected:
	GLuint _fbo;
	GLuint _rbo_color;
	GLuint _rbo_depth;

	virtual void init() override;

public:
	virtual void destroy() override;
	void draw_contour();
	std::vector<ViewInfo> sample_mesh_viewpoints(int num);
	ViewInfo collect_viewpoint(const std::tuple<trimesh::point, vec2d>& viewpoint_info, double angle);
};

#endif // _BATCH_MESH_WINDOW_H_