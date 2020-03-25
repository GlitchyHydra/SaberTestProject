#ifndef CALC_MESH_NORMALS_H
#define CALC_MESH_NORMALS_H

class vec3 {
public: float x = .0, y = .0, z = .0;
	  
	  void normalize();

	  //equality of two vec3 obj
	  bool operator ==(const vec3& obj) const
	  {
		  if ((x == obj.x) && (y == obj.y) && (z == obj.z))
			  return true;
		  else
			  return false;
	  }
};

void calc_mesh_normals(vec3* normals, const vec3* verts, const int* faces, size_t nverts, size_t nfaces);
#endif