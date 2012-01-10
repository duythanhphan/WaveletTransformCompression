/*
 * WaveletDecompressor.h
 *
 *  Created on: 31-12-2011
 *      Author: Michal
 */

#ifndef WAVELETDECOMPRESSOR_H_
#define WAVELETDECOMPRESSOR_H_

#include <string>
#include <fstream>
#include <map>
#include <FreeImage.h>

#include "HaarWaveletTransform.h"
#include "WaveletCompressor.h"

class WaveletDecompressor {
public:
	WaveletDecompressor();
	bool init(const char* inputFilename, const char* outputFilename);
	~WaveletDecompressor();

	bool decompress();

private:
	bool readHeader();
	bool readCodeTable();
	double* allocateTransformMemory();

	void setPixels(int pixelPosition);
	inline void readData(double* transformMemory);
	void decompressRGB(double* transformMemory);

private:
	WaveletTransform* m_pWaveletTransform;
	FIBITMAP* m_pDib;

	std::string m_sOutputFilename;
	std::ifstream m_inputFile;

	unsigned int m_iFileLength;
	WaveletCompressor::Header m_header;

	unsigned int m_iTransformWidth;
	unsigned int m_iTransformHeight;

	std::map<RLE<double>::Run, HuffmanCoding<RLE<double>::Run >::Code > m_codeTable;
};

void WaveletDecompressor::readData(double* transformMemory) {
	unsigned int size = m_pWaveletTransform->getWidth() * m_pWaveletTransform->getHeight() * sizeof(double);
	char* pTransformMemory = (char*)transformMemory;
	m_inputFile.read(pTransformMemory, size);
}

#endif /* WAVELETDECOMPRESSOR_H_ */
