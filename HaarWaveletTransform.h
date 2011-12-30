/*
 * HaarWaveletTransform.h
 *
 *  Created on: 29-12-2011
 *      Author: Michal
 */

#ifndef HAARWAVELETTRANSFORM_H_
#define HAARWAVELETTRANSFORM_H_

class HaarWaveletTransform {
public:
	HaarWaveletTransform();
	HaarWaveletTransform(double* imageData, unsigned int width, unsigned int height);
	~HaarWaveletTransform();

	void transform();
	void inverseTransform();

	inline unsigned int getWidth();
	inline unsigned int getHeight();
	inline double getItem(unsigned int row, unsigned int column);

private:
	void decompositionStep(double* data, double* transform, unsigned int size);
	void decomposition(double* data, double* transform, unsigned int size);

	void inverseDecompositionStep(double* data, double* inverseTransform, unsigned int size);
	void inverseDecomposition(double* data, double* inverseTransform, unsigned int size);

	void copyColumn(double* data, unsigned int column);
	void setColumn(double* data, unsigned int column);

private:
	double* m_pImageTransform;
	unsigned int m_iWidth;
	unsigned int m_iHeight;
};

unsigned int HaarWaveletTransform::getWidth() {
	return m_iWidth;
}

unsigned int HaarWaveletTransform::getHeight() {
	return m_iHeight;
}

double HaarWaveletTransform::getItem(unsigned int row, unsigned int column) {
	return m_pImageTransform[(row * m_iWidth) + column];
}

#endif /* HAARWAVELETTRANSFORM_H_ */
