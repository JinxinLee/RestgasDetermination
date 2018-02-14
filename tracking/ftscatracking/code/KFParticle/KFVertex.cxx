//----------------------------------------------------------------------------
// Implementation of the KFVertex class
// .
// @author  S.Gorbunov, I.Kisel
// @version 1.0
// @since   13.05.07
// 
// Class to reconstruct and store primary and secondary vertices
// The method is described in CBM-SOFT note 2007-003, 
// ``Reconstruction of decayed particles based on the Kalman filter'', 
// http://www.gsi.de/documents/DOC-2007-May-14-1.pdf
//
// This class is ALICE interface to general mathematics in KFParticleCore
// 
//  -= Copyright &copy ALICE HLT Group =-
//____________________________________________________________________________


#include "KFVertex.h"


KFVertex::KFVertex( const KFPVertex &vertex ): fIsConstrained(0)
{
  // Constructor from ALICE VVertex

  vertex.GetXYZ( fP );
  vertex.GetCovarianceMatrix( fC );  
  fChi2 = vertex.GetChi2();  
  fNDF = 2*vertex.GetNContributors() - 3;
  fQ = 0;
  fAtProductionVertex = 0;
  fIsLinearized = 0;
  fSFromDecay = 0;
}

/*
void     KFVertex::Print(Option_t* ) const
{  
  cout<<"KFVertex position:    "<<GetX()<<" "<<GetY()<<" "<<GetZ()<<endl;
  cout<<"KFVertex cov. matrix: "<<GetCovariance(0)<<endl;
  cout<<"                         "<<GetCovariance(1)<<" "<<GetCovariance(2)<<endl;
  cout<<"                         "<<GetCovariance(3)<<" "<<GetCovariance(4)<<" "<<GetCovariance(5)<<endl;
}
  */

void KFVertex::SetBeamConstraint( float x, float y, float z, 
				     float errX, float errY, float errZ )
{
  // Set beam constraint to the vertex
  fP[0] = x;
  fP[1] = y;
  fP[2] = z;
  fC[0] = errX*errX;
  fC[1] = 0;
  fC[2] = errY*errY;
  fC[3] = 0;
  fC[4] = 0;
  fC[5] = errZ*errZ;
  fIsConstrained = 1;
}

void KFVertex::SetBeamConstraintOff()
{
  fIsConstrained = 0;
}

void KFVertex::ConstructPrimaryVertex( const KFParticle *vDaughters[], 
					  int nDaughters, Bool_t vtxFlag[],
					  float ChiCut  )
{
  //* Primary vertex finder with simple rejection of outliers

  if( nDaughters<2 ) return;
  float constrP[3]={static_cast<float>(fP[0]), static_cast<float>(fP[1]), static_cast<float>(fP[2])};
  float constrC[6]={static_cast<float>(fC[0]), static_cast<float>(fC[1]), static_cast<float>(fC[2]),
                    static_cast<float>(fC[3]), static_cast<float>(fC[4]), static_cast<float>(fC[5])};

  Construct( vDaughters, nDaughters, 0, -1, fIsConstrained );

  SetVtxGuess( fVtxGuess[0], fVtxGuess[1], fVtxGuess[2] );

  for( int i=0; i<nDaughters; i++ ) vtxFlag[i] = 1;

  Int_t nRest = nDaughters;
  while( nRest>2 )
  {    
    float worstChi = 0.;
    Int_t worstDaughter = 0;
    for( Int_t it=0; it<nDaughters; it++ ){
      if( !vtxFlag[it] ) continue;	
      const KFParticle &p = *(vDaughters[it]);
      KFVertex tmp = *this - p;
      float chi = p.GetDeviationFromVertex( tmp );      
      if( worstChi < chi ){
        worstChi = chi;
        worstDaughter = it;
      }
    }
    if( worstChi < ChiCut ) break;
      
    vtxFlag[worstDaughter] = 0;    
    *this -= *(vDaughters[worstDaughter]);
    nRest--;
  } 

  if( nRest>=2 ) {// final refit     
    SetVtxGuess( fP[0], fP[1], fP[2] );
    if( fIsConstrained ){
      fP[0] = constrP[0];
      fP[1] = constrP[1];
      fP[2] = constrP[2];
      for( int i=0; i<6; i++ ) fC[i] = constrC[i];
    }
    int nDaughtersNew=0;
    const KFParticle **vDaughtersNew=new const KFParticle *[nDaughters];
    for( int i=0; i<nDaughters; i++ ){
      if( vtxFlag[i] )  vDaughtersNew[nDaughtersNew++] = vDaughters[i];
    }
    Construct( vDaughtersNew, nDaughtersNew, 0, -1, fIsConstrained );
    delete[] vDaughtersNew;
  }

  if( nRest<=2 && GetChi2() > ChiCut*ChiCut*GetNDF() ) {
    for( int i=0; i<nDaughters; i++ ) vtxFlag[i] = 0;
    fNDF = -3;
    fChi2 = 0;
  }
}
