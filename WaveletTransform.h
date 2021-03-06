/*
 * IWavelet.h
 *
 *  Created on: 30-12-2011
 *      Author: Michal
 */

#ifndef IWAVELET_H_
#define IWAVELET_H_

class WaveletTransform {
public:
	void transform();
	void inverseTransform();

	WaveletTransform();
	/*
	 * 		Constructor of transform if CopyData is set to true it calls copyData, else it
	 * 		calls setData method.
	 */
	WaveletTransform(double* imageData, unsigned int size, bool CopyData = true);

	/*
	 * 		Initialize wavelet transform data. This function don't copy data just assign
	 * 		pointer and image size, from this time pointer imageData belongs to class instance
	 * 		and will be deleted on destructor call. Width and height must be powers of two,
	 * 		and describe size of data. This function don't cleanup allocated memory, if
	 * 		transform contains data call before this method cleanup method.
	 */
	void setData(double* imageData, unsigned int size);
	/*
	 * 		Initialize wavelet transform data. This function copy data using pointer imageData.
	 * 		Width and height don't have to be powers of two, it must describe only size of input data.
	 * 		If width or height isn't powers of two function will calculate closest power of two and
	 * 		fill empty space with 0. This function cleans up previous allocated memory.
	 */
	void copyData(double* imageData, unsigned int size);

	virtual ~WaveletTransform();
	void cleanup();

	inline unsigned int getSize() const;
	inline double getItem(unsigned int row, unsigned int column);
	inline double* getTransformMemory();

protected:
	virtual void decompositionStep(double* data, double* transform, unsigned int size) = 0;
	void decomposition(double* data, double* transform, unsigned int size);

	virtual void inverseDecompositionStep(double* data, double* inverseTransform, unsigned int size) = 0;
	void inverseDecomposition(double* data, double* inverseTransform, unsigned int size);

	void copyColumn(double* data, unsigned int column, unsigned int size);
	void setColumn(double* data, unsigned int column, unsigned int size);

protected:
	double* m_pImageTransform;
	unsigned int m_iSize;
};

unsigned int WaveletTransform::getSize() const {
	return m_iSize;
}

double WaveletTransform::getItem(unsigned int row, unsigned int column) {
	return m_pImageTransform[(row * m_iSize) + column];	//m_iSize is equals to width and height
}

double* WaveletTransform::getTransformMemory() {
	return m_pImageTransform;
}

#endif /* IWAVELET_H_ */
