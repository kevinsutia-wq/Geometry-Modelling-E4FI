#include "myMesh.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <map>
#include <utility>
#include <cmath>
#include <GL/glew.h>
#include "myVector3D.h"

using namespace std;

myMesh::myMesh(void)
{
}


myMesh::~myMesh(void)
{
	clear();
}

void myMesh::clear()
{
	for (unsigned int i = 0; i < vertices.size(); i++) if (vertices[i]) delete vertices[i];
	for (unsigned int i = 0; i < halfedges.size(); i++) if (halfedges[i]) delete halfedges[i];
	for (unsigned int i = 0; i < faces.size(); i++) if (faces[i]) delete faces[i];

	vector<myVertex *> empty_vertices;    vertices.swap(empty_vertices);
	vector<myHalfedge *> empty_halfedges; halfedges.swap(empty_halfedges);
	vector<myFace *> empty_faces;         faces.swap(empty_faces);
}

void myMesh::checkMesh()
{
	vector<myHalfedge *>::iterator it;
	for (it = halfedges.begin(); it != halfedges.end(); it++)
	{
		if ((*it)->twin == NULL)
			break;
	}
	if (it != halfedges.end())
		cout << "Error! Not all edges have their twins!\n";
	else cout << "Each edge has a twin!\n";
}


bool myMesh::readFile(std::string filename)
{
	string s, t, u;
	vector<int> faceids;
	myHalfedge **hedges;

	ifstream fin(filename);
	if (!fin.is_open()) {
		cout << "Unable to open file!\n";
		return false;
	}
	name = filename;

	map<pair<int, int>, myHalfedge *> twin_map;
	map<pair<int, int>, myHalfedge *>::iterator it;

	while (getline(fin, s))
	{
		stringstream myline(s);
		myline >> t;
		if (t == "g") {}
		else if (t == "v")
		{
			float x, y, z;
			myline >> x >> y >> z;

			myVertex *v = new myVertex();
			v->point = new myPoint3D(x, y, z);
			v->index = vertices.size();
			v->originof = NULL;
			vertices.push_back(v);

			cout << "v " << x << " " << y << " " << z << endl;
		}
		else if (t == "mtllib") {}
		else if (t == "usemtl") {}
		else if (t == "s") {}
		else if (t == "f")
		{
			faceids.clear();
			cout << "f";
			while (myline >> u)
			{
				int vertex_index = atoi((u.substr(0, u.find("/"))).c_str()) - 1;
				faceids.push_back(vertex_index);
				cout << " " << vertex_index;
			}
			cout << endl;

			// Ignorer les faces d�g�n�r�es (moins de 3 sommets)
			if (faceids.size() < 3)
				continue;

			// Pr�-allouer les half-edges
			hedges = new myHalfedge *[faceids.size()];
			for (unsigned int i = 0; i < faceids.size(); i++)
				hedges[i] = new myHalfedge();

			// Cr�er la face
			myFace *f = new myFace();
			f->adjacent_halfedge = hedges[0];

			// Traiter chaque half-edge
			for (unsigned int i = 0; i < faceids.size(); i++)
			{
				int iplusone = (i + 1) % faceids.size();
				int iminusone = (i - 1 + faceids.size()) % faceids.size();

				// connect prevs, and next
				hedges[i]->next = hedges[iplusone];
				hedges[i]->prev = hedges[iminusone];

				// search for the twins using twin_map
				int curr_vertex = faceids[i];
				int next_vertex = faceids[iplusone];

				pair<int, int> edge_pair = make_pair(curr_vertex, next_vertex);
				pair<int, int> reverse_pair = make_pair(next_vertex, curr_vertex);

				if (twin_map.find(reverse_pair) != twin_map.end())
				{
					hedges[i]->twin = twin_map[reverse_pair];
					twin_map[reverse_pair]->twin = hedges[i];
					twin_map.erase(reverse_pair);
				}
				else
				{
					twin_map[edge_pair] = hedges[i];
				}

				// set originof
				if (vertices[curr_vertex]->originof == NULL)
					vertices[curr_vertex]->originof = hedges[i];

				hedges[i]->source = vertices[faceids[i]];
				hedges[i]->adjacent_face = f;

				hedges[i]->index = halfedges.size();
				halfedges.push_back(hedges[i]);

			}

			f->index = faces.size();
			faces.push_back(f);

			delete[] hedges;
			// push faces to faces in myMesh
		}
	}

	checkMesh();
	normalize();

	return true;
}


void myMesh::computeNormals()
{
	// Calcule la normale de chaque face
	for (unsigned int i = 0; i < faces.size(); i++) {
		faces[i]->computeNormal();
	}

	// Pareil pour sommet
	for (unsigned int i = 0; i < vertices.size(); i++) {
		vertices[i]->computeNormal();
	}
}

void myMesh::normalize()
{
	if (vertices.size() < 1) return;

	int tmpxmin = 0, tmpymin = 0, tmpzmin = 0, tmpxmax = 0, tmpymax = 0, tmpzmax = 0;

	for (unsigned int i = 0; i < vertices.size(); i++) {
		if (vertices[i]->point->X < vertices[tmpxmin]->point->X) tmpxmin = i;
		if (vertices[i]->point->X > vertices[tmpxmax]->point->X) tmpxmax = i;

		if (vertices[i]->point->Y < vertices[tmpymin]->point->Y) tmpymin = i;
		if (vertices[i]->point->Y > vertices[tmpymax]->point->Y) tmpymax = i;

		if (vertices[i]->point->Z < vertices[tmpzmin]->point->Z) tmpzmin = i;
		if (vertices[i]->point->Z > vertices[tmpzmax]->point->Z) tmpzmax = i;
	}

	double xmin = vertices[tmpxmin]->point->X, xmax = vertices[tmpxmax]->point->X,
		ymin = vertices[tmpymin]->point->Y, ymax = vertices[tmpymax]->point->Y,
		zmin = vertices[tmpzmin]->point->Z, zmax = vertices[tmpzmax]->point->Z;

	double scale = (xmax - xmin) > (ymax - ymin) ? (xmax - xmin) : (ymax - ymin);
	scale = scale > (zmax - zmin) ? scale : (zmax - zmin);

	for (unsigned int i = 0; i < vertices.size(); i++) {
		vertices[i]->point->X -= (xmax + xmin) / 2;
		vertices[i]->point->Y -= (ymax + ymin) / 2;
		vertices[i]->point->Z -= (zmax + zmin) / 2;

		vertices[i]->point->X /= scale;
		vertices[i]->point->Y /= scale;
		vertices[i]->point->Z /= scale;
	}
}


void myMesh::splitFaceTRIS(myFace *f, myPoint3D *p)
{
	// creer un nouveau vertex au centre de la face
	myVertex *v = new myVertex();
	v->point = p;
	v->index = vertices.size();
	v->originof = NULL;
	vertices.push_back(v);

	// stocker les edges originales
	vector<myHalfedge*> originalEdges;
	myHalfedge *courant = f->adjacent_halfedge;
	do {
		originalEdges.push_back(courant);
		courant = courant->next;
	} while (courant != f->adjacent_halfedge);

	int nombreEdges = originalEdges.size();

	// creer les triangles
	for (int i = 0; i < nombreEdges; i++)
	{
		// creer trois halfedges pour le triangle
		myHalfedge *he1 = new myHalfedge();
		myHalfedge *he2 = new myHalfedge();
		myHalfedge *he3 = new myHalfedge();

		// creer la nouvelle face
		myFace *newFace = new myFace();
		newFace->adjacent_halfedge = he1;
		newFace->index = faces.size();
		faces.push_back(newFace);

		// he1: edge externe (de la face originale)
		he1->source = originalEdges[i]->source;
		he1->adjacent_face = newFace;
		he1->next = he2;
		he1->prev = he3;
		he1->twin = originalEdges[i]->twin;
		he1->index = halfedges.size();
		halfedges.push_back(he1);

		// he2: vers le nouveau vertex
		he2->source = originalEdges[i]->next->source;
		he2->adjacent_face = newFace;
		he2->next = he3;
		he2->prev = he1;
		he2->twin = NULL;
		he2->index = halfedges.size();
		halfedges.push_back(he2);

		// he3: depuis le nouveau vertex
		he3->source = v;
		he3->adjacent_face = newFace;
		he3->next = he1;
		he3->prev = he2;
		he3->twin = NULL;
		he3->index = halfedges.size();
		halfedges.push_back(he3);
	}

	// mettre a jour originof du nouveau vertex
	v->originof = faces[faces.size() - 1]->adjacent_halfedge->next->next;

	// supprimer l'ancienne face
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		if (faces[i] == f)
		{
			faces.erase(faces.begin() + i);
			break;
		}
	}
}

void myMesh::splitEdge(myHalfedge *e1, myPoint3D *p)
{
	// creer un nouveau vertex au point p
	myVertex *v = new myVertex();
	v->point = p;
	v->index = vertices.size();
	v->originof = NULL;
	vertices.push_back(v);

	// creer deux nouveaux halfedges pour les deux parties de l'arete
	myHalfedge *e2 = new myHalfedge();
	myHalfedge *e3 = new myHalfedge();
	myHalfedge *e4 = new myHalfedge();

	// configurer e2 (deuxieme partie de e1)
	e2->source = e1->next->source;
	e2->adjacent_face = e1->adjacent_face;
	e2->next = e1->next;
	e2->prev = e1;
	e2->twin = e4;
	e2->index = halfedges.size();
	halfedges.push_back(e2);

	// configurer e3 (deuxieme partie de e1->twin)
	e3->source = e1->twin->source;
	e3->adjacent_face = e1->twin->adjacent_face;
	e3->next = e1->twin->next;
	e3->prev = e1->twin;
	e3->twin = NULL;
	e3->index = halfedges.size();
	halfedges.push_back(e3);

	// configurer e4 (twin de e2)
	e4->source = v;
	e4->adjacent_face = e1->twin->adjacent_face;
	e4->next = e3;
	e4->prev = e1->twin;
	e4->twin = e2;
	e4->index = halfedges.size();
	halfedges.push_back(e4);

	// mettre a jour e1
	e1->next->prev = e2;
	e1->next = e2;

	// mettre a jour e1->twin
	e1->twin->next = e4;
	e1->twin->next->prev = e1->twin;

	// configurer originof du nouveau vertex
	v->originof = e4;
}

void myMesh::splitFaceQUADS(myFace *f, myPoint3D *p)
{
	/**** TODO ****/
}


void myMesh::subdivisionCatmullClark()
{
	/**** TODO ****/
}


void myMesh::triangulate()
{
	// copier la liste des faces
	vector<myFace*> listeFaces = faces;


	for (unsigned int i = 0; i < listeFaces.size(); i++){
		this->triangulate(listeFaces[i]);
	}

	this->computeNormals();
}

//return false if already triangle, true othewise.
bool myMesh::triangulate(myFace* f)
{
	int nombrePoints = 0;

	myHalfedge* courant = f->adjacent_halfedge;

	// tourner autour de face
	do{
		nombrePoints = nombrePoints + 1;
		courant = courant->next;
	} while (courant != f->adjacent_halfedge);

	if (nombrePoints <= 3){
		return false;
	}


	// faire la moyenne des points 
	float sommeX = 0;
	float sommeY = 0;
	float sommeZ = 0;
	courant = f->adjacent_halfedge;

	do{
		sommeX = sommeX + courant->source->point->X;
		sommeY = sommeY + courant->source->point->Y;
		sommeZ = sommeZ + courant->source->point->Z;
		courant = courant->next;
	} 
	
	
	while (courant != f->adjacent_halfedge);


	// point au centre
	float centreX = sommeX / nombrePoints;
	float centreY = sommeY / nombrePoints;
	float centreZ = sommeZ / nombrePoints;

	myPoint3D* centre = new myPoint3D(centreX, centreY, centreZ);


	// Diviser la face en triangles
	this->splitFaceTRIS(f, centre);

	return true;
}

void myMesh::surfaceOfRevolution(int steps, float angleMax)
{
	// alloc normals pour les vertices existants
	for (unsigned int i = 0; i < vertices.size(); i++)
	{
		if (vertices[i]->normal == NULL)
			vertices[i]->normal = new myVector3D(0, 0, 0);
	}

	// stocker les vertices du profil original
	int profil_size = vertices.size();
	vector<myVertex*> originalVertices;
	for (int i = 0; i < profil_size; i++)
		originalVertices.push_back(vertices[i]);

	// dupliquer les vertices pour chaque etape de rotation
	for (int step = 1; step < steps; step++)
	{
		float angle = (angleMax / steps) * step;
		float cosA = cos(angle);
		float sinA = sin(angle);

		for (int i = 0; i < profil_size; i++)
		{
			float x = originalVertices[i]->point->X;
			float y = originalVertices[i]->point->Y;
			float z = originalVertices[i]->point->Z;

			float newX = x * cosA - z * sinA;
			float newZ = x * sinA + z * cosA;

			myVertex *v = new myVertex();
			v->point = new myPoint3D(newX, y, newZ);
			v->index = vertices.size();
			v->originof = NULL;
			v->normal = new myVector3D(0, 0, 0);
			vertices.push_back(v);
		}
	}

	// creer les faces avec halfedges
	for (int step = 0; step < steps - 1; step++)
	{
		for (int i = 0; i < profil_size - 1; i++)
		{
			int idx0 = step * profil_size + i;
			int idx1 = step * profil_size + i + 1;
			int idx2 = (step + 1) * profil_size + i;
			int idx3 = (step + 1) * profil_size + i + 1;

			// triangle 1: idx0, idx1, idx3
			myHalfedge *he1a = new myHalfedge();
			myHalfedge *he1b = new myHalfedge();
			myHalfedge *he1c = new myHalfedge();

			myFace *f1 = new myFace();
			f1->normal = new myVector3D(0, 0, 0);
			
			// calculer la normale du triangle
			myPoint3D* pA = vertices[idx0]->point;
			myPoint3D* pB = vertices[idx1]->point;
			myPoint3D* pC = vertices[idx3]->point;
			myVector3D v1(pB->X - pA->X, pB->Y - pA->Y, pB->Z - pA->Z);
			myVector3D v2(pC->X - pA->X, pC->Y - pA->Y, pC->Z - pA->Z);
			myVector3D normal = v1.crossproduct(v2);
			normal.normalize();
			f1->normal->dX = normal.dX;
			f1->normal->dY = normal.dY;
			f1->normal->dZ = normal.dZ;
			
			f1->index = faces.size();
			f1->adjacent_halfedge = he1a;
			faces.push_back(f1);

			he1a->source = vertices[idx0];
			he1a->adjacent_face = f1;
			he1a->next = he1b;
			he1a->prev = he1c;
			he1a->twin = NULL;
			he1a->index = halfedges.size();
			halfedges.push_back(he1a);

			he1b->source = vertices[idx1];
			he1b->adjacent_face = f1;
			he1b->next = he1c;
			he1b->prev = he1a;
			he1b->twin = NULL;
			he1b->index = halfedges.size();
			halfedges.push_back(he1b);

			he1c->source = vertices[idx3];
			he1c->adjacent_face = f1;
			he1c->next = he1a;
			he1c->prev = he1b;
			he1c->twin = NULL;
			he1c->index = halfedges.size();
			halfedges.push_back(he1c);

			// triangle 2: idx0, idx3, idx2
			myHalfedge *he2a = new myHalfedge();
			myHalfedge *he2b = new myHalfedge();
			myHalfedge *he2c = new myHalfedge();

			myFace *f2 = new myFace();
			f2->normal = new myVector3D(0, 0, 0);
			
			// calculer la normale du deuxième triangle
			pA = vertices[idx0]->point;
			pB = vertices[idx3]->point;
			pC = vertices[idx2]->point;
			v1 = myVector3D(pB->X - pA->X, pB->Y - pA->Y, pB->Z - pA->Z);
			v2 = myVector3D(pC->X - pA->X, pC->Y - pA->Y, pC->Z - pA->Z);
			normal = v1.crossproduct(v2);
			normal.normalize();
			f2->normal->dX = normal.dX;
			f2->normal->dY = normal.dY;
			f2->normal->dZ = normal.dZ;
			
			f2->index = faces.size();
			f2->adjacent_halfedge = he2a;
			faces.push_back(f2);

			he2a->source = vertices[idx0];
			he2a->adjacent_face = f2;
			he2a->next = he2b;
			he2a->prev = he2c;
			he2a->twin = NULL;
			he2a->index = halfedges.size();
			halfedges.push_back(he2a);

			he2b->source = vertices[idx3];
			he2b->adjacent_face = f2;
			he2b->next = he2c;
			he2b->prev = he2a;
			he2b->twin = NULL;
			he2b->index = halfedges.size();
			halfedges.push_back(he2b);

			he2c->source = vertices[idx2];
			he2c->adjacent_face = f2;
			he2c->next = he2a;
			he2c->prev = he2b;
			he2c->twin = NULL;
			he2c->index = halfedges.size();
			halfedges.push_back(he2c);
		}
	}

	cout << "Surface of revolution created!" << endl;
	cout << "Vertices: " << vertices.size() << endl;
	cout << "Faces: " << faces.size() << endl;
}

