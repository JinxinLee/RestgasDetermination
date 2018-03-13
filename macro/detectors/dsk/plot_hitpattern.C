/**

  This will plot the hit pattern, where the abscissa is the detector number
  as calced for PndDskDetector::fDetNumber and the ordinate is the time of flight
  PndDskDetector::fTime - fDetTime,

**/


/// theWhat: 1 = primary, 2 = secondary, 3 = both

//void plot_hitpattern(TString theRunName = "test", Int_t theWhat = 1, Int_t fEventMin = 0, Int_t fEventMax = -2)
int plot_hitpattern()
{

  TString theRunName = "test";
  Int_t theWhat = 3;
  Int_t fEventMin = 0;
  Int_t fEventMax = -1;
  Bool_t   fSecondaries = kTRUE;

  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  rootlogon();
  basiclibs();
  gSystem->Load("libDsk");

  TString       fTitle = "Hit Pattern: Pion 4GeV 15deg - hits: ";


//   TString fFileName = TString(fPath).Append("sim_dsk.").Append(fRunName).Append(".root");
  TString fFileName = TString("sim_dsk.").Append(theRunName).Append(".root");
  TFile *fFile = 0;
  fFile = gROOT->GetFile(fFileName);
  if (fFile == 0)
    fFile = new TFile(fFileName);

  TTree *fTree = (TTree *)fFile->Get("pndsim");
  fTree->SetBranchStatus("*",0);
  fTree->SetBranchStatus("DskCerenkov.*",1);
  fTree->SetBranchStatus("DskParticle.*",1);

  Int_t fNofEvents = fTree->GetEntries();

  TClonesArray *cArray = 0;
  cArray = new TClonesArray("PndDskCerenkov");
  fTree->SetBranchAddress("DskCerenkov",&cArray);
  PndDskCerenkov *fCerenkov = 0;

  TClonesArray *pArray = 0;
  pArray = new TClonesArray("PndDskParticle");
  fTree->SetBranchAddress("DskParticle",&pArray);
  PndDskParticle *fParticle = 0;


  if (fEventMax == -2) fEventMax = fEventMin;
  if (fEventMax == -1) fEventMax = fNofEvents-1;

//   const Int_t fArraySize = 600 * ( fEeventMax - fEeventMin +1);

  const Int_t fArraySize = 1000*600;
  Double_t priDetNum[fArraySize];
  Double_t priTof[fArraySize];
  Double_t secDetNum[fArraySize];
  Double_t secTof[fArraySize];

  Int_t    pris = 0;
  Int_t    secs = 0;

  printf("Will process events %i to %i\n",fEventMin,fEventMax);

  // select event
  for (Int_t j=fEventMin; j<=fEventMax; j++) {

    fTree->GetEntry(j);
    printf("Processing event %i\n",j);


    // first, find primary particle
    Bool_t searching = kTRUE;
    TIter pIter(pArray);
    PndDskParticle* pEntry;
    while ( searching && (pEntry = (PndDskParticle *)pIter.Next()) ) {
      if (pEntry->GetTrackID() == 0 )
      searching = false;
    }

    // now the Cerenkovs
    PndDskCerenkov* cEntry = 0;
    TIter cIter(cArray);
    while ( (cEntry = (PndDskCerenkov*)cIter.Next()) ) {

      if ( cEntry->GetDetTime() > 0. ) { // is detected

        if ( cEntry->GetMotherTrackID() == 0  && theWhat != 2)  { // is primary
          priDetNum[pris] = cEntry->GetDetNumber()+1;
          priTof[pris]    = cEntry->GetDetTime() - pEntry->GetTime();
          pris++;
          cout << "added primary "
               << cEntry->GetTrackID()
               << " with " << cEntry->GetWavelength()
               << " in " << cEntry->GetDetNumber() << endl;
        } else if (theWhat > 1) { // is from secondary
          secDetNum[secs] = cEntry->GetDetNumber()+1;
          secTof[secs]    = cEntry->GetDetTime() - pEntry->GetTime();
          secs++;
          cout << "added secondary (" << cEntry->GetMotherPdgCode() << ") "
               << cEntry->GetTrackID()
               << " with " << cEntry->GetWavelength()
               << " in " << cEntry->GetDetNumber() << endl;
        }
      }
    }
  }

//   delete cArray;
//   delete pArray;


  TMultiGraph *mg = new TMultiGraph();
  char priStr[3]; sprintf(priStr,"%i",pris);
  char secStr[3]; sprintf(secStr,"%i",secs);
  mg->SetTitle(TString(fTitle).Append(priStr).Append(". From Secondaries: ").Append(secStr));

  // now plot, the real hits
  TGraph* grPris = new TGraph(pris, priDetNum, priTof);
  grPris->SetMarkerColor(2);
  grPris->SetFillStyle(0);
  mg->Add(grPris);

  // hits from secondary particle cerenkovs
  if (fSecondaries && secs>0) {
    TGraph* grSecs = new TGraph(secs, secDetNum, secTof);
    grSecs->SetMarkerColor(4);
    grSecs->SetFillStyle(0);
    mg->Add(grSecs);
  }

  mg->Draw("A*");

  mg->GetXaxis()->SetTitle("detector number");
  mg->GetYaxis()->SetTitle("time of flight [ns]");

//   c1->Update();

  cout << "This is Event No. " << fEventMin << ". Detected Cerenkovs: " << pris << " from primary, " << secs << " from secondaries." << endl;
  return 0;
}


