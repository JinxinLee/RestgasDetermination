#ifndef PNDVTXPOCA_H
#define PNDVTXPOCA_H
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// PndVtxPoca			  		                                          			//
//                                                                      //
// Author: R. Kliemt 2010 			                              	        //
// idea taken from PndKinVtxFitter::GetStartVtx()                            //
// extended for multiple tracks                                         //
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
    void Fit(){std::cout<<"PndVtxPoca::Fit() -- this is an empty function -- "<<std::endl;};
    Double_t GetPocaVtx(TVector3 &vertex);
    Double_t GetPoca(TVector3 &vertex,TCandidate* a, TCandidate* b);
    
    ClassDef(PndVtxPoca,1) //A poca Vertex Finder
  };

#endif 
