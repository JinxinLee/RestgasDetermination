// -------------------------------------------------------------------------
// -----                      PndSttTrack source file                  -----
// -----                  Created 28/03/06  by R. Castelijns           -----
// -------------------------------------------------------------------------

#include "PndSttTrack.h"

#include "PndSttHit.h"

#include <iostream>

using std::cout;
using std::endl;
using std::map;

// -----   Default constructor   -------------------------------------------
PndSttTrack::PndSttTrack() 
{
    fRefAngle = 0.;
    fPidHypo  = 0;
    fFlag     = 0;
    fChi2Long = 0.;
    fChi2Rad  = 0.;
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
PndSttTrack::~PndSttTrack() 
{
    fHitMap.clear();
}
// -------------------------------------------------------------------------



// -----   Public method AddHit   --------------------------------------
void PndSttTrack::AddHit(Int_t hitID, PndSttHit* mHit) 
{
    Double_t 
	wireX = mHit->GetX(),
	wireY = mHit->GetY(),
	wireZ = mHit->GetZ();

//    fHitMap[wireZ] = hitID;
    fHitMap[wireX * wireX + wireY * wireY] = hitID;
}
// -------------------------------------------------------------------------



// -----   Public method Print   -------------------------------------------
void PndSttTrack::Print() 
{
  cout << " Number of attached hits : " 
       << fHits.GetSize()  << endl;
  cout << " Parameters at first point: " << endl;
  fParamFirst.Print();
  cout << " Chi2Long: " << fChi2Long << ", Chi2Rad: " << fChi2Rad << ", Quality flag " << fFlag << endl;
}
// -------------------------------------------------------------------------




void PndSttTrack::SortHits() 
{
    // find the biggest z difference between hits
    // get the hit closest to the target
    
    Int_t 
	index=0;
  
    map<Double_t, Int_t>::iterator 
	it;
    
    fHits.Reset();
    fHits.Set(fHitMap.size());
    index = 0;
    
    for (it = fHitMap.begin(); it != fHitMap.end(); it++) 
    {
	fHits[index] = it->second;
	index++;
    }
}


// -------------------------------------------------------------------------

Bool_t PndSttTrack::AlreadyHasHit(Int_t iHit)
{
    Bool_t
	retval = kFALSE;
    
    map<Double_t, Int_t>::iterator 
	it;
    
    for (it = fHitMap.begin(); it != fHitMap.end(); it++) 
    {
	if (it->second == iHit)
	{
	    retval = kTRUE;
	    break;
	}
    }  
    
    return retval;
}
// -------------------------------------------------------------------------


void PndSttTrack::AddHelixHit(Int_t size, Int_t index, Int_t helixhitindex) 
{
  //  fHelixHits.Reset();
    fHelixHits.Set(size);
    fHelixHits[index] = helixhitindex;
}

ClassImp(PndSttTrack)
