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
		Haar,
		NotSet
	};

	struct Header {
		unsigned int ImageWidth, ImageHeight;
		unsigned int BitsPerPixel;
		WaveletType wavletType;
		unsigned int CodeTableSize;
		unsigned int DataSize;
	};

	static const unsigned int HEADER_SIZE;

public:
	WaveletCompressor();
	bool init(const char* inputFilename, const char* outputFilename);
	~WaveletCompressor();

	bool compress(WaveletType waveletType);

private:
	void saveHeader(std::map<RLE<double>::Run, HuffmanCoding<RLE<double>::Run>::Code >& codeTable,
			unsigned int dataSize);

	double* allocateTransformMemory(WaveletTransform* pWaveletTransform);
	void setTransformMemory(double* pTransformMemoryR, double* pTransformMemoryG, double* pTransformMemoryB);

	void quantization(double* pTransformMemoryR, double* pTransformMemoryG, double* pTransformMemoryB);
	void compressRGB();

	void countRuns(RLE<double>::Run* pData, unsigned int size, std::map<RLE<double>::Run, unsigned int >& countTable);
	HuffmanCoding<RLE<double>::Run>::Leaf* getLeafs(
			RLE<double>& rleR, RLE<double>& rleG, RLE<double>& rleB, unsigned int* size);
	void encode();

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

	unsigned int m_iBitsPerPixel;
	WaveletType m_waveletType;
};

#endif /* WAVLETCOMPRESSOR_H_ */
