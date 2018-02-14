/*
 * PndBurstContainer.cxx
 *
 *  Created on: 23.08.2016
 *      Author: Stockmanns
 */

#include "PndBurstContainer.h"

std::vector<std::vector<FairTimeStamp*> > PndBurstContainer::ProcessData(std::vector<FairTimeStamp*> data)
{
	std::vector<std::vector<FairTimeStamp*> > result;
	for (auto dataItr : data){
//		LOG(INFO) << dataItr->GetTimeStamp();
		if ((dataItr->GetTimeStamp() - fCurrentThreshold)/fThreshold > 2.0)
			continue;
		if(IsAboveOffset(dataItr)){
			result.push_back(*fContainer.begin());
			fContainer.erase(fContainer.begin());
			fCurrentThreshold += fThreshold;
			fCurrentOffset += fThreshold;
		}
		int pos = CalcContainerPos(dataItr);
//		LOG(INFO) << "Pos: " << pos << " Thresh: " << fCurrentThreshold << " Offset: " << fCurrentOffset << " container.size " << fContainer.size();
		if (pos < 0) {
//			LOG(INFO) << "PndBurstContainer: negative Pos: TimeStamp " << dataItr->GetTimeStamp()
//					<< " fCurrenThreshold " << fCurrentThreshold << " fThreshold " << fThreshold
//					<< " fCurrentOffset " << fCurrentOffset << " fOffset " << fOffset;
			continue;
		}
		if (pos > fContainer.size() - 1){
//			LOG(INFO) << "Resize container: " << pos + 1;
			fContainer.resize(pos + 1);
		}
		fContainer[pos].push_back(dataItr);
	}
	return result;
}
