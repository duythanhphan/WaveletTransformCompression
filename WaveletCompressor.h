/*
 * WavletCompressor.h
 *
 *  Created on: 30-12-2011
 *      Author: Michal
 */

#ifndef WAVLETCOMPRESSOR_H_
#define WAVLETCOMPRESSOR_H_

#include <fstream>

#include "HaarWaveletTransform.h"
#include "Image.h"
#include "Quantizer.h"

class WaveletCompressor {
public:
	enum WaveletType {
		Haar
	};

	struct Header {
		unsigned int ImageWidth, ImageHeight;
		unsigned int BitsPerPixel;
		WaveletType wavletType;
	};

	static const unsigned int HEADER_SIZE;

public:
	WaveletCompressor();
	bool init(const char* inputFilename, const char* outputFilename);
	~WaveletCompressor();

	bool compress(WaveletType waveletType);

private:
	inline void saveHeader(unsigned int BitsPerPixel, WaveletType waveletType);
	inline void transformSaveComponent(double* transformMemory, int pixelPosition, unsigned int dataSize);

	double* allocateTransformMemory();
	void setTransformMemory(double* pTransformMemory, int pixelPosition);
	void quantization(double* pTransformMemory);
	void compressRGB(WaveletType waveletType);

private:
	WaveletTransform* m_pWaveletTransform;
	Image m_image;
	std::ofstream m_outputFile;

	unsigned int m_iImageWidth;
	unsigned int m_iImageHeight;
	unsigned int m_iBytesPerPixel;
};

void WaveletCompressor::saveHeader(unsigned int BitsPerPixel, WaveletType waveletType) {
	Header header;
	header.ImageWidth = FreeImage_GetWidth(m_image.getDib());
	header.ImageHeight = FreeImage_GetHeight(m_image.getDib());
	header.BitsPerPixel = BitsPerPixel;
	header.wavletType = waveletType;

	m_outputFile.write((char*)&header, HEADER_SIZE);
}

void WaveletCompressor::transformSaveComponent(double* transformMemory, int pixelPosition, unsigned int dataSize) {
	setTransformMemory(transformMemory, pixelPosition);
	m_pWaveletTransform->transform();
	quantization(transformMemory);
	m_outputFile.write((char*)transformMemory, dataSize);
}

#endif /* WAVLETCOMPRESSOR_H_ */
