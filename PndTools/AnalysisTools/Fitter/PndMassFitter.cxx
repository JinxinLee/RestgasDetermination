//////////////////////////////////////////////////////////////////////////
//                                                                      //
// PndMassFitter			  					//
//                                                                      //
// Author: D. Mishra, GSI,  2008		         		//
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <iostream>
#include "PndMassFitter.h"
#include "RhoBase/TCandListIterator.h"

#include "PndVtxFitterParticle.h"
#include "PndVtxFitter_Init.h"
#include "PndVtxFitterError.h"
#include "RhoBase/TFactory.h"

using namespace std;

ClassImp(PndMassFitter)

TBuffer &operator>>(TBuffer &buf, PndMassFitter *&obj)
{
  obj = (PndMassFitter *) buf.ReadObject(PndMassFitter::Class());
  return buf;
}

PndMassFitter::PndMassFitter( const TCandidate& b, double mass ) : VAbsFitter( b ) 
{
  m_atDecayPoint=KF_AT_DECAY_POINT;
  m_isFixMass=1;
  m_vertex_b=TVector3(0.,0.,0.);
  m_necessaryTrackNum = 2;
  m_invariantMass=mass;
  m_errorFlag = KF_NO_ERROR;
  fGlobChi2 = -1.;
}


PndMassFitter::~PndMassFitter()
{
  //if(fHeadOfTree) delete fHeadOfTree;
}

void PndMassFitter::Fit() 
{
  TCandidate theHead=*fHeadOfTree;
  TCandListIterator iter=theHead.DaughterIterator();
  //cout <<"Fit: head of Tree:"<<endl;
  //cout<<(*fHeadOfTree)<<endl;
  
  fDaughters.Cleanup();
  
  TCandidate *tc;
  //int num=0;
  while ((tc=iter.Next()))
  {
    TMatrixD cov=tc->Cov7();
    //cout <<"PndMassFitter: Daughter "<<num++<<endl;
    //cout <<(*tc)<<endl;
    //cov.Print();
    fDaughters.Add(*tc);
  }
  
  if(m_fitIncludingVertex == 0)DoVertexFitMassConstraintWOIncludeVertex();
  else DoVertexFitMassConstraintIncludeVertex();
  
}
/**
 This is same as "fit()" function in "kfitterbase".
 **/
unsigned PndMassFitter::DoVertexFitMassConstraintWOIncludeVertex()
{
  if(m_errorFlag != (int)KF_NO_ERROR)return m_errorFlag;
  //cout<<"======SetInputMatrix part Start========== "<<m_trackNum<<endl;
  m_trackNum = fDaughters.GetLength();
  if(m_trackNum < m_necessaryTrackNum){
    m_errorFlag = KF_TRACK_SIZE;
    return m_errorFlag;
  }
  //cout<<"======SetInputMatrix part Start========== "<<m_trackNum<<endl;
  /*
   if(m_trackNum > KF_MAX_TRACK_NUMBER){
   m_errorFlag = KF_INPUT_TRACK_SIZE;
   return m_errorFlag;
   }*/
  
  SetInputMatrix(); //Set input matrix
  CalDgf(); //  Calculate Dgf 
  
  
  /** Fit function _fit() Starts **/
  //cout<<"++++++ Fit function _fit() Starts +++++++ "<<endl;
  Double_t chiSq(0.);
  //  Double_t errInverse = 0; // if you need the determinant value
  Double_t *errInverse = 0;
  Double_t tmp_chiSq(KF_INIT_CHI2);
  TMatrixD tmp_al_1(m_al_1);
  TMatrixD tmp_V_al_1(m_V_al_1);
  TMatrixD    m_al_a = TMatrixD(KF_NUM7*m_trackNum,1);
  m_al_a = m_al_0;
  TMatrixD tmp_al_a(m_al_a);
  
  for(Int_t j=0;j<(int)KF_MAX_ITERATION_NUMBER;++j)
  {
    MakeCoreMatrix(); // MakeCoreMatrix() STARTS 
    
    //cout<<"++++++ Fit function _fit() Continues +++++++ "<<endl;
    
    m_V_D = (m_D*m_V_al_0*(m_D_T.Transpose(m_D))).Invert(errInverse);
    /**
	   if(errInverse != 0){
	   m_errorFlag = KF_INVERSE;
	   return m_errorFlag;
	   }
     **/
    
    m_lam    = m_V_D*(m_D*(m_al_0-m_al_1)+m_d);
    TMatrixD m_lam_tmp = m_lam;
    chiSq    = ((m_lam_tmp.T())*(m_D*(m_al_0-m_al_1)+m_d))(0,0);
    
    
    m_al_1   = m_al_0 - m_V_al_0*(m_D_T.Transpose(m_D))*m_lam;
    
    m_V_al_1 = m_V_al_0 - m_V_al_0*(m_D_T.Transpose(m_D))*m_V_D*m_D*m_V_al_0;
    
    if(tmp_chiSq > chiSq){
      tmp_chiSq  = chiSq;
      tmp_al_a   = tmp_al_1;
      tmp_al_1   = m_al_1;
      tmp_V_al_1 = m_V_al_1;
      if(j == KF_MAX_ITERATION_NUMBER-1){
        m_al_a = tmp_al_1;
        m_overIterationFlag = KF_OVER_ITERATION;
      }else continue;
    }else if(j != 0){
      chiSq    = tmp_chiSq;
      m_al_1   = tmp_al_1;
      m_al_a   = tmp_al_a;
      m_V_al_1 = tmp_V_al_1;
      break;
    }else if(j == 0){
      m_errorFlag = KF_INIT_CHISQ;
      return m_errorFlag;
    }
    
  } //Iteration Loop ends
  
  
  //cout<<"Chi2 is **********: "<<chiSq<<endl;
  
  fGlobChi2 = chiSq;
  
  SetOutputToTCandidate();
  return 0;
}


unsigned PndMassFitter::DoVertexFitMassConstraintIncludeVertex()
{
  //cout<<"Include Vertex in the Fitting ======== "<<endl;
  if(m_errorFlag != (int)KF_NO_ERROR)return m_errorFlag;
  m_trackNum = fDaughters.GetLength();
  if(m_trackNum < m_necessaryTrackNum){
    m_errorFlag = KF_TRACK_SIZE;
    return m_errorFlag;
  }
  
  SetInputMatrix(); //Set input matrix
  
  CalDgf(); //Calculate Dgf
  
  /** Fit function _fit() Starts **/
  Double_t chiSq(0.);
  //  Double_t errInverse = 0; // if you need the determinant value
  Double_t *errInverse = 0;
  Double_t tmp_chiSq(KF_INIT_CHI2);
  TMatrixD tmp_al_1(m_al_1);
  TMatrixD tmp_V_al_1(m_V_al_1);
  TMatrixD    m_al_a = TMatrixD(KF_NUM7*m_trackNum+3,1);
  m_al_a = m_al_0;
  TMatrixD tmp_al_a(m_al_a);
  for(Int_t j=0;j<(int)KF_MAX_ITERATION_NUMBER;++j)
  {
    MakeCoreMatrix(); //Make Core matrix
    
    //cout<<"++++++ Fit function _fit() Continues +++++++ "<<endl;
    //      TMatrixD mD_tmp = m_D;
    m_V_D = (m_D*m_V_al_0*(m_D_T.Transpose(m_D))).Invert(errInverse);
    /*** will 
	   if(errInverse != 0){
	   m_errorFlag = KF_INVERSE;
	   return m_errorFlag;
	   }
     **/
    m_lam    = m_V_D*(m_D*(m_al_0-m_al_1)+m_d);
    TMatrixD m_lam_tmp = m_lam;
    chiSq    = ((m_lam_tmp.T())*(m_D*(m_al_0-m_al_1)+m_d))(0,0);
    
    
    m_al_1   = m_al_0 - m_V_al_0*(m_D_T.Transpose(m_D))*m_lam;
    
    m_V_al_1 = m_V_al_0 - m_V_al_0*(m_D_T.Transpose(m_D))*m_V_D*m_D*m_V_al_0;
    
    if(tmp_chiSq > chiSq){
      tmp_chiSq  = chiSq;
      tmp_al_a   = tmp_al_1;
      tmp_al_1   = m_al_1;
      tmp_V_al_1 = m_V_al_1;
      if(j == KF_MAX_ITERATION_NUMBER-1){
        m_al_a = tmp_al_1;
        m_overIterationFlag = KF_OVER_ITERATION;
      }else continue;
    }else if(j != 0){
      chiSq    = tmp_chiSq;
      m_al_1   = tmp_al_1;
      m_al_a   = tmp_al_a;
      m_V_al_1 = tmp_V_al_1;
      break;
    }else if(j == 0){
      m_errorFlag = KF_INIT_CHISQ;
      return m_errorFlag;
    }
    
  } //Iteration Loop ends
  
  
  //cout<<"Chi2 is **********: "<<chiSq<<endl;
  fGlobChi2 = chiSq;
  
  SetOutputToTCandidate();
  return 0;
}

unsigned PndMassFitter::SetInputMatrix()
{
  //cout<<"Track NUMBER is ***** "<<m_trackNum<<endl;
  if(m_trackNum > (int)KF_MAX_TRACK_NUMBER){
    m_errorFlag = KF_INPUT_TRACK_SIZE;
    return m_errorFlag;
  }
  if(m_fitIncludingVertex==0){
    unsigned index(0);
    TMatrixDSym tmp_V_al_0(KF_NUM7*m_trackNum);
    
    m_al_0.ResizeTo(KF_NUM7*m_trackNum,1);
    m_al_1.ResizeTo(KF_NUM7*m_trackNum,1);
    m_property.ResizeTo(m_trackNum,3);
    
    m_D.ResizeTo(1,KF_NUM7*m_trackNum);
    m_d.ResizeTo(1,1);
    m_V_D.ResizeTo(1,1);
    m_lam.ResizeTo(1,1);
    
    TMatrixDSym tmp_ErrCov(KF_NUM6);
    
    m_D_T.ResizeTo(KF_NUM7*m_trackNum,1);    
    
    Int_t nTra = fDaughters.GetLength();
    
    it =new PndVtxFitterParticle;
    
    for(Int_t i=0;i<nTra;i++)
    {
      TCandidate tc=fDaughters[i];
      
      m_al_0[index*KF_NUM7+0][0] = tc.P4().Px();
      m_al_0[index*KF_NUM7+1][0] = tc.P4().Py();
      m_al_0[index*KF_NUM7+2][0] = tc.P4().Pz();
      m_al_0[index*KF_NUM7+3][0] = tc.P4().E();
      m_al_0[index*KF_NUM7+4][0] = tc.Pos().X();
      m_al_0[index*KF_NUM7+5][0] = tc.Pos().Y();
      m_al_0[index*KF_NUM7+6][0] = tc.Pos().Z();
      
      tmp_V_al_0.SetSub(index*KF_NUM7, it->GetCovMat1( tc.Cov7() ) );
      
      //Charge, Mass and a
      m_property[index][0] =  tc.GetCharge()/3;
      m_property[index][1] =  tc.P4().M(); //get the mass:Dummy value set by hand:Dipak
      m_property[index][2] = -KF_PHOTON_VELOCITY*m_magField*tc.GetCharge()/3;
      ++index;
      
    }
    delete it;
    //       //Error between the tracks : will be done later :Dipak
    //       if(m_correlationFlag == 1){
    // 	      m_errorFlag = m_setCorrelation();
    // 	      if(m_errorFlag != KF_NO_ERROR)return m_errorFlag;
    //       }
    m_V_al_0.ResizeTo(tmp_V_al_0);
    
    m_V_al_0 = tmp_V_al_0;
    m_al_1 = m_al_0;
    m_V_al_1.ResizeTo(KF_NUM7*m_trackNum,KF_NUM7*m_trackNum);
    m_D      = m_V_al_1.GetSub(0,0,0,KF_NUM7*m_trackNum-1); 
  }else{//m_fitIncludingVertex ==1
    
    unsigned index(0);
    TMatrixDSym tmp_V_al_0(KF_NUM7*m_trackNum+3);
    m_al_0.ResizeTo(KF_NUM7*m_trackNum+3,1);
    m_al_1.ResizeTo(KF_NUM7*m_trackNum+3,1);
    m_property.ResizeTo(m_trackNum,3);
    
    m_D.ResizeTo(1,KF_NUM7*m_trackNum+3);
    m_d.ResizeTo(1,1);
    m_V_D.ResizeTo(1,1);
    m_lam.ResizeTo(1,1);
    
    m_D_T.ResizeTo(KF_NUM7*m_trackNum+3,1);
    
    
    Int_t nTra = fDaughters.GetLength();
    
    it =new PndVtxFitterParticle;
    
    for(Int_t i=0;i<nTra;i++)
    {
      TCandidate tc=fDaughters[i];
      
      m_al_0[index*KF_NUM7+0][0] = tc.P4().Px();
      m_al_0[index*KF_NUM7+1][0] = tc.P4().Py();
      m_al_0[index*KF_NUM7+2][0] = tc.P4().Pz();
      m_al_0[index*KF_NUM7+3][0] = tc.P4().E();
      m_al_0[index*KF_NUM7+4][0] = tc.Pos().X();
      m_al_0[index*KF_NUM7+5][0] = tc.Pos().Y();
      m_al_0[index*KF_NUM7+6][0] = tc.Pos().Z();
      
      tmp_V_al_0.SetSub(index*KF_NUM7, it->GetCovMat1( tc.Cov7() ) );
      
      //Charge, Mass and a
      m_property[index][0] =  tc.GetCharge()/3;
      m_property[index][1] =  tc.P4().M(); //get the mass:Dummy value set by hand:Dipak
      m_property[index][2] = -KF_PHOTON_VELOCITY*m_magField*tc.GetCharge()/3;
      ++index;
      
    }
    delete it;
    
    m_al_0[KF_NUM7*m_trackNum+0][0] = m_vertex_b.X();
    m_al_0[KF_NUM7*m_trackNum+1][0] = m_vertex_b.Y();
    m_al_0[KF_NUM7*m_trackNum+2][0] = m_vertex_b.Z();
    tmp_V_al_0.SetSub(KF_NUM7*m_trackNum,it->GetErrVertexOut());
    //cout<<m_property[0][0]<<" m_property "<<m_property[0][2]<<" "<<m_property[1][2]<<endl;
    
    /***
     //   Dipak
     //error between track and track
     //setcorrelation function is in kmassfitter.cc
     //m_V_al_0 = tmp_V_al_0;
     if(m_correlationFlag == 1){
     m_errorFlag = m_setCorrelation();
     if(m_errorFlag != KF_NO_ERROR)return m_errorFlag;
     }
     *******/
    m_V_al_0.ResizeTo(tmp_V_al_0);
    m_V_al_1.ResizeTo(KF_NUM7*m_trackNum+3,KF_NUM7*m_trackNum+3);
    
    m_V_al_0 = tmp_V_al_0;
    
    m_al_1 = m_al_0;
    m_D      = m_V_al_1.GetSub(0,0,0,KF_NUM7*m_trackNum+3-1); 
  }
  return 0;
}

unsigned PndMassFitter::MakeCoreMatrix()
{
  if(m_fitIncludingVertex == 0)
  {
    TMatrixD al_1_prime(m_al_1);
    //cout<<"MAKE COREMATRIX *** "<<al_1_prime.GetNrows()<<" "<<al_1_prime.GetNcols()<<" "<<m_al_1.GetNrows()<<" "<<m_al_1.GetNcols()<<" "<<m_trackNum<<" "<<m_invariantMass<<endl;
    TMatrixD Sum_al_1(4,1);
    Double_t   *energy = new Double_t[m_trackNum];
    Double_t    a;
    
    for(int i=0;i<m_trackNum;++i){
      a = m_property[i][2];
      if(m_atDecayPoint == KF_NOT_DECAY_POINT)a = 0.;
      al_1_prime[i*KF_NUM7+0][0] -= a*(m_vertex_b.Y()-al_1_prime[i*KF_NUM7+5][0]);
      al_1_prime[i*KF_NUM7+1][0] += a*(m_vertex_b.X()-al_1_prime[i*KF_NUM7+4][0]);
      energy[i] = TMath::Sqrt(al_1_prime[i*KF_NUM7+0][0]*al_1_prime[i*KF_NUM7+0][0]+
                              al_1_prime[i*KF_NUM7+1][0]*al_1_prime[i*KF_NUM7+1][0]+
                              al_1_prime[i*KF_NUM7+2][0]*al_1_prime[i*KF_NUM7+2][0]+
                              m_property[i][1]*m_property[i][1]);
    }
    
    for(int i=0;i<m_trackNum;++i){
      if(m_isFixMass == (int)KF_FIX_MASS){
        Sum_al_1[3][0] += energy[i];
      }else{
        Sum_al_1[3][0] += al_1_prime[i*KF_NUM7+3][0];
      }
      for(unsigned jj=0;jj<3;++jj){
        Sum_al_1[jj][0] += al_1_prime[i*KF_NUM7+jj][0];
      }
    }
    m_d[0][0] = Sum_al_1[3][0]*Sum_al_1[3][0]-Sum_al_1[0][0]*Sum_al_1[0][0]
    -         Sum_al_1[1][0]*Sum_al_1[1][0]-Sum_al_1[2][0]*Sum_al_1[2][0]
    -         m_invariantMass*m_invariantMass; //Get the invariant mass properly
    
    for(int i=0;i<m_trackNum;++i){
      if(energy[i] == 0.){
        m_errorFlag = KF_DIV_ZERO;
        break;
      }
	    a = m_property[i][2];
	    if(m_atDecayPoint == KF_NOT_DECAY_POINT)a = 0.;
      
	    if(m_isFixMass == (int)KF_FIX_MASS){
	      Double_t invE = 1./energy[i];
	      m_D[0][i*KF_NUM7+0] = 2.*(Sum_al_1[3][0]*al_1_prime[i*KF_NUM7+0][0]*
                                  invE-Sum_al_1[0][0]);
	      m_D[0][i*KF_NUM7+1] = 2.*(Sum_al_1[3][0]*al_1_prime[i*KF_NUM7+1][0]*
                                  invE-Sum_al_1[1][0]);
	      m_D[0][i*KF_NUM7+2] = 2.*(Sum_al_1[3][0]*al_1_prime[i*KF_NUM7+2][0]*
                                  invE-Sum_al_1[2][0]);
	      m_D[0][i*KF_NUM7+3] = 0.;
	      m_D[0][i*KF_NUM7+4] =-2.*(Sum_al_1[3][0]*al_1_prime[i*KF_NUM7+1][0]*
                                  invE-Sum_al_1[1][0])*a;
	      m_D[0][i*KF_NUM7+5] = 2.*(Sum_al_1[3][0]*al_1_prime[i*KF_NUM7+0][0]*
                                  invE-Sum_al_1[0][0])*a;
	      m_D[0][i*KF_NUM7+6] = 0.;
	    }else{
	      m_D[0][i*KF_NUM7+0] = -2.*Sum_al_1[0][0];
	      m_D[0][i*KF_NUM7+1] = -2.*Sum_al_1[1][0];
	      m_D[0][i*KF_NUM7+2] = -2.*Sum_al_1[2][0];
	      m_D[0][i*KF_NUM7+3] =  2.*Sum_al_1[3][0];
	      m_D[0][i*KF_NUM7+4] =  2.*Sum_al_1[1][0]*a;
	      m_D[0][i*KF_NUM7+5] = -2.*Sum_al_1[0][0]*a;
	      m_D[0][i*KF_NUM7+6] =  0.;
	    }
	  }
	  delete [] energy;
  }else{
    
    TMatrixD al_1_prime(m_al_1);
    
    TMatrixD Sum_al_1(7,1);
    
    Double_t   *energy = new Double_t[m_trackNum];
    Double_t    a;
    //cout<<"MAKE COREMATRIX *** "<<al_1_prime.GetNrows()<<" "<<al_1_prime.GetNcols()<<" "<<m_al_1.GetNrows()<<" "<<m_al_1.GetNcols()<<" "<<m_trackNum<<" "<<m_invariantMass<<endl;
    for(int i=0;i<m_trackNum;++i){
      a = m_property[i][2];
      
      al_1_prime[i*KF_NUM7+0][0] -= a*(al_1_prime[KF_NUM7*m_trackNum+1][0]-
                                       al_1_prime[i*KF_NUM7+5][0]);
      al_1_prime[i*KF_NUM7+1][0] += a*(al_1_prime[KF_NUM7*m_trackNum+0][0]-
                                       al_1_prime[i*KF_NUM7+4][0]);
      energy[i] = TMath::Sqrt(al_1_prime[i*KF_NUM7+0][0]*al_1_prime[i*KF_NUM7+0][0]+
                              al_1_prime[i*KF_NUM7+1][0]*al_1_prime[i*KF_NUM7+1][0]+
                              al_1_prime[i*KF_NUM7+2][0]*al_1_prime[i*KF_NUM7+2][0]+
                              m_property[i][1]*m_property[i][1]);
      Sum_al_1[6][0] += a;
      
    }
    for(int i=0;i<m_trackNum;++i){
      if(energy[i] == 0.){
        m_errorFlag = KF_DIV_ZERO;
        break;
      }
      
      if(m_isFixMass == (int)KF_FIX_MASS){
        Double_t invE = 1./energy[i];
        Sum_al_1[3][0] += energy[i];
        Sum_al_1[4][0] += al_1_prime[i*KF_NUM7+1][0]*m_property[i][2]*invE;
        Sum_al_1[5][0] += al_1_prime[i*KF_NUM7+0][0]*m_property[i][2]*invE;
      }else{
        Sum_al_1[3][0] += al_1_prime[i*KF_NUM7+3][0];	
      }
      for(unsigned jj=0;jj<3;++jj){
        Sum_al_1[jj][0] += al_1_prime[i*KF_NUM7+jj][0];
      }
      
    }
    m_d[0][0] = Sum_al_1[3][0]*Sum_al_1[3][0]-Sum_al_1[0][0]*Sum_al_1[0][0]
    -         Sum_al_1[1][0]*Sum_al_1[1][0]-Sum_al_1[2][0]*Sum_al_1[2][0]
    -         m_invariantMass*m_invariantMass;
    
    for(int i=0;i<m_trackNum;++i){
      if(energy[i] == 0.){
        m_errorFlag = KF_DIV_ZERO;
        break;
      }
      a = m_property[i][2];
      if(m_isFixMass == (int)KF_FIX_MASS){
        Double_t invE = 1./energy[i];
        m_D[0][i*KF_NUM7+0] = 2.*(Sum_al_1[3][0]*al_1_prime[i*KF_NUM7+0][0]*
                                  invE-Sum_al_1[0][0]);
        m_D[0][i*KF_NUM7+1] = 2.*(Sum_al_1[3][0]*al_1_prime[i*KF_NUM7+1][0]*
                                  invE-Sum_al_1[1][0]);
        m_D[0][i*KF_NUM7+2] = 2.*(Sum_al_1[3][0]*al_1_prime[i*KF_NUM7+2][0]*
                                  invE-Sum_al_1[2][0]);
        m_D[0][i*KF_NUM7+3] = 0.;
        m_D[0][i*KF_NUM7+4] =-2.*(Sum_al_1[3][0]*al_1_prime[i*KF_NUM7+1][0]*
                                  invE-Sum_al_1[1][0])*a;
        m_D[0][i*KF_NUM7+5] = 2.*(Sum_al_1[3][0]*al_1_prime[i*KF_NUM7+0][0]*
                                  invE-Sum_al_1[0][0])*a;
        m_D[0][i*KF_NUM7+6] = 0.;
      }else{
        m_D[0][i*KF_NUM7+0] = -2.*Sum_al_1[0][0];
        m_D[0][i*KF_NUM7+1] = -2.*Sum_al_1[1][0];
        m_D[0][i*KF_NUM7+2] = -2.*Sum_al_1[2][0];
        m_D[0][i*KF_NUM7+3] =  2.*Sum_al_1[3][0];
        m_D[0][i*KF_NUM7+4] =  2.*Sum_al_1[1][0]*a;
        m_D[0][i*KF_NUM7+5] = -2.*Sum_al_1[0][0]*a;
        m_D[0][i*KF_NUM7+6] =  0.;
      }
    }
    m_D[0][KF_NUM7*m_trackNum+0] = 2.*(Sum_al_1[3][0]*Sum_al_1[4][0]-
                                       Sum_al_1[1][0]*Sum_al_1[6][0]);
    m_D[0][KF_NUM7*m_trackNum+1] =-2.*(Sum_al_1[3][0]*Sum_al_1[5][0]-
                                       Sum_al_1[0][0]*Sum_al_1[6][0]);
    m_D[0][KF_NUM7*m_trackNum+2] = 0.;
    delete [] energy;
    
  }
  
  return m_errorFlag;
}

/*unsigned PndMassFitter::SetOutputMatrix()
 {
 return m_errorFlag;
 }
 */
//Fill output to TCandidate
unsigned PndMassFitter::SetOutputToTCandidate()
{
	TVector3 h3v;
	//cout<<"# of Tracks are in TCandidate ************ "<<fDaughters.GetLength()<<endl;
	unsigned index1(0);
	TMatrixD trCov(7,7);
	
	TLorentzVector sum(0,0,0,0);
	
	
	for(Int_t i=0;i<fDaughters.GetLength();i++)
	{
    TLorentzVector lv;
		double px=m_al_1[index1*KF_NUM7+0][0];
		double py=m_al_1[index1*KF_NUM7+1][0];
		double pz=m_al_1[index1*KF_NUM7+2][0];
    double E =sqrt(fDaughters[i].Mass()*fDaughters[i].Mass()+px*px+py*py+pz*pz);
		
		if(!(m_isFixMass == (int)KF_FIX_MASS))
		{
		  E=m_al_1[index1*KF_NUM7+3][0];
		}
		
		//cout <<"SetOutputToTCandidate"<<px<<" "<<py<<" "<<pz<<" "<<E<<endl;
		
		lv.SetXYZT(px,py,pz,E);
		
		sum+=lv;
		++index1;
	}
	
	//cout <<"sum: "<<sum.Px()<<" "<<sum.Py()<<" "<<sum.Pz()<<" "<<sum.M()<<endl;
  fHeadOfTree->SetP4(sum);
	if(m_fitIncludingVertex == 0)fHeadOfTree->SetPos(m_vertex_b);
	else   fHeadOfTree->SetPos(TVector3(m_al_1[KF_NUM7*m_trackNum+0][0],m_al_1[KF_NUM7*m_trackNum+1][0],m_al_1[KF_NUM7*m_trackNum+2][0]));
	return m_errorFlag;
}


unsigned PndMassFitter::CalDgf()
{
  m_dgf = 1;
  return m_dgf;
}

void PndMassFitter::SetInvariantMass(Double_t m)
{m_invariantMass = m;}

void PndMassFitter::SetAtDecayPoint()
{m_atDecayPoint = KF_AT_DECAY_POINT;}

void PndMassFitter::SetNotDecayPoint()
{m_atDecayPoint = KF_NOT_DECAY_POINT;}

Double_t
PndMassFitter::GetInvariantMass()
{return m_invariantMass;}

unsigned PndMassFitter::GetDecayPoint()
{return m_atDecayPoint;}


