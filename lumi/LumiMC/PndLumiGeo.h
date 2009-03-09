#ifndef PNDLUMIGEO_H
#define PNDLUMIGEO_H

#include "FairGeoSet.h"

#include <sstream>

class  PndLumiGeo : public FairGeoSet
{
protected:
		char modName[20];  // name of module
		char eleName[20];  // substring for elements in module
public:
    // constructors and destructors
    PndLumiGeo();
    ~PndLumiGeo(){}


    // implementation of virtual functions from base class FairGeoSet
    /** return the name for a specific module number */
    const char* getModuleName(Int_t number);

    /** return the name for a specific element number */
    const char* getEleName(Int_t number);

    /** return the number for a specific module name */
    Int_t getModNumInMod(const TString& name);


    // macro (defined by ROOT) to generate streamer functions
    ClassDef(PndLumiGeo,1);
};
#endif
