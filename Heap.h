/*
 * Heap.h
 *
 *  Created on: 03-01-2012
 *      Author: Michal
 */

#ifndef HEAP_H_
#define HEAP_H_

#include <cassert>

template<typename T>
class Heap {
public:
	Heap(T* pHeap, unsigned int heapSize);
	Heap(T* pHeap, unsigned int heapSize, unsigned int arraySize);

	//indexes are from 1 to HeapSize, HeapWrapper does the rest of job
	inline unsigned int parent(int i) { return i >>= 1; }
	inline unsigned int left(int i) { return i <<= 1; }
	inline unsigned int right(int i) { return i = (i << 1) + 1; }

	void heapify(unsigned int i);
	void buildHeap();
	void heapSort();

	T dequeue();
	void enqueue(const T& item);

	inline T* getHeap() { return m_heap.m_pHeap; }
	inline int getHeapSize() { return m_iHeapSize; }

private:
	template<typename U>
	struct HeapWrapper {
		U* m_pHeap;
		HeapWrapper(U* pHeap);
		U& operator[](int i) { return m_pHeap[i - 1]; }
	};

	HeapWrapper<T> m_heap;
	unsigned int m_iHeapSize;
	unsigned int m_iArrayLength;
};

template<typename T>
template<typename U>
Heap<T>::HeapWrapper<U>::HeapWrapper(U* pHeap) : m_pHeap(pHeap) { }

template<typename T>
Heap<T>::Heap(T* pHeap, unsigned int heapSize) : m_heap(pHeap),  m_iHeapSize(heapSize), m_iArrayLength(heapSize) { }

template<typename T>
Heap<T>::Heap(T* pHeap, unsigned int heapSize, unsigned int arraySize) :
		m_heap(pHeap), m_iHeapSize(heapSize), m_iArrayLength(arraySize) { }

template<typename T>
void Heap<T>::heapify(unsigned int i) {

	unsigned int leftNode = 0;
	unsigned int rightNode = 0;
	unsigned int largest = 0;
	T tmp;

	while(left(i) <= m_iHeapSize) {
		leftNode = left(i);
		rightNode = right(i);

		if(m_heap[leftNode] > m_heap[i]) {
			largest = leftNode;
		} else {
			largest = i;
		}

		if(rightNode <= m_iHeapSize && m_heap[rightNode] > m_heap[largest]) {
			largest = rightNode;
		}

		if(largest == i) {
			//Node in right place
			break;
		}

		tmp = m_heap[i];
		m_heap[i] = m_heap[largest];
		m_heap[largest] = tmp;

		i = largest;
	}
}

template<typename T>
void Heap<T>::buildHeap() {
	for(unsigned int i = m_iHeapSize / 2; i > 0; --i) {
		heapify(i);
	}
}

template<typename T>
void Heap<T>::heapSort() {
	buildHeap();

	T tmp;
	for(unsigned int i = m_iHeapSize; i > 2; --i) {
		tmp = m_heap[m_iHeapSize];
		m_heap[m_iHeapSize] = m_heap[1];
		m_heap[1] = tmp;

		--m_iHeapSize; //m_iHeapSize = i - 1
		heapify(1);
	}

	tmp = m_heap[m_iHeapSize];
	m_heap[m_iHeapSize] = m_heap[1];
	m_heap[1] = tmp;
}

template<typename T>
T Heap<T>::dequeue() {
	assert(m_iHeapSize > 0 && "Trying to dequeue from empty queue");

	T max = m_heap[1];

	m_heap[1] = m_heap[m_iHeapSize];
	--m_iHeapSize;
	heapify(1);

	return max;
}

template<typename T>
void Heap<T>::enqueue(const T& item) {
	assert(m_iHeapSize <= m_iArrayLength && "Heap overflow");

	++m_iHeapSize;
	unsigned int i = m_iHeapSize;
	int Parent = parent(i);

	while(i > 1 && m_heap[Parent] < item) {
		m_heap[i] = m_heap[Parent];
		i = Parent;
		Parent = parent(i);
	}
	m_heap[i] = item;
}

#endif /* HEAP_H_ */
