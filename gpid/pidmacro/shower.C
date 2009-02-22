void shower()
{

 PndEmcMapper *emcMap=PndEmcMapper::Instance(2,"points_sttcombi.root");

 TFile* f = new TFile("reco_sttcombi.root","READ");
TTree *t=(TTree *) f->Get("cbmsim");

 TFile* f1 = new TFile("points_sttcombi.root","READ");
TTree *t1=(TTree *) f1->Get("cbmsim");

TNtuple *ntuple1 = new TNtuple("pion","shower","Px:Py:Pz:Pt:p:emc:z00:z11:z22:z20:z40:z42:z44:z31:z33");

TClonesArray* hit_array=new TClonesArray("PndEmcCluster");
 t->SetBranchAddress("EmcCluster",&hit_array);

TClonesArray* track_array=new TClonesArray("PndLhePidTrack");
 t->SetBranchAddress("LhePidTrack",&track_array);

TClonesArray* hit_array1=new TClonesArray("PndMCTrack");
 t1->SetBranchAddress("MCTrack",&hit_array1);


Float_t z00,z11,z22,z20,z40,z42,z44,z31,z33,p,e;
Int_t  emcI;



 for (Int_t j=0; j< t->GetEntriesFast(); j++)
        {
  t->GetEntry(j);
                t1->GetEntry(j);
         PndEmcCluster *cl = (PndEmcCluster *)hit_array->At(0);
         PndLhePidTrack *tr = (PndLhePidTrack*)track_array->At(0);
         PndMCTrack *mv = (PndMCTrack*)hit_array1->At(0);
   if(tr == NULL || cl == NULL || mv == NULL) continue;
emcI = tr->GetEmcIndex();
  if( emcI == -1 ) continue;
         PndEmcCluster *cl = (PndEmcCluster *) hit_array->At(emcI);
  if(cl == NULL ) continue;

PndEmcXClMoments mom = cl->Xmoments();
z00 = mom.AbsZernikeMoment(0,0,15);
z11 = mom.AbsZernikeMoment(1,1,15);
z22 = mom.AbsZernikeMoment(2,2,15);
z20 = mom.AbsZernikeMoment(2,0,15);
z40 = mom.AbsZernikeMoment(4,0,15);
z42 = mom.AbsZernikeMoment(4,2,15);
z44 = mom.AbsZernikeMoment(4,4,15);
z31 = mom.AbsZernikeMoment(3,1,15);
z33 = mom.AbsZernikeMoment(3,3,15);
p = tr->GetMomentum().Mag();
e = tr->GetEmcELoss();
ntuple->Fill(mv->Get4Momentum().Px(),mv->Get4Momentum().Py(),mv->Get4Momentum().Pz(),mv->Get4Momentum().Pt(),p,e/p
 ,z00,z11,z22,z20,z40,z42,z44,z31,z33);

 }
TFile *out = new TFile("pid_tree.root","RECREATE");
out->WriteObject(ntuple,"pidntuple");

}



