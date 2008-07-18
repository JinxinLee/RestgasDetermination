#include "AnalysisEvent.h"

AnalysisEvent::AnalysisEvent(){
  _clusters = new TClonesArray("AnalysisCluster");
  clear();
}

AnalysisEvent::~AnalysisEvent() {}

double AnalysisEvent::getMinYresid() {
  if(nClusters() == 0) return -1.;
  double min = fabs(getCluster(0)->resid().y());
  for(unsigned int i=1;i<nClusters();++i) {
	if( fabs(getCluster(i)->resid().y()) < min) {
	  min = fabs(getCluster(i)->resid().y());
	}
  }
  return min;
}

ClassImp(AnalysisEvent)
 
 
