/*
 * ReadHCal.C
 *
 *  Created on: Mar  21, 2010
 *      Author: Simone Bianco
 */

int ReadHCal(){

  // Customize

  const Int_t numStages = 15;

  Double_t MomInit = 0.7; // initial momentum

  TString ScintName[numStages];
  TString AbsName[numStages];

  bool ScintBool[numStages];
  bool AbsBool[numStages];

  cout << "Initialize names" << endl;

  for (Int_t w = 0 ; w < numStages ; w++)
    {
      ScintName[w] = Form("TestHCalScintillator%d_",w+1);
      AbsName[w] = Form("TestHCalAbsorber%d_",w+1);
      cout << ScintName[w] << "\t" << AbsName[w] << endl;

    }


  // load libs

  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");

  //

  TString directory = gSystem->Getenv("VMCWORKDIR");
  TString HitsFile = "HCalMC.root";
  TString ParFile = "HCalMC_Params.root";

  TFile *f = new TFile(HitsFile);

  TFile *par = new TFile(ParFile);
  par->Get("FairBaseParSet");

  TH1F *steps = new TH1F("Stages","Stages",numStages*2,0.5,2*numStages+0.5);

  TH1F *elossTot = new TH1F("eloss","eloss",1000,0.,1.1*MomInit);

  TCanvas *c1 = new TCanvas();
  TCanvas *c2 = new TCanvas();

  fGeoH = new PndGeoHandling();

  TTree *t=(TTree *) f->Get("pndsim") ;

  TClonesArray* mc_array=new TClonesArray("PndSdsMCPoint");
  t->SetBranchAddress("MVDPoint",&mc_array);//Branch names

  TClonesArray* tr_array=new TClonesArray("PndMCTrack");
  t->SetBranchAddress("MCTrack",&tr_array);//Branch names

  cout << "Events: " << t->GetEntries() << endl;

  TString name;

  Double_t eLoss = 0.; // eloss in one event
  Double_t Zv = 99.;

  for (Int_t j = 0 ; j < t->GetEntries() ; j++) // loop on events

    {

      for (Int_t o = 0 ; o < numStages ; o++)
	{
	  ScintBool[o] = false;
	  AbsBool[o] = false;
	}

      t->GetEvent(j);
      eLoss = 0;

      if (j%10000 == 0) cout << "Ev. " << j << endl;

      for (Int_t y = 0 ; y < mc_array->GetEntries() ; y++) // loop on hits
	{

	  PndSdsMCPoint *point = (PndSdsMCPoint*)mc_array->At(y);

	  PndMCTrack *track = (PndMCTrack*)tr_array->At(point->GetTrackID());

	  eLoss += point->GetEnergyLoss();

	  Zv = (track->GetStartVertex()).Z();

	  if (track->GetMotherID()<0.5 && track->GetPdgCode()==2212 && Zv<-1.) // check if primary
	    {
	      name =  fGeoH->GetPath(point->GetDetName());

	      for (Int_t r = 0; r < numStages ; r++)
		{

		  if (name.Contains(ScintName[r]))
		    {
		      ScintBool[r] = true;
		      steps->Fill(2*(r+1)-1);
		    }
		  if (name.Contains(AbsName[r]))
		    {
		      AbsBool[o] = true;
		      steps->Fill(2*(r+1));
		    }

		} // end loop on names
	    }

	} // end loop on entries

      elossTot->Fill(eLoss);

    } // end loop on events

  c1->cd();
  steps->Draw();
  steps->GetXaxis()->SetTitle("# of stages");
  steps->GetYaxis()->SetTitle("# of protons");


  c2->cd();
  elossTot->Draw();
  elossTot->GetXaxis()->SetTitle("Energy loss / event [GeV]");
  return 0;

}

