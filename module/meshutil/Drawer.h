/*
 * A virtual class defining a Drawer class, which is an abstract class defining a component
 * that can draw something on the screen, given a certain Model.
 *
 * In its current form, it only forces Drawers to supply a draw method which takes a model and a camera position.
 */

#ifndef DRAWER_H_
#define DRAWER_H_

#include "Model.h"

namespace meshutil {

	class Drawer{
	protected:
		bool visible_;
		Drawer(bool isvisible);
	public:
		virtual void draw(Model* m, trimesh::vec camera_position) = 0;
		void toggleVisibility();
		void setVisibility(bool visible) { visible_ = visible; }
		bool isVisible();
	};
};

#endif /* DRAWER_H_ */
