// root macro to analyze the simulation output
#include <vector>
#include <map>
int anaGemPointrate(int nEvents = 10, bool verbose = false)
{
//  int  nEvents = 1000;
//  bool verbose = false;
  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------

  TFile* f = new TFile("Gem_Test.root"); // the sim file you want to analyse
  TTree *t=(TTree *) f->Get("cbmsim") ;

  TClonesArray* hit_array=new TClonesArray("PndGemMCPoint");
  t->SetBranchAddress("GEMPoint",&hit_array);//Branch names

  TClonesArray* mc_array=new TClonesArray("PndMCTrack");
  t->SetBranchAddress("MCTrack",&mc_array);//Branch names

  TH2D* hisxy = new TH2D("hisxy","GEM MC Points, xy view",100,-150.,150.,100,-150.,150.);
  TH2D* hisrz = new TH2D("hisrz","GEM MC Points, rz view",100,0.,300.,100,-150.,150.);
  TH1D* hisde = new TH1D("hisde","GEM MC Points, Energyloss",100,0.,0.00002);


  TH1D *hisDisk1Theta = new TH1D("disk1theta","Gem Disk 1 pointrate per 6GeV DPM event;#theta;dN/d#theta   ",45,0.,45.);
  TH1D *hisDisk2Theta = new TH1D("disk2theta","Gem Disk 2 pointrate per 6GeV DPM event;#theta;dN/d#theta   ",45,0.,45.);
  TH1D *hisDisk3Theta = new TH1D("disk3theta","Gem Disk 3 pointrate per 6GeV DPM event;#theta;dN/d#theta   ",45,0.,45.);

  TH2D *histrackrate = new TH2D("trackrate","Gem trackrate per 6GeV DPM event;#theta;nPoints   ",45,0.,45.,3,1.,4.);


  TVector3 vecs;
  std::map<int,int> hitsontrack;
  TString detname;
  int trackno=0,hitcount=0;
  double scale=1., weight=1.,thetadeg=0.;
  if(nEvents!=0) scale=1./(double)nEvents;
  cout << "scale = "<<scale<<endl;

  for (Int_t j=0; j<nEvents && j<t->GetEntriesFast(); j++)
  {
    t->GetEntry(j);
    if(verbose) cout<<"Event No "<<j<<endl;
    for(int l=0;l<mc_array->GetEntriesFast();l++){
      hitsontrack[l]=0;
    } // end for Tracknumbers
    for (Int_t i=0; i<hit_array->GetEntriesFast(); i++)
    {
      if(verbose) cout<<"Point No "<<i<<endl;
      PndGemMCPoint *hit=(PndGemMCPoint*)hit_array->At(i);
      int mcpdg = -1;

//       PndMCTrack *mctruth = (PndMCTrack*)mc_array->At(hit->GetTrackID());
//       mcpdg = mctruth->GetPdgCode();
//       cout<<"mcpdg="<<mcpdg<<endl;

      vecs.SetXYZ(hit->GetX(), hit->GetY(), hit->GetZ());

      hisxy->Fill(vecs.x(),vecs.y());
      hisrz->Fill(vecs.z(),((vecs.y()>0.)?1.:-1.)*vecs.Perp());
      hisde->Fill(hit->GetEnergyLoss());

      detname = hit->GetDetName();
      thetadeg = vecs.Theta()*TMath::RadToDeg();
      if(detname .Contains("Disk1")){
        hisDisk1Theta->Fill(thetadeg);
      }else if(detname .Contains("Disk2")){
        hisDisk2Theta->Fill(thetadeg);
      }else if(detname .Contains("Disk3")){
        hisDisk3Theta->Fill(thetadeg);
      }
      trackno = hit->GetTrackID();
      hitsontrack[trackno]++;
    }//end for i (points in event)

    for(int k=0;k<mc_array->GetEntriesFast();k++){
      PndMCTrack *mctruth = (PndMCTrack*)mc_array->At(k);
      vecs=mctruth->GetMomentum();
      thetadeg = vecs.Theta()*TMath::RadToDeg();
      histrackrate->Fill(thetadeg,hitsontrack[k],scale);
    } // end for Tracknumbers
  }// end for j (events)

TCanvas* can1 = new TCanvas("can1","MCHit view in GEM",0,0,800,800);
can1->Divide(3,3);

can1->cd(1);
DrawNice2DHisto(hisxy);

can1->cd(2);
DrawNice2DHisto(hisrz);

can1->cd(3);
gPad->SetLogy();
hisde->DrawCopy("");

can1->cd(4);
hisDisk1Theta->Scale(scale,"width");
hisDisk1Theta->DrawCopy();

can1->cd(5);
hisDisk2Theta->Scale(scale,"width");
hisDisk2Theta->DrawCopy();

can1->cd(6);
hisDisk3Theta->Scale(scale,"width");
hisDisk3Theta->DrawCopy();

can1->cd(7);
//histrackrate->Scale(scale,"width");
//histrackrate->Draw("colz");
DrawNice2DHisto(histrackrate,"",histrackrate->GetMaximum());

can1->Print("outAnaGemSim.png");




  // -----   Finish   -------------------------------------------------------
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  //cout << "Output file is "    << outFile << endl;
  //cout << "Parameter file is " << parFile << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;
  // ------------------------------------------------------------------------

  return 0;
}
