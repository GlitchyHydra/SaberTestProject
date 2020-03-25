#ifndef GIVED_LIST_H
#define GIVED_LIST_H

#include <string>

struct ListNode {
    ListNode* prev;
    ListNode* next;
    ListNode* rand; // pointer to random element of this List or NULL
    std::string     data;
};

class List
{
public:
    void Serialize(FILE* file);  // saving in file (file opened with fopen(path, "wb"))
    void Deserialize(FILE* file);  // loading from file (file opened with fopen(path, "rb"))
    void AddNode(std::string data);
    void TestAfterSerialization(List* anotherList);

private:
    ListNode* head;
    ListNode* tail;
    size_t    count = 0; //was int

};
#endif
