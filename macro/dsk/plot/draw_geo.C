/**

  @brief Draws TGeoTracks in standard graphics or opengl

  you can choose to use only one event, or all
  a range within the trackIDs oder select
  a wavelength interval

  @author Peter Koch
  @date   2008-03-29
  @since  2007-12-23

**/

gROOT->Reset();

// use Sellmeier equation to calculate RefracGlass
const Double_t B1 = 0.67071081;
const Double_t C1 = 0.00449192312e6;
const Double_t B2 = 0.433322857;
const Double_t C2 = 0.0132812976e6;
const Double_t B3 = 0.877379057;
const Double_t C3 = 95.8899878e6;

Double_t RefIdx(Double_t Lambda) {
  Double_t x = Lambda*Lambda;
  return TMath::Sqrt(1.+B1*x/(x-C1)+B2*x/(x-C2)+B3*x/(x-C3));
}

void draw_geo(void)
{
  // switches and ranges
  Bool_t   USE_OPENGL   = false;
  Bool_t   SELECT_BY_ID = true;
  Bool_t   TRACK_ID     = false;       // false means Particle-ID (fStatusCode)
  Bool_t   CKOV         = true;
  Bool_t   OTHER        = false;
  Bool_t   NO_TRACKS    = false;

  Int_t    fEvent_min = 0;            // the event that should be drawn. First is 0
  Int_t    fEvent_max = 0;            // -1 means last event
  Double_t fRange_min = 111;          // This is the particleID or trackID or wavelength
  Double_t fRange_max = 120;          // -1 means last track
  Int_t    fAmount    = -1;           // -1 means all
  Int_t    fDet_min = 0;
  Int_t    fDet_max = 0;

  if (!SELECT_BY_ID) {
    // physical constants needed
    Double_t fConst_c = 299792458.;
    Double_t fConst_e = 1.602176487E-19;
    Double_t fConst_h = 6.6260693E-34;
    Double_t fnm2ev   = fConst_h*fConst_c/fConst_e;

    // we gave wavelength, but we compare energies, and its easier to calculate 2
    // energies than to calculate for each track the lambda!
    Double_t tmp;
    tmp        = fnm2ev/RefIdx(fRange_min)/fRange_min;
    fRange_min = fnm2ev/RefIdx(fRange_max)/fRange_max;
    fRange_max = tmp;
  }

  TString  fRootFile = "sim_dsk.root";

  // libs
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libField");
  gSystem->Load("libGen");
  gSystem->Load("libPassive");
  gSystem->Load("libPndBase");
  gSystem->Load("libGeom.so");
  gSystem->Load("libDsk");

  // select trees in RootFile
  TFile* f = new TFile(fRootFile); // the sim file you want to analyse
  TTree* tT = f->Get("cbmsim");    // for the tracks
//   TTree* tP = f->Get("cbmsim");    // for the points

  // get the tracks
  TClonesArray* fT = new TClonesArray("TGeoTrack");
  tT->SetBranchAddress("GeoTracks",&fT);

//   // get the DskPoints
//   TClonesArray* fP = new TClonesArray("PndDskPoint");
//   tP->SetBranchAddress("DskPoint",&fP);

  // access to GeoManager
  TGeoManager* geoMan = (TGeoManager*) f->Get("FAIRGeom");
  TObjArray*   TrList= geoMan->GetListOfTracks();
  TGeoTrack*   tr;
//   PndDskPoint* dp;
  Int_t        pdgc     = 0;
  TParticle*   particle = 0;
//   Double_t     point    = 0;   /// @todo no idea what type point shoudl have. at least not Double_t
//   TVector      point;
  Double_t     lambda   = 0;
  Double_t     energy   = 0;
  Int_t        counter  = 0;
  Int_t        fTrackID    = 0;
  Int_t        fParticleID = 0;  // this is actually fStatusCode !

  if (USE_OPENGL) {
    geoMan->GetTopVolume()->Draw("ogl");
  } else {
    geoMan->GetTopVolume()->Draw("same");
  }
  geoMan->GetVolume("cave")->SetVisibility(kFALSE);
  geoMan->GetVolume("dsk01")->SetVisibility(kFALSE);
  geoMan->GetVolume("dsk02")->SetVisibility(kTRUE);
  geoMan->GetVolume("dsk19")->SetVisibility(kFALSE);
  geoMan->GetVolume("dsk20")->SetVisibility(kFALSE);
  geoMan->GetVolume("dsk21")->SetVisibility(kFALSE);
  geoMan->GetVolume("dsk22")->SetVisibility(kFALSE);
//   geoMan->SetVisLevel(6);
//   geoMan->SetVisOption(0);
//   geoMan->SetTopVisible();


  // we will exit teh script here, if we dont need the tracks
  if (NO_TRACKS)
    return;


  // select events
  if ((fEvent_max<0) || (fEvent_max>tT->GetEntriesFast()))
    fEvent_max = tT->GetEntriesFast();

  clog << "-D- found " << tT->GetEntriesFast() << " event(s). We will draw " << fEvent_max - fEvent_min +1<< "." << endl;

  // we will select by TrackID Range
  if (SELECT_BY_ID) {
//     cout << "Select by ID" << endl;
    // for all selected events
    for (Int_t jt=fEvent_min; jt<fEvent_max+1; jt++) {
//       cout << "Getting track " << jt << endl;
      // select this event
      tT->GetEntry(jt);
      // for all tracks in this event
      clog << "-D- found " << fT->GetEntriesFast() << " track(s)." << endl;
      for (Int_t it=0; it<fT->GetEntriesFast(); it++) {
        // select a track
        tr=(TGeoTrack *)fT->At(it);
        // get the particle for this track
        particle = (TParticle *)tr->GetParticle();
        fTrackID    = tr->GetId();
        fParticleID = particle->GetStatusCode();
//         cout << fTrackID << " " << fParticleID << endl;
        // is this on of the tracks that we want to see?
        if (  ( TRACK_ID && ((fTrackID    >= fRange_min) && (fTrackID    <= fRange_max)))
            ||(!TRACK_ID && ((fParticleID >= fRange_min) && (fParticleID <= fRange_max))) ) {
//           cout << "we will plot this one" << endl;
          // get pdgc
          pdgc = particle->GetPdgCode();
          if (( CKOV && (pdgc != 50000050)) || ( OTHER && (pdgc == 50000050))) {
            clog << "-D- track with track-id " << fTrackID << " and particle-id " << fParticleID << " not painted cause it is a pdg " << pdgc << endl;
          } else {
            clog << "-D- drawing track with track-id " << fTrackID << " and particle-id " << fParticleID << " as a " << pdgc << " with " << tr->GetNpoints() << " points" << endl;
            // use opengl
            if (USE_OPENGL) {
              // draw tracks by hand
              Int_t Np = tr->GetNpoints();
              FairVTrack *pt = new FairVTrack(Np);
              pt->SetLineColor(tr->GetLineColor());
              pt->SetLineWidth(2);
              pt->SetTrack(tr);
              pt->SetParticle((TParticle *)tr->GetParticle());
              for (Int_t n=0; n<Np; n++){
                point=tr->GetPoint(n);
                pt->SetPoint(n,point[0],point[1],point[2]);
              }
              pt->Draw("SAME");
            // simple graphic
            } else {
              tr->Draw("same");
              TrList->AddLast(tr);
            }
          }
        }
      }
    }
   // we will select wavelength interval
   } else {
    // for all selected events
    for (Int_t jt=fEvent_min; jt<fEvent_max+1; jt++) {
//       cout << "Getting track " << jt << endl;
      // select this event
      tT->GetEntry(jt);
      // for all tracks in this event
      clog << "-D- found " << fT->GetEntriesFast() << " track(s)." << endl;
      for (Int_t it=0; it<fT->GetEntriesFast(); it++) {
        // select a track
        tr=(TGeoTrack *)fT->At(it);
        // get the particle for this track
        particle = (TParticle *)tr->GetParticle();
        fTrackID    = tr->GetId();
        fParticleID = particle->GetStatusCode();
        pdgc = particle->GetPdgCode();
        energy = particle->Energy();
        // is this on of the tracks that we want to see?
        if ( (!(CKOV && OTHER)) && (( CKOV && (pdgc != 50000050)) || ( OTHER && (pdgc == 50000050)))) {
          clog << "-D- track with track-id " << fTrackID << " and particle-id " << fParticleID << " not painted cause it is a pdg " << pdgc << endl;
        } else {
          // now check if we want to draw this track
          // we draw, if:
          // 1) particle is not a CKOV and we want them too
          // 2) it is a CKOV and the wavelength is in the given range
          if (
            (OTHER && (pdgc != 50000050)) ||
            ((pdgc == 50000050) && ((energy >= fRange_min) && (energy <= fRange_max)))
          ) {
            counter++;
            if ( (fAmount < 0) || (counter <= fAmount) ) {
              // now we can plot this track
              clog << "-D- drawing track with track-id " << fTrackID << " and particle-id " << fParticleID << " as a " << pdgc << " with " << tr->GetNpoints() << " points" << endl;
              // use opengl
              if (USE_OPENGL) {
                // draw tracks by hand
                Int_t Np = tr->GetNpoints();
                FairVTrack *pt = new FairVTrack(Np);
                pt->SetLineColor(tr->GetLineColor());
                pt->SetLineWidth(2);
                pt->SetTrack(tr);
                pt->SetParticle((TParticle *)tr->GetParticle());
                for (Int_t n=0; n<Np; n++){
                  point=tr->GetPoint(n);
                  pt->SetPoint(n,point[0],point[1],point[2]);
                }
                pt->Draw();
              // simple graphic
              } else {
                tr->Draw("same");
                TrList->AddLast(tr);
              }
            }
          }
        }
      }
    }
  }
}
