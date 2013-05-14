#ifndef PNDTRKCONFORMALTRANSFORM_H
#define PNDTRKCONFORMALTRANSFORM_H

#include "TVector2.h"
#include "PndTrkConformalHit.h"

class PndTrkConformalTransform : public TObject {
  
 public:
  
  PndTrkConformalTransform();
  PndTrkConformalTransform(double x, double y, double delta);
  ~PndTrkConformalTransform();

  void SetTranslation(double x, double y);
  void SetRotation(double delta); 
  TVector2 GetTranslation() { return fTrasl; }
  Double_t GetRotation() { return fAngle; }

  void PerformConformalTransformation(double x, double y, double rd, double &u, double &v, double &rc);

  PndTrkConformalHit *GetConformalSttHit(PndTrkHit *hit);
  PndTrkConformalHit * GetConformalHit(PndTrkHit *hit);
  double GetXConf(double x, double y, double rd);
  double GetYConf(double x, double y, double rd);
  double GetRConf(double x, double y, double rd);

  void SetOrigin(double x, double y, double delta);

 private:
  TVector2 fTrasl;
  Double_t fAngle;


  ClassDef(PndTrkConformalTransform, 1)
};


#endif
