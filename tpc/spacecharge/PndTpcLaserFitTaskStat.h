//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Results (Spline Objects) of PndTpcLaserFitTask
//     
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//	Felix Boehmer    	TUM     (original author)
//
//-----------------------------------------------------------

#ifndef TPCLASERFITTASKSTAT_H
#define TPCLASERFITTASKSTAT_H


// External Headers -----------------------------------------
#include "BiCubSpline.h"
#include "TPolyMarker3D.h"
#include "TF2.h"
#include "TObject.h"


class PndTpcLaserFitTaskStat : public TObject {

 public:

// Constructors ---------------------------------------------

  PndTpcLaserFitTaskStat();
  PndTpcLaserFitTaskStat(const std::vector<BiCubSpline*>);
			 
  
  virtual ~PndTpcLaserFitTaskStat();


// Acessors -------------------------------------------------

  std::vector<BiCubSpline*> getSplineList() const {return _splineList;}
    
  BiCubSpline* getDevMapR_Spline() {return &_devMapR;}
  BiCubSpline* getDevMapPerp_Spline() {return &_devMapPerp;}
  BiCubSpline* getRecoMapR_Spline() {return &_recoMapR;}
  BiCubSpline* getRecoMapPerp_Spline() {return  &_recoMapPerp;}


 private:
  
  // WATCH THE ORDER IN THE CONSTRUCTOR!!!
  // TODO: make this fool - proof.
  // TODO: make them const
 
    BiCubSpline _devMapR;         // reconstructed maps
    BiCubSpline _devMapPerp;
    BiCubSpline _recoMapR;        // maps in MEASURED coordinates
    BiCubSpline _recoMapPerp;
  
  std::vector<BiCubSpline*> _splineList;      
  
    
 public:
  ClassDef(PndTpcLaserFitTaskStat,1)

};

#endif
