#ifndef PNDVTXPOCA_H
#define PNDVTXPOCA_H
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// PndVtxPoca			  		                                          			//
//                                                                      //
// Author: R. Kliemt 2010 			                              	        //
// taken from PndKinVtxFitter::GetStartVtx()                            //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "RhoTools/VAbsFitter.h"
#include "RhoBase/TCandList.h"
#include "TVector3.h"

class TCandidate;

class PndVtxPoca : public VAbsFitter
  {
  public:
    PndVtxPoca( const TCandidate& b) ;
    virtual ~PndVtxPoca();// {if(fHeadOfTree) delete fHeadOfTree;}
    void Fit(){};
    void GetPocaVtx(TVector3 &vertex);
    
    ClassDef(PndVtxPoca,1) //A poca Vertex Finder
  };

#endif 
