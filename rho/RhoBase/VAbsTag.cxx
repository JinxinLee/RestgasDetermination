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

#include "RhoBase/VAbsTag.h"
#include "RhoBase/VAbsBit.h"

ClassImp(VAbsTag)

TBuffer &operator>>(TBuffer &buf, VAbsTag *&obj)
{
   obj = (VAbsTag *) buf.ReadObject(VAbsTag::Class());
   return buf;
}

#include <iomanip>
using namespace std;

//handling of the TAG

VAbsTag::VAbsTag() {}
VAbsTag::~VAbsTag() {}

void VAbsTag::PrintOn(std::ostream& o) const {
    o<<endl;
    o <<"############################ Tracking information ###########################" << endl;
    o << "Number of tracks                     : " << (Int_t) GetNumberOfTracks() << endl;
    o << "Number of good tracks loose & tight  : " << (Int_t) GetNumberOfGoodTracksLoose()  << '\t' << (Int_t) GetNumberOfGoodTracksTight() << endl;
    o << "Number of neutrals                   : " << (Int_t) GetNumberOfNeutralCands() << endl;
    o << "Number of neutrals loose & tight     : " << (Int_t) GetNumberOfNeutralCndLoose()  << '\t' << (Int_t) GetNumberOfNeutralCndTight() << endl;
    o << "Total momentum                       : " << GetTotalMomentum().X() << '\t' <<GetTotalMomentum().Y() << '\t' << GetTotalMomentum().Z() << '\t' << endl;
    o << "Total momentum scalar                : " << GetPTotalScalar() << endl;
    o << "Total energy                         : " << GetTotalEnergy() << endl;
    o << "Total mass                           : " << GetTotalMass() << endl;
    o << "Highest momentum track               : " << GetPrimaryTrackMomentum().X() << '\t' << GetPrimaryTrackMomentum().Y() << '\t' << GetPrimaryTrackMomentum().Z() << '\t' << endl;
    o << "  Shower energy                      : " << GetPrimaryTrackShowerEnergy() << endl;
    o << "  Particle hypothesis                : " << (Int_t) GetPrimaryTrackHypothesis() << endl;
    o << "2nd highest momentum track           : " << GetSecondaryTrackMomentum().X() << '\t' << GetSecondaryTrackMomentum().Y() << '\t' << GetSecondaryTrackMomentum().Z() << '\t' << endl;
    o << "  Shower energy                      : " << GetSecondaryTrackShowerEnergy() << endl;
    o << "  Particle hypothesis                : " << (Int_t) GetSecondaryTrackHypothesis() << endl;
    o << "Total charge loose and tight         : " << (Int_t) GetTotalChargeLoose() << '\t' << (Int_t) GetTotalChargeTight() << endl;
    o <<"########################## Calorimeter information #####################" << endl;
    o << "Total neutral energy                 : " << GetTotalNeutralEnergy().X() << '\t' << GetTotalNeutralEnergy().Y() << '\t' << GetTotalNeutralEnergy().Z() << '\t' << endl;
    o << "Highest energy shower                : " << GetHighestEnergyShower().X() << '\t' << GetHighestEnergyShower().Y() << '\t' << GetHighestEnergyShower().Z() << '\t' << endl;
    o << "2nd highest energy shower            : " << GetSecondHighestEnergyShower().X() << '\t' << GetSecondHighestEnergyShower().Y() << '\t' << GetSecondHighestEnergyShower().Z() << '\t' << endl;
    o <<"########################### Vertex and shape ###############################" << endl;
    o << "Primary vertex                       : " << GetPrimaryVertex().X() << '\t' << GetPrimaryVertex().Y() << '\t' << GetPrimaryVertex().Z() << '\t' << endl;
    o << "Thrust                               : " << GetThrust().X() << '\t' << GetThrust().Y() << '\t' << GetThrust().Z() << '\t' << endl;
    o << "ThrustAll                            : " << GetThrustAll().X() << '\t' << GetThrustAll().Y() << '\t' << GetThrustAll().Z() << '\t' << endl;
    o << "R2                                   : " << GetR2() << endl;
    o << "R2All                                : " << GetR2All() << endl;
    o << "SphericityAll                        : " << GetSphericityAll() << endl;
    o <<"########################### Particle numbers ############################" << endl;
    o << "Electrons loose and tight            : " << (Int_t) GetElectronsLoose() << '\t' << (Int_t) GetElectronsTight() << endl;
    o << "Muons loose and tight                : " << (Int_t) GetMuonsLoose() << '\t' << (Int_t) GetMuonsTight() << endl;
    //o << "Pions loose and tight                : " << (Int_t) GetPionsLoose() << '\t' << (Int_t) GetPionsTight() << endl;
    o << "Kaons loose and tight                : " << (Int_t) GetKaonsLoose() << '\t' << (Int_t) GetKaonsTight() << endl;
    o << "Protons loose and tight              : " << (Int_t) GetProtonsLoose() << '\t' << (Int_t) GetProtonsTight() << endl;
    o << "Gammas loose and tight               : " << (Int_t) GetGammasLoose() << '\t' << (Int_t) GetGammasTight() << endl;
    o << "Merged pi0s loose and tight          : " << (Int_t) GetMergedPi0Loose() << '\t' << (Int_t) GetMergedPi0Tight() << endl;
    o << "Ks loose and tight                   : " << (Int_t) GetKsLoose() << '\t' << (Int_t) GetKsTight() << endl;
    o <<"###################################################################" << endl;
    o << endl;
} 

std::ostream&  operator << (std::ostream& o, const VAbsTag& a) { a.PrintOn(o); return o; }

