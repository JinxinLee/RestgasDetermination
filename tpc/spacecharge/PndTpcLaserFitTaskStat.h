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
#include <vector>


class PndTpcLaserFitTaskStat : public TObject {
  
 public:
  
  // Constructors ---------------------------------------------
  
  PndTpcLaserFitTaskStat();
  PndTpcLaserFitTaskStat(const std::vector<BiCubSpline*>& spline_list, 
			 const std::vector<std::vector<std::vector<double>*>*>& data_list );
  
  virtual ~PndTpcLaserFitTaskStat();


// Acessors -------------------------------------------------

  BiCubSpline* getDevMapR_Spline()  {return &_devMapR;}
  BiCubSpline* getDevMapPerp_Spline() {return &_devMapPerp;}
  BiCubSpline* getRecoMapR_Spline() {return &_recoMapR;}
  BiCubSpline* getRecoMapPerp_Spline() {return  &_recoMapPerp;}

  const TPolyMarker3D* const getDevMapR_Poly();
  const TPolyMarker3D* const getDevMapPerp_Poly();
  const TPolyMarker3D* const getRecoMapR_Poly();
  const TPolyMarker3D* const getRecoMapPerp_Poly();


 private:
  
  // WATCH THE ORDER IN THE CONSTRUCTOR!!!
  // TODO: make this fool - proof.
  // TODO: make them const
 
    BiCubSpline _devMapR;         // reconstructed maps
    BiCubSpline _devMapPerp;
    BiCubSpline _recoMapR;        // maps in MEASURED coordinates
    BiCubSpline _recoMapPerp;

    std::vector<std::vector<double>*> _devMapR_data;
    std::vector<std::vector<double>*> _devMapPerp_data;
    std::vector<std::vector<double>*> _recoMapR_data;
    std::vector<std::vector<double>*> _recoMapPerp_data;

    TPolyMarker3D* _poly_devMapR;       //!
    TPolyMarker3D* _poly_devMapPerp;    //!
    TPolyMarker3D* _poly_recoMapR;      //!
    TPolyMarker3D* _poly_recoMapPerp;   //!
          
    std::vector<BiCubSpline*> _splineList; //!     
  
    
 public:
    ClassDef(PndTpcLaserFitTaskStat,1)

};

#endif
