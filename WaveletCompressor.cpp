/*
 * WavletCompressor.cpp
 *
 *  Created on: 30-12-2011
 *      Author: Michal
 */

#include <cstdio>
#include <cstring>

#include "WaveletCompressor.h"
#include "UnsignedInteger.h"

using namespace std;

const unsigned int WaveletCompressor::HEADER_SIZE = sizeof(Header);

WaveletCompressor::WaveletCompressor() :
		m_pWaveletTransform(0),
		m_iImageWidth(0), m_iImageHeight(0),
		m_iBytesPerPixel(0) { }

bool WaveletCompressor::init(const char* inputFilename, const char* outputFilename) {
	m_outputFile.open(outputFilename, ofstream::binary);
	if(!m_outputFile.good()) {
		fprintf(stderr, "Can't open file: [%s] for writing\n", outputFilename);
		return false;
	}

	if(!m_image.load(inputFilename)) {
		fprintf(stderr, "Error loading image: [%s]\n", inputFilename);
		return false;
	}

	return true;
}

WaveletCompressor::~WaveletCompressor() {
	if(m_pWaveletTransform != 0) {
		delete m_pWaveletTransform;
		m_pWaveletTransform = 0;
	}
}

bool WaveletCompressor::compress(WaveletType waveletType) {
	FREE_IMAGE_TYPE imageType = FreeImage_GetImageType(m_image.getDib());

	if(imageType != FIT_BITMAP) {
		fprintf(stderr, "Unsupported image bitmap type.\n");
		return false;
	}

	switch(waveletType) {
	case Haar:
		m_pWaveletTransform = new HaarWaveletTransform();
		break;
	default:
		fprintf(stderr, "Unsupported wavelet type.\n");
		return false;
	}

	unsigned int bitsPerPixel = FreeImage_GetBPP(m_image.getDib());
	m_iBytesPerPixel = bitsPerPixel / 8;
	switch(bitsPerPixel) {
	case 24:
		compressRGB(waveletType);
		break;
	default:
		fprintf(stderr, "Unsupported bits per pixel size.\n");
		return false;
	}

	m_outputFile.close();
	return true;
}

double* WaveletCompressor::allocateTransformMemory() {
	m_iImageWidth = FreeImage_GetWidth(m_image.getDib());
	m_iImageHeight = FreeImage_GetHeight(m_image.getDib());

	unsigned int transformWidth = UnsignedInteger::getClosestPowerOfTwo(m_iImageWidth);
	unsigned int transformHeight = UnsignedInteger::getClosestPowerOfTwo(m_iImageHeight);

	double* transformMemory = new double[transformWidth * transformHeight];
	m_pWaveletTransform->setData(transformMemory, transformWidth, transformHeight);

	return transformMemory;
}

void WaveletCompressor::setTransformMemory(double* pTransformMemory, int pixelPosition) {
	BYTE* bits = 0;
	unsigned int x = 0;
	unsigned int y = 0;
	const size_t bytesToFill = (m_pWaveletTransform->getWidth() - m_iImageWidth) * sizeof(double);

	for(y = 0; y < m_iImageHeight; ++y) {
		bits = FreeImage_GetScanLine(m_image.getDib(), y);

		for(x = 0; x < m_iImageWidth; ++x) {
			pTransformMemory[(y * m_pWaveletTransform->getWidth()) + x] = (double)bits[pixelPosition];

			bits += m_iBytesPerPixel;
		}

		memset(&pTransformMemory[(y * m_pWaveletTransform->getWidth()) + m_iImageWidth], 0, bytesToFill);
	}

	const size_t bytesInTransformRow = m_pWaveletTransform->getWidth() * sizeof(double);
	for(y = m_iImageHeight; y < m_pWaveletTransform->getHeight(); ++y) {
		memset(&pTransformMemory[y * m_pWaveletTransform->getWidth()], 0, bytesInTransformRow);
	}
}

void WaveletCompressor::quantization(double* pTransformMemory) {
	const double step = 1.0 / 8.0;
	for(unsigned int i = 0; i < m_pWaveletTransform->getWidth() * m_pWaveletTransform->getHeight(); ++i) {
		pTransformMemory[i] = Quantizer::getApproximation(pTransformMemory[i], step);
	}
}

void WaveletCompressor::compressRGB(WaveletType waveletType) {
	double* transformMemory = allocateTransformMemory();
	const unsigned int dataSize = m_pWaveletTransform->getWidth() * m_pWaveletTransform->getHeight() * sizeof(double);

	saveHeader(24, waveletType);
	transformSaveComponent(transformMemory, FI_RGBA_RED, dataSize);
	transformSaveComponent(transformMemory, FI_RGBA_GREEN, dataSize);
	transformSaveComponent(transformMemory, FI_RGBA_BLUE, dataSize);
}

void WaveletCompressor::buildCodeTable(double* transformMemory) {
//	m_codeTable.clear();
//	map<double, unsigned int> countTable;
//	map<double, unsigned int>::iterator it;
//
//	for(unsigned int i = 0; i < m_pWaveletTransform->getWidth() * m_pWaveletTransform->getHeight(); ++i) {
//		it = countTable.find(transformMemory[i]);
//		if(it == countTable.end()) {
//			countTable.insert(std::pair<double, unsigned int>(transformMemory[i], 1));
//		} else {
//			it->second += 1;
//		}
//	}

	RLE<double> rle(transformMemory, m_pWaveletTransform->getWidth() * m_pWaveletTransform->getHeight());
	rle.encode();
	RLE<double>::Run* pData = rle.getData();
	ofstream rleLog("rle.txt");
	for(unsigned int i = 0; i < rle.getEncodedDataSize(); ++i) {
		rleLog << pData[i].value << " => " << pData[i].run << endl;
	}
	rleLog.close();
}
