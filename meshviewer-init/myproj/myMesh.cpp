#include "myMesh.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <map>
#include <utility>
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
	/**** TODO ****/
}

void myMesh::splitEdge(myHalfedge *e1, myPoint3D *p)
{

	/**** TODO ****/
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

