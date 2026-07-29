#include "TFile.h"
#include "TTree.h"
#include "TClonesArray.h"
#include "TVector3.h"
#include "TLorentzVector.h"

//#include "PndTrack.h"
#include "FairTrackParP.h"

#include "RhoCandidate.h"
#include "RhoCandList.h"
#include "RhoVtxPoca.h"

void ConvertTrackToRhoCandList(TString prefix = "barrel") {
    TFile* file = TFile::Open(prefix + "_reco.root");
    if (!file || file->IsZombie()) {
        std::cerr << "Couldn't open the file!" << std::endl;
        return;
    }

    // get the TTree
    TTree* tree = (TTree*)file->Get("pndsim");  // Modify according to your file structure
    TClonesArray* finalGenTrackArray = new TClonesArray("PndTrack");
    tree->SetBranchAddress("FinalGenTrack", &finalGenTrackArray);

    // Create RhoCandList
    RhoCandList trackList;

    // Loop over events
    Long64_t nEvents = tree->GetEntries();
    for (Long64_t i = 0; i < nEvents; ++i) {
        tree->GetEntry(i);

        Int_t nTracks = finalGenTrackArray->GetEntries();
        for (Int_t j = 0; j < nTracks; ++j) {
            PndTrack* track = (PndTrack*)finalGenTrackArray->At(j);
            if (!track) continue;

            // Extract track parameters
            FairTrackParP par = track->GetParamFirst();  // Use starting point
            TVector3 pos(par.GetX(), par.GetY(), par.GetZ());
            TVector3 mom(par.GetPx(), par.GetPy(), par.GetPz());
            Double_t charge = par.GetQ();

            // Construct 4-momentum
            Double_t mass = 0.13957;  // Assume pi+, modify as needed
            Double_t energy = sqrt(mom.Mag2() + mass * mass);
            TLorentzVector p4(mom, energy);

            // Construct RhoCandidate
            RhoCandidate* cand = new RhoCandidate(p4, charge);
            cand->SetPos(pos);
            cand->SetType(211);  // Set as pi+, modify as needed

            // Optional: set covariance matrix if available
            // cand->SetCov7(...);

            // Add to list
            trackList.Put(cand);
        }

        // Vertex estimation
        TVector3 vertex;
        RhoVtxPoca pocaTool;
        Double_t quality = pocaTool.GetPocaVtx(vertex, trackList);

        std::cout << "Event " << i << " vertex estimate: (" 
                  << vertex.X() << ", " << vertex.Y() << ", " << vertex.Z() 
                  << "), quality = " << quality << std::endl;

        trackList.Clear();  // Clear list for next event
    }

    file->Close();
    delete finalGenTrackArray;
}

