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

private:
	const static double a1;
	const static double a2;
	const static double a3;
	const static double a4;

	const static double k1;
	const static double k2;

	const static double invA1;
	const static double invA2;
	const static double invA3;
	const static double invA4;

	const static double invK1;
	const static double invK2;
};

#endif /* CDF97WAVELETTRANSFORM_H_ */
