/*
 * D4WaveletTransform.cpp
 *
 *  Created on: 22-01-2012
 *      Author: Michal
 */

#include <cmath>
#include <cstring>
#include "D4WaveletTransform.h"

D4WaveletTransform::D4WaveletTransform() {
	h[0] = (1.0 + sqrt(3.0)) / (4.0 * sqrt(2.0));
	h[1] = (3.0 + sqrt(3.0)) / (4.0 * sqrt(2.0));
	h[2] = (3.0 - sqrt(3.0)) / (4.0 * sqrt(2.0));
	h[3] = (1.0 - sqrt(3.0)) / (4.0 * sqrt(2.0));

	g[0] = h[3];
	g[1] = -h[2];
	g[2] = h[1];
	g[3] = -h[0];
}

D4WaveletTransform::~D4WaveletTransform() { }

/*
 * 		Transform
 */
void D4WaveletTransform::decompositionStep(double* data, double* transform, unsigned int size) {
	const unsigned int halfSize = size / 2;

	unsigned int i = 0;
	for(i = 0; i < halfSize - 1; ++i) {

		transform[i] =
				(h[0] * data[2*i]) + (h[1] * data[(2*i)+1]) +
				(h[2] * data[(2*i)+2]) + (h[3] * data[(2*i)+3]);

		transform[halfSize + i] =
				(g[0] * data[2*i]) + (g[1] * data[(2*i)+1]) +
				(g[2] * data[(2*i)+2]) + (g[3] * data[(2*i)+3]);
	}

	transform[i] = (h[0] * data[2*i]) + (h[1] * data[(2*i)+1]) +
			(h[2] * data[0]) + (h[3] * data[1]);

	transform[halfSize + i] = (g[0] * data[2*i]) + (g[1] * data[(2*i)+1]) +
			(g[2] * data[0]) + (g[3] * data[1]);
}

/*
 * 		Inverse Transform
 */
void D4WaveletTransform::inverseDecompositionStep(double* data, double* inverseTransform, unsigned int size) {
	inverseTransform[0] = (h[2] * data[size - 1]) + (g[2] * data[(2*size) - 1]) +
			(h[0] * data[0]) + (g[0] * data[size]);

	inverseTransform[1] = (h[3] * data[size - 1]) + (g[3] * data[(2*size) - 1]) +
			(h[1] * data[0]) + (g[1] * data[size]);

	for(unsigned int i = 0; i < size - 1; ++i) {

		inverseTransform[2 * i] = (h[2] * data[i]) + (g[2] * data[size + i]) +
				(h[0] * data[i + 1]) + (g[0] * data[size + i + 1]);

		inverseTransform[(2 * i) + 1] = (h[3] * data[i]) + (g[3] * data[size + i]) +
				(h[1] * data[i + 1]) + (g[1] * data[size + i + 1]);
	}
}
