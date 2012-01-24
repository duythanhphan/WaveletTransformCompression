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
#include "D4WaveletTransform.h"
#include "WaveletCompressor.h"

class WaveletDecompressor {
public:
	WaveletDecompressor();
	bool init(const char* inputFilename, const char* outputFilename);
	~WaveletDecompressor();

	bool decompress();

private:
	bool readHeader();
	bool checkFileLength();
	bool readCodeTable();
	double* allocateTransformMemory(WaveletTransform* pWaveletTransform);

	void setPixels();
	inline void readData(double* transformMemory);
	void decompressRGB();

	void decode();
	void inverseQuantization();
	BYTE clamp(double d);

private:
	WaveletTransform* m_pWaveletTransformY;
	WaveletTransform* m_pWaveletTransformU;
	WaveletTransform* m_pWaveletTransformV;
	FIBITMAP* m_pDib;

	std::string m_sOutputFilename;
	std::ifstream m_inputFile;

	unsigned int m_iFileLength;
	WaveletCompressor::Header m_header;

	unsigned int m_iTransformWidth;
	unsigned int m_iTransformHeight;

	std::map<HuffmanCoding<RLE<double>::Run >::Code, RLE<double>::Run > m_codeTable;
};

void WaveletDecompressor::readData(double* transformMemory) {
	unsigned int size = m_pWaveletTransformY->getSize() * m_pWaveletTransformY->getSize() * sizeof(double);
	char* pTransformMemory = (char*)transformMemory;
	m_inputFile.read(pTransformMemory, size);
}

#endif /* WAVELETDECOMPRESSOR_H_ */
