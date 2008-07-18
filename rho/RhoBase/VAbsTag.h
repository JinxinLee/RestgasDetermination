#ifndef VABSTAG_H
#define VABSTAG_H
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// VAbsTag								//
//                                                                      //
// Abstract Definition of the TAG database				//
//                                                                      //
// Author: Marcel Kunze, RUB, Nov. 99					//
// Copyright (C) 1999-2001, Ruhr-University Bochum.			//
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <iostream>

#include "TObject.h"
#include "TLorentzVector.h"

class VAbsBitBoolIterator;
class VAbsBitIntIterator;
class VAbsBitFloatIterator;

class VAbsTag : public TObject {   

public:
    VAbsTag();
    virtual ~VAbsTag();
    virtual Int_t	GetNumberOfTracks() const = 0;
    virtual Int_t	GetNumberOfGoodTracksLoose() const = 0;
    virtual Int_t	GetNumberOfGoodTracksTight() const = 0;
    virtual TVector3    GetTotalMomentum() const = 0;
    virtual Float_t	GetTotalEnergy() const = 0;
    virtual Float_t	GetTotalMass() const = 0;
    virtual TVector3    GetPrimaryTrackMomentum() const = 0;
    virtual Float_t	GetPrimaryTrackShowerEnergy() const = 0;
    virtual Int_t	GetPrimaryTrackHypothesis() const = 0;
    virtual TVector3    GetSecondaryTrackMomentum() const= 0;
    virtual Float_t	GetSecondaryTrackShowerEnergy() const = 0;
    virtual Int_t	GetSecondaryTrackHypothesis() const = 0;
    virtual Int_t	GetTotalChargeLoose() const = 0;
    virtual Int_t	GetTotalChargeTight() const = 0;
    // Lepton Information
    virtual Int_t	GetPrimaryLeptonHypothesisLoose() const = 0;
    virtual Int_t	GetSecondaryLeptonHypothesisLoose() const = 0;
    virtual Int_t	GetPrimaryLeptonHypothesisTight() const = 0;
    virtual Int_t	GetSecondaryLeptonHypothesisTight() const = 0;
    virtual Int_t	GetPrimaryLeptonChargeLoose() const = 0;
    virtual Int_t	GetSecondaryLeptonChargeLoose() const = 0;
    virtual Int_t	GetPrimaryLeptonChargeTight() const = 0;
    virtual Int_t	GetSecondaryLeptonChargeTight() const = 0;
    virtual Float_t	GetPrimaryLeptonMomentumLoose() const = 0;
    virtual Float_t	GetSecondaryLeptonMomentumLoose() const = 0;
    virtual Float_t	GetPrimaryLeptonMomentumTight() const = 0;
    virtual Float_t	GetSecondaryLeptonMomentumTight() const = 0;
    // Calorimeter information
    virtual TVector3    GetTotalNeutralEnergy() const = 0;
    virtual TVector3    GetHighestEnergyShower() const = 0;
    virtual TVector3    GetSecondHighestEnergyShower() const = 0;
    // Vertex and shape
    virtual TVector3	GetPrimaryVertex() const = 0;
    virtual Float_t	GetPrimaryVertexProbability() const = 0;
    virtual TVector3	GetThrust() const = 0;
    virtual Float_t	GetR2() const = 0;   
    // Particle numbers
    virtual Int_t	GetLeptonsLoose() const = 0;
    virtual Int_t	GetElectronsLoose() const = 0;
    virtual Int_t	GetMuonsLoose() const = 0;
    virtual Int_t	GetPionsLoose() const = 0;
    virtual Int_t	GetKaonsLoose() const = 0;
    virtual Int_t	GetProtonsLoose() const = 0;
    virtual Int_t	GetGammasLoose() const = 0;
    virtual Int_t	GetMergedPi0Loose() const = 0;
    virtual Int_t	GetPi0Loose() const = 0;
    virtual Int_t	GetKsLoose() const = 0;
    virtual Int_t	GetLeptonsTight() const = 0;
    virtual Int_t	GetElectronsTight() const = 0;
    virtual Int_t	GetMuonsTight() const = 0;
    virtual Int_t	GetPionsTight() const = 0;
    virtual Int_t	GetKaonsTight() const = 0;
    virtual Int_t	GetProtonsTight() const = 0;
    virtual Int_t	GetGammasTight() const = 0;
    virtual Int_t	GetMergedPi0Tight() const = 0;
    virtual Int_t	GetPi0Tight() const = 0;
    virtual Int_t	GetKsTight() const = 0;
    // Trigger and Skim information
    virtual Float_t	GetBbqqDiscr() const = 0; 
    virtual Float_t	GetBGFR2() const = 0;
    virtual Float_t	GetBunchT0DeltaDircTrk() const = 0;
    virtual Float_t	GetBunchT0DeltaFF() const = 0; 
    virtual Float_t	GetBunchT0FinderErr() const = 0; 
    virtual Float_t	GetBunchT0FitterErr() const = 0;
    virtual Float_t	GetECharShow() const = 0; 
    virtual Float_t	GetElecLoose1cm() const = 0;
    virtual Float_t	GetElecLoose1l() const = 0;
    virtual Float_t	GetElecLoose2cm() const = 0; 
    virtual Float_t	GetElecLoose2l() const = 0; 
    virtual Float_t	GetElecTight1cm() const = 0;
    virtual Float_t	GetElecTight1l() const = 0; 
    virtual Float_t	GetElecTight2cm() const = 0; 
    virtual Float_t	GetElecTight2l() const = 0;
    virtual Float_t	GetENeutShow() const = 0;
    virtual Float_t	GetL3OutBhabhaFlatOprValue() const = 0; 
    virtual Float_t	GetL3OutBhabhaFlatValue() const = 0;
    virtual Float_t	GetMuonLoose1cm() const = 0; 
    virtual Float_t	GetMuonLoose1l() const = 0; 
    virtual Float_t	GetMuonLoose2cm() const = 0; 
    virtual Float_t	GetMuonLoose2l() const = 0; 
    virtual Float_t	GetMuonTight1cm() const = 0; 
    virtual Float_t	GetMuonTight1l() const = 0; 
    virtual Float_t	GetMuonTight2cm() const = 0; 
    virtual Float_t	GetMuonTight2l() const = 0;
    virtual Float_t	GetPTotalScalar() const = 0;
    virtual Float_t	GetR2All() const = 0;
    virtual Float_t	GetSphericityAll() const = 0; 
    virtual Float_t	GetTagLooseProton_maxCmsP() const = 0; 
    virtual Float_t	GetTagTightProton_maxCmsP() const = 0;
    virtual Float_t	GetThrustCosThAll() const = 0; 
    virtual Float_t	GetThrustMagAll() const = 0; 
    virtual Float_t	GetThrustPhiAll() const = 0;
	TVector3 GetThrustAll() const {return TVector3(GetThrustCosThAll(),GetThrustPhiAll(),GetThrustMagAll()); } 
    virtual Int_t	GetD3PS() const = 0; 
    virtual Int_t	GetDmeson0() const = 0; 
    virtual Int_t	GetDsmeson0() const = 0; 
    virtual Int_t	GetDstar0() const = 0; 
    virtual Int_t	GetDstar1() const = 0; 
    virtual Int_t	GetLumiBhabhaWeight() const = 0; 
    virtual Int_t	GetNumberOfNeutralCands() const = 0; 
    virtual Int_t	GetNumberOfNeutralCndLoose() const = 0; 
    virtual Int_t	GetNumberOfNeutralCndTight() const = 0; 
    virtual Int_t	GetTagEtaPeppTwoBody_maxCmsPmaxCosT() const = 0; 
    virtual Int_t	GetTagEtaPrhogTwoBody_maxCmsPmaxCosT() const = 0; 
    virtual Int_t	GetTagEtaTwoBody_maxCmsPmaxCosT() const = 0; 
    virtual Int_t	GetTagKstpTwoBody_maxCmsPmaxCosT() const = 0; 
    virtual Int_t	GetTagKstTwoBody_maxCmsPmaxCosT() const = 0; 
    virtual Int_t	GetTagKTwoBody_maxCmsPmaxCosT() const = 0; 
    virtual Int_t	GetTagK_S0TwoBody_maxCmsPmaxCosT() const = 0; 
    virtual Int_t	GetTagOmegaTwoBody_maxCmsPmaxCosT() const = 0; 
    virtual Int_t	GetTagPhiTwoBody_maxCmsPmaxCosT() const = 0; 
    virtual Int_t	GetTagPi0TwoBody_maxCmsPmaxCosT() const = 0; 
    virtual Int_t	GetTagRhopTwoBody_maxCmsPmaxCosT() const = 0; 
    virtual Int_t	GetTagRhoTwoBody_maxCmsPmaxCosT() const = 0; 
    
    virtual Int_t GetInt(const char* key) const = 0;
    virtual Float_t GetFloat(const char* key) const = 0;
    virtual Bool_t GetBool(const char* key) const = 0;

    virtual char getInt(Int_t &anInt,const char* key) const = 0;
    virtual char getFloat(Float_t &aFloat,const char* key) const = 0;
    virtual char getBool(char &aBool,const char* key) const = 0;

    // Store values and dynamically create new branches:
    virtual char putBool( char, const char* ) = 0;
    virtual char putInt( Int_t, const char* ) = 0;
    virtual char putFloat( Float_t, const char* ) = 0;

    //###################################################################    

    void PrintOn( std::ostream& o=std::cout ) const;

public:
    ClassDef(VAbsTag,1) //Abstract base class for TAG
	
};      

// standalone print
std::ostream&  operator <<(std::ostream& o, const VAbsTag&);

#endif
