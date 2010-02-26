//--------------------------------------------------------------------------
// Description:
//	EMC Mapper class. Map crystal index (fDetectorId) to two coordinate index.
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
	static PndEmcMapper* Instance();
	static void Init(Int_t MapVersion);
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
	static Int_t fMapVersion;
	Int_t GetDetId(Int_t iTheta,Int_t iPhi);
	std::map<Int_t,PndEmcTwoCoordIndex* > fIntTwoCoordMap;
        
	ClassDef(PndEmcMapper,1)
};

#endif //PndEmcMapper_H
