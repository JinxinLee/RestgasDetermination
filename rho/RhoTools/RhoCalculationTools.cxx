//////////////////////////////////////////////////////////////////////////
//                                                                      //
// RhoCalculationTools                                                  //
//                                                                      //
// Toolset to calculate things centrally.                               //
//                                                                      //
// Authors:                                                             //
// Ralf Kliemt, HIM/GSI Feb.2013                                        //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "RhoCalculationTools.h"
#include "FairRunAna.h"
#include "FairField.h"



Double_t RhoCalculationTools::fBz=0.;
Int_t RhoCalculationTools::fVerbose=0;
Bool_t RhoCalculationTools::fBzSet=kFALSE;




Double_t RhoCalculationTools::GetBz ( const TVector3& pos )
{
  // If field was forced, we return that.
  if ( kTRUE==fBzSet ) { return fBz; }
  // Read magnetic filed strength in z direction [kGs]
  double pnt[3], Bf[3];
  pnt[0]=pos.X();
  pnt[1]=pos.Y();
  pnt[2]=pos.Z();
  // retrieve the field from the framework
  FairRunAna::Instance()->GetField()->GetFieldValue ( pnt, Bf ); //[kGs]
  return Bf[2];
}
