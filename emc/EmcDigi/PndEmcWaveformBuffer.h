/*
 * PndEmcWaveformBuffer.h
 */

#ifndef PNDEMCWAVEFORMBUFFER_H_
#define PNDEMCWAVEFORMBUFFER_H_

#include "FairWriteoutBuffer.h"

class FairTimeStamp;
class PndEmcWaveformData;

class TClonesArray;

/**
 * @brief buffer for waveforms, used by PndEmcFWEndcapTimebasedWaveforms
 * @author Ph. Mahlberg <mahlberg@hiskp.uni-bonn.de>
 * @ingroup PndEmc
 */
class PndEmcWaveformBuffer : public FairWriteoutBuffer 
{
public:
	PndEmcWaveformBuffer();
	PndEmcWaveformBuffer(TString branchName, TString className, TString folderName, Bool_t persistance);

	virtual ~PndEmcWaveformBuffer();

	virtual void AddNewDataToTClonesArray(FairTimeStamp*);
	virtual double FindTimeForData(FairTimeStamp* data) ;
	virtual void FillDataMap(FairTimeStamp* data, double activeTime) ;
	virtual void EraseDataFromDataMap(FairTimeStamp* data);

	virtual void DeleteOldData();
	virtual void StoreWaveformData(TString branchName, TString folderName, bool persistance);

	virtual void FillNewData(PndEmcWaveformData*);

private:
	// don't allow copying (-Weffc++)
	PndEmcWaveformBuffer(const PndEmcWaveformBuffer&);	// no implementation
	PndEmcWaveformBuffer& operator= (const PndEmcWaveformBuffer&);	// no implementation

protected:	

	std::map<PndEmcWaveformData, double> fData_map;
	std::vector<std::pair<double, FairTimeStamp*> > Modify(std::pair<double, FairTimeStamp*> oldData, std::pair<double, FairTimeStamp* > newData);

	ClassDef(PndEmcWaveformBuffer, 0);

	bool fStoreWaveformData;
	TClonesArray* fWfDataArray;
	TString fWfDataBranchName;
};

#endif /* PNDEMCWAVEFORMBUFFER_H*/
