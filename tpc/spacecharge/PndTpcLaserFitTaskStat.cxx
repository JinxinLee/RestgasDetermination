//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      See Header
//     
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//	Felix Boehmer    	TUM     (original author)
//
//-----------------------------------------------------------

#include "PndTpcLaserFitTaskStat.h"

#include "SplineTF2Interface.h"

#include "assert.h"
#include <string>
#include <sstream>


// Class Member definitions -----------

ClassImp(PndTpcLaserFitTaskStat)

PndTpcLaserFitTaskStat::PndTpcLaserFitTaskStat() 
{;}
  
PndTpcLaserFitTaskStat::PndTpcLaserFitTaskStat(const std::vector<BiCubSpline*> sList)
{
  assert(sList.size()==4);
  _splineList = sList;
  //_markerList = mList;
  _devMapR = *sList[0];        
  _devMapPerp = *sList[1];
  _recoMapR = *sList[2];       
  _recoMapPerp = *sList[3];
  //_devMapR_poly = mList[0];        
  //_devMapPerp_poly = mList[1];
  //_recoMapR_poly = mList[2];       
  //_recoMapPerp_poly = mList[3];
}


//TODO: 
PndTpcLaserFitTaskStat::~PndTpcLaserFitTaskStat()
{;}

/*
void
PndTpcLaserFitTaskStat::setGeometry(double zmin, double zmax, double rmin, double rmax) {
  _rMin=rmin;                           
  _rMax=rmax;                           
  _zMin=zmin;                           
  _zMax=zmax;
  _geoInit=true;

  std::vector<TF2*> temp;
  
  // now we can build the TF2 interface...
  for(int i=0; i<4; i++) {
    std::vector<double>* knotsZ = (_splineList[i]).getKx();
    std::vector<double>* knotsR = (_splineList[i]).getKy();
    double lamdaZ = knotsZ->size();
    double lamdaR = knotsR->size();
    _ifcList.push_back(new SplineTF2Interface(&(_splineList[i])));
    (_ifcList[i])->setReadOnly(true);
    std::string fname = "f";
    std::stringstream ss;
    ss<<i;
    fname.append(ss.string());
    temp.push_back(new TF2(fname.c_str(),_ifcList[i],&SplineTF2Interface::eval,_zMin,_zMax,
			   _rMin,_rMax,(lamdaZ-4)*(lamdaR-4),"Function", "eval"));
    (temp[i])->SetNpx(50);
    (temp[i])->SetNpy(50);
  }
  
  _devMapR_TF2 = *(temp[0]);
  _devMapPerp_TF2 = *(temp[1]); 
  _recoMapR_TF2 = *(temp[2]); 
  _recoMapPerp_TF2 = *(temp[3]);
    
}
*/ 

ClassImp(PndTpcLaserFitTaskStat)
