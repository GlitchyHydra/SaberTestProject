#include "List.h"

#include <iostream>
#include <unordered_map> 
#include <thread>

struct ListHeader {
	uint32_t sizeOfList;
	uint16_t sizeOfAddr; //pointers have a different size on different machines such as 64bit, 32 bit, 16bit...
	uint32_t dataOffset; //where start a data
};

struct DataPortion {
	uint32_t id; //addr of Node, 4 or 8 bytes, depends on machine
	uint32_t rand_id; //addr of rand Node, 4 or 8 bytes, depends on machine
	uint32_t sizeOfData; //size of string
};

void fillDataPortion(DataPortion* dp, ListNode* currentNode, std::string allData) {
	dp->id = (uint32_t)currentNode;
	dp->rand_id = reinterpret_cast<uint32_t>(currentNode->rand);
	dp->sizeOfData = (uint32_t)currentNode->data.size();
	allData.append(currentNode->data);
}

void List::Serialize(FILE* file) {
	ListHeader* lh = new ListHeader{ (uint32_t)count, sizeof(struct ListNode*), sizeof(ListHeader)};
	fwrite(lh, sizeof(ListHeader), 1, file);

	if (head == nullptr || count == 0) {
		fclose(file);
		return;
	}

	std::string allData;
	DataPortion* dpArray = new DataPortion[count];
	ListNode* currentNode = head;
	/*ListNode* currentNodeFromHead = head;
	ListNode* currentNodeFromTail = tail;*/

	size_t halfCount = count >> 1;
	size_t length = count - 1;

	//write info about each Node
	for (size_t i = 0; i < count; i++) {
		/*fillDataPortion(&dpArray[i], currentNodeFromHead, allData);
		fillDataPortion(&dpArray[length - i], currentNodeFromTail, allData);
		currentNodeFromHead = currentNodeFromHead->next;
		currentNodeFromTail = currentNodeFromTail->prev;*/
		dpArray[i].id = (uint32_t)currentNode;
		dpArray[i].rand_id = reinterpret_cast<uint32_t>(currentNode->rand);
		dpArray[i].sizeOfData = (uint32_t)currentNode->data.size();
		allData.append(currentNode->data);
		currentNode = currentNode->next;
	}
	/*if ((count & 1) != 1)
		fillDataPortion(&dpArray[halfCount], currentNodeFromTail, allData);*/
	fwrite(dpArray, sizeof(DataPortion), count, file);

	//write strings
	uint32_t sizeOfData = allData.size();
	fwrite(&sizeOfData, sizeof(uint32_t), 1, file);
	fwrite(allData.c_str(), sizeof(const char), sizeOfData, file);
	
	delete lh;
	delete[] dpArray;

	fclose(file);
}

ListHeader DesearializeHeader(FILE* file) {
	char buffer[sizeof(ListHeader)];
	fread(buffer, sizeof(ListHeader), 1, file);
	ListHeader lh{
		(buffer[3] << 24) | (buffer[2] << 16) | (buffer[1] << 8) | (buffer[0]),
		(buffer[5] << 8) | (buffer[4]),
		(buffer[9] << 24) | (buffer[8] << 16) | (buffer[7] << 8) | (buffer[6])
	};
	return lh;
}

void fillMap(std::unordered_map <uint32_t, ListNode*> nodesMap, size_t count, ListNode* lnArray, DataPortion* dpArray) {
	for (size_t i = 0; i < count >> 1; i++) {
		nodesMap.insert({ dpArray[i].id, &lnArray[i] });
	}
}

void fillMap1(std::unordered_map <uint32_t, ListNode*> nodesMap, size_t count, ListNode* lnArray, DataPortion* dpArray) {
	for (size_t i = count >> 1; i < count; i++) {
		nodesMap.insert({ dpArray[i].id, &lnArray[i] });
	}
}

void List::Deserialize(FILE* file){
	ListHeader lh = DesearializeHeader(file);
	this->count = lh.sizeOfList;

	if (count == 0) return;

	DataPortion* dpArray = new DataPortion[count];
	fread(dpArray, sizeof(DataPortion), count, file);
	
	uint32_t sizeOfSt = 0;
	fread(&sizeOfSt, sizeof(uint32_t), 1, file);

	//map storing node:random node dependicies 
	std::unordered_map <uint32_t, ListNode*> nodesMap;
	size_t allSize = 0;
	ListNode* lnArray = new ListNode[count];
	
	/*std::thread threadMap;
	threadMap = std::thread(fillMap, nodesMap, count, lnArray, dpArray);
	std::thread threadMap1;
	threadMap1 = std::thread(fillMap1, nodesMap, count, lnArray, dpArray);*/

	for (size_t i = 0; i < count; i++) {
		
			nodesMap.insert({ dpArray[i].id, &lnArray[i] });
		if (i + 1 < count)
		{
			lnArray[i].next = &lnArray[i + 1];
			lnArray[i + 1].prev = &lnArray[i];
		}
		size_t sizeToRead = dpArray[i].sizeOfData;
		lnArray[i].data.resize(sizeToRead);
		fread(&lnArray[i].data[0], sizeof(char), sizeToRead, file);
	}

	/*if (threadMap.joinable())
		threadMap.join();
	if (threadMap1.joinable())
		threadMap1.join();*/

	size_t i = 0;
	head = lnArray;
	tail = &lnArray[count - 1];
	for (size_t i = 0; i < count; i++) {
		size_t randomNodeId = dpArray[i].rand_id;
		if (randomNodeId != NULL) {
			auto search = nodesMap.find(randomNodeId);
			lnArray[i].rand = search->second;
		}
	}

	delete[] dpArray;
	fclose(file);
}

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