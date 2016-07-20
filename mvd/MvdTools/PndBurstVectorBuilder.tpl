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
vector< vector<FairTimeStamp*> > PndBurstVectorBuilderT<T>::ProcessData(TClonesArray* data)
{

//			LOG(INFO) << "TopixData: " << fData.size();

	std::vector< std::vector<FairTimeStamp*> > result;
	for (int i = 0; i < data->GetEntriesFast(); i++){
		fData.push_back(new T(*(T*)data->At(i)));
	}

	bool switchChannels = false;
	for (auto itr : fData){
		if (itr->GetTimeStamp() < fCurrentThreshold)
			fCurrentOutput.push_back(itr);
		else {
			fNextOutput.push_back(itr);
			if(itr->GetTimeStamp() > fCurrentOffset){
//						LOG(INFO) << "Switch Channels: " << itr.GetTimeStamp() << " > " << currentOffset;
				switchChannels = true;
			}
		}
		if (switchChannels == true){
			result.push_back(fCurrentOutput);
			fCurrentOutput = fNextOutput;
			fNextOutput.clear();
			fCurrentThreshold += fThreshold;
			fCurrentOffset += fThreshold;
			switchChannels = false;
		}
	}
	fData.clear();

	return result;
}

template <typename T>
vector< vector<FairTimeStamp*> > PndBurstVectorBuilderT<T>::GetLastData()
{
	std::vector< std::vector<FairTimeStamp*> > result;
	result.push_back(fCurrentOutput);
	result.push_back(fNextOutput);
	fCurrentOutput.clear();
	fNextOutput.clear();

	return result;
}
