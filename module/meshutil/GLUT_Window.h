#ifndef _GLUT_WINDOW_H_
#define _GLUT_WINDOW_H_

#include <map>
#include <GL/glew.h>
#include <GL/freeglut.h>

class GLUT_Window {

protected:
	static std::map<int, GLUT_Window*> _glut_windows;
	static void add_window(int window_id, GLUT_Window *window_ptr);
	static void remove_window(int window_id);

	void _draw();
	void _reshape(int w, int h);
	void _mouse_button(int button, int state, int x, int y);
	void _mouse_motion(int x, int y);
	void _keyboard(unsigned char key, int x, int y);

	void register_callbacks(GLUT_Window* window_ptr);

	unsigned int _width, _height;
	bool _auto_swap_buffers = true;
	unsigned _button_state = 0;

	virtual void init();
	virtual void draw() = 0;
	virtual void reshape(int w, int h);
	virtual void mouse_motion(int x, int y) {};
	virtual void mouse_button_double_click(int button, int x, int y) {};
	virtual void mouse_button_click(int button, int x, int y) {};
	virtual void keyboard(unsigned char key, int x, int y) {};

	bool is_mouse_button_down() { return _button_state; }

public:
	int _window_id, _main_window_id;

	GLUT_Window(int w, int h) : _width(w), _height(h), _window_id(0), _main_window_id(0) {}
	virtual ~GLUT_Window() { remove_window(_window_id); }

	bool create(const char *title, unsigned int display_mode, int argc, char **argv);
	bool create_subwindow(GLUT_Window &subwindow, int x, int y);
	void need_redraw() { glutPostRedisplay(); };
	virtual void destroy();
};


#endif // _GLUT_WINDOW_H_