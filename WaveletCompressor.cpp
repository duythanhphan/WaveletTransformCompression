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
		m_pWaveletTransformY(0), m_pWaveletTransformU(0), m_pWaveletTransformV(0),
		m_iImageWidth(0), m_iImageHeight(0),
		m_iBytesPerPixel(0), m_iBitsPerPixel(0),
		m_waveletType(NotSet),
		m_dQuantizationY(8192),
		m_dQuantizationU(8192),
		m_dQuantizationV(8192){ }

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
}

bool WaveletCompressor::compress(WaveletType waveletType) {
	FREE_IMAGE_TYPE imageType = FreeImage_GetImageType(m_image.getDib());

	if(imageType != FIT_BITMAP) {
		fprintf(stderr, "Unsupported image bitmap type.\n");
		return false;
	}

	switch(waveletType) {
	case Haar:
		m_pWaveletTransformY = new HaarWaveletTransform();
		m_pWaveletTransformU = new HaarWaveletTransform();
		m_pWaveletTransformV = new HaarWaveletTransform();
		m_waveletType = Haar;
		break;
	case D4:
		m_pWaveletTransformY = new D4WaveletTransform();
		m_pWaveletTransformU = new D4WaveletTransform();
		m_pWaveletTransformV = new D4WaveletTransform();
		m_waveletType = D4;
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
	unsigned int transformSize = 0;
	if(transformWidth > transformHeight) {
		transformSize = transformWidth;
	} else {
		transformSize = transformHeight;
	}

	double*	pMemory = new double[transformSize * transformSize];
	pWaveletTransform->setData(pMemory, transformSize, transformSize);

	return pMemory;
}

void WaveletCompressor::setTransformMemory(
		double* pTransformMemoryY, double* pTransformMemoryU, double* pTransformMemoryV) {
	BYTE* bits = 0;
	unsigned int x = 0;
	unsigned int y = 0;
	unsigned int i = 0;
	const size_t bytesToFill = (m_pWaveletTransformY->getWidth() - m_iImageWidth) * sizeof(double);
	double R = 0.0, G = 0.0, B = 0.0;

	for(y = 0; y < m_iImageHeight; ++y) {
		bits = FreeImage_GetScanLine(m_image.getDib(), y);

		for(x = 0; x < m_iImageWidth; ++x) {
			i = (y * m_pWaveletTransformY->getWidth()) + x;

			R = (double)bits[FI_RGBA_RED];
			G = (double)bits[FI_RGBA_GREEN];
			B = (double)bits[FI_RGBA_BLUE];

			pTransformMemoryY[i] = 0.299 * R + 0.587 * G + 0.114 * B; //Y
			pTransformMemoryU[i] = 0.492 * (B - pTransformMemoryY[i]); //U
			pTransformMemoryV[i] = 0.877 * (R - pTransformMemoryY[i]); //V

			bits += m_iBytesPerPixel;
		}

		i = (y * m_pWaveletTransformY->getWidth()) + m_iImageWidth;
		memset(&pTransformMemoryY[i], 0, bytesToFill);
		memset(&pTransformMemoryU[i], 0, bytesToFill);
		memset(&pTransformMemoryV[i], 0, bytesToFill);
	}

	const size_t bytesInTransformRow = m_pWaveletTransformY->getWidth() * sizeof(double);
	for(y = m_iImageHeight; y < m_pWaveletTransformY->getHeight(); ++y) {
		i = y * m_pWaveletTransformY->getWidth();
		memset(&pTransformMemoryY[i], 0, bytesInTransformRow);
		memset(&pTransformMemoryU[i], 0, bytesInTransformRow);
		memset(&pTransformMemoryV[i], 0, bytesInTransformRow);
	}
}

void WaveletCompressor::quantization(double* pTransformMemoryY, double* pTransformMemoryU, double* pTransformMemoryV) {

	for(unsigned int i = 0; i < m_pWaveletTransformY->getWidth() * m_pWaveletTransformY->getHeight(); ++i) {
		pTransformMemoryY[i] = (int)(pTransformMemoryY[i] / m_dQuantizationY);
		pTransformMemoryU[i] = (int)(pTransformMemoryU[i] / m_dQuantizationU);
		pTransformMemoryV[i] = (int)(pTransformMemoryV[i] / m_dQuantizationV);
	}
}

void WaveletCompressor::compressRGB() {
	printf("Transform...\n");

	double* transformMemoryY = 0;
	double* transformMemoryU = 0;
	double* transformMemoryV = 0;
	transformMemoryY = allocateTransformMemory(m_pWaveletTransformY);
	transformMemoryU = allocateTransformMemory(m_pWaveletTransformU);
	transformMemoryV = allocateTransformMemory(m_pWaveletTransformV);

	setTransformMemory(transformMemoryY, transformMemoryU, transformMemoryV);
	m_pWaveletTransformY->transform();
	m_pWaveletTransformU->transform();
	m_pWaveletTransformV->transform();

	printf("Quantization...\n");
	quantization(transformMemoryY, transformMemoryU, transformMemoryV);

	encode();
}

void WaveletCompressor::countRuns(RLE<double>::Run* pData, unsigned int size,
		map<RLE<double>::Run, unsigned int >& countTable) {

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
	printf("RLE encode...\n");

	const unsigned int size = m_pWaveletTransformY->getWidth() * m_pWaveletTransformY->getHeight();
	RLE<double> rleR(m_pWaveletTransformY->getTransformMemory(), size);
	rleR.encode();
	RLE<double> rleG(m_pWaveletTransformU->getTransformMemory(), size);
	rleG.encode();
	RLE<double> rleB(m_pWaveletTransformV->getTransformMemory(), size);
	rleB.encode();

	printf("Huffman Coding...\n");

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

	printf("Saving file...\n");

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
	header.QuantizationIntervals[0] = m_dQuantizationY;
	header.QuantizationIntervals[1] = m_dQuantizationU;
	header.QuantizationIntervals[2] = m_dQuantizationV;
	m_outputFile.write((char*)&header, HEADER_SIZE);

	map<RLE<double>::Run, HuffmanCoding<RLE<double>::Run>::Code >::iterator it;

	for(it = codeTable.begin(); it != codeTable.end(); ++it) {
		m_outputFile.write((char*)&it->first.value, sizeof(double));
		m_outputFile.write((char*)&it->first.run, sizeof(unsigned int));

		m_outputFile.write((char*)&it->second.code, sizeof(unsigned int));
		m_outputFile.write((char*)&it->second.size, sizeof(unsigned int));
	}
}
