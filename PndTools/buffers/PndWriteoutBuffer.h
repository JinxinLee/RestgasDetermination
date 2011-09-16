/**
 * @class PndWriteoutBufferT
 *
 * @author Tobias Stockmanns
 * @brief A container class to store digi data during events
 *
 * The PndWriteoutBufferT template class provides a containers which handles the storage of data between
 * events. The data has to be given with an active time, the absolute time the data is active in the
 * detector and can influence the output of the data.
 * FillNewData is used to put new data into the containes.
 * WriteOutData with a given actual time stores the data which has an active time older than the given time
 * in the corresponding TClonesArray of the FairRootManager.
 * At the end of the task WriteOutAllData has to be called to store the data which is still in the buffer.
 * If one tries to store data, for a detector element which is already present in the buffer, Modify and
 * CalcNewActiveTime are called.
 * Modify changes the values of the old data,
 * CalcNewActiveTime changes the time this detector element is active.
 * Both methods have to be specialized for the corresponding subdetector.
 * Both methods rely on an operator== defined in the data element.
 */

#ifndef PNDWRITEOUTBUFFER_H_
#define PNDWRITEOUTBUFFER_H_



#include "TObject.h"
#include "TString.h"
#include "FairWriteoutBuffer.h"
#include "FairTimeStamp.h"
#include <map>


class PndWriteoutBuffer: public FairWriteoutBuffer{
public:
	PndWriteoutBuffer():fTreeSave(false), fActivateBuffering(kFALSE), fVerbose(0) {};
	PndWriteoutBuffer(TString branchName, TString className);
	virtual ~PndWriteoutBuffer(){};

	virtual void WriteOutData(double time);
	virtual void WriteOutAllData();

	virtual void SaveDataToTree(Bool_t val = kTRUE){fTreeSave = val;}
	virtual void ActivateBuffering(Bool_t val = kTRUE){fActivateBuffering=val;}

	virtual std::vector<FairTimeStamp*> GetRemoveOldData(double time);
	virtual std::vector<FairTimeStamp*> GetAllData();
	virtual void FillNewData(FairTimeStamp* data, double activeTime);
	virtual void AddNewDataToTClonesArray(FairTimeStamp* data) = 0;
	
	virtual double FindTimeForData(FairTimeStamp* data) = 0;
	virtual void FillDataMap(FairTimeStamp* data, double activeTime) = 0;
	virtual void EraseDataFromDataMap(FairTimeStamp* data) = 0;

protected:

	virtual std::vector<std::pair<double, FairTimeStamp*> > Modify(std::pair<double, FairTimeStamp*> oldData, std::pair<double, FairTimeStamp*> newData){
		std::vector<std::pair<double, FairTimeStamp*> > result;
	//	std::cout << "Modify of PndWriteoutBuffer called!" << std::endl;
		result.push_back(oldData);
		return result;
	}

	virtual double CalcNewActiveTime(double oldActiveTime, FairTimeStamp* newData){ return oldActiveTime;};


	std::multimap<double, FairTimeStamp*> fDeadTime_map;
	//std::map<FairTimeStamp, double> fData_map;

	TString fBranchName;
	TString fClassName;
	Bool_t fTreeSave;
	Bool_t fActivateBuffering;
	int fVerbose;
	ClassDef(PndWriteoutBuffer, 1);
};

#endif /* PNDWRITEOUTBUFFER_H_ */
