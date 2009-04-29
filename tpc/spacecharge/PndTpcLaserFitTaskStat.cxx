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

#include "assert.h"
#include <string>
#include <sstream>


// Class Member definitions -----------

ClassImp(PndTpcLaserFitTaskStat)

PndTpcLaserFitTaskStat::PndTpcLaserFitTaskStat() 
{
  _poly_devMapR = NULL;
  _poly_devMapPerp = NULL;
  _poly_recoMapR = NULL;
  _poly_recoMapPerp = NULL;
}
  
PndTpcLaserFitTaskStat::PndTpcLaserFitTaskStat(const std::vector<BiCubSpline*> & sList,
					       const std::vector<std::vector<std::vector<double>*>*> & dList)
{
  assert(sList.size()==4);
  assert(dList.size()==4);
  _splineList = sList;
  _devMapR = *sList[0];        
  _devMapPerp = *sList[1];
  _recoMapR = *sList[2];       
  _recoMapPerp = *sList[3];
  _devMapR_data = *dList[0];
  _devMapPerp_data = *dList[1];
  _recoMapR_data = *dList[2];
  _recoMapPerp_data = *dList[3];
  _poly_devMapR = NULL;
  _poly_devMapPerp = NULL;
  _poly_recoMapR = NULL;
  _poly_recoMapPerp = NULL;
}


PndTpcLaserFitTaskStat::~PndTpcLaserFitTaskStat()
{
  if(_poly_devMapR!=NULL)
    delete _poly_devMapR;
  if(_poly_devMapPerp!=NULL)
    delete _poly_devMapPerp;
  if(_poly_recoMapR!=NULL)
    delete _poly_recoMapR;
  if(_poly_recoMapPerp!=NULL)
    delete _poly_recoMapPerp;  
  
}


const TPolyMarker3D* const 
PndTpcLaserFitTaskStat::getDevMapR_Poly() {
  if(_poly_devMapR!=NULL) {
    delete _poly_devMapR;
  }
  std::cout<<"creating new TPolyMarker3D"<<std::endl;
  _poly_devMapR = new TPolyMarker3D(_devMapR_data.size());
  for(unsigned int i=0; i<_devMapR_data.size(); ++i) {
    std::vector<double>* temp = _devMapR_data[i];
    _poly_devMapR->SetNextPoint(temp->at(0), temp->at(1), temp->at(2));
  }
  return _poly_devMapR;
}
  

const TPolyMarker3D* const  
PndTpcLaserFitTaskStat::getDevMapPerp_Poly() {
  if(_poly_devMapPerp!=NULL)
    delete _poly_devMapPerp;
  _poly_devMapPerp = new TPolyMarker3D(_devMapPerp_data.size());
  for(unsigned int i=0; i<_devMapPerp_data.size(); ++i) {
      std::vector<double>* temp = _devMapPerp_data[i];
      _poly_devMapPerp->SetNextPoint(temp->at(0), temp->at(1), temp->at(2));
  }
  return _poly_devMapPerp;
}

const TPolyMarker3D* const  
PndTpcLaserFitTaskStat::getRecoMapR_Poly() {
  if(_poly_recoMapR!=NULL)
    delete _poly_recoMapR;
  _poly_recoMapR = new TPolyMarker3D(_recoMapR_data.size());
  for(unsigned int i=0; i<_recoMapR_data.size(); ++i) {
      std::vector<double>* temp = _recoMapR_data[i];
      _poly_recoMapR->SetNextPoint(temp->at(0), temp->at(1), temp->at(2));
  }
  return _poly_recoMapR;
}

const TPolyMarker3D* const 
PndTpcLaserFitTaskStat::getRecoMapPerp_Poly() {
  if(_poly_recoMapPerp!=NULL)
    delete _poly_recoMapPerp;
  _poly_recoMapPerp = new TPolyMarker3D(_recoMapPerp_data.size());
  for(unsigned int i=0; i<_recoMapPerp_data.size(); ++i) {
      std::vector<double>* temp = _recoMapPerp_data[i];
      _poly_recoMapPerp->SetNextPoint(temp->at(0), temp->at(1), temp->at(2));
  }
  return _poly_recoMapPerp;
}
  


ClassImp(PndTpcLaserFitTaskStat)
