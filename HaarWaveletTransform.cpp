/*
 * HaarWaveletTransform.cpp
 *
 *  Created on: 29-12-2011
 *      Author: Michal
 */

#include <cstring>
#include <cmath>

#include "HaarWaveletTransform.h"

const double HaarWaveletTransform::SQRT2 = sqrt(2.0);

HaarWaveletTransform::HaarWaveletTransform() : WaveletTransform() { }

HaarWaveletTransform::HaarWaveletTransform(double* imageData, unsigned int width, unsigned int height, bool CopyData) :
	WaveletTransform(imageData, width, height, CopyData) {
}

HaarWaveletTransform::~HaarWaveletTransform() { }

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
		transform[i] = (data[2 * i] + data[(2 * i) + 1]) / SQRT2;
		transform[halfSize + i] = (data[2 * i] - data[(2 * i) + 1]) / SQRT2;
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
		inverseTransform[2 * i] = (data[i] + data[size + i]) / SQRT2;
		inverseTransform[(2 * i) + 1] = (data[i] - data[size + i]) / SQRT2;
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
