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
	virtual void transform() = 0;
	virtual void inverseTransform() = 0;

	WaveletTransform();
	/*
	 * 		Constructor of transform if CopyData is set to true it calls copyData, else it
	 * 		calls setData method.
	 */
	WaveletTransform(double* imageData, unsigned int width, unsigned int height, bool CopyData = true);

	/*
	 * 		Initialize wavelet transform data. This function don't copy data just assign
	 * 		pointer and image size, from this time pointer imageData belongs to class instance
	 * 		and will be deleted on destructor call. Width and height must be powers of two,
	 * 		and describe size of data. This function don't cleanup allocated memory, if
	 * 		transform contains data call before this method cleanup method.
	 */
	void setData(double* imageData, unsigned int width, unsigned int height);
	/*
	 * 		Initialize wavelet transform data. This function copy data using pointer imageData.
	 * 		Width and height don't have to be powers of two, it must describe only size of input data.
	 * 		If width or height isn't powers of two function will calculate closest power of two and
	 * 		fill empty space with 0. This function cleans up previous allocated memory.
	 */
	void copyData(double* imageData, unsigned int width, unsigned int height);

	virtual ~WaveletTransform();
	void cleanup();

	inline unsigned int getWidth();
	inline unsigned int getHeight();
	inline double getItem(unsigned int row, unsigned int column);

protected:
	double* m_pImageTransform;
	unsigned int m_iWidth;
	unsigned int m_iHeight;
};

unsigned int WaveletTransform::getWidth() {
	return m_iWidth;
}

unsigned int WaveletTransform::getHeight() {
	return m_iHeight;
}

double WaveletTransform::getItem(unsigned int row, unsigned int column) {
	return m_pImageTransform[(row * m_iWidth) + column];
}

#endif /* IWAVELET_H_ */
