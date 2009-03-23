void grep_all()
{
 PndEmcMapper *emcMap=PndEmcMapper::Instance(2,"points_sttcombi.root");

 TFile* f = new TFile("reco_sttcombi.root","READ");
TTree *t=(TTree *) f->Get("cbmsim");

 TFile* f1 = new TFile("points_sttcombi.root","READ");
TTree *t1=(TTree *) f1->Get("cbmsim");

TNtuple *ntuple = new TNtuple("pidntuple","pidntuple","Px:Py:Pz:Pt:p:stt:mvd:tof:thetaC:emc:delphi:delz:z00:z11:z22:z20:z40:z42:44:z31:33");
Int_t  emcI,n;
Float_t z00,z11,z22,z20,z40,z42,z44,z31,z33,p,e;
float val[21];

TClonesArray* hit_array=new TClonesArray("PndEmcCluster");
 t->SetBranchAddress("EmcCluster",&hit_array);


TClonesArray* track_array=new TClonesArray("PndLhePidTrack");
 t->SetBranchAddress("LhePidTrack",&track_array);

TClonesArray* hit_array1=new TClonesArray("PndMCTrack");
 t1->SetBranchAddress("MCTrack",&hit_array1);


 for (Int_t j=0; j< t->GetEntriesFast(); j++)
        {
                t->GetEntry(j);
                 t1->GetEntry(j);               

 
         PndLhePidTrack *tr = (PndLhePidTrack*)track_array->At(0);
         PndMCTrack *mv = (PndMCTrack*)hit_array1->At(0);
      if(tr == NULL || mv == NULL) continue;        
emcI = tr->GetEmcIndex();
if( emcI == -1 ) continue;
         PndEmcCluster *cl = (PndEmcCluster *) hit_array->At(emcI);
  if(cl == NULL ) continue;


PndEmcXClMoments mom = cl->Xmoments();

//cl->Maxima();
n = cl->NumberOfDigis();
//cl->x();
//cl->y();
//cl->z();
//cout<<&mom<<"  "<<cl<<"  "<<emcI<<"  "<<n<<"  "<<j<<endl;

val[0] = mv->Get4Momentum().X();
val[1] = mv->Get4Momentum().Y();
val[2] = mv->Get4Momentum().Z();
val[3] = mv->Get4Momentum().T();

val[4] = tr->GetMomentum().Mag();
val[5] = tr->GetSttDEDX();
val[6] = tr->GetMvdDEDX();
val[7] = tr->GetBeta();
val[8] = tr->GetDrcThetaC();
val[9] = tr->GetEmcELoss()/val[4];
val[10]= tr->GetEmcDeltaPhi();
val[11]= tr->GetEmcDeltaZ();


val[12] = mom.AbsZernikeMoment(0,0,15);
val[13] = mom.AbsZernikeMoment(1,1,15);
val[14] = mom.AbsZernikeMoment(2,2,15);
val[15] = mom.AbsZernikeMoment(2,0,15);
val[16] = mom.AbsZernikeMoment(4,0,15);
val[17] = mom.AbsZernikeMoment(4,2,15);
val[18] = mom.AbsZernikeMoment(4,4,15);
val[19] = mom.AbsZernikeMoment(3,1,15);
val[20] = mom.AbsZernikeMoment(3,3,15);


//z00 = 


//cout<<val[12]<<"  "<<val[13]<<"  "<<val[14]<<"  "<<val[15]<<endl;
//cout<<tr->GetEmcELoss()<<"  "<<mv->Get4Momentum().Px()<<endl;
ntuple->Fill(val);
}

TFile *out = new TFile("pid_tree.root","RECREATE");
out->WriteObject(ntuple,"pidntuple");
}



