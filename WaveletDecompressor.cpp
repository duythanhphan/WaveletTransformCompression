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
#include "HuffmanDecoder.h"
#include "RLEDecoder.h"
#include "Quantizer.h"

using namespace std;

WaveletDecompressor::WaveletDecompressor() :
		m_pWaveletTransformY(0), m_pWaveletTransformU(0), m_pWaveletTransformV(0),
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
	if(m_pWaveletTransformY != 0) {
		delete m_pWaveletTransformY;
		m_pWaveletTransformY = 0;
	}
	if(m_pWaveletTransformU != 0) {
		delete m_pWaveletTransformU;
		m_pWaveletTransformU = 0;
	}
	if(m_pWaveletTransformV != 0) {
		delete m_pWaveletTransformV;
		m_pWaveletTransformV = 0;
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

bool WaveletDecompressor::checkFileLength() {
	unsigned int codeTableSize = m_header.CodeTableSize *
			( sizeof(double) + (3 * sizeof(unsigned int)) );
	unsigned int dataSize = m_header.DataSize * sizeof(unsigned int);
	unsigned int totalSize = WaveletCompressor::HEADER_SIZE + codeTableSize + dataSize;

	if(totalSize != m_iFileLength) {
		return false;
	}

	return true;
}

bool WaveletDecompressor::decompress() {
	if(!readHeader()) {
		fprintf(stderr, "Error reading compressed image file header.\n");
		return false;
	}

	if(!checkFileLength()) {
		fprintf(stderr, "Error, file size.\n");
		return false;
	}

	switch(m_header.wavletType) {
	case WaveletCompressor::Haar:
		m_pWaveletTransformY = new HaarWaveletTransform();
		m_pWaveletTransformU = new HaarWaveletTransform();
		m_pWaveletTransformV = new HaarWaveletTransform();
		break;
	case WaveletCompressor::CDF97:
		m_pWaveletTransformY = new CDF97WaveletTransform();
		m_pWaveletTransformU = new CDF97WaveletTransform();
		m_pWaveletTransformV = new CDF97WaveletTransform();
		break;
	case WaveletCompressor::D4:
		m_pWaveletTransformY = new D4WaveletTransform();
		m_pWaveletTransformU = new D4WaveletTransform();
		m_pWaveletTransformV = new D4WaveletTransform();
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
	unsigned int transformSize = 0;
	if(transformWidth > transformHeight) {
		transformSize = transformWidth;
	} else {
		transformSize = transformHeight;
	}

	double* transformMemory = new double[transformSize * transformSize];

	pWaveletTransform->setData(transformMemory, transformSize);

	return transformMemory;
}

void WaveletDecompressor::setPixels() {
	BYTE* bits;
	unsigned int x = 0;
	unsigned int y = 0;
	const unsigned int BytesPerPixel = m_header.BitsPerPixel / 8;
	double Y = 0.0, U = 0.0, V = 0.0;
	double R = 0.0, G = 0.0, B = 0.0;

	for(y = 0; y < m_header.ImageHeight; ++y) {
		bits = FreeImage_GetScanLine(m_pDib, y);

		for(x = 0; x < m_header.ImageWidth; ++x) {

			Y = m_pWaveletTransformY->getItem(y, x);
			U = m_pWaveletTransformU->getItem(y, x);
			V = m_pWaveletTransformV->getItem(y, x);

			R = Y + (V / 0.877);
			B = Y + (U / 0.492);
			G = (Y - 0.299 * R - 0.114 * B) / 0.587;

			bits[FI_RGBA_RED] = clamp(R);//(BYTE)R;
			bits[FI_RGBA_GREEN] = clamp(G);//(BYTE)G;
			bits[FI_RGBA_BLUE] = clamp(B);//(BYTE)B;

			bits += BytesPerPixel;
		}
	}
}

void WaveletDecompressor::decode() {
	unsigned int* encodedData = new unsigned int[m_header.DataSize];
	m_inputFile.read((char*)encodedData, m_header.DataSize * sizeof(unsigned int));

	printf("Decoding Huffman codes...\n");

	HuffmanDecoder<RLE<double>::Run > huffmanDecoder(
			encodedData, m_header.DataSize, &m_codeTable, m_header.EncodedItems);
	huffmanDecoder.decode();

	printf("Decoding RLE...\n");

	RLE<double>::Run* pRuns = huffmanDecoder.getDecodedData();

	double* pTransformY = allocateTransformMemory(m_pWaveletTransformY);
	double* pTransformU = allocateTransformMemory(m_pWaveletTransformU);
	double* pTransformV = allocateTransformMemory(m_pWaveletTransformV);
	unsigned int transformSize =  m_pWaveletTransformY->getSize() * m_pWaveletTransformY->getSize();

	RLEDecoder<double> rleDecoder(pRuns, huffmanDecoder.getDecodedDataSize());
	rleDecoder.decode(pTransformY, transformSize);
	rleDecoder.decode(pTransformU, transformSize);
	rleDecoder.decode(pTransformV, transformSize);

	delete[] encodedData;
}

void WaveletDecompressor::inverseQuantization() {
	double* pTransformY = m_pWaveletTransformY->getTransformMemory();
	double* pTransformU = m_pWaveletTransformU->getTransformMemory();
	double* pTransformV = m_pWaveletTransformV->getTransformMemory();

	for(unsigned int i = 0; i < m_pWaveletTransformY->getSize() * m_pWaveletTransformY->getSize(); ++i) {
		pTransformY[i] *= m_header.QuantizationIntervals[0];
		pTransformU[i] *= m_header.QuantizationIntervals[1];
		pTransformV[i] *= m_header.QuantizationIntervals[2];
	}
}

void WaveletDecompressor::decompressRGB() {
	readCodeTable();

	decode();

	printf("Assign values to quantized indexes...\n");
	inverseQuantization();

	printf("Inverse Transform...\n");

	m_pWaveletTransformY->inverseTransform();
	m_pWaveletTransformU->inverseTransform();
	m_pWaveletTransformV->inverseTransform();

	printf("Setting image pixels...\n");

	m_pDib = FreeImage_Allocate(m_header.ImageWidth, m_header.ImageHeight, 24);
	setPixels();
	FreeImage_Save(FIF_BMP, m_pDib, m_sOutputFilename.c_str());
}

BYTE WaveletDecompressor::clamp(double d) {
	if(d > 255.0) {
		d = 255.0;
	} else if(d < 0.0) {
		d = 0.0;
	}
	return (BYTE)d;
}
