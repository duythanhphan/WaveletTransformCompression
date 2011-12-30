/*
 * Image.h
 *
 *  Created on: 23-10-2011
 *      Author: Michal
 */

#ifndef IMAGE_H_
#define IMAGE_H_

#include <FreeImage.h>

class Image {
private:
	//Free image data variables
	FIBITMAP* m_pDib;			//pointer to image
	FREE_IMAGE_FORMAT m_fif;	//image format

private:
	bool checkFileFormat(const char* fileName);
public:
	Image();
	Image(FIBITMAP* dib, FREE_IMAGE_FORMAT fif);
	~Image();

	bool load(const char* fileName);
	void convertToGreyScale();
	void convertToMonochrome();

	void save(const char* filename = "output.png");

	inline FIBITMAP* getDib() { return m_pDib; }
	inline FREE_IMAGE_FORMAT getFif() { return m_fif; }
};

#endif /* IMAGE_H_ */
