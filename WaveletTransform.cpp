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


