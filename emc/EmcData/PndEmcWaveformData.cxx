#include "PndEmcWaveformData.h"

#include <map>
#include <ostream>
#include "assert.h"

class PndEmcAbsWaveformSimulator;
class PndEmcWaveform;

using std::cout;
using std::pair;
using std::map;
using std::endl;

ClassImp(PndEmcWaveformData);

PndEmcWaveformData::PndEmcWaveformData() : FairTimeStamp(), fDetectorId(-1), fWfSimulator() {
}


PndEmcWaveformData::PndEmcWaveformData(Int_t detId, PndEmcAbsWaveformSimulator* wfSimulator) : FairTimeStamp(), fDetectorId(detId), fWfSimulator(wfSimulator) {
}



bool PndEmcWaveformData::operator< (const PndEmcWaveformData& wfData) const {
	return (fDetectorId < wfData.fDetectorId);
}


std::ostream& operator<< (std::ostream& out, PndEmcWaveformData& wfData) {
      out << "waveform links to " << wfData.fHitMap.size() <<" following PndEmcHits: " << " NTB " << endl;

      for(map<Double_t,Double_t>::iterator it = wfData.fHitMap.begin(); it!=wfData.fHitMap.end(); ++it) {
	      out << "\t time: " << it->first << "\t energy: " << it->second << endl;
      }

      return out;
}

PndEmcWaveformData& PndEmcWaveformData::operator+=(const PndEmcWaveformData& toAdd) {
	fHitMap.insert(toAdd.fHitMap.begin(), toAdd.fHitMap.end());
	AddLinks(toAdd.GetLinks());
	return *this;
}

bool PndEmcWaveformData::equal(FairTimeStamp* data) {
	PndEmcWaveformData* wfData = dynamic_cast<PndEmcWaveformData*>(data);
	if(wfData != 0 && wfData->GetDetectorId() == fDetectorId) {
		return true;
	} else {
		return false;
	}
}




void PndEmcWaveformData::AddHit(const FairLink& link, Double_t absEventTime, Double_t energy) {
	this->AddLink(link);
	fHitMap.insert(pair<Double_t,Double_t>(absEventTime, energy));
}


void PndEmcWaveformData::GetHitParameter(Int_t hitNo, Double_t& time, Double_t& energy) {
	assert(hitNo>=0 && hitNo<fHitMap.size());	
	map<Double_t, Double_t>::iterator it = fHitMap.begin();
	while(hitNo--) it++;
	time = it->first;	
	energy = it->second;
}
