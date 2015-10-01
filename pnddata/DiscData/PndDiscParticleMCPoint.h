//-------------------------------------------------------------------------
// Author:      Oliver Merle (Oliver.Merle@exp2.physik.uni-giessen.de)
// Changes:     Mustafa Schmidt (Mustafa.A.Schmidt@physik.uni-giessen.de)
// Date:        30.11.2015
// Description: Information of charged particle tracks
//-------------------------------------------------------------------------

#ifndef PNDDISCPARTICLEMCPOINT_HH
#define PNDDISCPARTICLEMCPOINT_HH


#include "FairMCPoint.h"


class PndDiscParticleMCPoint : public FairMCPoint
{
public:
    PndDiscParticleMCPoint();
    // trackID, detID, posin, momin, tof, length, eLoss
    PndDiscParticleMCPoint(Int_t track_id,      // base
                           //Int_t event_id,      // in FairMCEventHeader
                           Int_t det_id,          // base
                           Int_t volume_id,       // -- use some kind of volume identifier here (radiator, lightguide etc) (PndGeoHandling short id??)
                           TVector3 pos_in,       // base
                           TVector3 mom_in,       // base
                           TVector3 pos_out,      // -- makes sense
                           TVector3 mom_out,      // -- makes sense
                           Double_t total_tof,    // base
                           Double_t total_length, // base
                           Double_t energy_loss,
                           Double_t charge,
                           Double_t mass,
                           Int_t    pdgcode,
                           Bool_t   pos_in_inside, // whether the particle was entering the volume at pos_in
                           Bool_t   is_primary
                           );


    virtual ~PndDiscParticleMCPoint();


    virtual void Print(const Option_t* opt = 0) const; /// FairMCPoint forces the implementation


    Int_t volume_id;
    TVector3 pos_out;
    TVector3 mom_out;
    Double_t charge;
    Double_t mass;
    Int_t pdgCode;
    Bool_t pos_in_inside;
    Bool_t is_primary;

    Double_t my_energy_loss;

    ClassDef(PndDiscParticleMCPoint,2)
};

#endif // PNDDISCPARTICLEMCPOINT_HH
