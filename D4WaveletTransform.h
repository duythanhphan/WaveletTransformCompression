/*
 * D4WaveletTransform.h
 *
 *  Created on: 22-01-2012
 *      Author: Michal
 */

#ifndef D4WAVELETTRANSFORM_H_
#define D4WAVELETTRANSFORM_H_

#include "WaveletTransform.h"

class D4WaveletTransform : public WaveletTransform {
public:
	D4WaveletTransform();
	virtual ~D4WaveletTransform();

private:
	void decompositionStep(double* data, double* transform, unsigned int size);
	void inverseDecompositionStep(double* data, double* inverseTransform, unsigned int size);

private:
	double h[4];	//scaling function coefficients
	double g[4];	//wavelet function coefficients
};

#endif /* D4WAVELETTRANSFORM_H_ */
