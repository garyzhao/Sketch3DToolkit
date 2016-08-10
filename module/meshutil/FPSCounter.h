/*
 * FPSCounter.h
 *
 *	Definition of an easy and slightly inaccurate FPS counter
 *
 */

#include "timestamp.h"


#ifndef FPSCOUNTER_H_
#define FPSCOUNTER_H_

class FPSCounter{
private:
	trimesh::timestamp lasttime;
	int frames;
public:
	FPSCounter();
	void updateCounter();
	int FPS;
};

#endif /* FPSCOUNTER_H_ */
