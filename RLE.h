/*
 * RLE.h
 *
 *  Created on: 05-01-2012
 *      Author: Michal
 */

#ifndef RLE_H_
#define RLE_H_

#include <cstdlib>

template<typename T>
class RLE {
public:
	struct Run {
		T value;
		unsigned int run;
		bool operator<(const Run& right) const;
		bool operator==(const Run& right) const;
	};

public:
	RLE(T* pData, unsigned int dataSize);
	~RLE();

	inline unsigned int getEncodedDataSize() const;
	Run* getData();

	inline void saveRun(T value, unsigned int run);
	void encode();

private:
	T* m_pData;
	unsigned int m_iDataSize;

	Run* m_pEncodedData;
	//how many Runs have been encoded
	unsigned int m_iEncodedDataSize;

	//space in bytes allocated for storing Runs
	unsigned int m_iAllocatedMemory;

	//how much additional memory to allocate if out of space
	unsigned int m_iAdditionalMemorySize;
};

template<typename T>
bool RLE<T>::Run::operator <(const Run& right) const {
	if(run == right.run) {
		return value < right.value;
	} else {
		return run < right.run;
	}
}

template<typename T>
bool RLE<T>::Run::operator==(const Run& right) const {
	return (value == right.value) && (run == right.run);
}

template<typename T>
RLE<T>::RLE(T* pData, unsigned int dataSize) :
		m_pData(pData), m_iDataSize(dataSize),
		m_iEncodedDataSize(0), m_iAdditionalMemorySize(dataSize / 8) {

	unsigned int memoryToAllocate = dataSize / 4;

	m_pEncodedData = (Run*)malloc(memoryToAllocate * sizeof(Run));
	m_iAllocatedMemory = memoryToAllocate;
}

template<typename T>
RLE<T>::~RLE() {
	free(m_pEncodedData);
}

template<typename T>
unsigned int RLE<T>::getEncodedDataSize() const {
	return m_iEncodedDataSize;
}

template<typename T>
typename RLE<T>::Run* RLE<T>::getData() {
	return m_pEncodedData;
}

template<typename T>
void RLE<T>::saveRun(T value, unsigned int run) {
	if(m_iEncodedDataSize >= m_iAllocatedMemory) {
		m_pEncodedData = (Run*)realloc(m_pEncodedData, (m_iAllocatedMemory + m_iAdditionalMemorySize) * sizeof(Run));
		m_iAllocatedMemory += m_iAdditionalMemorySize;
	}

	m_pEncodedData[m_iEncodedDataSize].value = value;
	m_pEncodedData[m_iEncodedDataSize].run = run;
	++m_iEncodedDataSize;
}

template<typename T>
void RLE<T>::encode() {
	T currentValue = m_pData[0];
	unsigned int counter = 1;

	for(unsigned int i = 1; i < m_iDataSize; ++i) {
		if(m_pData[i] == currentValue) {
			++counter;
		} else {
			saveRun(currentValue, counter);
			counter = 1;
			currentValue = m_pData[i];
		}
	}

	saveRun(currentValue, counter);
}

#endif /* RLE_H_ */
