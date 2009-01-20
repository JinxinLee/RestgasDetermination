//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id:$
//
// Description:
//	EMC Mapper.
// This class uses the singleton pattern design		
//
// Author List:
// Dima Melnychuk
//
/////////////////////////////////////////////////////////////// 

#pragma once
#ifndef PNDEMCMAPPER_H
#define PNDEMCMAPPER_H

#include "PndEmcTwoCoordIndex.h"
#include "TObject.h"
#include "TString.h"
#include <map>
	 
class PndEmcMapper: public TObject
{

public:
  static PndEmcMapper* Instance(Int_t, TString geoName="");
	const std::map<Int_t,PndEmcTwoCoordIndex* >& GetTciMap();

	/** Destructor **/
	virtual ~PndEmcMapper();    
	
	/** Accessors **/
	Int_t GetMapVersion()  const { return fMapVersion ;};
	
	PndEmcTwoCoordIndex* GetTCI(Int_t DetectorId);
	
	
	
protected:
	PndEmcMapper(Int_t fMapVersion);
  
private:
	static PndEmcMapper* _instance;
	bool IsValidIndex(Int_t detectorId);
	Int_t GetDetId(Int_t iTheta,Int_t iPhi);
	Int_t fMapVersion;
	std::map<Int_t,PndEmcTwoCoordIndex* > fIntTwoCoordMap;
        
	ClassDef(PndEmcMapper,1)
};

#endif //PndEmcMapper_H
