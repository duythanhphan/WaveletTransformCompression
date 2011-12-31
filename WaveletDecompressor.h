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
#include <FreeImage.h>

#include "HaarWaveletTransform.h"
#include "Image.h"

class WaveletDecompressor {
public:
	WaveletDecompressor();
	bool init(const char* inputFilename, const char* outputFilename);
	~WaveletDecompressor();

	bool decompress();

private:
	WaveletTransform* m_pWaveletTransform;
	FIBITMAP* m_pDib;

	std::string m_sOutputFilename;
	std::ifstream m_inputFile;

	unsigned int m_iFileLength;
};

#endif /* WAVELETDECOMPRESSOR_H_ */
