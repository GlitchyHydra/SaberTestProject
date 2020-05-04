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
			assert(thisNode->rand->data.compare(anotherNode->rand->data) == 0);
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
	ls->AddNode("One element");
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
	vec3 verts[8]{
		vec3{ -0.163f, -0.201f,  0.0f   },
		vec3{  0.292f, -0.201f,  0.0f   },
		vec3{ -0.163f,  0.254f, -0.0f   },
		vec3{  0.292f,  0.254f, -0.0f   },
		vec3{ -0.163f, -0.201f,  0.089f },
		vec3{  0.292f, -0.201f,  0.089f },
		vec3{ -0.163f,  0.254f,  0.089f },
		vec3{  0.292f,  0.254f,  0.089f }
	};
	

	int faces[36] = {
		0, 2, 3,
		3, 1, 0,
		4, 5, 7,
		7, 6, 4,
		0, 1, 5,
	    5, 4, 0,
	    1, 3, 7,
	    7, 5, 1,
	    3, 2, 6,
	    6, 7, 3,
	    2, 0, 4,
	    4, 6, 2
	};

	//output array
	vec3* normals = new vec3[8];
	calc_mesh_normals(normals, verts, faces, 8, 36);
	for (size_t i = 0; i < 8; i++) {
		cout << normals[i].x << " " << normals[i].y << " " << normals[i].z <<endl;
	}
}
