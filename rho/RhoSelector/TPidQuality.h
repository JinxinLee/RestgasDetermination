#ifndef TPIDQUALITY_H
#define TPIDQUALITY_H
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TPidQuality								//
//                                                                      //
// Mix-in class for particle identification quality control		//
//                                                                      //
//                                                                      //
// Author: Marcel Kunze, Bochum University, Aug. 99			//
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <iostream>

#include "TObject.h"

class TTuple;
class TCandidate;
class VAbsMicroCandidate;

class TPidQuality : public TObject {
    
public:
    
    //Constructor
    TPidQuality(const char* name="PidQuality");
    //Destructor
    virtual ~TPidQuality();
    
    //operations
    void Control(TCandidate& c, Bool_t decision);
    void Control(VAbsMicroCandidate& c, Bool_t decision);

protected:    
    TTuple		*fAccepted;	    //! Accepted candidates
    TTuple		*fRejected;	    //! Rejected candidates

public:
    ClassDef(TPidQuality,1) // Pid quality control base class
};  



#endif
