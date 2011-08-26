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

#ifndef PNDWRITEOUTBUFFERT_H_
#define PNDWRITEOUTBUFFERT_H_



#include "TObject.h"
#include "TString.h"
#include "FairWriteoutBufferAbsBasis.h"
#include <map>


template <class T> class PndWriteoutBufferT: public FairWriteoutBufferAbsBasis{
public:
	PndWriteoutBufferT():fTreeSave(false), fActivateBuffering(kFALSE), fVerbose(0) {};
	PndWriteoutBufferT(TString branchName, TString className);
	virtual ~PndWriteoutBufferT(){};

	virtual void WriteOutData(double time);
	virtual void WriteOutAllData();

	virtual void SaveDataToTree(Bool_t val = kTRUE){fTreeSave = val;}
	virtual void ActivateBuffering(Bool_t val = kTRUE){fActivateBuffering=val;}

	virtual std::vector<T> GetRemoveOldData(double time);
	virtual std::vector<T> GetAllData();
	virtual void FillNewData(T& data, double activeTime);

protected:

	virtual std::vector<std::pair<double, T> > Modify(std::pair<double, T> oldData, std::pair<double, T> newData){
		std::vector<std::pair<double, T> > result;
		result.push_back(oldData);
		return result;
	}

	virtual double CalcNewActiveTime(double oldActiveTime, T& newData){ return oldActiveTime;};


	std::multimap<double, T> fDeadTime_map;
	std::map<T, double> fData_map;

	TString fBranchName;
	TString fClassName;
	Bool_t fTreeSave;
	Bool_t fActivateBuffering;
	int fVerbose;
	ClassDef(PndWriteoutBufferT, 2);
};

#endif /* PNDWRITEOUTBUFFERT_H_ */
