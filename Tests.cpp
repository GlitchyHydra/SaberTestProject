#include <assert.h>
#include <chrono>
#include <iostream>

#include "Tests.h"
#include "Calc_mesh_normals.h"
#include "List.h"

using namespace std;

//test two List
void List::TestAfterSerialization(List* anotherList) {
	ListNode* thisNode = head;
	ListNode* anotherNode = anotherList->head;
	for (size_t i = 1; i < count; i++)
	{
		assert(thisNode->data == anotherNode->data);
		thisNode = thisNode->next;
		anotherNode = anotherNode->next;
		if (thisNode->rand != NULL && anotherNode->rand != NULL)
		{
			assert(thisNode->rand->data == anotherNode->rand->data);
		}
	}
	cout << "TEST DONE" << endl;
}

//stress test. All Nodes have rand except head Node.
void prepareSerialization() {

	FILE* f;
	fopen_s(&f, "serializedData", "wb");
	List* ls = new List();

	chrono::steady_clock::time_point begin = chrono::steady_clock::now();
	for (size_t i = 0; i < 1000000; i++)
		ls->AddNode(to_string(i));
	ls->AddNode("durak");
	chrono::steady_clock::time_point end = chrono::steady_clock::now();
	cout << "Time difference = " << (chrono::duration_cast<chrono::microseconds>(end - begin).count()) / 1000000.0 << "[µs]" << endl;

	begin = chrono::steady_clock::now();
	ls->Serialize(f);
	end = chrono::steady_clock::now();
	cout << "Time difference = " << (chrono::duration_cast<chrono::microseconds>(end - begin).count()) / 1000000.0 << "[µs]" << endl;

	List* ls2 = new List();
	fopen_s(&f, "serializedData", "rb");
	begin = chrono::steady_clock::now();
	ls2->Deserialize(f);
	end = chrono::steady_clock::now();
	cout << "Time difference = " << (chrono::duration_cast<chrono::microseconds>(end - begin).count()) / 1000000.0 << "[µs]" << endl;
	ls->TestAfterSerialization(ls2);
}

//Test empty List
void prepareSerializationEmpty() {
	FILE* f;
	fopen_s(&f, "serializedData", "wb");
	List* ls = new List();
	ls->Serialize(f);
	List* ls2 = new List();
	fopen_s(&f, "serializedData", "rb");
	ls2->Deserialize(f);
	ls->TestAfterSerialization(ls2);
}

//Test List with One element
void prepareSerializationOneElement() {
	FILE* f;
	fopen_s(&f, "serializedData", "wb");
	List* ls = new List();
	ls->AddNode("durak");
	ls->Serialize(f);
	List* ls2 = new List();
	fopen_s(&f, "serializedData", "rb");
	ls2->Deserialize(f);
	ls->TestAfterSerialization(ls2);
}
//This is not a test. This was need for debugging of third task.
//Cube was create in 3dsMax for third task.
//Then was created one script for printing vertex.
//All was got from 3ds Max. (check cover letter in email)
void testAverageNormals() {
	vec3* verts = new vec3[8];
	verts[0] = vec3{ -0.163f, -0.201f, .0f };
	verts[1] = vec3{ 0.292f, -0.201f, .0f };
	verts[2] = vec3{ -0.163f,  0.254f, -.0f };
	verts[3] = vec3{ 0.292f,  0.254f, -.0f };
	verts[4] = vec3{ -0.163f, -0.201f, 0.089f };
	verts[5] = vec3{ 0.292f, -0.201f, 0.089f };
	verts[6] = vec3{ -0.163f,  0.254f, 0.089f };
	verts[7] = vec3{ 0.292f,  0.254f, 0.089f };

	int* faces = new int[36];
	faces[0] = 0; faces[1] = 2; faces[2] = 3;
	faces[3] = 3; faces[4] = 1; faces[5] = 0;
	faces[6] = 4; faces[7] = 5; faces[8] = 7;
	faces[9] = 7; faces[10] = 6; faces[11] = 4;
	faces[12] = 0; faces[13] = 1; faces[14] = 5;
	faces[15] = 5; faces[16] = 4; faces[17] = 0;
	faces[18] = 1; faces[19] = 3; faces[20] = 7;
	faces[21] = 7; faces[22] = 5; faces[23] = 1;
	faces[24] = 3; faces[25] = 2; faces[26] = 6;
	faces[27] = 6; faces[28] = 7; faces[29] = 3;
	faces[30] = 2; faces[31] = 0; faces[32] = 4;
	faces[33] = 4; faces[34] = 6; faces[35] = 2;
	//output array
	vec3* normals = new vec3[8];
	calc_mesh_normals(normals, verts, faces, 8, 36);
	for (size_t i = 0; i < 8; i++) {
		cout << normals[i].x << " " << normals[i].y << " " << normals[i].z <<endl;
	}
}
