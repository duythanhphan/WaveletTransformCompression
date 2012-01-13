/*
 * WaveletDecompressor.cpp
 *
 *  Created on: 31-12-2011
 *      Author: Michal
 */

#include <cstdio>
#include <iostream>

#include "WaveletDecompressor.h"
#include "UnsignedInteger.h"

using namespace std;

WaveletDecompressor::WaveletDecompressor() :
		m_pWaveletTransformR(0), m_pWaveletTransformG(0), m_pWaveletTransformB(0),
		m_pDib(0), m_iFileLength(0) { }

bool WaveletDecompressor::init(const char* inputFilename, const char* outputFilename) {
	m_inputFile.open(inputFilename, ifstream::binary);
	if(!m_inputFile.good()) {
		fprintf(stderr, "Can't open file [%s] for reading\n", inputFilename);
		return false;
	}

	m_inputFile.seekg(0, ios::end);
	m_iFileLength = m_inputFile.tellg();
	m_inputFile.seekg(0, ios::beg);

	if(m_iFileLength < WaveletCompressor::HEADER_SIZE) {
		fprintf(stderr, "Input file is corrupted, it's size is less than size of file header.\n");
		return false;
	}

	m_sOutputFilename.assign(outputFilename);

	return true;
}

WaveletDecompressor::~WaveletDecompressor() {
	if(m_pWaveletTransformR != 0) {
		delete m_pWaveletTransformR;
		m_pWaveletTransformR = 0;
	}
	if(m_pWaveletTransformG != 0) {
		delete m_pWaveletTransformG;
		m_pWaveletTransformG = 0;
	}
	if(m_pWaveletTransformB != 0) {
		delete m_pWaveletTransformB;
		m_pWaveletTransformB = 0;
	}
	if(m_pDib != 0) {
		FreeImage_Unload(m_pDib);
		m_pDib = 0;
	}
}

bool WaveletDecompressor::readHeader() {
	WaveletCompressor::Header* pHeader = &m_header;
	m_inputFile.read((char*)pHeader, WaveletCompressor::HEADER_SIZE);

	return true;
}

bool WaveletDecompressor::readCodeTable() {
	RLE<double>::Run run;
	HuffmanCoding<RLE<double>::Run >::Code code;
	typedef pair<HuffmanCoding<RLE<double>::Run >::Code, RLE<double>::Run > CodeTablePair;

	for(unsigned int i = 0; i < m_header.CodeTableSize; ++i) {
		m_inputFile.read((char*)&run.value, sizeof(double));
		m_inputFile.read((char*)&run.run, sizeof(unsigned int));

		m_inputFile.read((char*)&code.code, sizeof(unsigned int));
		m_inputFile.read((char*)&code.size, sizeof(unsigned int));

		m_codeTable.insert(CodeTablePair(code, run));
	}

	return true;
}

double* WaveletDecompressor::allocateTransformMemory(WaveletTransform* pWaveletTransform) {
	unsigned int transformWidth = UnsignedInteger::getClosestPowerOfTwo(m_header.ImageWidth);
	unsigned int transformHeight = UnsignedInteger::getClosestPowerOfTwo(m_header.ImageHeight);
	double* transformMemory = new double[transformWidth * transformHeight];

	pWaveletTransform->setData(transformMemory, transformWidth, transformHeight);

	return transformMemory;
}

bool WaveletDecompressor::decompress() {
	if(!readHeader()) {
		fprintf(stderr, "Error reading compressed image file header.\n");
		return false;
	}

	switch(m_header.wavletType) {
	case WaveletCompressor::Haar:
		m_pWaveletTransformR = new HaarWaveletTransform();
		break;
	default:
		fprintf(stderr, "Unsupported wavelet.\n");
		return false;
	}

	switch(m_header.BitsPerPixel) {
	case 24:
		decompressRGB();
		break;
	default:
		fprintf(stderr, "Unsupported bit depth.\n");
		return false;
	}

	return true;
}

void WaveletDecompressor::setPixels() {
	BYTE* bits;
	unsigned int x = 0;
	unsigned int y = 0;
	const unsigned int BytesPerPixel = m_header.BitsPerPixel / 8;

	for(y = 0; y < m_header.ImageHeight; ++y) {
		bits = FreeImage_GetScanLine(m_pDib, y);

		for(x = 0; x < m_header.ImageWidth; ++x) {
			bits[FI_RGBA_RED] = (BYTE)m_pWaveletTransformR->getItem(y, x);
			bits[FI_RGBA_GREEN] = (BYTE)m_pWaveletTransformG->getItem(y, x);
			bits[FI_RGBA_BLUE] = (BYTE)m_pWaveletTransformB->getItem(y, x);

			bits += BytesPerPixel;
		}
	}
}

void WaveletDecompressor::decompressRGB() {
	readCodeTable();

	/*m_pDib = FreeImage_Allocate(m_header.ImageWidth, m_header.ImageHeight, 24);

	readData(transformMemory);
	m_pWaveletTransform->inverseTransform();
	setPixels(FI_RGBA_RED);

	readData(transformMemory);
	m_pWaveletTransform->inverseTransform();
	setPixels(FI_RGBA_GREEN);

	readData(transformMemory);
	m_pWaveletTransform->inverseTransform();
	setPixels(FI_RGBA_BLUE);

	FreeImage_Save(FIF_BMP, m_pDib, m_sOutputFilename.c_str());*/
}

