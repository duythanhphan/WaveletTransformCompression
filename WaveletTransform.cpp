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

WaveletTransform::WaveletTransform() : m_pImageTransform(0), m_iWidth(0), m_iHeight(0) { }

WaveletTransform::WaveletTransform(double* imageData, unsigned int width, unsigned int height, bool CopyData) :
	m_pImageTransform(0), m_iWidth(0), m_iHeight(0) {

	if(CopyData) {
		copyData(imageData, width, height);
	} else {
		setData(imageData, width, height);
	}
}

void WaveletTransform::copyData(double* imageData, unsigned int width, unsigned int height) {
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

void WaveletTransform::setData(double* imageData, unsigned int width, unsigned int height) {
	assert(width == UnsignedInteger::getClosestPowerOfTwo(width) &&
			"WaveletTransform::init width must be power of two");
	assert(height == UnsignedInteger::getClosestPowerOfTwo(height) &&
			"WaveletTransform::init height must be power of two");

	m_pImageTransform = imageData;
	m_iWidth = width;
	m_iHeight = height;
}

WaveletTransform::~WaveletTransform() {
	cleanup();
}

void WaveletTransform::cleanup() {
	if(m_pImageTransform != 0) {
		delete[] m_pImageTransform;
		m_pImageTransform = 0;
		m_iWidth = 0;
		m_iHeight = 0;
	}
}

void WaveletTransform::copyColumn(double* data, unsigned int column, unsigned int size) {
	for(unsigned int i = 0; i < size; ++i) {
		data[i] = m_pImageTransform[column + (i * m_iWidth)];
	}
}

void WaveletTransform::setColumn(double* data, unsigned int column, unsigned int size) {
	for(unsigned int i = 0; i < size; ++i) {
		m_pImageTransform[column + (i * m_iWidth)] = data[i];
	}
}

/*
 * 		Transform
 */
void WaveletTransform::decomposition(double* data, double* transform, unsigned int size) {
	while(size > 1) {
		decompositionStep(data, transform, size);
		memcpy(data, transform, sizeof(double) * size);
		size /= 2;
	}
}

void WaveletTransform::transform() {
	unsigned int i = 0;
	double* transform = new double[m_iWidth];
	double* data = new double[m_iHeight];

	unsigned int size = m_iWidth;

	while(size > 1) {
		for(i = 0; i < m_iHeight; ++i) {
			decompositionStep(&m_pImageTransform[i * m_iWidth], transform, size);
			memcpy(&m_pImageTransform[i * m_iWidth], transform, sizeof(double) * size);
		}

		for(i = 0; i < m_iWidth; ++i) {
			copyColumn(data, i, size);

			decompositionStep(data, transform, size);
			//memcpy(data, transform, sizeof(double) * size);

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
void WaveletTransform::inverseDecomposition(double* data, double* inverseTransform, unsigned int size) {
	unsigned int currentSize = 1;
	while(currentSize < size) {
		inverseDecompositionStep(data, inverseTransform, currentSize);
		currentSize <<= 1;
		memcpy(data, inverseTransform, sizeof(double) * currentSize);
	}
}

void WaveletTransform::inverseTransform() {
	unsigned int i = 0;
	double* data = new double[m_iHeight];
	double* inverseTransform = new double[m_iHeight];

	unsigned int currentSize = 1;

	while(currentSize < m_iHeight) {
		for(i = 0; i < m_iWidth; ++i) {
			copyColumn(data, i, currentSize * 2);

			inverseDecompositionStep(data, inverseTransform, currentSize);
			//memcpy(data, inverseTransform, sizeof(double) * currentSize);

			setColumn(inverseTransform, i, currentSize * 2);
		}

		for(i = 0; i < m_iHeight; ++i) {
			inverseDecompositionStep(&m_pImageTransform[i * m_iWidth], inverseTransform, currentSize);
			memcpy(&m_pImageTransform[i * m_iWidth], inverseTransform, sizeof(double) * currentSize * 2);
		}

		currentSize <<= 1;
	}


	delete[] data;
	delete[] inverseTransform;
}


