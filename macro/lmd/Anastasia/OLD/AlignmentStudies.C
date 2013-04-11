// #include "TString.h"
// #include "TStopwatch.h"
// #include "TChain.h"
// #include "TClonesArray.h"
// #include "TFile.h"
// using namespace std;
void combitransToLumiFrame(TVector3& hitPos){
  //do the transformation from lab frame to LUMI frame (with z-axis perp. to lumi planes)
  const Double_t  kHalfFoilThickness  = 0.0075; // Thickness of sensitive foil (cm)
  const Double_t  kTransZ = 1100.; //(cm) //move at z-position
  const Double_t  kRotUmZ = 476.03; //(cm) //z-point to rotate
  const Double_t  kTransX = 25; //(cm) //move at x-position
  const Double_t  kRot =  0.040596358401388; // 2.326 degree  = 4.05963584013881024e-02 rad
  TVector3 LumiTrans(0,0,kRotUmZ);
  hitPos -=LumiTrans;
  hitPos.RotateY(-kRot);
  LumiTrans = TVector3(0,0,kTransZ-kRotUmZ);
  hitPos -=LumiTrans;
  // cout<<"!!! NEW HIT position in LUMI frame!!! "<<endl;
  // hitPos.Print();
}
void rotateToLumiFrame(TVector3& hitPos){

  // cout<<"!!! OLD HIT direction in LUMI frame!!! "<<endl;
  // hitPos.Print();
  TMatrixD hitCov(3,3);
  hitCov[0][0] = hitPos[0];
  hitCov[1][0] = hitPos[1];
  hitCov[2][0] = hitPos[2];
  Double_t theta=2.326;

  // Set matrix elements according to Euler angles
  Double_t degrad = TMath::Pi()/180.;
  Double_t sintheta = TMath::Sin(degrad*theta);
  Double_t costheta = TMath::Cos(degrad*theta);
  TMatrixD rot(3,3);
  rot[0][0]= costheta;
  rot[0][1]= 0;
  rot[0][2]= sintheta;
  rot[1][0]= 0;
  rot[1][1]= 1;
  rot[1][2]= 0;
  rot[2][0]= -sintheta;
  rot[2][1]= 0;
  rot[2][2]= costheta;
  TMatrixD result = rot;
  result.T();
  result*=hitCov;
  // result.Print();
  hitPos = TVector3(result(0,0),result(1,0),result(2,0));
  //hitPos *=  1./hitPos.Mag();
  // cout<<"!!! NEW HIT direction in LUMI frame!!! "<<endl;
  // hitPos.Print();
}

void AlignmentStudies(const int nEvents=100000, const int startEvent=0, TString storePath="tmpOutput", const int verboseLevel=0, const int output=1)
{
  // ----  Load libraries   -------------------------------------------------
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  gSystem->Load("libLmdTrk");
  // gROOT->LoadMacro("line3Dfit.C");
  // ------------------------------------------------------------------------
  
  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------
  
  // ---- Input files --------------------------------------------------------
  TString simMC=storePath+"/Lumi_MC_";
  simMC += startEvent;
  simMC += ".root";
  TChain tMC("cbmsim");
  tMC.Add(simMC);

  TString DigiFile = storePath+"/Lumi_digi_";
  DigiFile += startEvent;
  DigiFile += ".root";
  TChain tdigiHits("cbmsim");
  tdigiHits.Add(DigiFile);
  
  TString recHit=storePath+"/Lumi_reco_";
  recHit += startEvent;
  recHit += ".root";
  TChain tHits("cbmsim");
  tHits.Add(recHit);
  
  TString trkCand = storePath+"/Lumi_TCand_";
  trkCand += startEvent;
  trkCand += ".root";
  TChain tTrkCand("cbmsim");
  tTrkCand.Add(trkCand);
  
  TString recTrack = storePath+"/Lumi_Track_";
  recTrack += startEvent;
  recTrack += ".root";
  TChain tTrkRec("cbmsim");
  tTrkRec.Add(recTrack);
  
  // ---------------------------------------------------------------------------------
  
  // ---- Output file ----------------------------------------------------------------
  TString out=storePath+"/Lumi_alignmentInfo_";
  out += startEvent;
  out +="_Mom_";
  out +=mom;
  out += ".root";
  TFile *f = new TFile(out,"RECREATE");
  // ---------------------------------------------------------------------------------
  
  //--- MC info -----------------------------------------------------------------
  TClonesArray* true_tracks=new TClonesArray("PndMCTrack");
  tMC.SetBranchAddress("MCTrack",&true_tracks);  //True Track to compare
  
  TClonesArray* true_points=new TClonesArray("PndSdsMCPoint");
  tMC.SetBranchAddress("LMDPoint",&true_points);  //True Points to compare
  //----------------------------------------------------------------------------------
  //--- Digitization info ------------------------------------------------------------
  TClonesArray* fStripClusterArray = new TClonesArray("PndSdsClusterStrip");
  tHits.SetBranchAddress("LMDStripClusterCand",&fStripClusterArray); 
  
  TClonesArray* fStripDigiArray = new TClonesArray("PndSdsDigiStrip");
  tdigiHits.SetBranchAddress("LMDStripDigis",&fStripDigiArray); 
  //----------------------------------------------------------------------------------
  
  //--- Real Hits --------------------------------------------------------------------
  TClonesArray* rechit_array=new TClonesArray("PndSdsHit");
  tHits.SetBranchAddress("LMDHitsStrip",&rechit_array);  //Points for Tracks
  //----------------------------------------------------------------------------------
  
  
  //--- Track Candidate ---------------------------------------------------------------
  TClonesArray* trkcand_array=new TClonesArray("PndTrackCand");
  tTrkCand.SetBranchAddress("LMDTrackCand",&trkcand_array); //Points for Track Canidates 
  //-----------------------------------------------------------------------------------
  
  //--- Real tracks -------------------------------------------------------------------
  TClonesArray* rec_trk=new TClonesArray("PndLinTrack");
  tTrkRec.SetBranchAddress("LMDTrack",&rec_trk);  //Tracks
  //----------------------------------------------------------------------------------  
  
  //--- Output histogram -----------------------------------------------------
  //residuals between trk and hits
  TH1* r1 = new TH1F("hr1","distanse between track and hits on plane #1",1e3,-1e-1,1e-1);
  TH1* r2 = new TH1F("hr2","distanse between track and hits on plane #2",1e3,-1e-1,1e-1);
  TH1* r3 = new TH1F("hr3","distanse between track and hits on plane #3",1e3,-1e-1,1e-1);
  TH1* r4 = new TH1F("hr4","distanse between track and hits on plane #4",1e3,-1e-1,1e-1);
  //z coordinate on plane
  TH1* hhitz1 = new TH1F("hhitz1","z on plane #1",1e3,-0.1,0.1);
  TH1* hhitz2 = new TH1F("hhitz2","z on plane #2",1e3,9.9,10.1);
  TH1* hhitz3 = new TH1F("hhitz3","z on plane #3",1e3,19.9,20.1);
  TH1* hhitz4 = new TH1F("hhitz4","z on plane #4",1e3,29.9,30.1);
  TH1* htrkz1 = new TH1F("htrkz1","trk_{z} on plane #1",1e3,1e1,10);
  //xy coordinate on plane
  TH2* hhitxy1 = new TH2F("hhitxy1","xy on plane #1;x, cm;y, cm",1e2,-11,11,1e2,-11,11);
  TH2* hhitxy2 = new TH2F("hhitxy2","xy on plane #2;x, cm;y, cm",1e2,-11,11,1e2,-11,11);
  TH2* hhitxy3 = new TH2F("hhitxy3","xy on plane #3;x, cm;y, cm",1e2,-11,11,1e2,-11,11);
  TH2* hhitxy4 = new TH2F("hhitxy4","xy on plane #4;x, cm;y, cm",1e2,-11,11,1e2,-11,11);

  TH2* hhitxnh = new TH2F("hhitxnh","vtx_{x} vs. number of hits ;n; x, cm;",4,2,6,1e2,-10,10);


  TH1* hChi2 = new TH1F("hChi2","#chi^{2}",1e3,0,1e2);
  TH1* hnhits = new TH1I("hnhits","Number of hits per trk",5,0,5);

  TH1 *hVxRec = new TH1F("hVxRec","Vx^{Rec};Vx, cm;",1e3,-1e-2,1e-2);
  TH1 *hVyRec = new TH1F("hVyRec","Vy^{Rec};Vy, cm;",1e3,-1e-2,1e-2);
  TH1 *hVzRec = new TH1F("hVzRec","Vz^{Rec};Vz, cm;",1e3,-1e-2,1e-2);
  TH1 *hpullVxRec = new TH1F("hpullVxRec","(Vx^{MC}-Vx^{Rec})/#sigma_{Vx};;",1e2,-1e1,1e1);
  TH1 *hpullVyRec = new TH1F("hpullVyRec","(Vy^{MC}-Vy^{Rec})/#sigma_{Vy};;",1e2,-1e1,1e1);
  TH1 *hpullVzRec = new TH1F("hpullVzRec","(Vz^{MC}-Vz^{Rec})/#sigma_{Vz};;",1e2,-1e3,1e3);
  TH2 *hpullVxRecPhi = new TH2F("hpullVxRecPhi","(Vx^{MC}-Vx^{Rec})/#sigma_{Vx} vs. #phi;#phi, rad;",1e1,-1.,1.,1e2,-1e1,1e1);
  TH2 *hpullVyRecPhi = new TH2F("hpullVyRecPhi","(Vy^{MC}-Vy^{Rec})/#sigma_{Vy} vs. #phi;#phi, rad;",1e1,-1.,1.,1e2,-1e1,1e1);
  TH2 *hpullVzRecPhi = new TH2F("hpullVzRecPhi","(Vz^{MC}-Vz^{Rec})/#sigma_{Vz} vs. #phi;#phi, rad;",1e1,-1.,1.,1e2,-1e3,1e3);
  TH1 *hPxRec = new TH1F("hPxRec","Px^{Rec};Px, GeV/c;",1e3,-1e-2,1e-2);
  TH1 *hPyRec = new TH1F("hPyRec","Py^{Rec};Py, GeV/c;",1e3,-1e-2,1e-2);
  TH1 *hPzRec = new TH1F("hPzRec","Pz^{Rec};Pz, GeV/c;",1e3,-1e-5,1e-5);
  TH1 *hpullPxRec = new TH1F("hpullPxRec","(Px^{MC}-Px^{Rec})/#sigma_{Px};;",1e2,-1e1,1e1);
  TH1 *hpullPyRec = new TH1F("hpullPyRec","(Py^{MC}-Py^{Rec})/#sigma_{Py};;",1e2,-1e1,1e1);
  TH1 *hpullPzRec = new TH1F("hpullPzRec","(Pz^{MC}-Pz^{Rec})/#sigma_{Pz};;",1e2,-1e1,1e1);
  TH2 *hpullPxRecPhi = new TH2F("hpullPxRecPhi","(Px^{MC}-Px^{Rec})/#sigma_{Px} vs. #phi;#phi, rad;",1e1,-1.,1.,1e2,-1e1,1e1);
  TH2 *hpullPyRecPhi = new TH2F("hpullPyRecPhi","(Py^{MC}-Py^{Rec})/#sigma_{Py} vs. #phi;#phi, rad;",1e1,-1.,1.,1e2,-1e1,1e1);
  TH2 *hpullPzRecPhi = new TH2F("hpullPzRecPhi","(Pz^{MC}-Pz^{Rec})/#sigma_{Pz} vs. #phi;#phi, rad;",1e1,-1.,1.,1e2,-1e1,1e1);
  TH1 *hsensorsID = new TH1I("hsensorsID","sensors ID",34,-1,33);
  TH2 *hsensorsPhi = new TH2F("hsensorsPhi","sensors ID vs. #phi",1e3,-3.15,3.15,34,-1,33);
  TH2 *hsensorsPhiMC = new TH2F("hsensorsPhiMC","sensors ID vs. #phi_{MC}",1e3,-3.15,3.15,34,-1,33);
  for (Int_t j=0; j<nEvents; j++){
    // Read REC & MC tree -----------------------------------------------------------------
    tMC.GetEntry(j);
    tTrkCand.GetEntry(j);
    tTrkRec.GetEntry(j);
    tHits.GetEntry(j);
    tdigiHits.GetEntry(j);

    const int nParticles = true_tracks->GetEntriesFast();
    const int numTrk = rec_trk->GetEntriesFast();
    if(nParticles!=1) continue;
    if(verboseLevel>0){
      cout<<"Event#"<<j<<endl;
      cout<<"Number of sim.particles: "<<nParticles<<" number of rec.trks: "<<numTrk<<endl;
    }
    ///-----------------------------------------------------------------------------------------

  
    /// Read INFO from reconstructed tracks ----------------------------------------------------
    for (Int_t iN=0; iN<numTrk; iN++){
      PndLinTrack *trk_lin = (PndLinTrack*)rec_trk->At(iN);
      double trkpar[6];
      trk_lin->GetPar(trkpar);
      TVector3  trkVtx = trk_lin->GetStartVec();
      // cout<<"reconstr vtx:"<<endl;
      // trkVtx.Print();
    
      TVector3 errtrkVtx = trk_lin->GetStartErrVec();
      TVector3 trkDir = trk_lin->GetDirectionVec();
      // cout<<"reconstructed dir:"<<endl;
      // trkDir.Print();
      TVector3 errtrkDir = trk_lin->GetDirectionErrVec();
      htrkz1->Fill(trkVtx.Z());
      //      TVector3  trkDir = trk_lin->GetDirectionVec();

      double chi2 = trk_lin->GetChiSquare();
      hChi2->Fill(chi2);
      Int_t candID = trk_lin->GetTCandID();
      PndTrackCand *trkcand = (PndTrackCand*)trkcand_array->At(candID);
      const int Ntrkcandhits= trkcand->GetNHits();
      // if(trkVtx.X()>-6 && trkVtx.X()<-4 && fabs(trkVtx.Y())<2)
      // 	cout<<"Event#"<<j<<" trk has "<<Ntrkcandhits<<" hits"<<endl;
      hhitxnh->Fill(Ntrkcandhits,trkVtx.X());
      //  if(Ntrkcandhits<4) continue;
      hnhits->Fill(Ntrkcandhits);
      TVector3 VtxTrue;
      TVector3 DirTrue;
      double phiMCgl;
	for (Int_t iHit = 0; iHit < Ntrkcandhits; iHit++){
	  PndTrackCandHit candhit = (PndTrackCandHit)(trkcand->GetSortedHit(iHit));
	  Int_t hitID = candhit.GetHitId();
	  PndSdsHit* myHit = (PndSdsHit*)(rechit_array->At(hitID));
	  if(iHit==0){
	    ///Top cluster
	    PndSdsClusterStrip* myCluster =  (PndSdsClusterStrip*)(fStripClusterArray->At(myHit->GetClusterIndex()));
	    PndSdsDigiStrip* astripdigi = (PndSdsDigiStrip*)(fStripDigiArray->At(myCluster->GetDigiIndex(0)));
	    if (astripdigi->GetIndex(0) == -1) continue; // sort out noise
	    PndSdsMCPoint* MCPoint = (PndSdsMCPoint*)(true_points->At(astripdigi->GetIndex(0)));
	    // ///Bottom cluster
	    // Int_t  botIndex = myHit->GetBotIndex();
	    // PndSdsClusterStrip* myClusterBot =  (PndSdsClusterStrip*)(fStripClusterArray->At(botIndex));
	    // PndSdsDigiStrip* astripdigiBot = (PndSdsDigiStrip*)(fStripDigiArray->At(myClusterBot->GetDigiIndex(0)));
	    // if (astripdigiBot->GetIndex(0) == -1) continue; // sort out noise
	    // PndSdsMCPoint* MCPoint = (PndSdsMCPoint*)(true_points->At(astripdigiBot->GetIndex(0)));

	    double xTrue = MCPoint->GetX();
	    double yTrue = MCPoint->GetY();
	    double zTrue = MCPoint->GetZ();
	    VtxTrue = TVector3(xTrue,yTrue,zTrue);
	    combitransToLumiFrame(VtxTrue);
	    double pxTrue =  MCPoint->GetPx();
	    double pyTrue =  MCPoint->GetPy();
	    double pzTrue =  MCPoint->GetPz();
	    double Pmom = 1./sqrt(pxTrue*pxTrue+pyTrue*pyTrue+pzTrue*pzTrue);
	    DirTrue = TVector3(Pmom*pxTrue,Pmom*pyTrue,Pmom*pzTrue);
	    PndMCTrack *mctrk =(PndMCTrack*) true_tracks->At(0);
	    TVector3 MomMC = mctrk->GetMomentum();
	    phiMCgl = 	MomMC.Phi();
	    rotateToLumiFrame(DirTrue);
	    //   DirTrue *=1./DirTrue.Mag();
	  }
	  TVector3 HitPos = myHit->GetPosition();
	  int endtrk=1;
	  if((Ntrkcandhits-iHit)>1) endtrk=0;
	  if(output>0){
	  cout<<HitPos.X()<<" "<<myHit->GetDx()<<" ";
	  cout<<HitPos.Y()<<" "<<myHit->GetDy()<<" ";
	  cout<<HitPos.Z()<<" "<<myHit->GetDz()<<" ";
	  cout<<myHit->GetSensorID()<<" "<<endtrk<<endl;
	  hsensorsID->Fill(myHit->GetSensorID());
	  hsensorsPhi->Fill(trkDir.Phi(),myHit->GetSensorID());
	  hsensorsPhiMC->Fill(phiMCgl,myHit->GetSensorID());
	  }
	  Double_t t_min = (HitPos.Z()-trkpar[4]);
	  //	  Double_t t_min = trkpar[1]*(HitPos.X()-trkpar[0])+trkpar[3]*(HitPos.Y()-trkpar[2])+trkpar[5]*(HitPos.Z()-trkpar[4]);
	  Double_t dx = trkpar[0]+trkpar[1]*t_min-HitPos.X();
	  Double_t dy = trkpar[2]+trkpar[3]*t_min-HitPos.Y();
	  Double_t dz = trkpar[4]+trkpar[5]*t_min-HitPos.Z();
	  double dr = TMath::Sqrt(dx*dx+dy*dy+dz*dz);
	  //  cout<<"dx = "<<dx<<" dy = "<<dy<<" dz = "<<dz<<endl;
	  //	  if(HitPos.Z()<5){ 
	 
	  if(int(myHit->GetSensorID()/8)==0){
	    // if(HitPos.Z()>1.)   cout<<"int(myHit->GetSensorID()/8) = "<<int(myHit->GetSensorID()/8)
	    // 	<<" myHit->GetSensorID()="<<myHit->GetSensorID()
	    // 	<<" z = "<<HitPos.Z()<<endl;
	    r1->Fill(dr);
	    hhitz1->Fill(HitPos.Z());
	    hhitxy1->Fill(HitPos.X(),HitPos.Y());
	  }
	  else{
	    //	    if(HitPos.Z()<15){
	    if(int(myHit->GetSensorID()/8)==1){
	      r2->Fill(dr);
	      hhitz2->Fill(HitPos.Z());
	      hhitxy2->Fill(HitPos.X(),HitPos.Y());
	    }
	    else{
	      //	      if(HitPos.Z()<25){
	      if(int(myHit->GetSensorID()/8)==2){
		r3->Fill(dr);
		hhitz3->Fill(HitPos.Z());
		hhitxy3->Fill(HitPos.X(),HitPos.Y());
	      }
	      else{
		r4->Fill(dr);
		hhitz4->Fill(HitPos.Z());
		hhitxy4->Fill(HitPos.X(),HitPos.Y());
	      }
	    }
	  }
	}
	/// Compare true and reconstructed info
	hVxRec->Fill(trkVtx.X()-VtxTrue.X());
	hVyRec->Fill(trkVtx.Y()-VtxTrue.Y());
	hVzRec->Fill(trkVtx.Z()-VtxTrue.Z());
	hpullVxRec->Fill((trkVtx.X()-VtxTrue.X())/errtrkVtx.X());
	hpullVyRec->Fill((trkVtx.Y()-VtxTrue.Y())/errtrkVtx.Y());
	hpullVzRec->Fill((trkVtx.Z()-VtxTrue.Z())/errtrkVtx.Z());
	hpullVxRecPhi->Fill(DirTrue.Phi(),(trkVtx.X()-VtxTrue.X())/errtrkVtx.X());
	hpullVyRecPhi->Fill(DirTrue.Phi(),(trkVtx.Y()-VtxTrue.Y())/errtrkVtx.Y());
	hpullVzRecPhi->Fill(DirTrue.Phi(),(trkVtx.Z()-VtxTrue.Z())/errtrkVtx.Z());
 
	hPxRec->Fill(trkDir.X()-DirTrue.X());
	hPyRec->Fill(trkDir.Y()-DirTrue.Y());
	hPzRec->Fill(trkDir.Z()-DirTrue.Z());
	hpullPxRec->Fill((trkDir.X()-DirTrue.X())/errtrkDir.X());
	hpullPyRec->Fill((trkDir.Y()-DirTrue.Y())/errtrkDir.Y());
	hpullPzRec->Fill((trkDir.Z()-DirTrue.Z())/errtrkDir.Z());
	hpullPxRecPhi->Fill(DirTrue.Phi(),(trkDir.X()-DirTrue.X())/errtrkDir.X());
	hpullPyRecPhi->Fill(DirTrue.Phi(),(trkDir.Y()-DirTrue.Y())/errtrkDir.Y());
	hpullPzRecPhi->Fill(DirTrue.Phi(),(trkDir.Z()-DirTrue.Z())/errtrkDir.Z());
    }/// end rec-trks
    ///-----------------------------------------------------------------------------------------
  }/// end events
  r1->Write();
  r2->Write(); 
  r3->Write();
  r4->Write(); 
  hChi2->Write();
  hnhits->Write(); 
 

  TCanvas *c1 = new TCanvas("hits_res");
  c1->Divide(3,2);
  c1->cd(1);
  r1->Draw();
  c1->cd(2);
  r2->Draw();
  c1->cd(3);
  r3->Draw();
  c1->cd(4);
  r4->Draw();
  c1->cd(5);
  hChi2->Draw();
  c1->cd(6);
  hnhits->Draw();
  c1->Write();

  TCanvas *c2 = new TCanvas("hits_z");
  c2->Divide(2,2);
  c2->cd(1);
  hhitz1->Draw();
  htrkz1->SetLineColor(2);
  htrkz1->Draw("same");
  c2->cd(2);
  hhitz2->Draw();
  c2->cd(3);
  hhitz3->Draw();
  c2->cd(4);
  hhitz4->Draw();
  c2->Write();

  gStyle->SetPalette(1,0);
  TCanvas *c5 = new TCanvas("hits_xy","XY",800,800);
  c5->Divide(2,2);
  c5->cd(1);
  hhitxy1->Draw("colz");
  c5->cd(2);
  hhitxy2->Draw("colz");
  c5->cd(3);
  hhitxy3->Draw("colz");
  c5->cd(4);
  hhitxy4->Draw("colz");
  c5->Write();

  gStyle->SetPalette(1,0);
  TCanvas *c3 = new TCanvas("pulls_vtx");
  c3->Divide(3,3);
  c3->cd(1);
  hVxRec->Draw();
  c3->cd(2);
  hVyRec->Draw();
  c3->cd(3);
  hVzRec->Draw();
  c3->cd(4);
  hpullVxRec->Draw();
  c3->cd(5);
  hpullVyRec->Draw();
  c3->cd(6);
  hpullVzRec->Draw();
  c3->cd(7);
  hpullVxRecPhi->Draw("colz");
  c3->cd(8);
  hpullVyRecPhi->Draw("colz");
  c3->cd(9);
  hpullVzRecPhi->Draw("colz");
  c3->Write();

  TCanvas *c4 = new TCanvas("pulls_dir");
  c4->Divide(3,3);
  c4->cd(1);
  hPxRec->Draw();
  c4->cd(2);
  hPyRec->Draw();
  c4->cd(3);
  hPzRec->Draw();
  c4->cd(4);
  hpullPxRec->Draw();
  c4->cd(5);
  hpullPyRec->Draw();
  c4->cd(6);
  hpullPzRec->Draw();
  c4->cd(7);
  hpullPxRecPhi->Draw("colz");
  c4->cd(8);
  hpullPyRecPhi->Draw("colz");
  c4->cd(9);
  hpullPzRecPhi->Draw("colz");
  c4->Write();
  hpullVxRecPhi->Write();
  hhitxnh->Write();
  hsensorsID->Write();
  hsensorsPhi->Write();
  hsensorsPhiMC->Write();
  TCanvas *c5 = new TCanvas("sensors");
  c5->Divide(2,2);
  c5->cd(1);
  hsensorsPhi->Draw("colz");
  c5->cd(2);
  hsensorsPhiMC->Draw("colz");
  c5->cd(3);
  hsensorsID->Draw();
  c5->cd(4);
  //  hhitxnh->Draw();
  c5->Write();

  f->Close();
 if(verboseLevel>0)  cout<<"Macro succsefully finished!"<<endl;
}
