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
		m_pWaveletTransformR(0), m_pWaveletTransformG(0), m_pWaveletTransformB(0),
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
	if(m_pWaveletTransformR != 0) {
		delete m_pWaveletTransformR;
		m_pWaveletTransformR = 0;
	}
	if(m_pWaveletTransformR != 0) {
		delete m_pWaveletTransformG;
		m_pWaveletTransformG = 0;
	}
	if(m_pWaveletTransformR != 0) {
		delete m_pWaveletTransformB;
		m_pWaveletTransformB = 0;
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
		m_pWaveletTransformR = new HaarWaveletTransform();
		m_pWaveletTransformG = new HaarWaveletTransform();
		m_pWaveletTransformB = new HaarWaveletTransform();
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

double* WaveletCompressor::allocateTransformMemory(WaveletTransform* pWaveletTransform) {
	m_iImageWidth = FreeImage_GetWidth(m_image.getDib());
	m_iImageHeight = FreeImage_GetHeight(m_image.getDib());

	unsigned int transformWidth = UnsignedInteger::getClosestPowerOfTwo(m_iImageWidth);
	unsigned int transformHeight = UnsignedInteger::getClosestPowerOfTwo(m_iImageHeight);

	double*	pMemory = new double[transformWidth * transformHeight];
	pWaveletTransform->setData(pMemory, transformWidth, transformHeight);

	return pMemory;
}

void WaveletCompressor::setTransformMemory(
		double* pTransformMemoryR, double* pTransformMemoryG, double* pTransformMemoryB) {
	BYTE* bits = 0;
	unsigned int x = 0;
	unsigned int y = 0;
	unsigned int i = 0;
	const size_t bytesToFill = (m_pWaveletTransformR->getWidth() - m_iImageWidth) * sizeof(double);

	for(y = 0; y < m_iImageHeight; ++y) {
		bits = FreeImage_GetScanLine(m_image.getDib(), y);

		for(x = 0; x < m_iImageWidth; ++x) {
			i = (y * m_pWaveletTransformR->getWidth()) + x;
			pTransformMemoryR[i] = (double)bits[FI_RGBA_RED];
			pTransformMemoryG[i] = (double)bits[FI_RGBA_GREEN];
			pTransformMemoryB[i] = (double)bits[FI_RGBA_BLUE];

			bits += m_iBytesPerPixel;
		}

		i = (y * m_pWaveletTransformR->getWidth()) + m_iImageWidth;
		memset(&pTransformMemoryR[i], 0, bytesToFill);
		memset(&pTransformMemoryG[i], 0, bytesToFill);
		memset(&pTransformMemoryB[i], 0, bytesToFill);
	}

	const size_t bytesInTransformRow = m_pWaveletTransformR->getWidth() * sizeof(double);
	for(y = m_iImageHeight; y < m_pWaveletTransformR->getHeight(); ++y) {
		i = y * m_pWaveletTransformR->getWidth();
		memset(&pTransformMemoryR[i], 0, bytesInTransformRow);
		memset(&pTransformMemoryG[i], 0, bytesInTransformRow);
		memset(&pTransformMemoryB[i], 0, bytesInTransformRow);
	}
}

void WaveletCompressor::quantization(double* pTransformMemoryR, double* pTransformMemoryG, double* pTransformMemoryB) {
	const double step = 1.0 / 16.0;
	for(unsigned int i = 0; i < m_pWaveletTransformR->getWidth() * m_pWaveletTransformR->getHeight(); ++i) {
		pTransformMemoryR[i] = Quantizer::getApproximation(pTransformMemoryR[i], step);
		pTransformMemoryG[i] = Quantizer::getApproximation(pTransformMemoryG[i], step);
		pTransformMemoryB[i] = Quantizer::getApproximation(pTransformMemoryB[i], step);
	}
}

void WaveletCompressor::compressRGB(WaveletType waveletType) {
	double* transformMemoryR = 0;
	double* transformMemoryG = 0;
	double* transformMemoryB = 0;
	transformMemoryR = allocateTransformMemory(m_pWaveletTransformR);
	transformMemoryG = allocateTransformMemory(m_pWaveletTransformG);
	transformMemoryB = allocateTransformMemory(m_pWaveletTransformB);
	const unsigned int dataSize = m_pWaveletTransformR->getWidth() * m_pWaveletTransformR->getHeight() * sizeof(double);

	saveHeader(24, waveletType);

	setTransformMemory(transformMemoryR, transformMemoryG, transformMemoryB);
	m_pWaveletTransformR->transform();
	m_pWaveletTransformG->transform();
	m_pWaveletTransformB->transform();

	quantization(transformMemoryR, transformMemoryG, transformMemoryB);

	buildCodeTable();

	m_outputFile.write((char*)transformMemoryR, dataSize);
	m_outputFile.write((char*)transformMemoryG, dataSize);
	m_outputFile.write((char*)transformMemoryB, dataSize);
}

void countRuns(RLE<double>::Run* pData, unsigned int size, std::map<RLE<double>::Run, unsigned int >& countTable) {
	map<RLE<double>::Run, unsigned int>::iterator it;

	for(unsigned int i = 0; i < size; ++i) {
		it = countTable.find(pData[i]);
		if(it == countTable.end()) {
			countTable.insert(std::pair<RLE<double>::Run, unsigned int>(pData[i], 1));
		} else {
			it->second += 1;
		}
	}
}

void WaveletCompressor::buildCodeTable() {
	const unsigned int size = m_pWaveletTransformR->getWidth() * m_pWaveletTransformR->getHeight();
	RLE<double> rleR(m_pWaveletTransformR->getTransfomrMemory(), size);
	rleR.encode();
	RLE<double> rleG(m_pWaveletTransformG->getTransfomrMemory(), size);
	rleG.encode();
	RLE<double> rleB(m_pWaveletTransformR->getTransfomrMemory(), size);
	rleB.encode();

	map<RLE<double>::Run, unsigned int> countTable;
	countRuns(rleR.getData(), rleR.getEncodedDataSize(), countTable);
	countRuns(rleG.getData(), rleG.getEncodedDataSize(), countTable);
	countRuns(rleB.getData(), rleB.getEncodedDataSize(), countTable);

	//map<RLE<double>::Run, unsigned int>::iterator it;

	//ofstream rleOut("rleR.txt");

//	for(unsigned int i = 0; i < rleR.getEncodedDataSize(); ++i) {
//		//rleOut << pData[i].value << " => " << pData[i].run << endl;
//		it = countTable.find(pData[i]);
//		if(it == countTable.end()) {
//			countTable.insert(std::pair<RLE<double>::Run, unsigned int>(pData[i], 1));
//		} else {
//			it->second += 1;
//		}
//	}

	//rleOut.close();

	map<RLE<double>::Run, unsigned int>::iterator it;
	ofstream log("countTableRuns.txt");
	for(it = countTable.begin(); it != countTable.end(); ++it) {
		log << "(" << it->first.value << ", " << it->first.run << ") => " << it->second << endl;
	}
	log.close();
}
