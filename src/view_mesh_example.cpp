/*
 * view_mesh_example.cpp
 */

#include <iostream>
#include "retrieval.h"

using namespace std;
using namespace trimesh;
using namespace meshutil;

class MainWindow : public MeshWindow
{
public:
	using MeshWindow::MeshWindow;
	void init_viewpoints(int n) { 
		_viewpoint_info_vector = _mesh_ptr->sample_uniform_viewpoints(n, n);
	}
	
protected:
	vector<tuple<point, vec2d>> _viewpoint_info_vector;
	int idx = 0;

	vec2d position, position_alt;

	void change_viewpoint(int& i)
	{
		i = max(i, 0);
		i = min(i, (int)_viewpoint_info_vector.size() - 1);

		xform xf_alt = *_xf_ptr;
		_mesh_ptr->look_at(
			get<0>(_viewpoint_info_vector[i]),	// eye
			_mesh_ptr->bsphere().center,		// center
			vec3(0.0, 1.0, 0.0),				// up hint
			*_xf_ptr							// resulting look at matrix
			);

		position_alt = position;
		position = get<1>(_viewpoint_info_vector[i]);

		vec3d angle_vec = euler_angle(*_xf_ptr);
		double dist = central_angle_dist(position, position_alt);

		need_redraw();
		printf("Look at viewpoint #%d. [%f, %f, %f] | %f\n", i, angle_vec[0], angle_vec[1], angle_vec[2], dist);
	}

	void keyboard(unsigned char key, int x, int y) override
	{
		switch (key)
		{
		case '[': change_viewpoint(--idx); break;
		case ']': change_viewpoint(++idx); break;
		default:
			break;
		}
	}
};

MainWindow *main_window_ptr_;

// main()
int main(int argc, char* argv[])
{	
	/*
	if (argc < 2)
	{
		cout << "usage: view_mesh_example <mesh_file_path> [xform_file_path]" << endl;

		getchar();
		return -1;
	}
	
	const char *filename = argv[1];
	const char *xformname = (argc > 2) ? argv[2] : nullptr;
	*/

	const char *filename = "C:\\Users\\Gary\\Workspaces\\Sketch3DToolkit\\matlab\\dataset\\SHREC2013\\Models\\m1176.off";
	const char *xformname = nullptr;

	// Load mesh
	shared_ptr<Mesh> mesh_ptr = Mesh::read(filename);
	if (!mesh_ptr) 
	{
		printf("Failed to load mesh from %s.\n", filename);
		getchar();
		return EXIT_FAILURE;
	}

	// Initialize GLUT and create window
	auto_ptr<MainWindow> main_window(main_window_ptr_ = new MainWindow(MESH_WIN_SIZE, MESH_WIN_SIZE, mesh_ptr));
	main_window->create("GLUI Mesh Viewer", GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH, argc, argv);

	// Initialize shape & set default view
	mesh_ptr->init_mesh();
	main_window->init_viewpoints(MESH_VIEW_NUM);
	if (!xformname || !main_window->_xf_ptr->read(xformname))
	{
		*main_window->_xf_ptr = xform::trans(0, 0, -5.0f * mesh_ptr->bsphere().r) * xform::trans(-mesh_ptr->bsphere().center);
		printf("Use default XForm view setting.\n");
	}

	// GLUI code
	GLUI *glui = GLUI_Master.create_glui("GLUI", 0); // name, flags, x, and y
	new GLUI_StaticText(glui, "GLUI Mesh Viewer");
	new GLUI_Separator(glui);

	new GLUI_Checkbox(glui, "Draw Mesh", &main_window->_draw_mesh);

	GLUI_Panel *ui_contour_panel = new GLUI_Panel(glui, "Hybrid Contour");

	new GLUI_Checkbox(ui_contour_panel, "Draw Boundary", &main_window->_draw_boundary);
	new GLUI_Checkbox(ui_contour_panel, "Draw Edge Contour", &main_window->_draw_edge_contour);
	new GLUI_Checkbox(ui_contour_panel, "Draw Suggestive Contour", &main_window->_draw_suggestive_contour);
	new GLUI_Checkbox(ui_contour_panel, "Draw Face Contour", &main_window->_draw_face_contour);

	new GLUI_Button(glui, "Quit", 0, (GLUI_Update_CB)exit);

	glui->set_main_gfx_window(main_window->_window_id);

	// Register the idle callback with GLUI, *not* with GLUT
	GLUI_Master.set_glutIdleFunc([] {
		if (glutGetWindow() != main_window_ptr_->_window_id)
			glutSetWindow(main_window_ptr_->_window_id);
		glutPostRedisplay();
	});

	glutMainLoop();

	return EXIT_SUCCESS;
}
