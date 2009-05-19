/*
 * Macro to extract PID parameters for each particle type from the
 * simulation and the reconstruction files. It is implemented on
 * behalve of production of event features for training the
 * classifiers.
 * Author:
 * M. Babai
 */

void ExtractParameters(const char* inSimFile, const char* RecoFile, 
		       const char* treeName, const char* OutPutName)
{

  
  TFile* simF = new TFile(inSimFile, "READ");
  TTree *simTree = (TTree *) simF->Get("cbmsim");

  TFile* recoF = new TFile( RecoFile, "READ");
  TTree *recoTree = (TTree *) recoF->Get("cbmsim");  

  PndEmcMapper *emcMap=PndEmcMapper::Instance(2,inSimFile);

  TFile *out = new TFile(OutPutName, "RECREATE");
  
  TNtuple *ntuple = new TNtuple( treeName, "treeName Desc",
				 "Px:Py:Pz:Pt:p:stt:mvd:tof:thetaC:emc:delphi:delz:z00:z11:z22:z20:z40:z42:z31");
  float val[19];
  
  TClonesArray* hit_array=new TClonesArray("PndEmcCluster");
  recoTree->SetBranchAddress("EmcCluster",&hit_array);
  
  
  TClonesArray* track_array=new TClonesArray("PndLhePidTrack");
  recoTree->SetBranchAddress("LhePidTrack",&track_array);
  
  TClonesArray* hit_array1=new TClonesArray("PndMCTrack");
  simTree->SetBranchAddress("MCTrack",&hit_array1);
  
  std::cout << "Done reading the files. Start filling the NTuple."
	    << std::endl;

  for (Int_t j=0; j< recoTree->GetEntriesFast(); j++){
    recoTree->GetEntry(j);
    simTree->GetEntry(j);               
    
    
    PndLhePidTrack *tr = (PndLhePidTrack*) track_array->At(0);
    PndMCTrack *mv     = (PndMCTrack*) hit_array1->At(0);
    
    if(tr == NULL || mv == NULL) continue;
    
    int emcI = tr->GetEmcIndex();
    
    if( emcI == -1 ) continue;
    
    PndEmcCluster *cl = (PndEmcCluster *) hit_array->At(emcI);
    
    if(cl == NULL ) continue;

    
    PndEmcXClMoments mom = cl->Xmoments();
    
    //n = cl->NumberOfDigis();
    
    val[0] = mv->Get4Momentum().X();
    val[1] = mv->Get4Momentum().Y();
    val[2] = mv->Get4Momentum().Z();
    val[3] = mv->Get4Momentum().T();
    
    val[4] = tr->GetMomentum().Mag();
    val[5] = tr->GetSttDEDX();
    val[6] = tr->GetMvdDEDX();
    val[7] = tr->GetBeta();
    val[8] = tr->GetDrcThetaC();
    val[9] = tr->GetEmcELoss()/val[4];// E/p
    val[10]= tr->GetEmcDeltaPhi();
    val[11]= tr->GetEmcDeltaZ();
    
    
    val[12] = mom.AbsZernikeMoment(0,0,15);
    val[13] = mom.AbsZernikeMoment(1,1,15);
    val[14] = mom.AbsZernikeMoment(2,2,15);
    val[15] = mom.AbsZernikeMoment(2,0,15);
    val[16] = mom.AbsZernikeMoment(4,0,15);
    val[17] = mom.AbsZernikeMoment(4,2,15);
    val[18] = mom.AbsZernikeMoment(3,1,15);
    
    ntuple->Fill(val);
  }
  // Close and clean.
  std::cout << "Cleaning." << std::endl;
  out->WriteObject(ntuple,"pidntuple");
  out->Close();
  recoF->Close();
  simF->Close();
  delete out;
  delete recoF;
  delete simF;
  exit(0);
}
