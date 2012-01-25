/*
 * CDF97WaveletTransform.cpp
 *
 *  Created on: 24-01-2012
 *      Author: Michal
 */

#include "CDF97WaveletTransform.h"
#include <cstring>
#include <cmath>

const double CDF97WaveletTransform::a1 = -1.586134342;
const double CDF97WaveletTransform::a2 = -0.05298011854;
const double CDF97WaveletTransform::a3 = 0.8829110762;
const double CDF97WaveletTransform::a4 = 0.4435068522;

const double CDF97WaveletTransform::k1 = 1.149604398;//0.81289306611596146;
const double CDF97WaveletTransform::k2 = 1 / 1.149604398;//0.61508705245700002;

const double CDF97WaveletTransform::invA1 = -0.4435068522;
const double CDF97WaveletTransform::invA2 = -0.8829110762;
const double CDF97WaveletTransform::invA3 = 0.05298011854;
const double CDF97WaveletTransform::invA4 = 1.586134342;

const double CDF97WaveletTransform::invK1 = 1 / 1.149604398;//1.230174104914;
const double CDF97WaveletTransform::invK2 = 1.149604398;//1.6257861322319229;

CDF97WaveletTransform::CDF97WaveletTransform() { }

CDF97WaveletTransform::~CDF97WaveletTransform() { }

void CDF97WaveletTransform::decompositionStep(double* data, double* transform, unsigned int size) {
	unsigned int i = 0;
	unsigned int halfSize = size / 2;

	//Predict 1
	for(i = 1; i < size - 2; i += 2) {
		data[i] += a1 * (data[i - 1] + data[i + 1]);
	}
	data[size - 1] += 2.0 * a1 * data[size - 2];

	//Update 1
	data[0] += 2.0 * a2 * data[1];
	for(i = 2; i < size; i += 2) {
		data[i] += a2 * (data[i - 1] + data[i + 1]);
	}

	//Predict 2
	for(i = 1; i < size - 2; i += 2) {
		data[i] += a3 * (data[i - 1] + data[i + 1]);
	}
	data[size - 1] += 2.0 * a3 * data[size - 2];

	//Update 2
	data[0] += 2.0 * a4 * data[1];
	for(i = 2; i < size; i += 2) {
		data[i] += a4 * (data[i - 1] + data[i + 1]);
	}

	//Scale
	for(i = 0; i < halfSize; ++i) {
		data[2 * i] *= k1;
		data[(2 * i) + 1] *= k2;
	}

	//Pack
	for(i = 0; i < halfSize; ++i) {
		transform[i] = data[2 * i];
		transform[halfSize + i] = data[(2 * i) + 1];
	}
}

void CDF97WaveletTransform::inverseDecompositionStep(double* data, double* inverseTransform, unsigned int size) {
	unsigned int i = 0;
	unsigned int transformSize = 2 * size;

	//Unpack
	for(i = 0; i < size; ++i) {
		inverseTransform[2 * i] = data[i];
		inverseTransform[(2 * i) + 1] = data[size + i];
	}

	//Undo scale
	for(i = 0; i < size; ++i) {
		inverseTransform[2 * i] *= invK1;
		inverseTransform[(2 * i) + 1] *= invK2;
	}

	//Undo update 2
	inverseTransform[0] += 2.0 * invA1 * inverseTransform[1];
	for(i = 2; i < transformSize; i += 2) {
		inverseTransform[i] += invA1 * (inverseTransform[i - 1] + inverseTransform[i + 1]);
	}

	//Undo predict 2
	for(i = 1; i < transformSize - 2; i += 2) {
		inverseTransform[i] += invA2 * (inverseTransform[i - 1] + inverseTransform[i + 1]);
	}
	inverseTransform[transformSize - 1] = 2.0 * invA2 * inverseTransform[transformSize - 2];

	//Undo update 1
	inverseTransform[0] = 2.0 * invA3 * inverseTransform[1];
	for(i = 2; i < transformSize; i += 2) {
		inverseTransform[i] += invA3 * (inverseTransform[i - 1] + inverseTransform[i + 1]);
	}

	//Undo predict 1
	for(i = 1; i < transformSize - 2; i += 2) {
		inverseTransform[i] += invA4 * (inverseTransform[i - 1] + inverseTransform[i + 1]);
	}
	inverseTransform[transformSize - 1] += 2.0 * invA4 * inverseTransform[transformSize - 2];
}

