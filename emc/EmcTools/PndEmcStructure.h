//======================================================================
// File and Version Information:
// 	$Id: $
//
//------------------------------------------------------------------------

#ifndef PNDEMCSTRUCTURE_HH
#define PNDEMCSTRUCTURE_HH

#include <map>
#include <string>		
#include "TObject.h"
#include "TString.h"
		
class PndEmcXtal;
class PndEmcTwoCoordIndex;
class TGeoManager;

using std::string;

class PndEmcStructure: public TObject
{
	
public:
	typedef std::map<Int_t, Float_t> mapper;
	static PndEmcStructure* Instance();
	static PndEmcStructure* Instance(TGeoManager*);
	
	virtual ~PndEmcStructure();
	
	const mapper& GetEmcX() const { return emcX ;};
	const mapper& GetEmcY() const { return emcY ;};
	const mapper& GetEmcZ() const { return emcZ ;};
	const std::map<PndEmcTwoCoordIndex*, PndEmcXtal*>& GetTciXtalMap() const { return fTciXtalMap ;};
	PndEmcTwoCoordIndex* locateIndex( double theta, double phi ) const;
	void Print(string) const;

private:
	bool crystal_name_analysis(TString,int &module,int &copy,int &row,int &crystal);

protected:
	PndEmcStructure(TGeoManager*);		
private:
	static PndEmcStructure* _instance;
  
	mapper emcX;
	mapper emcY;
	mapper emcZ;
	
	std::map<PndEmcTwoCoordIndex*, PndEmcXtal*> fTciXtalMap;

	ClassDef(PndEmcStructure,1)
};


#endif  // PNDEMCSTRUCTURE_HH
