/*
 * HuffmanCoding.h
 *
 *  Created on: 03-01-2012
 *      Author: Michal
 */

#ifndef HUFFMANCODING_H_
#define HUFFMANCODING_H_

#include <map>

#include "Heap.h"
#include "UnsignedInteger.h"

template<typename T>
class HuffmanCoding {
public:
	struct Node {
		Node();

		Node* parent;
		Node* left;
		Node* right;

		int count;
	};

	struct Leaf : public Node {
		Leaf();
		Leaf(const T& v);
		T value;
	};

	struct QueueItem {
		QueueItem();
		QueueItem(const QueueItem& queueItem);
		bool operator<(const QueueItem& right);
		bool operator>(const QueueItem& right);
		QueueItem& operator=(const QueueItem& right);

		Node* node;
	};

	struct Code {
		unsigned int code;
		unsigned int size;
	};

public:
	HuffmanCoding(Leaf* pLeafs, unsigned int size);

	~HuffmanCoding();

	void createCodeTable();
	bool getTable(std::map<T, Code>& codeTable);

private:
	Leaf* m_pLeafs;
	unsigned int m_iNumberOfLeafs;
	Node* m_pRoot;
};

/*
 * 		Node
 */
template<typename T>
HuffmanCoding<T>::Node::Node() : parent(0), left(0), right(0), count(0) { }

/*
 * 		Leaf
 */
template<typename T>
HuffmanCoding<T>::Leaf::Leaf() : Node() { }

template<typename T>
HuffmanCoding<T>::Leaf::Leaf(const T& v) : Node(), value(v) { }

/*
 * 		QueueItem
 */
template<typename T>
HuffmanCoding<T>::QueueItem::QueueItem() : node(0) { }

template<typename T>
HuffmanCoding<T>::QueueItem::QueueItem(const QueueItem& queueItem) : node(queueItem.node) { }

/*
 * 		Operators < and > are negated because QueueItem is bigger when it have
 * 		higher priority in queue, higher priority means less probability and this
 * 		means less count value.
 */
template<typename T>
bool HuffmanCoding<T>::QueueItem::operator <(const QueueItem& right) {
	return node->count > right.node->count;
}

template<typename T>
bool HuffmanCoding<T>::QueueItem::operator >(const QueueItem& right) {
	return node->count < right.node->count;
}

template<typename T>
typename HuffmanCoding<T>::QueueItem& HuffmanCoding<T>::QueueItem::operator =(const QueueItem& right) {
	if(this == &right) {
		return *this;
	}
	node = right.node;
	return *this;
}

/*
 * 		HuffmanCoding
 */
template<typename T>
HuffmanCoding<T>::HuffmanCoding(Leaf* pLeafs, unsigned int size) :
		m_pLeafs(pLeafs), m_iNumberOfLeafs(size), m_pRoot(0) { }

template<typename T>
HuffmanCoding<T>::~HuffmanCoding() {
	//TODO delete right stuff
}

template<typename T>
void HuffmanCoding<T>::createCodeTable() {
	QueueItem* pQueueItems = new QueueItem[m_iNumberOfLeafs];
	for(unsigned int i = 0; i < m_iNumberOfLeafs; ++i) {
		pQueueItems[i].node = &m_pLeafs[i];
	}

	Heap<QueueItem> heap(pQueueItems, m_iNumberOfLeafs);
	heap.buildHeap();
	QueueItem item1, item2, newItem;
	Node* newNode = 0;

	while(heap.getHeapSize() > 1) {
		item1 = heap.dequeue();
		item2 = heap.dequeue();

		newNode = new Node();
		newNode->left = item1.node;
		newNode->right = item2.node;
		newNode->count = item1.node->count + item2.node->count;

		item1.node->parent = newNode;
		item2.node->parent = newNode;

		newItem.node = newNode;

		heap.enqueue(newItem);
	}

	m_pRoot = heap.dequeue().node;

	delete[] pQueueItems;
}

template<typename T>
bool HuffmanCoding<T>::getTable(std::map<T, Code>& codeTable) {
	if(m_pRoot == 0) {
		return false;
	}

	Node* node = 0;
	unsigned int bitPosition = 0;
	unsigned int reverseCode = 0;
	Code code;
	const unsigned int shiftMax = UnsignedInteger::NUMBER_OF_BITS - 1;
	unsigned int shift = 0;

	for(unsigned int i = 0; i < m_iNumberOfLeafs; ++i) {
		node = &m_pLeafs[i];
		bitPosition = 0;
		reverseCode = 0;
		code.code = 0;

		while(node->parent != 0) {
			if(node->parent->right == node) {
				//node is on the right of parent encode 1
				UnsignedInteger::setBitFromRight(&reverseCode, bitPosition);
			}
			//else node is on the left of parent encode 0,
			//already set reversedCode = 0 at beginning of for loop

			++bitPosition;
			node = node->parent;
		}

		shift = shiftMax - (bitPosition - 1);
		code.code = reverseCode << shift;
		//code.code = UnsignedInteger::reverse(reverseCode);
		code.size = bitPosition;
		codeTable[m_pLeafs[i].value] = code;
	}

	return true;
}

#endif /* HUFFMANCODING_H_ */
