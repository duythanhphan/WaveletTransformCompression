/*
 * Wavelet.cpp
 *
 *  Created on: 30-12-2011
 *      Author: Michal
 */

#include <cstring>
#include <cstdio>
#include <cassert>

#include "UnsignedInteger.h"
#include "WaveletTransform.h"

WaveletTransform::WaveletTransform() : m_pImageTransform(0), m_iSize(0) { }

WaveletTransform::WaveletTransform(double* imageData, unsigned int size, bool CopyData) :
	m_pImageTransform(0), m_iSize(0) {

	if(CopyData) {
		copyData(imageData, size);
	} else {
		setData(imageData, size);
	}
}

void WaveletTransform::copyData(double* imageData, unsigned int size) {
	cleanup();

	m_iSize = UnsignedInteger::getClosestPowerOfTwo(size);

	m_pImageTransform = new double[m_iSize * m_iSize];
	const size_t bytesInRow = size * sizeof(double);
	const size_t bytesToFill = (m_iSize - size) * sizeof(double);

	for(unsigned int y = 0; y < size; ++y) {
		memcpy(&m_pImageTransform[y * m_iSize], &imageData[y * size], bytesInRow);
		memset(&m_pImageTransform[(y * m_iSize) + size], 0, bytesToFill);
	}

	const size_t bytesInTransformRow = m_iSize * sizeof(double);
	for(unsigned int y = size; y < m_iSize; ++y) {
		memset(&m_pImageTransform[y * m_iSize], 0, bytesInTransformRow);
	}
}

void WaveletTransform::setData(double* imageData, unsigned int size) {
	assert(size == UnsignedInteger::getClosestPowerOfTwo(size) &&
			"WaveletTransform::init size must be power of two");

	m_pImageTransform = imageData;
	m_iSize = size;
}

WaveletTransform::~WaveletTransform() {
	cleanup();
}

void WaveletTransform::cleanup() {
	if(m_pImageTransform != 0) {
		delete[] m_pImageTransform;
		m_pImageTransform = 0;
		m_iSize = 0;
	}
}

void WaveletTransform::copyColumn(double* data, unsigned int column, unsigned int size) {
	for(unsigned int i = 0; i < size; ++i) {
		data[i] = m_pImageTransform[column + (i * m_iSize)];
	}
}

void WaveletTransform::setColumn(double* data, unsigned int column, unsigned int size) {
	for(unsigned int i = 0; i < size; ++i) {
		m_pImageTransform[column + (i * m_iSize)] = data[i];
	}
}

/*
 * 		Transform
 */
void WaveletTransform::transform() {
	unsigned int i = 0;
	double* transform = new double[m_iSize];
	double* data = new double[m_iSize];

	unsigned int size = m_iSize;

	while(size > 1) {
		//One transform step for every row
		for(i = 0; i < m_iSize; ++i) {
			decompositionStep(&m_pImageTransform[i * m_iSize], transform, size);
			memcpy(&m_pImageTransform[i * m_iSize], transform, sizeof(double) * size);
		}

		//One transform step for every column
		for(i = 0; i < m_iSize; ++i) {
			copyColumn(data, i, size);
			decompositionStep(data, transform, size);
			setColumn(transform, i, size);
		}

		size /= 2;
	}

	delete[] transform;
	delete[] data;
}

/*
 * 		Inverse Transform
 */
void WaveletTransform::inverseTransform() {
	unsigned int i = 0;
	double* data = new double[m_iSize];
	double* inverseTransform = new double[m_iSize];

	unsigned int currentSize = 1;

	while(currentSize < m_iSize) {
		//One inverse transform step for every column
		for(i = 0; i < m_iSize; ++i) {
			copyColumn(data, i, currentSize * 2);
			inverseDecompositionStep(data, inverseTransform, currentSize);
			setColumn(inverseTransform, i, currentSize * 2);
		}

		//One inverse transform step for every row
		for(i = 0; i < m_iSize; ++i) {
			inverseDecompositionStep(&m_pImageTransform[i * m_iSize], inverseTransform, currentSize);
			memcpy(&m_pImageTransform[i * m_iSize], inverseTransform, sizeof(double) * currentSize * 2);
		}

		currentSize <<= 1;
	}


	delete[] data;
	delete[] inverseTransform;
}


