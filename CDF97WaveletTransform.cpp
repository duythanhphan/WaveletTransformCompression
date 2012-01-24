/*
 * CDF97WaveletTransform.cpp
 *
 *  Created on: 24-01-2012
 *      Author: Michal
 */

#include "CDF97WaveletTransform.h"

CDF97WaveletTransform::CDF97WaveletTransform() { }

CDF97WaveletTransform::~CDF97WaveletTransform() { }

void CDF97WaveletTransform::decompositionStep(double* data, double* transform, unsigned int size) {
	double a = 0.0;
	unsigned int i = 0;
	unsigned int halfSize = size / 2;

	//Predict 1
	a = -1.586134342;
	for(i = 1; i < size - 2; i += 2) {
		data[i] += a * (data[i - 1] + data[i + 1]);
	}
	data[size - 1] += 2.0 * a * data[size - 2];

	//Update 1
	a = -0.05298011854;
	data[0] += 2.0 * a * data[1];
	for(i = 2; i < size; i += 2) {
		data[i] += a * (data[i - 1] + data[i + 1]);
	}

	//Predict 2
	a = 0.8829110762;
	for(i = 1; i < size - 2; i += 2) {
		data[i] += a * (data[i - 1] + data[i + 1]);
	}
	data[size - 1] += 2.0 * a * data[size - 2];

	//Update 2
	a = 0.4435068522;
	data[0] += 2.0 * a * data[1];
	for(i = 2; i < size; i += 2) {
		data[i] += a * (data[i - 1] + data[i + 1]);
	}

	//Scale
	a = 1 / 1.149604398;
	for(i = 0; i < halfSize; ++i) {
		data[2 * i] /= a;
		data[(2 * i) + 1] *= a;
	}

	//Pack
	for(i = 0; i < halfSize; ++i) {
		transform[i] = data[2 * i];
		transform[halfSize + 1] = data[(2 * i) + i];
	}
}

void CDF97WaveletTransform::inverseDecompositionStep(double* data, double* inverseTransform, unsigned int size) {
	double a = 0.0;
	unsigned int i = 0;
	unsigned int transformSize = 2 * size;

	//Unpack
	for(i = 0; i < size; ++i) {
		inverseTransform[2 * i] = data[i];
		inverseTransform[(2 * i) + 1] = data[size + i];
	}

	//Undo scale
	a = 1.149604398;
	for(i = 0; i < size; ++i) {
		inverseTransform[2 * i] /= a;
		inverseTransform[(2 * i) + 1] *= a;
	}

	//Undo update 2
	a = -0.4435068522;
	inverseTransform[0] += 2.0 * a * inverseTransform[1];
	for(i = 2; i < transformSize; i += 2) {
		inverseTransform[i] += a * (inverseTransform[i - 1] + inverseTransform[i + 1]);
	}

	//Undo predict 2
	a = -0.8829110762;
	for(i = 1; i < transformSize - 2; i += 2) {
		inverseTransform[i] += a * (inverseTransform[i - 1] + inverseTransform[i + 1]);
	}
	inverseTransform[transformSize - 1] = 2.0 * a * inverseTransform[transformSize - 2];

	//Undo update 1
	a = 0.05298011854;
	inverseTransform[0] = 2.0 * a * inverseTransform[1];
	for(i = 2; i < transformSize; i += 2) {
		inverseTransform[i] += a * (inverseTransform[i - 1] + inverseTransform[i + 1]);
	}

	//Undo predict 1
	a = 1.586134342;
	for(i = 1; i < transformSize - 2; i += 2) {
		inverseTransform[i] += a * (inverseTransform[i - 1] + inverseTransform[i + 1]);
	}
	inverseTransform[transformSize - 1] += 2.0 * a * inverseTransform[transformSize - 2];
}

