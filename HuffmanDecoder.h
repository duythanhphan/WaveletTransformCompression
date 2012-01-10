/*
 * HuffmanDecoder.h
 *
 *  Created on: 10-01-2012
 *      Author: Michal
 */

#ifndef HUFFMANDECODER_H_
#define HUFFMANDECODER_H_

#include <map>
#include <cstdlib>
#include "HuffmanCoding.h"

template<typename T>
class HuffmanDecoder {
public:
	HuffmanDecoder(unsigned int* pEncodedBits, unsigned int encodedDataSize,
			std::map<HuffmanCoding<T>::Code, T>* pCodeTable);
	~HuffmanDecoder();

	inline T* getDecodedData();
	inline unsigned int getDecodedDataSize() const;

private:
	void allocateMoreMemory();

private:
	T* m_pDecodedData;
	unsigned int m_iDataSize;
	unsigned int m_iDecodedDataSize;

	unsigned int* m_pEncodedBits;
	unsigned int m_iEncodedBitsSize;
	std::map<HuffmanCoding<T>::Code, T>* m_pCodeTable;
};

template<typename T>
HuffmanDecoder<T>::HuffmanDecoder(unsigned int* pEncodedBits, unsigned int encodedDataSize,
		std::map<HuffmanCoding<T>::Code, T>* pCodeTable) :
		m_iDecodedDataSize(0),
		m_pEncodedBits(pEncodedBits), m_iEncodedBitsSize(encodedDataSize), m_pCodeTable(pCodeTable) {

	m_iDataSize = 2 * encodedDataSize;
	m_pDecodedData = (T*)malloc(m_iDataSize * sizeof(T));
}

template<typename T>
HuffmanDecoder<T>::~HuffmanDecoder() {
	if(m_pDecodedData != 0) {
		free(m_pDecodedData);
		m_pDecodedData = 0;
	}
}

template<typename T>
void HuffmanDecoder<T>::allocateMoreMemory() {
	m_iDecodedDataSize += m_iDecodedDataSize * 0.5;
	m_pDecodedData = realloc(m_pDecodedData, m_iDecodedDataSize);
}

template<typename T>
T* HuffmanDecoder<T>::getDecodedData() {
	return m_pDecodedData;
}

template<typename T>
unsigned int HuffmanDecoder<T>::getDecodedDataSize() const {
	return m_iDecodedDataSize;
}

#endif /* HUFFMANDECODER_H_ */
