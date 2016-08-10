/*
 * MeshWindow.h
 */ 

#ifndef _MESH_WINDOW_H_
#define _MESH_WINDOW_H_

#include "Mesh.h"
#include "GLUT_Window.h"
#include <GLCamera.h>

class MeshWindow : public GLUT_Window
{
public:
	MeshWindow(int width, int height, 
		std::shared_ptr<meshutil::Mesh> mesh_ptr = std::make_shared<meshutil::Mesh>(),
		std::shared_ptr<trimesh::GLCamera> camera_ptr = std::make_shared<trimesh::GLCamera>(),
		std::shared_ptr<trimesh::xform> xf_ptr = std::make_shared<trimesh::xform>());
	
	int _draw_mesh					= false;
	int _draw_edge_contour			= true;
	int _draw_suggestive_contour	= true;
	int _draw_boundary				= true;
	int _draw_face_contour			= false;

	std::shared_ptr<meshutil::Mesh>		_mesh_ptr;
	std::shared_ptr<trimesh::GLCamera>	_camera_ptr;
	std::shared_ptr<trimesh::xform>		_xf_ptr;

protected:
	virtual void cls();
	virtual void draw() override;
	virtual void reshape(int w, int h) override;
	virtual void mouse_button_click(int button, int x, int y) override;
	virtual void mouse_motion(int x, int y) override;
};

#endif // _MESH_WINDOW_H_