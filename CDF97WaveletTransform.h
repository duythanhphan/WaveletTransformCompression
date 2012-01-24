/*
 * CDF97WaveletTransform.h
 *
 *  Created on: 24-01-2012
 *      Author: Michal
 */

#ifndef CDF97WAVELETTRANSFORM_H_
#define CDF97WAVELETTRANSFORM_H_

#include "WaveletTransform.h"

class CDF97WaveletTransform: public WaveletTransform {
public:
	CDF97WaveletTransform();
	virtual ~CDF97WaveletTransform();

protected:
	virtual void decompositionStep(double* data, double* transform, unsigned int size);
	virtual void inverseDecompositionStep(double* data, double* inverseTransform, unsigned int size);
};

#endif /* CDF97WAVELETTRANSFORM_H_ */
