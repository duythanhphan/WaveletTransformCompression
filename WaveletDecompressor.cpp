/*
 * WaveletDecompressor.cpp
 *
 *  Created on: 31-12-2011
 *      Author: Michal
 */

#include <cstdio>

#include "WaveletDecompressor.h"

using namespace std;

WaveletDecompressor::WaveletDecompressor() : m_pWaveletTransform(0), m_pDib(0), m_iFileLength(0) { }

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
	if(m_pWaveletTransform != 0) {
		delete m_pWaveletTransform;
		m_pWaveletTransform = 0;
	}
}

bool WaveletDecompressor::decompress() {
	WaveletCompressor::Header header;
	m_inputFile.read(&(char)header, WaveletCompressor::HEADER_SIZE);

	printf("ImageWidth: %d\n", header.ImageWidth);
	printf("ImageHeight: %d\n", header.ImageHeight);
	printf("TransformWidth: %d\n", header.TransformWidth);
	printf("TransformHeight: %d\n", header.TransformHeight);

	return true;
}

