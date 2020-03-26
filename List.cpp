#include "List.h"

#include <iostream>
#include <unordered_map> 
#include <thread>

struct ListHeader {
	uint32_t sizeOfList;//count of elements
	uint16_t sizeOfAddr; //pointers have a different size on different machines such as 64bit, 32 bit...
};
//assuming that number of elements will not be exceed 2^32 
struct DataPortion {
	uint32_t id; //order number of Node
	uint32_t rand_id; //order number of rand Node
	uint32_t sizeOfData; //size of data (size of string)
};

//fillMap with Node addr and Node order number
//It's need for next searching of rand Node order number
void fillMap(std::unordered_map <ListNode *, uint32_t>* nodesMap, size_t count, ListNode* currentNode, bool forHead) {
	size_t halfSize = count >> 1; // median num
	size_t length = count - 1; // length of List
	if (forHead) {
		for (size_t i = 0; i <= halfSize; i++) {
			nodesMap->insert({ currentNode, i });
			currentNode = currentNode->next;
		}
	}
	else {
		for (size_t i = length; i > halfSize; i--) {
			nodesMap->insert({ currentNode, i });
			currentNode = currentNode->prev;
		}
	}
}

//creating two maps from head and from tail to median
std::unordered_map <ListNode *, uint32_t> getMapOfNodes(size_t count, ListNode* head, ListNode* tail) {
	std::unordered_map <ListNode *, uint32_t> headMap;
	std::unordered_map <ListNode*, uint32_t> tailMap;
	std::thread threadHeadMap;
	std::thread threadTailMap;
	threadHeadMap = std::thread(fillMap, &headMap, count, head, true);
	threadTailMap = std::thread(fillMap, &tailMap, count, tail, false);
	//wait for result
	if (threadHeadMap.joinable())
		threadHeadMap.join();
	if (threadTailMap.joinable())
		threadTailMap.join();
	//concatenate two maps together
	headMap.insert(tailMap.begin(), tailMap.end());
	return headMap;
}

void List::Serialize(FILE* file) {
	//creating info about List size and size of pointers
	//for x64 - 8 byte, for x32 - 4 byte
	ListHeader* lh = new ListHeader{ (uint32_t)count, sizeof(struct ListNode*)};
	fwrite(lh, sizeof(ListHeader), 1, file);
	//if List is empty is nothing else to write
	if (head == NULL || count == 0) {
		fclose(file);
		return;
	}
	//allData gathered together for one string
	//It will be write with one fwrite to file
	std::string allData;
	DataPortion* dpArray = new DataPortion[count];
	ListNode* currentNode = head;
	//creating map of Node addr and Node order number
	std::unordered_map <ListNode*, uint32_t> mapOfNodes = getMapOfNodes(count, head, tail);
	//write info about each Node
	for (size_t i = 0; i < count; i++) {
		dpArray[i].id = i;
		if (currentNode->rand == NULL)
		{
			dpArray[i].rand_id = NULL;
		}
		else {
			dpArray[i].rand_id = mapOfNodes.find(currentNode->rand)->second;
		}
		dpArray[i].sizeOfData = (uint32_t)currentNode->data.size();
		allData.append(currentNode->data);
		currentNode = currentNode->next;
	}
	fwrite(dpArray, sizeof(DataPortion), count, file);
	//write strings
	fwrite(allData.c_str(), sizeof(const char), allData.size(), file);
	//free allocated memory and close file for writing
	delete lh;
	delete[] dpArray;
	fclose(file);
}
//read header
ListHeader DesearializeHeader(FILE* file) {
	char buffer[sizeof(ListHeader)];
	fread(buffer, sizeof(ListHeader), 1, file);
	ListHeader lh{
		(buffer[3] << 24) | (buffer[2] << 16) | (buffer[1] << 8) | (buffer[0]),
		(buffer[5] << 8) | (buffer[4])
	};
	return lh;
}

void List::Deserialize(FILE* file){
	ListHeader lh = DesearializeHeader(file);
	this->count = lh.sizeOfList;
	if (count == 0) {
		fclose(file);
		return;
	}
	//allocate memory for reading info about info
	DataPortion* dpArray = new DataPortion[count];
	fread(dpArray, sizeof(DataPortion), count, file);
	//map storing node:random node dependicies 
	std::unordered_map <uint32_t, ListNode*> nodesMap;
	size_t allSize = 0;
	//Array with Nodes
	ListNode* lnArray = new ListNode[count];
	//read all data, place to Nodes
	//and linking in proper order each Node
	for (size_t i = 0; i < count; i++) {
		lnArray[i].rand = &lnArray[dpArray[i].rand_id];
		//check if i_Node is not a last Node
		//then i + 1 will be out of range
		if (i + 1 < count)
		{
			lnArray[i].next = &lnArray[i + 1];
			lnArray[i + 1].prev = &lnArray[i];
		}
		size_t sizeToRead = dpArray[i].sizeOfData;
		//set proper size for string
		lnArray[i].data.resize(sizeToRead);
		//write to char array data
		fread(&lnArray[i].data[0], sizeof(char), sizeToRead, file);
	}
	head = lnArray;
	tail = &lnArray[count - 1];
	//free memory for info Of Node array and close file 
	delete[] dpArray;
	fclose(file);
}
//Additional function to fill List
void List::AddNode(std::string data) {
	count++;
	if (head == nullptr) {
		head = new ListNode();
		head->data = data;
		tail = head;
		return;
	}
	ListNode* newListNode = new ListNode();
	newListNode->data = data;
	newListNode->prev = tail;
	newListNode->rand = tail->prev;
	tail->next = newListNode;
	tail = newListNode;
	return;
}