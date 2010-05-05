#ifndef PndMassFitter_H
#define PndMassFitter_H
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// PndMassFitter			  				//
//                                                                      //
// Author: D. Mishra, K. Goetzen, GSI, 2008				//
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "RhoTools/VAbsFitter.h"
#include "TVector3.h"
#include "TMatrixDSym.h"
#include "TMatrixD.h"
#include "RhoBase/TCandList.h"
#include "PndVtxFitterParticle.h"

class PndVtxFitterParticle;
class TCandidate;
 
class PndMassFitter : public VAbsFitter
{
public:
    PndMassFitter( const TCandidate& b , double mass) ;
    virtual ~PndMassFitter();// {if(fHeadOfTree) delete fHeadOfTree;}
    void Fit();
    virtual void SetFitIncludingVertex(Int_t);
  double 		GetChi2() {return fGlobChi2;}
  unsigned     CalDgf(); //Calculate the Degree of freedom
  unsigned     GetNdf() {return CalDgf();} 

private:    
	
  TCandList   fDaughters;
  
  unsigned    DoVertexFitMassConstraintWOIncludeVertex(); // Fit with "Mass" constraint and without including vertex
  unsigned    DoVertexFitMassConstraintIncludeVertex(); // Fit with "Mass" constraint and including vertex
  void        SetInvariantMass(Double_t);
  void        SetAtDecayPoint();
  void        SetNotDecayPoint();
  void        SetFixMass();
  //output functions/ Accessors
  Double_t     GetInvariantMass();
  unsigned     GetDecayPoint();
  unsigned     FitWithVertex();

  unsigned    SetInputMatrix(); // set the input matrix
  unsigned    MakeCoreMatrix(); //Make core matrix
//   unsigned    SetOutputMatrix(); // set teh output matrix
  unsigned    SetOutputToTCandidate(); //fill output to TCandidate

 
 private:
 //from PndVtxFitterBase1.h
  //virtual unsigned m_setInputSubMatrix(void) = 0;

  //these were from Base2 
  //matrix definitions
  TMatrixD    m_E;
  TMatrixD    m_V_E;
  TMatrixD    m_lam0;
  TMatrixD    m_v;
  TMatrixD    m_v_a;
  TMatrixD    m_V_Dt;
  TMatrixD    m_Cov_v_al_1;

  //Following are from Base

  
  //statistic parameter
  Double_t   fGlobChi2;
  
  Int_t      m_dgf;
  Double_t   m_chisq;
  Double_t   m_cl;

  Int_t       m_trackNum;
  Int_t       m_necessaryTrackNum;
  Int_t       m_errorFlag;
  Int_t       m_overIterationFlag;
  Double_t    m_magField;

  //  TMatrixDSym  m_V_al_0;
  TMatrixD     m_V_al_0;
  TMatrixD     m_al_0;
  TMatrixD     m_al_1;
  //TMatrixD     m_al_a;
  TMatrixD     m_property;
  TMatrixD     m_V_al_1;

  TMatrixD     m_D;
  TMatrixD     m_d;
  TMatrixD     m_V_D;
  TMatrixD     m_lam;
 
  TMatrixD     m_E_T;
  TMatrixD     m_D_T;
 
 
  
  //follwoing might be deleted later
  unsigned     m_mode;
  Double_t     m_eachChisq[KF_MAX_TRACK_NUMBER2];
  Double_t     m_vertexChisq;
  TVector3     m_vertex_b;
  TVector3     m_vertex_a;
  TMatrixDSym  m_errVertex_a;
  TMatrixDSym   m_errBeam;
  unsigned      m_beam;
  unsigned      m_knownVertex;
  Bool_t        m_tube;
  Int_t         m_itrk_tube;
  ///////////
  //other parameters
  Double_t   m_invariantMass;
  unsigned   m_fitIncludingVertex;
  unsigned   m_atDecayPoint;
  unsigned   m_isFixMass;


  PndVtxFitterParticle*   it;
  TCandidate*          tcand;
  
  
  ClassDef(PndMassFitter,1) //A super duper fitter
};
inline void PndMassFitter::SetFitIncludingVertex(Int_t var){m_fitIncludingVertex = var;}

#endif 
