#include <timestamp.h>
#include "GLUT_Window.h"

using namespace std;
using namespace trimesh;

std::map<int, GLUT_Window*> GLUT_Window::_glut_windows;

void GLUT_Window::add_window(int window_id, GLUT_Window *window_ptr)
{	
	_glut_windows.insert(make_pair(window_id, window_ptr));
}

void GLUT_Window::remove_window(int window_id) 
{	
	auto it = _glut_windows.find(window_id);
	if (it != _glut_windows.end()) { _glut_windows.erase(it); }
}

void GLUT_Window::init()
{
	// do nothing here
}

void GLUT_Window::destroy()
{
	// do nothing here
}

void GLUT_Window::register_callbacks(GLUT_Window* window_ptr) 
{
	add_window(window_ptr->_window_id, window_ptr);

	glutDisplayFunc([] { _glut_windows[::glutGetWindow()]->_draw(); });
	glutReshapeFunc([](int w, int h) { _glut_windows[::glutGetWindow()]->_reshape(w, h); });
	glutKeyboardFunc([](unsigned char key, int x, int y) { _glut_windows[::glutGetWindow()]->_keyboard(key, x, y); });
	glutMotionFunc([](int x, int y) { _glut_windows[::glutGetWindow()]->_mouse_motion(x, y); });
	glutMouseFunc([](int button, int state, int x, int y) { _glut_windows[::glutGetWindow()]->_mouse_button(button, state, x, y); });

	window_ptr->init();
}

bool GLUT_Window::create(const char *title, unsigned int display_mode, int argc, char **argv)
{	
	glutInit(&argc, argv);
	glutInitDisplayMode(display_mode);
	glutInitWindowPosition(50, 50);
	glutInitWindowSize(_width, _height);

	_window_id = glutCreateWindow(title);
	if (_window_id <= 0) { return false; }
	
	glewInit();
	
	register_callbacks(this);
	return true;
}

bool GLUT_Window::create_subwindow(GLUT_Window &subwindow, int x, int y)
{
	subwindow._window_id = glutCreateSubWindow(_window_id, x, y, subwindow._width, subwindow._height);
	if (subwindow._window_id <= 0) { return false; }

	subwindow._main_window_id = _window_id;
	register_callbacks(&subwindow);
	return true;
}

void GLUT_Window::reshape(int w, int h)
{
	_width = w, _height = h;
	glViewport(0, 0, w, h);
}

void GLUT_Window::_reshape(int w, int h)
{
	reshape(w, h);
	need_redraw();
}

void GLUT_Window::_draw()
{
	draw();
	if (_auto_swap_buffers) { glutSwapBuffers(); }
}

void GLUT_Window::_mouse_motion(int x, int y)
{
	mouse_motion(x, y);
}

void GLUT_Window::_keyboard(unsigned char key, int x, int y)
{
	keyboard(key, x, y);
}

void GLUT_Window::_mouse_button(int button, int state, int x, int y)
{
	static timestamp last_click_time;
	static unsigned last_click_button_state = 0;
	static float double_click_threshold = 0.4f;

	if (glutGetModifiers() & GLUT_ACTIVE_CTRL)
		_button_state |= (1 << 30);
	else
		_button_state &= ~(1 << 30);

	if (state == GLUT_DOWN) 
	{
		_button_state |= (1 << button);
		if (_button_state == last_click_button_state && now() - last_click_time < double_click_threshold) 
		{
			last_click_button_state = 0;
			mouse_button_double_click(button, x, y);
		}
		else 
		{
			last_click_time = now();
			last_click_button_state = _button_state;
		}
	}
	else 
	{
		_button_state &= ~(1 << button);
	}

	mouse_button_click(button, x, y);
}