/*
 * Implementation of the protected methods in the Drawer class.
 */

#include "Drawer.h"

using namespace meshutil;

Drawer::Drawer(bool isvisible) : visible_(isvisible){

}

void Drawer::toggleVisibility(){
	visible_ = !visible_;
}

bool Drawer::isVisible(){
	return visible_;
}