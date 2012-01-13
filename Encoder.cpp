/*
 * Encoder.cpp
 *
 *  Created on: 06-01-2012
 *      Author: Michal
 */

#include <cstdlib>
#include <cstring>

#include "Encoder.h"

Encoder::Encoder(unsigned int initialSize) :
		m_iCurrentSize(initialSize), m_iAdditionalMemorySize(initialSize),
		m_iCurrentIndex(0), m_iCurrentBitPosition(0) {

	m_pData = (unsigned int*)malloc(m_iCurrentSize * sizeof(unsigned int));
	memset(m_pData, 0, sizeof(unsigned int) * m_iCurrentSize);

	m_iMaxBitPosition = (sizeof(unsigned int) * 8) - 1;

	unsigned int mask = 0;
	unsigned int offIndex = m_iMaxBitPosition;
	for(unsigned int i = 0; i < m_iMaxBitPosition; ++i) {
		mask |= 1 << i;
		m_offMasks.insert(std::pair<unsigned int, unsigned int>(offIndex, mask));
		--offIndex;
	}
}

Encoder::~Encoder() {
	if(m_pData != 0) {
		free(m_pData);
		m_pData = 0;
	}
}

void Encoder::allocateAdditionalMemory() {
	unsigned int prevSize = m_iCurrentSize;

	m_iCurrentSize += m_iAdditionalMemorySize;
	m_pData = (unsigned int*)realloc(m_pData, sizeof(unsigned int) * m_iCurrentSize);

	memset(&m_pData[prevSize], 0, sizeof(unsigned int) * m_iAdditionalMemorySize);
}

void Encoder::encode(unsigned int code, unsigned int numberOfBits) {
	unsigned int shiftedCode = code >> m_iCurrentBitPosition;
	m_pData[m_iCurrentIndex] |= shiftedCode;

	m_iCurrentBitPosition += numberOfBits;
	unsigned int lastBitSetPosition = m_iCurrentBitPosition - 1;
	if(lastBitSetPosition > m_iMaxBitPosition) {
		unsigned int bitsSet = numberOfBits - (lastBitSetPosition - m_iMaxBitPosition);
		shiftedCode = code & m_offMasks[bitsSet];
		shiftedCode <<= bitsSet;

		++m_iCurrentIndex;
		if(m_iCurrentIndex >= m_iCurrentSize) {
			allocateAdditionalMemory();
		}

		m_pData[m_iCurrentIndex] |= shiftedCode;
		m_iCurrentBitPosition = numberOfBits - bitsSet;
	} else if(lastBitSetPosition == m_iMaxBitPosition) {

		++m_iCurrentIndex;
		m_iCurrentBitPosition = 0;
		if(m_iCurrentIndex >= m_iCurrentSize) {
			allocateAdditionalMemory();
		}
	}
}

