/*
 * HaarWaveletTransform.cpp
 *
 *  Created on: 29-12-2011
 *      Author: Michal
 */

#include <cstring>
#include <cstdio>

#include "HaarWaveletTransform.h"

HaarWaveletTransform::HaarWaveletTransform() : m_pImageTransform(0), m_iWidth(0), m_iHeight(0) { }

HaarWaveletTransform::HaarWaveletTransform(double* imageData, unsigned int width, unsigned int height, bool CopyData) :
	m_pImageTransform(0), m_iWidth(0), m_iHeight(0) {

	if(CopyData) {
		copyData(imageData, width, height);
	} else {
		setData(imageData, width, height);
	}
}

void HaarWaveletTransform::copyData(double* imageData, unsigned int width, unsigned int height) {
	cleanup();

	m_iWidth = UnsignedInteger::getClosestPowerOfTwo(width);
	m_iHeight = UnsignedInteger::getClosestPowerOfTwo(height);

	m_pImageTransform = new double[m_iWidth * m_iHeight];
	const size_t bytesInRow = width * sizeof(double);
	const size_t bytesToFill = (m_iWidth - width) * sizeof(double);

	for(unsigned int y = 0; y < height; ++y) {
		memcpy(&m_pImageTransform[y * m_iWidth], &imageData[y * width], bytesInRow);
		memset(&m_pImageTransform[(y * m_iWidth) + width], 0, bytesToFill);
	}

	const size_t bytesInTransformRow = m_iWidth * sizeof(double);
	for(unsigned int y = height; y < m_iHeight; ++y) {
		memset(&m_pImageTransform[y * m_iWidth], 0, bytesInTransformRow);
	}
}

void HaarWaveletTransform::setData(double* imageData, unsigned int width, unsigned int height) {
	assert(width == UnsignedInteger::getClosestPowerOfTwo(width) &&
			"HaarWaveletTransform::init width must be power of two");
	assert(height == UnsignedInteger::getClosestPowerOfTwo(height) &&
			"HaarWaveletTransform::init height must be power of two");

	m_pImageTransform = imageData;
	m_iWidth = width;
	m_iHeight = height;
}

void HaarWaveletTransform::cleanup() {
	if(m_pImageTransform != 0) {
		delete[] m_pImageTransform;
		m_pImageTransform = 0;
		m_iWidth = 0;
		m_iHeight = 0;
	}
}

HaarWaveletTransform::~HaarWaveletTransform() {
	cleanup();
}

void HaarWaveletTransform::copyColumn(double* data, unsigned int column) {
	for(unsigned int i = 0; i < m_iHeight; ++i) {
		data[i] = m_pImageTransform[column + (i * m_iWidth)];
	}
}

void HaarWaveletTransform::setColumn(double* data, unsigned int column) {
	for(unsigned int i = 0; i < m_iHeight; ++i) {
		m_pImageTransform[column + (i * m_iWidth)] = data[i];
	}
}

/*
 * 		Transform
 */
void HaarWaveletTransform::decompositionStep(double* data, double* transform, unsigned int size) {
	const unsigned int halfSize = size / 2;

	for(unsigned int i = 0; i < halfSize; ++i) {
		transform[i] = (data[2 * i] + data[(2 * i) + 1]) / 2.0;
		transform[halfSize + i] = (data[2 * i] - data[(2 * i) + 1]) / 2.0;
	}
}

void HaarWaveletTransform::decomposition(double* data, double* transform, unsigned int size) {
	while(size > 1) {
		decompositionStep(data, transform, size);
		memcpy(data, transform, sizeof(double) * size);
		size /= 2;
	}
}

void HaarWaveletTransform::transform() {
	unsigned int i = 0;
	double* transform = new double[m_iWidth];

	for(i = 0; i < m_iHeight; ++i) {
		decomposition(&m_pImageTransform[i * m_iWidth], transform, m_iWidth);
	}

	if(m_iWidth != m_iHeight) {
		delete[] transform;
		transform = new double[m_iHeight];
	}

	double* data = new double[m_iHeight];
	for(i = 0; i < m_iWidth; ++i) {
		copyColumn(data, i);
		decomposition(data, transform, m_iHeight);
		setColumn(transform, i);
	}

	delete[] transform;
	delete[] data;
}

/*
 * 		Inverse Transform
 */
void HaarWaveletTransform::inverseDecompositionStep(double* data, double* inverseTransform, unsigned int size) {
	for(unsigned int i = 0; i < size; ++i) {
		inverseTransform[2 * i] = data[i] + data[size + i];
		inverseTransform[(2 * i) + 1] = data[i] - data[size + i];
	}
}

void HaarWaveletTransform::inverseDecomposition(double* data, double* inverseTransform, unsigned int size) {
	unsigned int currentSize = 1;
	while(currentSize < size) {
		inverseDecompositionStep(data, inverseTransform, currentSize);
		currentSize <<= 1;
		memcpy(data, inverseTransform, sizeof(double) * currentSize);
	}
}

void HaarWaveletTransform::inverseTransform() {
	unsigned int i = 0;
	double* data = new double[m_iHeight];
	double* inverseTransform = new double[m_iHeight];

	for(i = 0; i < m_iWidth; ++i) {
		copyColumn(data, i);
		inverseDecomposition(data, inverseTransform, m_iHeight);
		setColumn(inverseTransform, i);
	}
	delete[] data;

	if(m_iWidth != m_iHeight) {
		delete[] inverseTransform;
		inverseTransform = new double[m_iWidth];
	}

	for(i = 0; i < m_iHeight; ++i) {
		inverseDecomposition(&m_pImageTransform[i * m_iWidth], inverseTransform, m_iWidth);
	}

	delete[] inverseTransform;
}
