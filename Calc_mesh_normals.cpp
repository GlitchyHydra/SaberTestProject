#include "Calc_mesh_normals.h"
#include <math.h>

using namespace std;

void vec3::operator += (vec3 const& anotherVec) {
	this->x += anotherVec.x;
	this->y += anotherVec.y;
	this->z += anotherVec.z;
}

void vec3::normalize() {
	float length = sqrtf(x * x + y * y + z * z);
	x = x / length;
	y = y / length;
	z = z / length;
}

vec3 calcLineCoord(const vec3* firstVec, const vec3* secondVec) {
	float x = secondVec->x - firstVec->x;
	float y = secondVec->y - firstVec->y;
	float z = secondVec->z - firstVec->z;
	return vec3{ x, y, z };
}

//find normal of a face
vec3 crossProduct(const vec3* firstVec, const vec3* secondVec, const vec3* thirdVec) {
	vec3 lineA = calcLineCoord(firstVec, secondVec);
	vec3 lineB = calcLineCoord(firstVec, thirdVec);
	vec3 normal;

	//calc normal
	normal.x = lineA.y * lineB.z - lineA.z * lineB.y; // Nx = Ay * Bz - Az * By
	normal.y = lineA.z * lineB.x - lineA.x * lineB.z; // Ny = Az * Bx - Ax * Bz
	normal.z = lineA.x * lineB.y - lineA.y * lineB.x; // Nz = Ax * By - Ay * Bx
	normal.normalize();
	return normal;
}

//
// Calculate smooth (average) per-vertex normals
//
// [out] normals - output per-vertex normals
// [in] verts - input per-vertex positions
// [in] faces - triangles (triplets of vertex indices)
// [in] nverts - total number of vertices (# of elements in verts and normals arrays)
// [in] nfaces - total number of faces (# of elements in faces array)
//
void calc_mesh_normals(vec3* normals, const vec3* verts, const int* faces, size_t nverts, size_t nfaces)
{
	//for one triangle need three vectors 
	if (nverts < 3)
	{
		return;
	}
	
	//O(number of adj triangles/3)
	for (size_t i = 0; i < nfaces; i+=3) {
		int vecIndicie1 = faces[i]; //1st index of triangle
		int vecIndicie2 = faces[i + 1]; //2nd
		int vecIndicie3 = faces[i + 2]; //3rd

		vec3 normal = crossProduct(&verts[vecIndicie1], &verts[vecIndicie2], &verts[vecIndicie3]);
		normal.normalize();
		 
		normals[vecIndicie1] += normal;
		normals[vecIndicie2] += normal;
		normals[vecIndicie3] += normal;
	}

	//O(count of adj faces to vert * nverts)
	for (size_t i = 0; i < nverts; i++) {
		normals[i].normalize();
	}

	return;
}
