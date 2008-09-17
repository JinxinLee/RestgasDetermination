{
#include <vector>
  //  gROOT->Reset();
  TStopwatch timer;
  timer.Start();
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  
  // Load this example libraries
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libMCStack");
  gSystem->Load("libField");
  gSystem->Load("libPassive");
  //gSystem->Load("libMvd");
  gSystem->Load("libEmc");
  gSystem->Load("libDrcProp");
  gSystem->Load("libDrc");
  gSystem->Load("libGen");
  //   gSystem->Load("libTrkBase");
  //   gSystem->Load("libGeane");
  //   //  gSystem->Load("libPGen");
  gSystem->Load("libStt");
  //   gSystem->Load("libgenfit");
  //   gSystem->Load("libtrackrep");
  //   gSystem->Load("libSttReco");
  
  // MCpoints
  TFile filerun("testrun.root");
  TTree *treepnt = (TTree*) filerun.Get("cbmsim");
  TClonesArray *pnt = new TClonesArray("PndSttPoint");
  treepnt->SetBranchAddress("STTPoint",&pnt);
    
  // Hits
  TFile filedigi("testdigi.root");
  TTree *treedigi = (TTree*) filedigi.Get("cbmsim");
  TClonesArray *digi = new TClonesArray("PndSttHit");
  treedigi->SetBranchAddress("STTHit",&digi);

  TH1F *samplenum = new TH1F("samplenum","number of samplings",50,0.,50.);
  TH2F *hdedxvsp = new TH2F("hdedxvsp","dedx vs p",100,0.,3., 100,0.,40.);
  
  TH2F *hdedxvsp_p = new TH2F("hdedxvsp_p","dedx vs p for p",100,0.,3., 100,0.,40.);
  TH2F *hdedxvsp_pi = new TH2F("hdedxvsp_pi","dedx vs p for #pi",100,0.,3., 100,0.,40.);
  TH2F *hdedxvsp_e = new TH2F("hdedxvsp_e","dedx vs p for e",100,0.,3., 100,0.,40.);
  TH2F *hdedxvsp_k = new TH2F("hdedxvsp_k","dedx vs p for K",100,0.,3., 100,0.,40.);
  
  cout << treedigi->GetEntriesFast() << " events" << endl;
  // loop on evts
  for(Int_t evt = 0; evt < treedigi->GetEntriesFast(); evt++) {
  if(evt%100 == 0) cout << evt << endl;
  treepnt->GetEntry(evt);
  treedigi->GetEntry(evt);
  
  Int_t trackid = -1;
  Int_t hitcounter = 0;
  Int_t total = 0;
  std::vector<double> dedxvec;
  dedxvec.clear();

  // hits loop
  for (Int_t k = 0; k < digi->GetEntriesFast(); k++) {

    PndSttHit *hit = (PndSttHit*) digi->At(k);
    if(!hit) continue;
    PndSttPoint *point = (PndSttPoint*) pnt->At(hit->GetRefIndex());
  
    TVector3 *momentum;
    Double_t mass;

    // trackID
    if(point->GetTrackID() == trackid)
      {
	dedxvec.push_back(hit->GetdEdx());
	hitcounter++;
	total++;
      }

    if ((point->GetTrackID() != trackid) || total == digi->GetEntriesFast()){
      total++;
    

      if(hitcounter > 0) {
	// truncated mean
	Double_t perc = 0.50;
	// sort
	std::sort(dedxvec.begin(), dedxvec.end());
      
	//truncated mean
	Double_t sum = 0;
	Int_t endnum = floor(hitcounter * perc);

	for(Int_t m = 0; m < endnum; m++) sum += dedxvec[m];

	Double_t tmean;
	if(endnum > 0) {
	  tmean = sum/(Double_t) endnum;
	  hdedxvsp->Fill(momentum->Mag(), tmean);
	  samplenum->Fill(hitcounter);
	  if(mass > 0.0001 && mass < 0.0006) hdedxvsp_e->Fill(momentum->Mag(), tmean);
	  else if(mass > 0.1 && mass < 0.2)  hdedxvsp_pi->Fill(momentum->Mag(), tmean);
	  else if(mass > 0.3 && mass < 0.5)  hdedxvsp_k->Fill(momentum->Mag(), tmean);
	  else if(mass > 0.8 && mass < 1.1)  hdedxvsp_p->Fill(momentum->Mag(), tmean);

	}
      }

      // new track
      dedxvec.clear();
      hitcounter = 0;
      trackid = point->GetTrackID();
      dedxvec.push_back(hit->GetdEdx());
      hitcounter++;
      momentum = new TVector3(point->GetPx(), point->GetPy(), point->GetPz());
      mass = point->GetMass();

      
    }
  }
  }
  TCanvas c;
  c->Divide(1,2);
  c->cd(1);
  hdedxvsp->Draw();
  c->cd(2);
  samplenum->Draw();

  TCanvas c1;
  c1->Divide(2,2);
  //  hdedxvsp->Draw();
  c1->cd(1);
  hdedxvsp_e->SetMarkerColor(2); // red = electron
  hdedxvsp_e->Draw("SAME");
  c1->cd(2);
  hdedxvsp_k->SetMarkerColor(3); // green = kaon
  hdedxvsp_k->Draw("SAME");
  c1->cd(3);
  hdedxvsp_pi->SetMarkerColor(4); // blue = pion
  hdedxvsp_pi->Draw("SAME");
  c1->cd(4);
  hdedxvsp_p->SetMarkerColor(5); // yellow = proton
  hdedxvsp_p->Draw("SAME");


}

