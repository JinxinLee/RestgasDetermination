/********************************************************************************
 *    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             * 
 *         GNU Lesser General Public Licence version 3 (LGPL) version 3,        *  
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/
/**
 * PndBurstVectorBuilder.cxx
 *
 * @since 2012-12-06
 * @author D. Klein, A. Rybalchenko
 */


//template class PndBurstVectorBuilderT<PndSdsHit>;

template <typename T>
vector< vector<FairTimeStamp* > > PndBurstVectorBuilderT<T>::ProcessData(TClonesArray* data)
{

//			LOG(INFO) << "TopixData: " << fData.size();

	std::vector< std::vector<FairTimeStamp* > > result;
	for (int i = 0; i < data->GetEntriesFast(); i++){
		fData.push_back((FairTimeStamp*) (new T(*(T*)data->At(i))));
	}
//	LOG(INFO) << "fData.size " << fData.size();
	fBurstContainer.SetThreshold(2000);
	result = fBurstContainer.ProcessData(fData);

//	bool switchChannels = false;
//	double newTimeStamp = 0;
//	for (auto itr : fData){
//		LOG(INFO) << itr->GetTimeStamp() << " " << fCurrentThreshold;
//		if (itr->GetTimeStamp() < fCurrentThreshold)
//			fCurrentOutput.push_back(std::move(itr));
//		else if ((itr->GetTimeStamp() - fCurrentThreshold) / fThreshold < 2.0){	// no data allowed which is more than one burst away from the previous data
//			LOG(INFO) << "Data put in fNextOutput";
//			fNextOutputs.push_back(itr);
//			if(itr->GetTimeStamp() > fCurrentOffset){
////				LOG(INFO) << "Switch Channels: " << itr.GetTimeStamp() << " > " << currentOffset;
//				switchChannels = true;
//				newTimeStamp = itr->GetTimeStamp();
//			}
//		}
//		if (switchChannels == true){
//			int multiply = (newTimeStamp / fThreshold) + 1;
//			result.push_back(fCurrentOutput);
//			fCurrentOutput = fNextOutputs;
//			fNextOutputs.clear();
//			fCurrentThreshold = multiply * fThreshold;
//			fCurrentOffset = (multiply * fThreshold) + fOffset;
//			LOG(INFO) << "SWITCH Channels: newTS " << newTimeStamp << " mult: " << multiply << " Thresh: " << fThreshold << " CurrentThr: " << fCurrentThreshold << " CurrentOff: " << fCurrentOffset;
//			switchChannels = false;
//		}
//	}
	fData.clear();

	return result;
}

template <typename T>
vector< vector<FairTimeStamp* > > PndBurstVectorBuilderT<T>::GetLastData()
{
//	std::vector< std::vector<FairTimeStamp* > > result;
//	result.push_back(fCurrentOutput);
//	result.insert(fNextOutputs);
//	fCurrentOutput.clear();
//	fNextOutputs.clear();

	return fBurstContainer.GetLastData();
}
