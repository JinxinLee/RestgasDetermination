//////////////////////////////////////////////////////////////////////////
//                                                                      //
// PndMicroCandidate	                                                //
//                                                                      //
// Definition of the Panda micro candidate.	                            //
//                                                                      //
// Author: Klaus Goetzen, GSI, 12.06.08		                            //
// Copyright (C) 2008, GSI Darmstadt.									//
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "RhoBase/PndMicroCandidate.h"

//class VAbsPidInfo;

//  ========================================================================
//  ===== PndMicroCandidate - Class definig the AOD interface           ====
//  ========================================================================

PndMicroCandidate::PndMicroCandidate() :
	fLocked(false),
    fCharge(0),		
    fXposition(0),		
	fYposition(0),		
	fZposition(0),		
	fXmomentum(0),		
	fYmomentum(0),		
	fZmomentum(0),		
	fEnergy(0),		
    fFirstHitX(0),		
	fFirstHitY(0),		
	fFirstHitZ(0),		
    fLastHitX(0),		
	fLastHitY(0),		
	fLastHitZ(0),		
	fMcIndex(-1),
	fMvdMeanDEdx(0),
	fMvdDEdxErr(0),
	fMvdHits(0),
    fSttMeanDEdx(0),
	fSttDEdxErr(0),
	fSttHits(0),
	fTpcMeanDEdx(0),
	fTpcDEdxErr(0),
	fTpcHits(0),
	fTofM2(0),
	fTofM2Err(0),
	fBarrelDrcThetaC(0),
    fBarrelDrcThetaCErr(0),
    fBarrelDrcNumberOfPhotons(0),
	fDiscDrcThetaC(0),
    fDiscDrcThetaCErr(0),
    fDiscDrcNumberOfPhotons(0),
	fRichThetaC(0),
    fRichThetaCErr(0),
    fRichNumberOfPhotons(0),
	fEmcRawEnergy(0),
    fEmcCalEnergy(0),
    fEmcNumberOfCrystals(0),
    fEmcNumberOfBumps(0),
	fMuoNumberOfLayers(0),
	fMuoProbability(0),
	fTrackLength(0),
    fDegreesOfFreedom(0),
    fFitStatus(0),
	fProbability(0),
	fChiSquared(0)
{	
	fMvdHitIndexArray.Reset();
	fSttHitIndexArray.Reset();
	fTpcHitIndexArray.Reset();
	
    int i;
    for (i=0; i<28;i++) fErrP7[i] = 0;
    for (i=0; i<5;i++) fParams[i] = 0;
    for (i=0; i<15;i++) fCov[i] = 0;
}

PndMicroCandidate::PndMicroCandidate(Int_t charge, TVector3& pos, TLorentzVector& p4, TMatrixD& cov7 ) :
	fLocked(false),
    fCharge(charge),		
    fFirstHitX(0),		
	fFirstHitY(0),		
	fFirstHitZ(0),		
    fLastHitX(0),		
	fLastHitY(0),		
	fLastHitZ(0),		
	fMcIndex(-1),
	fMvdMeanDEdx(0),
	fMvdDEdxErr(0),
	fMvdHits(0),
    fSttMeanDEdx(0),
	fSttDEdxErr(0),
	fSttHits(0),
	fTpcMeanDEdx(0),
	fTpcDEdxErr(0),
	fTpcHits(0),
	fTofM2(0),
	fTofM2Err(0),
	fBarrelDrcThetaC(0),
    fBarrelDrcThetaCErr(0),
    fBarrelDrcNumberOfPhotons(0),
	fDiscDrcThetaC(0),
    fDiscDrcThetaCErr(0),
    fDiscDrcNumberOfPhotons(0),
	fRichThetaC(0),
    fRichThetaCErr(0),
    fRichNumberOfPhotons(0),
	fEmcRawEnergy(0),
    fEmcCalEnergy(0),
    fEmcNumberOfCrystals(0),
    fEmcNumberOfBumps(0),
	fMuoNumberOfLayers(0),
	fMuoProbability(0),
	fTrackLength(0),
    fDegreesOfFreedom(0),
    fFitStatus(0),
	fProbability(0),
	fChiSquared(0)
{
	fMvdHitIndexArray.Reset();
	fSttHitIndexArray.Reset();
	fTpcHitIndexArray.Reset();
	
    int i;
    for (i=0; i<5;i++) fParams[i] = 0;
    for (i=0; i<15;i++) fCov[i] = 0;
	
	SetPosition(pos);
	SetLorentzVector(p4);
	SetCov7(cov7);
}

PndMicroCandidate::PndMicroCandidate(Int_t charge, TVector3& pos, TLorentzVector& p4) :
	fLocked(false),
    fCharge(charge),		
    fFirstHitX(0),		
	fFirstHitY(0),		
	fFirstHitZ(0),		
    fLastHitX(0),		
	fLastHitY(0),		
	fLastHitZ(0),		
	fMcIndex(-1),
	fMvdMeanDEdx(0),
	fMvdDEdxErr(0),
	fMvdHits(0),
    fSttMeanDEdx(0),
	fSttDEdxErr(0),
	fSttHits(0),
	fTpcMeanDEdx(0),
	fTpcDEdxErr(0),
	fTpcHits(0),
	fTofM2(0),
	fTofM2Err(0),
	fBarrelDrcThetaC(0),
    fBarrelDrcThetaCErr(0),
    fBarrelDrcNumberOfPhotons(0),
	fDiscDrcThetaC(0),
    fDiscDrcThetaCErr(0),
    fDiscDrcNumberOfPhotons(0),
	fRichThetaC(0),
    fRichThetaCErr(0),
    fRichNumberOfPhotons(0),
	fEmcRawEnergy(0),
    fEmcCalEnergy(0),
    fEmcNumberOfCrystals(0),
    fEmcNumberOfBumps(0),
	fMuoNumberOfLayers(0),
	fMuoProbability(0),
	fTrackLength(0),
    fDegreesOfFreedom(0),
    fFitStatus(0),
	fProbability(0),
	fChiSquared(0)
{
	fMvdHitIndexArray.Reset();
	fSttHitIndexArray.Reset();
	fTpcHitIndexArray.Reset();
	
    int i;
    for (i=0; i<28;i++) fErrP7[i] = 0;
    for (i=0; i<5;i++) fParams[i] = 0;
    for (i=0; i<15;i++) fCov[i] = 0;
	
	SetPosition(pos);
	SetLorentzVector(p4);
}

	
PndMicroCandidate::~PndMicroCandidate()
{
}

TMatrixD& PndMicroCandidate::Cov7() const
{
    static TMatrixD cov(7,7);
    
    // position error
    
    cov(0,0) = fErrP7[0];  cov(1,0) = fErrP7[1]; cov(1,1) = fErrP7[2];  
    cov(2,0) = fErrP7[3];  cov(2,1) = fErrP7[4]; cov(2,2) = fErrP7[5];
    
    // position-momentum covariance
    
    cov(3,0) = fErrP7[6];   cov(3,1) = fErrP7[7];  cov(3,2) = fErrP7[8];
    cov(4,0) = fErrP7[9];   cov(4,1) = fErrP7[10]; cov(4,2) = fErrP7[11];
    cov(5,0) = fErrP7[12];  cov(5,1) = fErrP7[13]; cov(5,2) = fErrP7[14];
    cov(6,0) = fErrP7[15];  cov(6,1) = fErrP7[16]; cov(6,2) = fErrP7[17];
    
    // momentum error
    cov(3,3) = fErrP7[18];  cov(4,3) = fErrP7[19]; cov(4,4) = fErrP7[20];
    cov(5,3) = fErrP7[21];  cov(5,4) = fErrP7[22]; cov(5,5) = fErrP7[23];
    cov(6,3) = fErrP7[24];  cov(6,4) = fErrP7[25]; cov(6,5) = fErrP7[26]; 
    cov(6,6) = fErrP7[27];
    
    for (int i=0; i<6; i++)
	    for (int j=i+1; j<7; j++)
		    cov(i,j)=cov(j,i);
   
    return cov;
}

TMatrixD& PndMicroCandidate::P4Cov() const
{
    static TMatrixD covP4(4,4);
	
    covP4(0,0) = fErrP7[18];  covP4(1,0) = fErrP7[19]; covP4(1,1) = fErrP7[20];
    covP4(2,0) = fErrP7[21];  covP4(2,1) = fErrP7[22]; covP4(2,2) = fErrP7[23];
    covP4(3,0) = fErrP7[24];  covP4(3,1) = fErrP7[25]; covP4(3,2) = fErrP7[26]; 
    covP4(3,3) = fErrP7[27];
	
    for (int i=0; i<3; i++)
	    for (int j=i+1; j<4; j++)
		    covP4(i,j)=covP4(j,i);
			
    return covP4;
}

void PndMicroCandidate::SetLorentzVector(TLorentzVector p4)
{
    fXmomentum = p4.X();
    fYmomentum = p4.Y();
    fZmomentum = p4.Z();
    fEnergy    = p4.T();	
}

void PndMicroCandidate::SetCov7(const TMatrixD& cov7 )
{
    // position error
    
    fErrP7[0] = cov7(0,0);  fErrP7[1] = cov7(1,0); fErrP7[2] = cov7(1,1);  
    fErrP7[3] = cov7(2,0);  fErrP7[4] = cov7(2,1); fErrP7[5] = cov7(2,2);
    
    // position-momentum covariance
    
    fErrP7[6] = cov7(3,0);   fErrP7[7] = cov7(3,1);  fErrP7[8] = cov7(3,2);
    fErrP7[9] = cov7(4,0);   fErrP7[10] = cov7(4,1); fErrP7[11] = cov7(4,2);
    fErrP7[12] = cov7(5,0);  fErrP7[13] = cov7(5,1); fErrP7[14] = cov7(5,2);
    fErrP7[15] = cov7(6,0);  fErrP7[16] = cov7(6,1); fErrP7[17] = cov7(6,2);
    
    // momentum error
    fErrP7[18] = cov7(3,3);  fErrP7[19] = cov7(4,3); fErrP7[20] = cov7(4,4);
    fErrP7[21] = cov7(5,3);  fErrP7[22] = cov7(5,4); fErrP7[23] = cov7(5,5);
    fErrP7[24] = cov7(6,3);  fErrP7[25] = cov7(6,4); fErrP7[26] = cov7(6,5); 
    fErrP7[27] = cov7(6,6);
}

void PndMicroCandidate::SetP4Cov(const TMatrixD& covP4 )
{
    // position error
    
    fErrP7[0] = 0;  fErrP7[1] = 0; fErrP7[2] = 0;  
    fErrP7[3] = 0;  fErrP7[4] = 0; fErrP7[5] = 0;
    
    // position-momentum covariance
    
    fErrP7[6] = 0;   fErrP7[7] = 0;  fErrP7[8] = 0;
    fErrP7[9] = 0;   fErrP7[10] = 0; fErrP7[11] = 0;
    fErrP7[12] = 0;  fErrP7[13] = 0; fErrP7[14] = 0;
    fErrP7[15] = 0;  fErrP7[16] = 0; fErrP7[17] = 0;
    
    // momentum error
    fErrP7[18] = covP4(0,0);  fErrP7[19] = covP4(1,0); fErrP7[20] = covP4(1,1);
    fErrP7[21] = covP4(2,0);  fErrP7[22] = covP4(2,1); fErrP7[23] = covP4(2,2);
    fErrP7[24] = covP4(3,0);  fErrP7[25] = covP4(3,1); fErrP7[26] = covP4(3,2); 
    fErrP7[27] = covP4(3,3);
}


