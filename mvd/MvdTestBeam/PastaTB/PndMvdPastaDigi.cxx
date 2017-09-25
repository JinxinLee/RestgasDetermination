/*
 * PndMvdPastaDigi.cxx
 *
 *  Created on: 23.09.2017
 *      Author: Stockmanns
 */

#include <PndMvdPastaDigi.h>

ClassImp(PndMvdPastaDigi);
ClassImp(ThresholdDataFullMode);
ClassImp(FrameHeader);


FrameHeader::FrameHeader(){}

ThresholdDataFullMode::ThresholdDataFullMode(){}

PndMvdPastaDigi::PndMvdPastaDigi() {
	// TODO Auto-generated constructor stub

}

PndMvdPastaDigi::~PndMvdPastaDigi() {
	// TODO Auto-generated destructor stub
}

int PndMvdPastaDigi::GetChannelId(enum PndMvdPastaDigi::branchId id)
{
	switch (id){
	case TIME :
		return fTimeBranch.channelId;
		break;
	case ENERGY :
		return fEnergyBranch.channelId;
		break;
	default :
		return -1;
	}
}

