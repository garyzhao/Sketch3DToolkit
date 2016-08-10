/*
 * Mesh.cpp
 */

#define _USE_MATH_DEFINES

#include <math.h>
#include <GL/glew.h>
#include <cassert>

#include "XForm.h"
#include "GLCamera.h"
#include "Mesh.h"

using namespace std;
using namespace trimesh;
using namespace meshutil;

constexpr static float fov = 0.7f;

Mesh::Mesh(shared_ptr<TriMesh> mesh_ptr)
{
	_mesh_ptr = mesh_ptr;
}

shared_ptr<Mesh> Mesh::read(const char *filename) 
{
	TriMesh *mesh_ptr = TriMesh::read(filename);	
	return make_shared<Mesh>(shared_ptr<TriMesh>(mesh_ptr));
}

void Mesh::init_mesh(vec color, float line_width) 
{
	assert(_mesh_ptr != nullptr);

	_model_ptr = make_unique<Model>(_mesh_ptr);
	_mesh_ptr->need_faces();

	BaseDrawer* base_drawer = new BaseDrawer();
	EdgeContourDrawer* edge_contour_drawer = new EdgeContourDrawer(color, line_width);
	SuggestiveContourDrawer* suggestive_contour_drawer = new SuggestiveContourDrawer(color, line_width, true, 0.001f);
	FaceContourDrawer* face_contour_drawer = new FaceContourDrawer(color, line_width);
	BoundaryDrawer* boundary_drawer_ptr = new BoundaryDrawer(color, line_width);

	_base_drawer_ptr = unique_ptr<BaseDrawer>(base_drawer);
	_edge_contour_drawer_ptr = unique_ptr<EdgeContourDrawer>(edge_contour_drawer);
	_suggestive_contour_drawer_ptr = unique_ptr<SuggestiveContourDrawer>(suggestive_contour_drawer);
	_face_contour_drawer_ptr = unique_ptr<FaceContourDrawer>(face_contour_drawer);
	_boundary_drawer_ptr = unique_ptr<BoundaryDrawer>(boundary_drawer_ptr);

	_model_ptr->pushDrawer(base_drawer);
	_model_ptr->pushDrawer(edge_contour_drawer);
	_model_ptr->pushDrawer(suggestive_contour_drawer);
	_model_ptr->pushDrawer(face_contour_drawer);
	_model_ptr->pushDrawer(boundary_drawer_ptr);
}

vector<point> Mesh::sample_spiral_viewpoints(int n) const
{
	assert(n > 1);

	vector<point> points;
	points.reserve(n);

	double phi1, phi, theta, h, x, y, z;
	double r1, r2, r3;	// ellipsoid axis lengths

	r1 = r2 = r3 = 3.5f / fov * _mesh_ptr->bsphere.r;
	phi1 = 0.0;
	points.push_back(point(0.0f, 0.0f, -1.0f * r3));

	for (int k = 2; k <= n - 1; k++) 
	{
		// Generate a random point on a sphere of radius 1.
		h = -1 + 2 * (k - 1) / (double)(n - 1);
		theta = acos(h);

		assert(theta >= 0);
		assert(theta <= M_PI);

		phi = phi1 + 3.6 / (sqrt((double)n * (1 - h * h)));
		phi = fmod(phi, 2 * M_PI);
		phi1 = phi;

		x = cos(phi) * sin(theta);
		y = sin(phi) * sin(theta);
		z = h;

		points.push_back(point(r1 * x, r2 * y, r3 * z));
	}
	points.push_back(point(0.0f, 0.0f, 1.0f * r3));

	for (int i = 0; i < points.size(); i++)
		points[i] += bsphere().center;
	
	return points;
}

vector<tuple<point, vec2d>> Mesh::sample_uniform_viewpoints(int num_latitude, int num_longitude) const
{
	assert(num_latitude > 1 && num_longitude > 1);

	vector<tuple<point, vec2d>> point_info_vector;
	point_info_vector.reserve((num_latitude - 1) * num_longitude);

	double latitude_increment = M_PI / num_latitude;
	double longitude_increment = (2.0 * M_PI) / num_longitude;

	for (int latitude_index = 1; latitude_index < num_latitude; latitude_index++)
	{
		double latitude = latitude_index * latitude_increment;

		for (int longitude_index = 0; longitude_index < num_longitude; longitude_index++)
		{
			double longitude = longitude_index * longitude_increment;

			double cos_latitude = cos(latitude);
			double sin_latitude = sin(latitude);
			double cos_longitude = cos(longitude);
			double sin_longitude = sin(longitude);

			double render_angle_scaling = 3.5f / fov * _mesh_ptr->bsphere.r;

			double scaled_x = render_angle_scaling * sin_latitude * cos_longitude;
			double scaled_y = render_angle_scaling * cos_latitude;
			double scaled_z = render_angle_scaling * sin_latitude * sin_longitude;

			assert(abs(sqrt((scaled_x * scaled_x) + (scaled_y * scaled_y) + (scaled_z * scaled_z)) - render_angle_scaling) < 0.001);

			double position[] = { latitude, longitude };
			point_info_vector.emplace_back(_mesh_ptr->bsphere.center + point(scaled_x, scaled_y, scaled_z), position);
		}
	}

	return point_info_vector;
}

void Mesh::setup_lighting(bool draw_mesh) {

	if (draw_mesh) 
	{
		GLfloat light0_diffuse[] = { 0.85f, 0.85f, 0.85f, 0.85f };
		glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
		glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_FALSE);
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		glEnable(GL_COLOR_MATERIAL);
		glEnable(GL_NORMALIZE);
	}
	else
	{
		glDisable(GL_LIGHTING);
		glDisable(GL_COLOR_MATERIAL);
		glDisable(GL_NORMALIZE);
	}
}

void Mesh::draw(point camera_position, bool draw_base, bool draw_edge_contour,
	bool draw_suggestive_contour, bool draw_boundary, bool draw_face_contour) {

	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);

	_base_drawer_ptr->setVisibility(draw_base);
	_edge_contour_drawer_ptr->setVisibility(draw_edge_contour);
	_suggestive_contour_drawer_ptr->setVisibility(draw_suggestive_contour);
	_boundary_drawer_ptr->setVisibility(draw_boundary);
	_face_contour_drawer_ptr->setVisibility(draw_face_contour);

	_model_ptr->draw(camera_position);
}

void Mesh::look_at(trimesh::vec3 eye, trimesh::vec3 center, trimesh::vec3 up_hint, trimesh::xform& xf) const
{
	vec3 forward, side, up;
	xform m;

	up = up_hint;

	forward = center - eye;
	normalize(forward);

	side = forward CROSS up;
	normalize(side);

	up = side CROSS forward;

	m(0, 0) = side[0];
	m(0, 1) = side[1];
	m(0, 2) = side[2];

	m(1, 0) = up[0];
	m(1, 1) = up[1];
	m(1, 2) = up[2];

	m(2, 0) = -forward[0];
	m(2, 1) = -forward[1];
	m(2, 2) = -forward[2];

	xf = m * xform::trans(-eye[0], -eye[1], -eye[2]);
}