/*
 * FPSCounter.cpp
 *
 * Easy and slightly inaccurate FPS counter
 *
 */

#include "FPSCounter.h"
#include "timestamp.h"
#include <iostream>
#include <string>

using std::string;
using std::cout;
using std::endl;

using namespace trimesh;

FPSCounter::FPSCounter(){
	lasttime = now();
	FPS = 0;
	frames = 0;
}

void FPSCounter::updateCounter(){
	if (now() - lasttime >= 1){
		FPS = frames;
		frames = 0;
	}
	lasttime = now();
	frames++;
}
