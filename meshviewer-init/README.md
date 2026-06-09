# Mesh Viewer

A 3D mesh viewer and editor built with C++ and OpenGL. This tool lets you load, visualize, and modify 3D geometric models with various mesh editing operations.

## What it does

This application shows 3D mesh files on your screen. You can interact with the mesh to:

- **View**: Display the mesh with different shading options
- **Subdivide**: Refine the mesh using Catmull-Clark or Loop algorithms
- **Edit**: Split or merge edges and faces
- **Transform**: Apply operations like surface of revolution and mesh simplification
- **Inspect**: Select and examine vertices, edges, and faces

## Build

You need these dependencies installed:
- OpenGL
- GLEW (OpenGL Extension Wrangler)
- FreeGLUT
- X11 (on Linux)
- GLM (math library, included)

To build:

```bash
mkdir build
cd build
cmake ..
make
```

This creates an executable called `meshviewer` in the build folder.

## How to use

Run the program:

```bash
./meshviewer
```

Use the right-click menu to:
- Open a mesh file
- Toggle drawing options (wireframe, vertices, normals)
- Select and edit mesh elements
- Apply mesh operations (subdivide, split, triangulate)

Use your mouse and keyboard to rotate, zoom, and pan the view.

## Project structure

- `myproj/` - Source code
  - `main.cpp` - Application entry point and graphics code
  - `myMesh.*` - Main mesh class
  - `myVertex.*, myHalfedge.*, myFace.*` - Mesh data structures
  - `myPoint3D.*, myVector3D.*` - Math utilities
  - `shaders/` - OpenGL shader files
  - `include/` - External libraries (GLM, GLEW, FreeGLUT)

## Technical details

The mesh uses a half-edge data structure, which is good for mesh editing because it tracks all connections between vertices, edges, and faces. This makes complex operations like subdivision and edge splitting easy to implement correctly.

## Supported formats

The application can read 3D mesh files. Use the file dialog to load your meshes.

---

Built for geometry modelling coursework (E4FI).
