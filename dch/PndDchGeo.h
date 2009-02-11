// -------------------------------------------------------------------------
// -----                PndDchGeo header file                          -----
// -----                Created 15/05/07  by P. Hawranek               -----
// -------------------------------------------------------------------------

/** \class PndDchGeo
 *  \author P.Hawranek
 *  \date 15/05/07
 *  \brief Class for handling dch geometry
 * 
 * 	
 * 	
 **/
#ifndef PNDDCHGEO_H
#define PNDDCHGEO_H

#include "FairGeoSet.h"

class  PndDchGeo : public FairGeoSet {

public:
	/** Default constructor **/ 
	PndDchGeo();

	/** Destructor **/
	~PndDchGeo() {}

	/** Public method getModuleName(...) **/
	const char* getModuleName(Int_t);

	/** Public method getEleName(...) **/
	const char* getEleName(Int_t);

protected:
	char eleName[20]; 
	char modName[20];

	ClassDef(PndDchGeo,0) // Class for PndDch
};



#endif  /* !PNDDCHGEO_H */



