//-------------------------------------------------------------------------
// Author:      Oliver Merle (Oliver.Merle@exp2.physik.uni-giessen.de)
// Changes:     Mustafa Schmidt (Mustafa.A.Schmidt@physik.uni-giessen.de)
// Date:        30.11.2015
// Description: Information of charged particle tracks
//-------------------------------------------------------------------------

#include "PndDiscParticleMCPoint.h"


#include <iostream>




ClassImp(PndDiscParticleMCPoint)




PndDiscParticleMCPoint::PndDiscParticleMCPoint() : FairMCPoint(),
volume_id(0),
pos_out(0.,0.,0.),
mom_out(0.,0.,0.),
charge(0.),
mass(0.),
pdgCode(0),
pos_in_inside(false),
is_primary(false),
my_energy_loss(0.)
{
}


PndDiscParticleMCPoint::PndDiscParticleMCPoint(Int_t track_id_, Int_t det_id_, Int_t volume_id_, TVector3 pos_in_,
                       TVector3 mom_in_, TVector3 pos_out_, TVector3 mom_out_,
                       Double_t total_tof_, Double_t total_length_, Double_t energy_loss_,
                       Double_t charge_, Double_t mass_, Int_t pdgCode_, Bool_t pos_in_inside_,
                       Bool_t is_primary_)

    : FairMCPoint(track_id_, det_id_, pos_in_, mom_in_, total_tof_, total_length_, energy_loss_),
      volume_id(volume_id_), pos_out(pos_out_), mom_out(mom_out_), charge(charge_), mass(mass_),
      pdgCode(pdgCode_), my_energy_loss(energy_loss_), pos_in_inside(pos_in_inside_), 
      is_primary(is_primary_)
{

}


PndDiscParticleMCPoint::~PndDiscParticleMCPoint() {}


void PndDiscParticleMCPoint::Print(const Option_t *opt) const
{
    std::cout << "-I- PndDiscParticleMCPoint"
              << "  fTrackID      " << fTrackID << std::endl
              << "  fDetectorID   " << fDetectorID << std::endl
              << "  Position in   " << fX  << "; " << fY  << "; " << fZ  << std::endl
              << "  Momentum in   " << fPx << "; " << fPy << "; " << fPz << std::endl
              << "  Position out  " << pos_out.x() << "; " << pos_out.y() << "; " << pos_out.z() << std::endl
              << "  Momentum out  " << mom_out.x() << "; " << mom_out.y() << "; " << mom_out.z() << std::endl
              << "  fTime         " << fTime << std::endl
              << "  Length        " << fLength << std::endl
              << "  fELoss        " << fELoss << std::endl
              << "  volume_id     " << volume_id << std::endl
              << "  charge        " << charge << std::endl
              << "  mass          " << mass << std::endl
              << "  pdgCode       " << pdgCode << std::endl
              << "  Pos in inside " << pos_in_inside << std::endl
              << "  is primary    " << is_primary << std::endl
              ;
}
