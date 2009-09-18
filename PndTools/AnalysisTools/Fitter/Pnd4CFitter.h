#ifndef Pnd4CFitter_H
#define Pnd4CFitter_H
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// Pnd4CFitter			  					//
//                                                                      //
// Author: K. Goetzen, GSI, 2008				        //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "RhoTools/VAbsFitter.h"
#include "TVector3.h"
#include "TMatrixDSym.h"
#include "TMatrixD.h"
#include "TLorentzVector.h"
#include "RhoBase/TCandList.h"

#include "PndVtxFitterParticle.h"
class PndVtxFitterParticle;
class TCandidate;
 
class Pnd4CFitter : public VAbsFitter
{
public:
    Pnd4CFitter( const TCandidate& b , TLorentzVector lv) ;
    virtual ~Pnd4CFitter();// {if(fHeadOfTree) delete fHeadOfTree;}
    void Fit();
    void FitConserveMasses();
    double GlobalChi2() {return fGlobChi2;}
	void PrintTree(TCandidate *c, int l=0);
    
private:    
  //  unsigned   _fit2(PndVtxFitterParticleCollection& ); // small Matrix
  
  TCandList   fDaughters;
  
  void    Do4CFit(); //Fit, can freely change energy
  void    Do4CFitWithMassConservation(); //Fit, conserving daughter masses
  
  void FindAndAddGenericDaughters(TCandidate *head);
  
  TLorentzVector fLv4C;
  int            fNDau;
  double         fGlobChi2;
  double         fConserveDaughterMasses; //true = energy of daughters cannot be modified by fit
  
  ClassDef(Pnd4CFitter,1) //A 4c fitter
};

#endif 
