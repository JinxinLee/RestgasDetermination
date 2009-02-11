#include "fillTree.h"

void fillTree(TString const inf1, TString const inf2, 
	      Int_t const pdg_code, TTree& outTree)
{
  TFile *fsim1 = new TFile(inf1);
  TTree *tsim1 = ( TTree* )fsim1->Get("cbmsim");
  
  TFile *freco1 = new TFile(inf2);
  TTree *treco1 = ( TTree* )freco1->Get("cbmsim");
  
  TClonesArray *ArrMCTrack1 = new TClonesArray("CbmMCTrack");
  tsim1->SetBranchAddress("MCTrack",&ArrMCTrack1);
  
  TClonesArray *ArrTof1 = new TClonesArray("PndTofPoint");
  tsim1->SetBranchAddress("TofPoint",&ArrTof1);
  
  TClonesArray *ArrTpc1 = new TClonesArray("PndTpcPoint");
  tsim1->SetBranchAddress("PndTpcPoint",&ArrTpc1);
  
  TClonesArray *ArrMvd1 = new TClonesArray("PndMvdMCPoint");
  tsim1->SetBranchAddress("MVDPoint",&ArrMvd1);
  
  TClonesArray *ArrTofHit1 = new TClonesArray("PndTofHit");
  treco1->SetBranchAddress("TofHit",&ArrTofHit1);
  
  TClonesArray *ArrTrackPar1 = new TClonesArray("FairTrackParH");
  treco1->SetBranchAddress("GeaneTrackPar",&ArrTrackPar1);
  
  TClonesArray *ArrPndPidCand1 = new TClonesArray("PndPidCand");
  treco1->SetBranchAddress("PndPidCand",&ArrPndPidCand1);

  // Parameters to use for the tree branches
  float p1,s1,mm,tpc1,mvd1;
  p1 = s1 = mm = tpc1 = mvd1 = 0.0;
  
  // Define the tree branches
  outTree.Branch("p1"  , &p1  ,"p1/F");//Reconstructed momentum
  outTree.Branch("s1"  , &s1  ,"s1/F");//Speed
  outTree.Branch("mm"  , &mm  ,"mm/F");//m^2
  outTree.Branch("tpc1", &tpc1,"tpc1/F");//TPC output
  outTree.Branch("mvd1", &mvd1,"mvd1/F");//MVD output

  for (Int_t i =0; i < tsim1->GetEntries();i++){
    tsim1->GetEntry(i);
    treco1->GetEntry(i);
    
    PndTofPoint *tof = (PndTofPoint *) ArrTof1->At(0);
    PndTofHit *tofHit = (PndTofHit *) ArrTofHit1->At(0);
    PndPidCand *track = (PndPidCand *) ArrPndPidCand1->At(0);
    FairTrackParH *trackpar = (FairTrackParH *) ArrTrackPar1->At(0);
    
    Double_t de_tpc=0;
    Double_t dx_tpc=0;
    
    for (Int_t j = 0; j < ArrTpc1->GetEntriesFast(); j++ ){
      PndTpcPoint *tpc = (PndTpcPoint *) ArrTpc1->At(j);
    
      if(tpc == 0)continue;
      
      Int_t trkID = tpc->GetTrackID();
      CbmMCTrack *mctrack_g = (CbmMCTrack *) ArrMCTrack1->At(trkID);
      Int_t pdg = mctrack_g->GetPdgCode();
      
      if (pdg == pdg_code ){
	de_tpc += tpc->GetEnergyLoss();
	dx_tpc += tpc->GetLength();
      }
    }
    
    Double_t de_mvd = 0.0;
    Double_t dx_mvd = 0.0;
    
    for (Int_t j = 0; j < ArrMvd1->GetEntriesFast(); j++ ){
      PndMvdMCPoint *mvd = (PndMvdMCPoint *) ArrMvd1->At(j);
    
      if(mvd == 0)continue;
      
      Int_t trkID = mvd->GetTrackID();
      CbmMCTrack *mctrack_g = (CbmMCTrack *) ArrMCTrack1->At(trkID);
      Int_t pdg = mctrack_g->GetPdgCode();
      
      if (pdg == pdg_code ){
	de_mvd += mvd->GetEnergyLoss();
	dx_mvd += mvd->GetLength();
      }
    }

    if (track == NULL) continue;
    if (tofHit == NULL) continue;
    if (tof == NULL) continue;
    if (trackpar == NULL) continue;
    
    Double_t s = track->Get("speed");
    Double_t p = track->Get("PMag");
    Double_t gama2 = 0.0;

    if( s != 1){
      gama2 = 1.0/(1-s*s);
    }
    
    Double_t m2 = 0.0;
    
    if(s != 0 ){
      m2 = p*p/(s*s*gama2);
    }
    
    Double_t tpc = 0.0;
    Double_t mvd = 0.0;
    
    if(dx_tpc != 0){ 
      tpc = (de_tpc/dx_tpc)*1000/0.00173;
    }
    
    if(dx_mvd != 0){
      mvd = (de_mvd/dx_mvd)*1000/2.33;
    }
     
    s1   = (float) s;
    p1   = (float) p;
    mm   = (float) m2;
    tpc1 = (float) tpc;
    mvd1 = (float) mvd;

    outTree.Fill();//Fill the output tree.
  }
}
