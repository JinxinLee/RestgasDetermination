#ifndef FITTER_TPCHIT_H
#define FITTER_TPCHIT_H

#include "AbsRecoHit.h"
#include "AbsTrackRep.h"

#include "PndTpcPoint.h"
#include "PndTpcDigi.h"
#include "PndTpcDigiMapper.h"
//#include "CbmTstPoint.h"
    
class PndTpcRecoHit : public AbsRecoHit {
 private:
  double amp;
  static const int NparHitRep = 2;
 public:
  virtual ~PndTpcRecoHit();
  PndTpcRecoHit();
  PndTpcRecoHit(double r, double phi, double z, double sig_rphi, double sig_z, double amplitude = 0.);
  PndTpcRecoHit(PndTpcPoint*);
  PndTpcRecoHit(PndTpcDigi*);

  //temporary solution: create PndTpcHits from CbmTstPoint s
  //PndTpcRecoHit(CbmTstPoint*);

  virtual double residualScalar(double s, 
								const AbsTrackRep* stateVector,
								const TMatrixT<double>& state);
 void setHit(double r, double phi, double z, double sig_rphi, double sig_z, double amplitude = 0.); 
 inline double getAmp() {
   return amp;
 }
 void setHMatrix(double s,const AbsTrackRep* stateVector,
				 const TMatrixT<double>& state);

 virtual AbsRecoHit* clone(){return new PndTpcRecoHit(*this);}

  
  virtual inline TMatrixT<double> getHitCov(double s,
					    AbsTrackRep* const trkRep) const;
  
  virtual inline TMatrixT<double> getHitCoord(double s,
					      AbsTrackRep* const trkRep) const;

};




#endif //FITTER_TPCHIT_H
