#include "myVertex.h"
#include "myvector3d.h"
#include "myHalfedge.h"
#include "myFace.h"

myVertex::myVertex(void)
{
	point = NULL;
	originof = NULL;
	normal = new myVector3D(1.0,1.0,1.0);
}

myVertex::~myVertex(void)
{
	if (normal) delete normal;
}

void myVertex::computeNormal() {
	myHalfedge* h = originof;
	normal->dX = 0.0;
	normal->dY = 0.0;
	normal->dZ = 0.0;
	do {
		normal->dX += h->adjacent_face->normal->dX;
		normal->dY += h->adjacent_face->normal->dY;
		normal->dZ += h->adjacent_face->normal->dZ;
		h = h->twin->next;
	} while (h != originof);
	normal->normalize();
}