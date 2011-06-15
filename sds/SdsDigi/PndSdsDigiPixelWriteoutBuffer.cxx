/*
 * PndSdsDigiPixelWriteoutBuffer.cxx
 *
 *  Created on: May 10, 2011
 *      Author: stockman
 */

#include <PndSdsDigiPixelWriteoutBuffer.h>

ClassImp(PndSdsDigiPixelWriteoutBuffer);

PndSdsDigiPixelWriteoutBuffer::PndSdsDigiPixelWriteoutBuffer() {
	// TODO Auto-generated constructor stub

}

PndSdsDigiPixelWriteoutBuffer::PndSdsDigiPixelWriteoutBuffer(TString branchName): PndWriteoutBufferT<PndSdsDigiPixel>(branchName, "PndSdsDigiPixel")
{
}

PndSdsDigiPixelWriteoutBuffer::~PndSdsDigiPixelWriteoutBuffer() {
	// TODO Auto-generated destructor stub
}

std::vector<std::pair<double, PndSdsDigiPixel> > PndSdsDigiPixelWriteoutBuffer::Modify(std::pair<double, PndSdsDigiPixel> oldData, std::pair<double, PndSdsDigiPixel> newData)
{
	std::vector<std::pair<double, PndSdsDigiPixel> > result;
	std::pair<double, PndSdsDigiPixel> singleResult;
	singleResult.first = oldData.first + newData.first;
	singleResult.second = oldData.second;
	singleResult.second.AddCharge(newData.second.GetCharge());
	std::cout << "Modify hit" << std::endl;

	result.push_back(oldData);
	return result;
}

