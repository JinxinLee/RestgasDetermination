//////////////////////////////////////////////////////////////////////////
//                                                                      //
// VAbsMicroCandidate	                                                //
//                                                                      //
// Definition of an abstract interface to a micro candidate.	        //
//                                                                      //
// Author: Sascha Berger and Marcel Kunze, RUB, March 1999		//
// Copyright (C) 1999-2001, Ruhr-University Bochum.			//
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TVector3.h"
#include "RhoBase/VAbsMicroCandidate.h"

ClassImp(VAbsMicroCandidate);

TBuffer &operator>>(TBuffer &buf, VAbsMicroCandidate *&obj)
{
   obj = (VAbsMicroCandidate *) buf.ReadObject(VAbsMicroCandidate::Class());
   return buf;
}

#include <iostream>
using namespace std;

void VAbsMicroCandidate::PrintOn(std::ostream &o) const 
{
    o << " ======= Fit Params ======= " << endl; 
    TVector3 r = GetPosition();
    TVector3 p = GetMomentum();
	o << " Position              : (" << r.X() << ";" << r.Y() << ";" << r.Z() << ")" << endl;
	o << " Momentum              : (" << p.X() << ";" << p.Y() << ";" << p.Z() << ")" << endl;
	o << " Momentum.mag          : " << p.Mag() << endl;
	o << " Charge                : " << GetCharge() << endl;
	o << " Energy                : " << GetEnergy() << endl;
    
    if (GetCharge() != 0) {
	o << " ======= Track Quality ======= " 
	  << "\n Fit quality         : Ndof " << GetDegreesOfFreedom()<< " chi2 " << GetChiSquared() 
	  << "\n number of hits      : MVD  " << GetMvdHits()<< " Stt " << GetSttHits()  
	  << endl;//"\n track length        : " << GetTrackLength()<<endl; 
    }
    
    o << " ======= Calor Neutral Quality ======= " << endl ; 
    o << " number of chrystals : " << GetEmcNumberOfCrystals()<< " , " <<GetEmcNumberOfBumps()<<endl; 
    o << " raw energy          : " << GetEmcRawEnergy()<<endl; 
    o << " calibrated energy   : " << GetEmcCalEnergy()<<endl;
        
    if (GetCharge() != 0) {
	o << " ======= Pid Quality ======= " << endl ; 
	o << " Fit Quality         : Nphot DRC Barrel " << GetDrcNumberOfPhotons()
			                      << " DRC Disc " << GetDiscNumberOfPhotons()
			                      << " RICH " << GetRichNumberOfPhotons()<<endl; 
	o << " Cherenkov angle     :  Barrel " << GetDrcThetaC() << " +- " << GetDrcThetaCErr()
			                  <<" Disc "<< GetDiscThetaC() << " +- " << GetDiscThetaCErr()
			                  <<" RICH "<< GetRichThetaC() << " +- " << GetRichThetaCErr()<<endl; 
	o << " dEdx                :  MVD " << GetMvdDEDX() 
		                     << " STT " << GetSttMeanDEDX() << endl; 
   } 
} 
/*
void VAbsMicroCandidate::PrintOn(std::ostream &o) const 
{
    o << " ======= FitParams ======= " << endl; 
    TVector3 r = GetPosition();
    TVector3 p = GetMomentum();
	o << " Position              : (" << r.X() << ";" << r.Y() << ";" << r.Z() << ")" << endl;
	o << " Momentum              : (" << p.X() << ";" << p.Y() << ";" << p.Z() << ")" << endl;
	o << " Momentum.mag          : " << p.Mag() << endl;
	o << " Charge                : " << GetCharge() << endl;
	o << " Energy                : " << GetEnergy() << endl;
    
    if (GetCharge() != 0) {
	o << " ======= BtaTrkQual ======= " 
	  << "\n Fit quality         : Ndof " << GetDegreesOfFreedom()<< " chi2 " << GetChiSquared() 
	  << "\n number of hits      : Dch  " << GetDchHits()<< " SVT " << GetSvtHits() 
	  << "\n range in Dch        : " << GetDchInnerLayer()<<" - "<<GetDchOuterLayer()<<endl; 
    }
    
    o << " ======= BtaCalQual ======= " << endl ; 
    o << " lateral shape       : " << GetEmcS9S25()<< " , " <<GetEmcS1S9() << " , " << 
				    GetEmcLateralShape()<< " , " <<GetEmcSecondMoment() <<endl; 
    o << " Zernike 42 31 20    : " << GetEmcZernikeMoment42() << " , " << GetEmcZernikeMoment31() << " , " << GetEmcZernikeMoment20() << endl; 
    o << " number of chrystals : " << GetEmcNumberOfCrystals()<< " , " <<GetEmcNumberOfBumps()<<endl; 
    o << " raw energy          : " << GetEmcRawEnergy()<<endl; 
    o << " calibrated energy   : " << GetEmcCalEnergy()<<endl;
    o << " status   : " << hex<<GetEmcStatus()<<dec<<endl;
    o << " Emc-track match     : " << GetEmcConsistencyValue()<<endl; 
    o << " centroid            : " << GetEmcCentroid().X()<<'\t'<<GetEmcCentroid().Y()<<'\t'<<GetEmcCentroid().Z()<<endl; 
    o << " Covariance          : " << GetEmcCovarianceTheta()<<'\t'<<GetEmcCovariancePhi()<<'\t'<<GetEmcCovarianceRho()<<'\t'<<GetEmcCovarianceEnergy()<<endl; 
    
    if (GetIfrPattern()!=0) {
	o << " ======= BtaIfrQual ======= " << endl ; 
	if(GetIfrPattern() & 1) o<< " Has Inner " <<endl; 
	if(GetIfrPattern() & 2) o<< " Has Barrel " <<endl; 
	if(GetIfrPattern() & 8) o<< " Has BackWard " <<endl; 
	if(GetIfrPattern() & 4) o<< " Has ForWard " <<endl; 
	o<< " match trk-ifr " << GetIfrTrackMatch() << endl;
	o<< " match emc-ifr " << GetIfrEmcMatch() << endl;
	o<< " number of layers (Inner + IFR) "<<GetIfrHitLayers()<<endl; 
	o<< " number of strips "<<GetIfrNumberOfStrips()<<endl; 
	o<< " expected penetration  " << GetIfrExpectedInteractionLengths() << endl; 
	o<< " penetration EMC " << GetIfrInteractionLengthsBeforeIron() << endl; 
	o<< " measured penetration " << GetIfrMeasuredInteractionLengths() << endl; 
	o<< " log10 RPC mu probability " << GetIfrRpcMuonProbability() << endl;
	o<< " first and last hit layers: " << GetIfrFirstHit() << " - " << GetIfrLastHit() <<endl; 
	for(int i=0;i<20;i++)  
	    o<< " strips in layer " << i << ": " << GetIfrNumberOfStrips(i) << endl; 
    }
    
    if (GetCharge() != 0) {
	o << " ======= BtaPidQual ======= " << endl ; 
	o << " Fit Quality         : Nphot " << GetDrcNumberOfPhotons()<< " Nbkgd " << GetDrcNumberOfBackgroundPhotons()<<endl; 
	o << " Cherenkov angle     : " << GetDrcThetaC()<<" +- "<<GetDrcThetaCError()<<endl; 
	o << " dEdx                : DCH " << GetDchMeanDEdx() << " SVT " << GetSvtMeanDEdx() << endl; 
	o << " mom in middle of flight  :" << GetDchDeltaMomentum()<< endl; 
	o << " Samples             : DCH " << GetDchSamples() << " SVT " << GetSvtSamples() << endl; 
    } 
} 

*/
std::ostream&  operator << (std::ostream& o, const VAbsMicroCandidate& a) { a.PrintOn(o); return o; }
