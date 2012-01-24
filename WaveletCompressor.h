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
#include "D4WaveletTransform.h"
#include "CDF97WaveletTransform.h"
#include "Image.h"
#include "Quantizer.h"
#include "RLE.h"
#include "HuffmanCoding.h"

class WaveletCompressor {
public:
	enum WaveletType {
		NotSet = 0,
		Haar,
		CDF97,
		D4
	};

	struct Header {
		unsigned int ImageWidth, ImageHeight;
		unsigned int BitsPerPixel;
		WaveletType wavletType;
		unsigned int CodeTableSize;
		unsigned int DataSize;
		unsigned int EncodedItems;
		unsigned int QuantizationIntervals[3];
	};

	static const unsigned int HEADER_SIZE;

public:
	WaveletCompressor();
	bool init(const char* inputFilename, const char* outputFilename);
	~WaveletCompressor();

	bool compress(WaveletType waveletType);

	inline void setQuantizationIntervalsY(double Y, double U, double V);

private:
	void saveHeader(std::map<RLE<double>::Run, HuffmanCoding<RLE<double>::Run>::Code >& codeTable,
			unsigned int dataSize, unsigned int encodedItems);

	double* allocateTransformMemory(WaveletTransform* pWaveletTransform);
	void setTransformMemory(double* pTransformMemoryR, double* pTransformMemoryG, double* pTransformMemoryB);

	void quantization(double* pTransformMemoryR, double* pTransformMemoryG, double* pTransformMemoryB);
	void compressRGB();

	void countRuns(RLE<double>::Run* pData, unsigned int size, std::map<RLE<double>::Run, unsigned int >& countTable);
	HuffmanCoding<RLE<double>::Run>::Leaf* getLeafs(
			RLE<double>& rleR, RLE<double>& rleG, RLE<double>& rleB, unsigned int* size);
	void encode();

private:
	WaveletTransform* m_pWaveletTransformY;
	WaveletTransform* m_pWaveletTransformU;
	WaveletTransform* m_pWaveletTransformV;
	Image m_image;
	std::ofstream m_outputFile;
	std::map<double, HuffmanCoding<double>::Code > m_codeTable;

	unsigned int m_iImageWidth;
	unsigned int m_iImageHeight;
	unsigned int m_iBytesPerPixel;

	unsigned int m_iBitsPerPixel;
	WaveletType m_waveletType;

	double m_dQuantizationY;
	double m_dQuantizationU;
	double m_dQuantizationV;
};

void WaveletCompressor::setQuantizationIntervalsY(double Y, double U, double V) {
	m_dQuantizationY = Y;
	m_dQuantizationU = U;
	m_dQuantizationV = V;
}

#endif /* WAVLETCOMPRESSOR_H_ */
