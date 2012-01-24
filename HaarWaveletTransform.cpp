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

HaarWaveletTransform::HaarWaveletTransform(double* imageData, unsigned int size, bool CopyData) :
	WaveletTransform(imageData, size, CopyData) {
}

HaarWaveletTransform::~HaarWaveletTransform() { }

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

/*
 * 		Inverse Transform
 */
void HaarWaveletTransform::inverseDecompositionStep(double* data, double* inverseTransform, unsigned int size) {
	for(unsigned int i = 0; i < size; ++i) {
		inverseTransform[2 * i] = (data[i] + data[size + i]) / SQRT2;
		inverseTransform[(2 * i) + 1] = (data[i] - data[size + i]) / SQRT2;
	}
}
