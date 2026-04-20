#include "myFace.h"
#include "myvector3d.h"
#include "myHalfedge.h"
#include "myVertex.h"
#include <GL/glew.h>
#include "myPoint3D.h"

myFace::myFace(void)
{
	adjacent_halfedge = NULL;
	normal = new myVector3D(1.0, 1.0, 1.0);
}

myFace::~myFace(void)
{
	if (normal) delete normal;
}

void myFace::computeNormal()
{
	// Obtenir les points A, B, C de la face
	myPoint3D* pA = this->adjacent_halfedge->source->point;
	myPoint3D* pB = this->adjacent_halfedge->next->source->point;
	myPoint3D* pC = this->adjacent_halfedge->next->next->source->point;

	// Construit les vecteurs v1 (AB) et v2 (AC)
	myVector3D v1(pB->X - pA->X, pB->Y - pA->Y, pB->Z - pA->Z);
	myVector3D v2(pC->X - pA->X, pC->Y - pA->Y, pC->Z - pA->Z);

	//Calcul du produit pour avoir un vecteur perpendiculaire à la face
	myVector3D res = v1.crossproduct(v2);
	res.normalize();

	//Copier les composantes dX, dY, dZ dans l'objet normal de la face
	this->normal->dX = res.dX;
	this->normal->dY = res.dY;
	this->normal->dZ = res.dZ;
}
