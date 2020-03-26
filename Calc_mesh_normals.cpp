#include "Calc_mesh_normals.h"
#include <math.h>
#include <unordered_set>

using namespace std;

//set hash calc for vec3
namespace std
{
	template<>
	struct hash<vec3>
	{
		size_t
			operator()(const vec3& obj) const
		{
			return hash<int>()(obj.x + obj.y + obj.z);
		}
	};
};

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

vec3 calcAverage(unordered_set<vec3> normals) {
	vec3 sumVec = vec3{ .0, .0, .0 };
	
	//sum all coordinates
	for (unordered_set<vec3>::const_iterator normal = normals.begin(); normal != normals.end(); ++normal)
	{
		sumVec.x += normal->x;
		sumVec.y += normal->y;
		sumVec.z += normal->z;
	}

	float number = (float) normals.size();

	return vec3{ sumVec.x / number, sumVec.y / number, sumVec.z / number };
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
	//catch bad allocation
	unordered_set<vec3>* adjArr = new unordered_set < vec3>[nverts];
	
	//O(number of adj triangles/3)
	for (size_t i = 0; i < nfaces; i+=3) {
		int vecIndicie1 = faces[i]; //1st index of triangle
		int vecIndicie2 = faces[i + 1]; //2nd
		int vecIndicie3 = faces[i + 2]; //3rd

		//getNormal and place to adjNode of vector
		vec3 normal = crossProduct(&verts[vecIndicie1], &verts[vecIndicie2], &verts[vecIndicie3]);
		adjArr[vecIndicie1].insert(normal);
		adjArr[vecIndicie2].insert(normal);
		adjArr[vecIndicie3].insert(normal);
	}

	//O(count of adj faces to vert * nverts)
	for (size_t i = 0; i < nverts; i++) {
		normals[i] = calcAverage(adjArr[i]);
		normals[i].normalize();
	}
	delete[] adjArr;
	return;
}
