/*
 * HaarWaveletTransform.h
 *
 *  Created on: 29-12-2011
 *      Author: Michal
 */

#ifndef HAARWAVELETTRANSFORM_H_
#define HAARWAVELETTRANSFORM_H_

#include "WaveletTransform.h"

class HaarWaveletTransform : public WaveletTransform {
public:
	HaarWaveletTransform();
	/*
	 * 		Constructor of transform if CopyData is set to true it calls copyData, else it
	 * 		calls setData method.
	 */
	HaarWaveletTransform(double* imageData, unsigned int width, unsigned int height, bool CopyData = true);

	virtual ~HaarWaveletTransform();


	virtual void transform();
	virtual void inverseTransform();

private:
	void decompositionStep(double* data, double* transform, unsigned int size);
	void decomposition(double* data, double* transform, unsigned int size);

	void inverseDecompositionStep(double* data, double* inverseTransform, unsigned int size);
	void inverseDecomposition(double* data, double* inverseTransform, unsigned int size);

	void copyColumn(double* data, unsigned int column);
	void setColumn(double* data, unsigned int column);
};

#endif /* HAARWAVELETTRANSFORM_H_ */
