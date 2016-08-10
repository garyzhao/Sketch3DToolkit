#ifndef _MESH_H_
#define _MESH_H_

/*
 * Mesh.h
 */

#include <tuple>
#include <memory>
#include <TriMesh.h>
#include <XForm.h>
#include "Model.h"

#include "BaseDrawer.h"
#include "EdgeContourDrawer.h"
#include "SuggestiveContourDrawer.h"
#include "FaceContourDrawer.h"
#include "BoundaryDrawer.h"

typedef trimesh::Vec<2, double> vec2d;
typedef trimesh::Vec<3, double> vec3d;

namespace meshutil {

	class Mesh {

	protected:
		std::shared_ptr<trimesh::TriMesh> _mesh_ptr;
		std::unique_ptr<Model> _model_ptr;

		std::unique_ptr<BaseDrawer> _base_drawer_ptr;
		std::unique_ptr<EdgeContourDrawer> _edge_contour_drawer_ptr;
		std::unique_ptr<SuggestiveContourDrawer> _suggestive_contour_drawer_ptr;
		std::unique_ptr<FaceContourDrawer> _face_contour_drawer_ptr;
		std::unique_ptr<BoundaryDrawer> _boundary_drawer_ptr;

	public:
		Mesh(std::shared_ptr<trimesh::TriMesh> mesh_ptr = nullptr);
		static std::shared_ptr<Mesh> read(const char* filename);

		inline const trimesh::TriMesh::BSphere& bsphere() const { return _mesh_ptr->bsphere; };

		void init_mesh(trimesh::vec color = trimesh::vec(0, 0, 0), float line_width = 1.0);
		void setup_lighting(bool draw_mesh);
		void draw(trimesh::point camera_position, bool draw_base = true, bool draw_edge_contour = true,
			bool draw_suggestive_contour = true, bool draw_boundary = true, bool draw_face_contour = false);

		void look_at(trimesh::vec3 eye, trimesh::vec3 center, trimesh::vec3 up_hint, trimesh::xform& xf) const;

		std::vector<trimesh::point> sample_spiral_viewpoints(int n) const;
		std::vector<std::tuple<trimesh::point, vec2d>> sample_uniform_viewpoints(int num_latitude, int num_longitude) const;
	};

};

#endif // _MESH_H_
