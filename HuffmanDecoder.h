/*
 * HuffmanDecoder.h
 *
 *  Created on: 10-01-2012
 *      Author: Michal
 */

#ifndef HUFFMANDECODER_H_
#define HUFFMANDECODER_H_

#include <map>
#include <vector>
#include <cstdlib>

#include "HuffmanCoding.h"
#include "UnsignedInteger.h"

template<typename T>
class HuffmanDecoder {
public:
	HuffmanDecoder(unsigned int* pEncodedBits, unsigned int encodedDataSize,
			std::map<typename HuffmanCoding<T>::Code, T>* pCodeTable);
	~HuffmanDecoder();

	void decode();

	inline T* getDecodedData();
	inline unsigned int getDecodedDataSize() const;

private:
	void allocateMoreMemory();
	void setItem(const T& item);

private:
	T* m_pDecodedData;
	unsigned int m_iDataSize; //Allocated memory
	unsigned int m_iDecodedDataSize; //Number of currently decoded items

	unsigned int* m_pEncodedBits;
	unsigned int m_iEncodedBitsSize;
	std::map<typename HuffmanCoding<T>::Code, T >* m_pCodeTable;

	std::vector<unsigned int> m_masks;
};

template<typename T>
HuffmanDecoder<T>::HuffmanDecoder(unsigned int* pEncodedBits, unsigned int encodedDataSize,
		std::map<typename HuffmanCoding<T>::Code, T>* pCodeTable) :
		m_iDecodedDataSize(0),
		m_pEncodedBits(pEncodedBits), m_iEncodedBitsSize(encodedDataSize), m_pCodeTable(pCodeTable) {

	m_iDataSize = 2 * encodedDataSize;
	m_iDataSize += 100;
	m_pDecodedData = (T*)malloc(m_iDataSize * sizeof(T));

	m_masks.push_back(0);
	unsigned int mask = 0;
	for(unsigned int i = 0; i < UnsignedInteger::NUMBER_OF_BITS; ++i) {
		UnsignedInteger::setBitFromLeft(&mask, i);
		m_masks.push_back(mask);
	}
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
	m_iDataSize += (m_iDataSize * 0.5) + 1;
	m_pDecodedData = (T*)realloc(m_pDecodedData, m_iDataSize);
}

template<typename T>
void HuffmanDecoder<T>::setItem(const T& item) {
	if(m_iDecodedDataSize >= m_iDataSize) {
		allocateMoreMemory();
	}

	m_pDecodedData[m_iDecodedDataSize] = item;
	++m_iDecodedDataSize;
}

template<typename T>
T* HuffmanDecoder<T>::getDecodedData() {
	return m_pDecodedData;
}

template<typename T>
unsigned int HuffmanDecoder<T>::getDecodedDataSize() const {
	return m_iDecodedDataSize;
}

template<typename T>
void HuffmanDecoder<T>::decode() {
	unsigned int position = 0;
	unsigned int index = 0;

	typename std::map<typename HuffmanCoding<T>::Code, T >::iterator it;
	bool symbolDecoded = false;
	unsigned int testCode = 0;
	unsigned int codeSizeOnFirstInt = 0;
	unsigned int restOfCode = 0;
	unsigned int restCodeSize = 0;

	do {
		it = m_pCodeTable->begin();
		symbolDecoded = false;

		while(!symbolDecoded && it != m_pCodeTable->end()) {

			if(position + it->first.size > 31) {
				//data stored on two integers

				if((index + 1) >= m_iEncodedBitsSize) {
					break;
				}

				testCode = m_pEncodedBits[index] & m_masks[it->first.size];

				codeSizeOnFirstInt = UnsignedInteger::NUMBER_OF_BITS - position;
				restCodeSize = it->first.size - codeSizeOnFirstInt;
				restOfCode = m_pEncodedBits[index + 1] & m_masks[restCodeSize];
				restOfCode >>= codeSizeOnFirstInt;
				testCode |= restOfCode;

				if(testCode == it->first.code) {
					++index;
					position = restCodeSize;
					m_pEncodedBits[index] <<= restCodeSize;

					setItem(it->second);
					symbolDecoded = true;
				}

			} else {
				//data is on current integer

				testCode = m_pEncodedBits[index] & m_masks[it->first.size];

				if(testCode == it->first.code) {
					m_pEncodedBits[index] <<= it->first.size;
					position += it->first.size;

					if(position == 31) {
						position = 0;
						++index;
					}

					setItem(it->second);
					symbolDecoded = true;
				}
			}

			++it;
		}
	} while(symbolDecoded && (index < m_iEncodedBitsSize));
}

#endif /* HUFFMANDECODER_H_ */
