#include"TChoughPatternReco.h"
#include"TCabsHough.h"
#include"TChough1.h"

void TChoughPatternReco::process(std::vector<TCcluster>& _c){
  static TCabsHough* hXZ=NULL;
  static TCabsHough* hYZ=NULL;
  static bool first=true;
  if(first){
    first=false;
    TVector3 X(1.,0.,0.);
    TVector3 Y(0.,1.,0.);
    TVector3 Z(0.,0.,1.);

    hXZ=new TChough1(X,Z,0.3,0.35);
    hYZ=new TChough1(Y,Z,0.3,0.35);
  }
  hXZ->make(_c);
  hYZ->make(_c);

  std::vector<TCcluster> ret;
  for (unsigned int i=0 ; i < _c.size(); ++i ){
    if(hYZ->hot(i)){
      _c.at(i).setFit();
    }
  }

  //hXZ->draw(false,50,50,600,600);
  //hYZ->draw(true,700,50,600,600);

}
