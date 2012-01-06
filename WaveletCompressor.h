/*
 * WavletCompressor.h
 *
 *  Created on: 30-12-2011
 *      Author: Michal
 */

#ifndef WAVLETCOMPRESSOR_H_
#define WAVLETCOMPRESSOR_H_

#include <fstream>
#include <map>

#include "HaarWaveletTransform.h"
#include "Image.h"
#include "Quantizer.h"
#include "RLE.h"
#include "HuffmanCoding.h"

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

	double* allocateTransformMemory(WaveletTransform* pWaveletTransform);
	void setTransformMemory(double* pTransformMemoryR, double* pTransformMemoryG, double* pTransformMemoryB);

	void quantization(double* pTransformMemoryR, double* pTransformMemoryG, double* pTransformMemoryB);
	void compressRGB(WaveletType waveletType);

	void countRuns(RLE<double>::Run* pData, unsigned int size, std::map<RLE<double>::Run, unsigned int >& countTable);
	void buildCodeTable();

private:
	WaveletTransform* m_pWaveletTransformR;
	WaveletTransform* m_pWaveletTransformG;
	WaveletTransform* m_pWaveletTransformB;
	Image m_image;
	std::ofstream m_outputFile;
	std::map<double, HuffmanCoding<double>::Code > m_codeTable;

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

#endif /* WAVLETCOMPRESSOR_H_ */
