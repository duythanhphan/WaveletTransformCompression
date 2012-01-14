/*
 * RLEDecoder.h
 *
 *  Created on: 14-01-2012
 *      Author: Michal
 */

#ifndef RLEDECODER_H_
#define RLEDECODER_H_

#include "RLE.h"

template<typename T>
class RLEDecoder {
public:
	RLEDecoder(typename RLE<T>::Run* pCodedData, unsigned int dataSize);

	bool decode(T* memory, unsigned int size);

private:
	typename RLE<T>::Run* m_pCodedData;
	unsigned int m_iDataSize;
	unsigned int m_iPosition;
};

template<typename T>
RLEDecoder<T>::RLEDecoder(typename RLE<T>::Run* pCodedData, unsigned int dataSize) :
		m_pCodedData(pCodedData), m_iDataSize(dataSize), m_iPosition(0) { }

template<typename T>
bool RLEDecoder<T>::decode(T* memory, unsigned int size) {
	unsigned int decodedItems = 0;
	unsigned int i = 0;

	while(decodedItems < size && m_iPosition < m_iDataSize) {

		for(i = 0; i < m_pCodedData[m_iPosition].run; ++i) {
			memory[decodedItems] = m_pCodedData[m_iPosition].value;
			++decodedItems;
		}

		++m_iPosition;
	}

	if(decodedItems != size) {
		return false;
	}

	return true;
}

#endif /* RLEDECODER_H_ */
