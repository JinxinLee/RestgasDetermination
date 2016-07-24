// -------------------------------------------------------------------------
// -----                       PndRichMirrorSegment source file        -----
// -----               Created 22/07/16  by Beloborodov Konstantin     -----
// -----                                                               -----
// -----                                                               -----
// -------------------------------------------------------------------------
#include <iostream>

using std::cout;
using std::endl;

#include "PndRichMirrorSegment.h"

// -----   Default constructor   -------------------------------------------
PndRichMirrorSegment::PndRichMirrorSegment():
fMiddleFlatMirrorPoint(TVector3(0,0,0)),
fSizeOfFlatMirror(TVector3(0,0,0)),
fNormalOfFlatMirror(TVector3(0,0,0))
{  
}
// -------------------------------------------------------------------------


// -----   Standard constructor   ------------------------------------------

PndRichMirrorSegment::PndRichMirrorSegment(TVector3 point,
                                           TVector3 dims,
                                           TVector3 normal):
fMiddleFlatMirrorPoint(point),
fSizeOfFlatMirror(dims),
fNormalOfFlatMirror(normal)
{    
}

// -----   Destructor   ----------------------------------------------------
PndRichMirrorSegment::~PndRichMirrorSegment(){}
// -------------------------------------------------------------------------


// -----   Public method Print   -------------------------------------------
void PndRichMirrorSegment::Print(const Option_t* opt) const {
//  cout << "RICH Photon hit: Time = " << fTime << endl;
}
// -------------------------------------------------------------------------

ClassImp(PndRichMirrorSegment)
