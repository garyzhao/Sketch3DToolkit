/*
 * MeshWindow.cpp
 */

#include "MeshWindow.h"

using namespace std;
using namespace trimesh;
using namespace meshutil;

MeshWindow::MeshWindow(int width, int height, shared_ptr<Mesh> mesh_ptr, shared_ptr<GLCamera> camera_ptr, 
	shared_ptr<xform> xf_ptr) : GLUT_Window(width, height), _mesh_ptr(mesh_ptr), _camera_ptr(camera_ptr), _xf_ptr(xf_ptr) 
{
	// do nothing here
}

void MeshWindow::cls()
{
	glDisable(GL_DITHER);
	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_NORMALIZE);
	glDisable(GL_LIGHTING);
	glDisable(GL_NORMALIZE);
	glDisable(GL_COLOR_MATERIAL);
	glClearColor(1.0, 1.0, 1.0, 0);

	glClearDepth(1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void MeshWindow::draw()
{
	cls();
	glPushMatrix();
	glMultMatrixd(*_xf_ptr);

	_mesh_ptr->setup_lighting(_draw_mesh);
	point camera_position = inv(*_xf_ptr) * point(0, 0, 0);
	_mesh_ptr->draw(camera_position, true, _draw_edge_contour, _draw_suggestive_contour, _draw_boundary, _draw_face_contour);

	glPopMatrix();
}

void MeshWindow::reshape(int w, int h)
{
	GLUT_Window::reshape(w, h);

	// reset view
	_camera_ptr->setupGL((*_xf_ptr) * _mesh_ptr->bsphere().center, _mesh_ptr->bsphere().r);
}

void MeshWindow::mouse_button_click(int button, int x, int y)
{
	_camera_ptr->mouse(x, y, Mouse::NONE, (*_xf_ptr) * _mesh_ptr->bsphere().center, _mesh_ptr->bsphere().r, *_xf_ptr);
}

void MeshWindow::mouse_motion(int x, int y)
{
	if (is_mouse_button_down())
	{
		_camera_ptr->mouse(x, y, Mouse::ROTATE, (*_xf_ptr) * _mesh_ptr->bsphere().center, _mesh_ptr->bsphere().r, (*_xf_ptr));
		need_redraw();
	}
}