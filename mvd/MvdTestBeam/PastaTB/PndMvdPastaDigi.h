/*
 * PndMvdPastaDigi.h
 *
 *  Created on: 23.09.2017
 *      Author: Stockmanns
 */

#ifndef MVD_MVDTESTBEAM_PASTATB_PNDMVDPASTADIGIFULL_H_
#define MVD_MVDTESTBEAM_PASTATB_PNDMVDPASTADIGIFULL_H_

#include "Rtypes.h"
#include "TObject.h"

#include <ostream>


	class ThresholdDataFullMode : public TObject
	{
	public :
		ThresholdDataFullMode();
		~ThresholdDataFullMode(){};
		int t_coarse;
		int t_soc;
		int t_eoc;
		int channelId;
		int tacId;

		friend std::ostream& operator<< (std::ostream& out, const ThresholdDataFullMode& data){
			out << "t_coarse: " << data.t_coarse << " t_soc " << data.t_soc << " t_eoc " << data.t_eoc <<
					" channelId " << data.channelId << " tacId " << data.tacId;
			return out;
		}
		ClassDef(ThresholdDataFullMode, 1);
	};

	class FrameHeader : public TObject
	{
	public :
		FrameHeader();
		~FrameHeader(){};
		ULong64_t frameId;
		int nEvents;
		ClassDef(FrameHeader,1);

	};

class PndMvdPastaDigi : public TObject {
public:

	enum branchId{TIME, ENERGY, UNDEFINED};

	PndMvdPastaDigi();
	PndMvdPastaDigi(FrameHeader header, ThresholdDataFullMode time, ThresholdDataFullMode energy) :
		fFrameHeader(header), fTimeBranch(time), fEnergyBranch(energy)
	{
		fToT = CalcTot();
		fTimeStamp = CalcTimeStamp();
	};

	virtual ~PndMvdPastaDigi();

	void SetTimeBranch  (ThresholdDataFullMode time)   { fTimeBranch = time; }
	void SetEnergyBranch(ThresholdDataFullMode energy) { fEnergyBranch = energy; }
	void SetHeader (FrameHeader header) { fFrameHeader = header; }

	ULong64_t CalcTimeStamp(){
		ULong64_t timeStamp = fFrameHeader.frameId * 1024 + fTimeBranch.t_coarse;
		fTimeStamp = timeStamp;
		return timeStamp;
	}

	int CalcTot(){
		int tot = fEnergyBranch.t_coarse - fTimeBranch.t_coarse;

		if (tot < 0){
			tot += 1024;
		}
		fToT = tot;
		return tot;
	}

	int GetTot(){ return fToT; }

	ULong64_t GetTimeStamp(){ return fTimeStamp; };
	int GetChannelId(branchId id);


	friend std::ostream& operator<< (std::ostream& out, const PndMvdPastaDigi& digi) {
	  out << std::dec << " Header: nHits " << digi.fFrameHeader.nEvents << " frameId: " << digi.fFrameHeader.frameId << std::endl;
	  out << "TimeBranch: " << digi.fTimeBranch << std::endl;
	  out << "EnergyBranch: " << digi.fEnergyBranch << std::endl;
	  return out;
	}

private:
	ThresholdDataFullMode fTimeBranch;
	ThresholdDataFullMode fEnergyBranch;
	FrameHeader fFrameHeader;
	ULong64_t fTimeStamp;
	int fToT;

	ClassDef(PndMvdPastaDigi, 1);
};

#endif /* MVD_MVDTESTBEAM_PASTATB_PNDMVDPASTADIGIFULL_H_ */
