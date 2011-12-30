/*
 * Image.cpp
 *
 *  Created on: 23-10-2011
 *      Author: Michal
 */

#include "Image.h"

Image::Image() : m_pDib(0), m_fif(FIF_UNKNOWN) { }

Image::Image(FIBITMAP* dib, FREE_IMAGE_FORMAT fif) : m_pDib(dib), m_fif(fif) { }

Image::~Image() {
	FreeImage_Unload(m_pDib);
}

bool Image::checkFileFormat(const char* fileName) {
	//check file signature and deduct it format
	m_fif = FreeImage_GetFileType(fileName, 0);

	//if still unknown check extension
	if(m_fif == FIF_UNKNOWN) {
		m_fif = FreeImage_GetFIFFromFilename(fileName);
	}
	if(m_fif == FIF_UNKNOWN) {
		return false;
	}

	//check if FreeImage can read this type of file
	if(FreeImage_FIFSupportsReading(m_fif)) {
		return true;
	} else {
		return false;
	}
}


bool Image::load(const char* fileName) {
	if(!checkFileFormat(fileName)) {
		return false;
	}

	m_pDib = FreeImage_Load(m_fif, fileName);
	if(!m_pDib) {
		return false;
	}

	if(FreeImage_GetBits(m_pDib) == 0 || (FreeImage_GetWidth(m_pDib) == 0) || (FreeImage_GetHeight(m_pDib) == 0)) {
		return false;
	}

	return true;
}

void Image::convertToGreyScale() {
	FIBITMAP* greyScaleImg = FreeImage_ConvertToGreyscale(m_pDib);
	FreeImage_Unload(m_pDib);
	m_pDib = greyScaleImg;
}

void Image::convertToMonochrome() {
	FIBITMAP* monochromeImg = FreeImage_Dither(m_pDib, FID_FS);
	FreeImage_Unload(m_pDib);
	m_pDib = monochromeImg;
}

void Image::save(const char* filename) {
	FreeImage_Save(m_fif, m_pDib, filename);
}
