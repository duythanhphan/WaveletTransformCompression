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
#include "Encoder.h"

using namespace std;

const unsigned int WaveletCompressor::HEADER_SIZE = sizeof(Header);

WaveletCompressor::WaveletCompressor() :
		m_pWaveletTransformR(0), m_pWaveletTransformG(0), m_pWaveletTransformB(0),
		m_iImageWidth(0), m_iImageHeight(0),
		m_iBytesPerPixel(0), m_iBitsPerPixel(0),
		m_waveletType(NotSet) { }

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
		m_waveletType = Haar;
		break;
	default:
		fprintf(stderr, "Unsupported wavelet type.\n");
		return false;
	}

	m_iBitsPerPixel = FreeImage_GetBPP(m_image.getDib());
	m_iBytesPerPixel = m_iBitsPerPixel / 8;
	switch(m_iBitsPerPixel) {
	case 24:
		compressRGB();
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

void WaveletCompressor::compressRGB() {
	double* transformMemoryR = 0;
	double* transformMemoryG = 0;
	double* transformMemoryB = 0;
	transformMemoryR = allocateTransformMemory(m_pWaveletTransformR);
	transformMemoryG = allocateTransformMemory(m_pWaveletTransformG);
	transformMemoryB = allocateTransformMemory(m_pWaveletTransformB);

	setTransformMemory(transformMemoryR, transformMemoryG, transformMemoryB);
	m_pWaveletTransformR->transform();
	m_pWaveletTransformG->transform();
	m_pWaveletTransformB->transform();

	quantization(transformMemoryR, transformMemoryG, transformMemoryB);

	encode();

//	m_outputFile.write((char*)transformMemoryR, dataSize);
//	m_outputFile.write((char*)transformMemoryG, dataSize);
//	m_outputFile.write((char*)transformMemoryB, dataSize);
}

void WaveletCompressor::countRuns(RLE<double>::Run* pData, unsigned int size, map<RLE<double>::Run, unsigned int >& countTable) {
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

HuffmanCoding<RLE<double>::Run >::Leaf* WaveletCompressor::getLeafs(
		RLE<double>& rleR, RLE<double>& rleG, RLE<double>& rleB, unsigned int* size) {

	map<RLE<double>::Run, unsigned int> countTable;
	countRuns(rleR.getData(), rleR.getEncodedDataSize(), countTable);
	countRuns(rleG.getData(), rleG.getEncodedDataSize(), countTable);
	countRuns(rleB.getData(), rleB.getEncodedDataSize(), countTable);

	*size = countTable.size();
	HuffmanCoding<RLE<double>::Run >::Leaf* pLeafs = new HuffmanCoding<RLE<double>::Run >::Leaf[countTable.size()];

	map<RLE<double>::Run, unsigned int>::iterator it;

	unsigned int i = 0;
	for(it = countTable.begin(); it != countTable.end(); ++it, ++i) {
		pLeafs[i].value = it->first;
		pLeafs[i].count = it->second;
	}

	return pLeafs;
}

void WaveletCompressor::encode() {
	const unsigned int size = m_pWaveletTransformR->getWidth() * m_pWaveletTransformR->getHeight();
	RLE<double> rleR(m_pWaveletTransformR->getTransfomrMemory(), size);
	rleR.encode();
	RLE<double> rleG(m_pWaveletTransformG->getTransfomrMemory(), size);
	rleG.encode();
	RLE<double> rleB(m_pWaveletTransformR->getTransfomrMemory(), size);
	rleB.encode();


	unsigned int leafsSize = 0;
	HuffmanCoding<RLE<double>::Run>::Leaf* pLeafs = getLeafs(rleR, rleG, rleB, &leafsSize);
	HuffmanCoding<RLE<double>::Run> huffmanCoding(pLeafs, leafsSize);
	huffmanCoding.createCodeTable();

	map<RLE<double>::Run, HuffmanCoding<RLE<double>::Run>::Code > codeTable;
	huffmanCoding.getTable(codeTable);

	Encoder encoder(size / 4);

	map<RLE<double>::Run, HuffmanCoding<RLE<double>::Run>::Code >::iterator it;
	RLE<double>::Run* pRun = rleR.getData();
	for(unsigned int i = 0; i < rleR.getEncodedDataSize(); ++i) {
		it = codeTable.find(pRun[i]);
		encoder.encode(it->second.code, it->second.size);
	}

	pRun = rleG.getData();
	for(unsigned int i = 0; i < rleG.getEncodedDataSize(); ++i) {
		it = codeTable.find(pRun[i]);
		encoder.encode(it->second.code, it->second.size);
	}

	pRun = rleB.getData();
	for(unsigned int i = 0; i < rleB.getEncodedDataSize(); ++i) {
		it = codeTable.find(pRun[i]);
		encoder.encode(it->second.code, it->second.size);
	}

	unsigned int encodedItems = rleR.getEncodedDataSize() + rleG.getEncodedDataSize() + rleB.getEncodedDataSize();
	saveHeader(codeTable, encoder.encodedSize(), encodedItems);
	m_outputFile.write((char*)encoder.getData(), encoder.encodedSize() * sizeof(unsigned int));
}

void WaveletCompressor::saveHeader(map<RLE<double>::Run, HuffmanCoding<RLE<double>::Run>::Code >& codeTable,
		unsigned int dataSize, unsigned int encodedItems) {

	Header header;
	header.ImageWidth = FreeImage_GetWidth(m_image.getDib());
	header.ImageHeight = FreeImage_GetHeight(m_image.getDib());
	header.BitsPerPixel = m_iBitsPerPixel;
	header.wavletType = m_waveletType;
	header.CodeTableSize = codeTable.size();
	header.DataSize = dataSize;
	header.EncodedItems = encodedItems;
	m_outputFile.write((char*)&header, HEADER_SIZE);

	map<RLE<double>::Run, HuffmanCoding<RLE<double>::Run>::Code >::iterator it;
	const unsigned int runSize = sizeof(RLE<double>::Run);
	const unsigned int codeSize = sizeof(HuffmanCoding<RLE<double>::Run>::Code);

	for(it = codeTable.begin(); it != codeTable.end(); ++it) {
		m_outputFile.write((char*)&it->first, runSize);
		m_outputFile.write((char*)&it->second, codeSize);
	}
}
