#include"TChoughPatternReco.h"
#include"TCabsHough.h"
#include"TChough1.h"

void TChoughPatternReco::process(TCevent &event,std::vector<TCcluster>& _c){
  static TCabsHough* hXZ=NULL;
  static TCabsHough* hYZ=NULL;
  static bool first=true;
  if(first){
    first=false;
    TVector3 X(1.,0.,0.);
    TVector3 Y(0.,1.,0.);
    TVector3 Z(0.,0.,1.);

    hYZ=new TChough2(Y,Z,0.1);
  }
  hXZ->make(_c,2);
  hYZ->make(_c,2);

  std::vector<TCcluster> ret1;
  std::vector<TCcluster> ret2;
  

  for (unsigned int i=0 ; i < _c.size(); ++i ){
    if(hYZ->hot(i,0)){
      _c.at(i).setFit();
      ret1.push_back(_c.at(i));
    }
    if(hYZ->hot(i,1)){
      _c.at(i).setFit();
      ret2.push_back(_c.at(i));
    }

  }

  //hXZ->draw(false,50,50,600,600);
  hYZ->draw(true,700,50,600,600);

}
