#ifndef CALC_MESH_NORMALS_H
#define CALC_MESH_NORMALS_H

class vec3 {
public:
	 float x = .0, y = .0, z = .0;
	  
	 void operator += (vec3 const &anotherVec);
	 void normalize();
};

void calc_mesh_normals(vec3* normals, const vec3* verts, const int* faces, size_t nverts, size_t nfaces);
#endif