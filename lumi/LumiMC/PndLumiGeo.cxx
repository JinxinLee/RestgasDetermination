#include "PndLumiGeo.h"

// macro (defined by ROOT) to generate streamer functions

PndLumiGeo::PndLumiGeo()
{
    // Detector name
    fName="lumi";

    // Sectors are not used for panda, kept for backwards compatibility with HADES geometry
    maxSectors=9;

    // Just a single module is used for panda, kept for backwards compatibility with HADES geometry
    maxModules=9;
}

Int_t PndLumiGeo::getModNumInMod(const TString& name)
{
   return 1;
}

const char* PndLumiGeo::getModuleName(Int_t m)
{
	sprintf(modName,"lumiVolume%i",m+1);
	return modName;
	//return "lumi";


}

const char* PndLumiGeo::getEleName(Int_t m)
{
	//if (m < 2)
	//	return"";
	//else
	sprintf(eleName,"lumiSensor0%i",m+1);
	return eleName;
	//return "lumi";


}
ClassImp(PndLumiGeo)

