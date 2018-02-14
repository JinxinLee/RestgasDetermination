
int draw_track(TString the_RunName = "test", Bool_t do_plot = kTRUE)
{

  if (do_plot) {
    cout << "--I--  Cerenkovs will be plotted." << endl;
  } else {
    cout << "--I--  Cerenkovs will be listed." << endl;
  }


  TString  fRootFile = TString("sim_dsk.").Append(the_RunName).Append(".root");

  // libs
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  rootlogon();
  basiclibs();
  gSystem->Load("libDsk");

  // select trees in RootFile
  TFile* fFile = new TFile(fRootFile); // the sim file you want to analyse
  TTree* fTree = fFile->Get("cbmsim");    // for the tracks

  fTree->SetBranchStatus("*",0);

  /// @todo support for particles is not yet implemented.
//   TClonesArray* aryParticles = 0;
//   aryParticles = new TClonesArray("PndDskParticle");
//   fTree->SetBranchAddress("DskParticle",&aryParticles);
//   fTree->SetBranchStatus("DskParticle.*",1);
//   PndDskParticle* fParticle = 0;

  fTree->SetBranchStatus("DskCerenkov.*",1);
  TClonesArray* aryCerenkov = 0;
  aryCerenkov = new TClonesArray("PndDskCerenkov");
  fTree->SetBranchAddress("DskCerenkov",&aryCerenkov);
  PndDskCerenkov* fCerenkov = 0;

  fTree->SetBranchStatus("DskTrackPoints.*",1);
  TClonesArray* aryTrackPoints = 0;
  aryTrackPoints = new TClonesArray("PndDskTrackPoint");
  fTree->SetBranchAddress("DskTrackPoints",&aryTrackPoints);
  PndDskTrackPoint* fTrackPoint = 0;

  Int_t fNofEvents = fTree->GetEntries();
  Int_t fStepCnt; // count the steps for a track

  Int_t const maxPos = 1000; // ommit all tracks with more steps
  Double_t posX[maxPos];
  Double_t posY[maxPos];
  Double_t posZ[maxPos];

  if (do_plot) {
  // access to GeoManager
  TGeoManager* geoMan = (TGeoManager*) fFile->Get("FAIRGeom");
  // draw detector
  geoMan->GetTopVolume()->Draw("ogl");
  }

  // for all events
  for (Int_t ev=0; ev<=fTree->GetEntriesFast(); ev++) {

    // get entry
    fTree->GetEntry(ev);

    /// <<<<< select the events <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
    if ((
          // every event
//           ev > -1
          // a special event
            ev == 0
        )
       ) {
    /// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

      // for all cerenkovs in event
      for (Int_t iC=0; iC<aryCerenkov->GetEntriesFast(); iC++) {

        // get the current photon
        fCerenkov = (PndDskCerenkov*)aryCerenkov->At(iC);
        /// <<<<< select the events <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

          // plot all detected photons
          if ( fCerenkov->GetDetTime() > 0. ) {

//         if (    (fCerenkov->GetDetNumber() ==  4)
//              && (fCerenkov->GetDetTime() < 3.3)
//            ) {


//         if (((
//               (fCerenkov->GetDetTime() > 0.730)
//             &&(fCerenkov->GetDetTime() < 0.850)
//             ) || (
//               (fCerenkov->GetDetTime() > 2.700)
//             &&(fCerenkov->GetDetTime() < 2.750)
//             ) || (
//               (fCerenkov->GetDetTime() > 3.250)
//             &&(fCerenkov->GetDetTime() < 3.350)
//             )
//           ) && (
//               (fCerenkov->GetDetNumber() !=  3)
//           )) {
//           if ( (fCerenkov->GetDetNumber() > 1) && (fCerenkov->GetDetNumber() < 5) && (fCerenkov->GetNofReflections() < 100)  ) {
        /// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

          printf("ev: %4d  id: %5d  time: %6.3lf  det: %4d\n",
                 ev,
                 fCerenkov->GetTrackID(),
                 fCerenkov->GetDetTime(),
                 fCerenkov->GetDetNumber()
                );

          if (do_plot) {
          // now get all TrackPoints to the selected TrackID
          fStepCnt = 0;
          for (Int_t iT=0; iT<aryTrackPoints->GetEntriesFast(); iT++) {
 
            fTrackPoint = (PndDskTrackPoint*)aryTrackPoints->At(iT);
            if (fStepCnt>=maxPos) {
              cout <<   "id = " << fCerenkov->GetTrackID()
                   << "  omitted, cause it uses more than 10000 points!"
                   << endl;
              break;
            }
            if (fTrackPoint->GetTrackID() == fCerenkov->GetTrackID()) {
              posX[fStepCnt] = fTrackPoint->GetX();
              posY[fStepCnt] = fTrackPoint->GetY();
              posZ[fStepCnt] = fTrackPoint->GetZ();
              fStepCnt++;
            }

          }
          // and draw it
          if (fStepCnt > 0) { // should actually always be the case as the photon exists ...
            FairVTrack *pt = new FairVTrack(fStepCnt);
            if (fCerenkov->GetMotherTrackID() == 0) {
              pt->SetLineColor(kRed);
            } else {
              pt->SetLineColor(kBlue);
            }
            pt->SetLineWidth(2);
            for (Int_t n=0; n<fStepCnt; n++){
              pt->SetPoint(n,posX[n],posY[n],posZ[n]);
            }
            pt->Draw("SAME");
          }
          }
        }
      }
    }
  }
  return 0;
}
