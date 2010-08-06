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
	std::map<Int_t,PndEmcTwoCoordIndex* > fIntTwoCoordMap;
	PndEmcMapper(){};  
private:
	
	static PndEmcMapper* _instance;
	static Int_t fMapVersion;
	Int_t GetDetId(Int_t iTheta,Int_t iPhi);

        
	ClassDef(PndEmcMapper,1)
};

class PndEmcMapperGeoAllDat: public PndEmcMapper
{
	public:
		PndEmcMapperGeoAllDat();
};

class PndEmcMapperGeo1245Dat3Root: public PndEmcMapper
{
	public:
		PndEmcMapperGeo1245Dat3Root();
};

class PndEmcMapperGeo3Dat: public PndEmcMapper
{
	public:
		PndEmcMapperGeo3Dat();
};

class PndEmcMapperGeo3Root: public PndEmcMapper
{
	public:
		PndEmcMapperGeo3Root();
};

class PndEmcMapperGeo4Root: public PndEmcMapper
{
	public:
		PndEmcMapperGeo4Root();
};

class PndEmcMapperGeo5Root: public PndEmcMapper
{
	public:
		PndEmcMapperGeo5Root();
};

class PndEmcMapperGeo125Dat34Root: public PndEmcMapper
{
	public:
		PndEmcMapperGeo125Dat34Root();
};

class PndEmcMapperGeo12Dat345Root: public PndEmcMapper
{
	public:
		PndEmcMapperGeo12Dat345Root();
};

class PndEmcMapperGeo1235Dat4Root: public PndEmcMapper
{
	public:
		PndEmcMapperGeo1235Dat4Root();
};

class PndEmcMapperGeoProto60Root: public PndEmcMapper
{
	public:
		PndEmcMapperGeoProto60Root();
};

#endif //PndEmcMapper_H
