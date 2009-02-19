void grep()
{
 PndEmcMapper *emcMap=PndEmcMapper::Instance(2,"points_sttcombi.root");

 TFile* f = new TFile("reco_sttcombi.root","READ");
TTree *t=(TTree *) f->Get("cbmsim");

 TFile* f1 = new TFile("points_sttcombi.root","READ");
TTree *t1=(TTree *) f1->Get("cbmsim");

TNtuple *ntuple = new TNtuple("pidntuple","pidntuple","Px:Py:Pz:Pt:p:stt:mvd:tof:thetaC:emc");


TClonesArray* hit_array=new TClonesArray("PndLhePidTrack");
 t->SetBranchAddress("LhePidTrack",&hit_array);

TClonesArray* hit_array1=new TClonesArray("PndMCTrack");
 t1->SetBranchAddress("MCTrack",&hit_array1);

 for (Int_t j=0; j< t->GetEntriesFast(); j++)
        {
                t->GetEntry(j);
                 t1->GetEntry(j);                
         PndLhePidTrack *tr = (PndLhePidTrack*)hit_array->At(0);
         PndMCTrack *mv = (PndMCTrack*)hit_array1->At(0);
      if(tr == NULL || mv == NULL) continue;        
 
cout<<tr->GetEmcELoss()<<"  "<<mv->Get4Momentum().Px()<<endl;
ntuple->Fill(mv->Get4Momentum().Px(),mv->Get4Momentum().Py(),mv->Get4Momentum().Pz(),mv->Get4Momentum().Pt(),tr->GetMomentum().Mag(),tr->GetSttDEDX(),
tr->GetMvdDEDX(),tr->GetBeta(),tr->GetDrcThetaC(),tr->GetEmcELoss());
 }
TFile *out = new TFile("pid_tree.root","RECREATE");
out->WriteObject(ntuple,"pidntuple");
}
