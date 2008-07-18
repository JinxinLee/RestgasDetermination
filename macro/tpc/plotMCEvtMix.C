void plotMCEvtMix(int evt=0, int opt=0, TString name=TString()){
gStyle->SetFillStyle(0);
gStyle->SetFillColor(0);
gStyle->SetPadColor(0);
gStyle->SetCanvasColor(0);
TClonesArray* trkl=new TClonesArray("PndTpcMCTracklet");
 if(cbmsim->FindBranch("DeconvTrkl")!=NULL)
   cbmsim->SetBranchAddress("DeconvTrkl",&trkl);
 else cbmsim->SetBranchAddress("PndTpcMCTracklet",&trkl);
 TClonesArray* mvd=new TClonesArray("PndMvdHit");
 if(cbmsim->FindBranch("MVDHit")!=NULL)
   cbmsim->SetBranchAddress("MVDHit",&mvd);


cbmsim->GetEntry(evt);
TCanvas* c=new TCanvas("c","c",10,10,600,600);
TView3D* view=new TView3D(1,0,0);
view->SetRange(-20,-20,-200,20,20,400);
//view->SetLatitude(90);
//view->SetLongitude(90);
 view->Top();

TPolyMarker3D* target=new TPolyMarker3D(1);
target->SetPoint(0,0,0,0);
target->SetMarkerStyle(20);
target->SetMarkerColor(kBlue);
target->Draw();

// plot MVD
 int nmvd=mvd->GetEntriesFast();
 if(nmvd>0 && opt>3){
   TPolyMarker3D* mvdp=new TPolyMarker3D(nmvd);
   for(int i=0; i<nmvd; ++i){
     PndMvdHit* hit=(PndMvdHit*)mvd->At(i);
     TVector3 pos=hit->GetPosition();
     mvdp->SetPoint(i,pos.X(),pos.Y(),pos.Z());
   }
   mvdp->SetMarkerStyle(24);
   mvdp->SetMarkerSize(0.5);
   mvdp->Draw();
 }


int ntrkl=trkl->GetEntriesFast();
for(int i=0;i<ntrkl;++i){
  PndTpcMCTracklet* trk=(PndTpcMCTracklet*)trkl->At(i);
  TVector3 pos=trk->pos();
  TVector3 mom=trk->mom();
  mom.SetMag(5);
  if(opt>1 && fabs(trk->t0())>1000)continue;
  if(opt>2 && fabs(trk->t0())>200)continue;
  //std::cout<<"ID="<<trk->mcid().mceventID()<<std::endl;
  //std::cout<<trk->t0()<<std::endl;
  //std::cout<<trk->nmvd()<<std::endl;
  if(opt>3 && trk->nmvdAcc(1)<1 && trk->q()!=0)continue;
  if(opt>4 && fabs(trk->V0res())>0.5 && trk->q()==0)continue;

  TPolyLine3D* line=new TPolyLine3D(2);
  TPolyMarker3D* start=new TPolyMarker3D(1);
  start->SetMarkerStyle(20);
  start->SetMarkerSize(0.5);
  line->SetPoint(0,pos.X(),pos.Y(),pos.Z());
  start->SetPoint(0,pos.X(),pos.Y(),pos.Z());
  
  // check if physics track 
  if(opt>0 && trk->mcid().mceventID()==0){
    line->SetLineColor(kRed);
    if(trk->q()==0){
      line->SetLineColor(kBlue);
    }
    line->SetLineWidth(3);
    mom.SetMag(10);
  }
  mom+=pos;
  line->SetPoint(1,mom.X(),mom.Y(),mom.Z());

  line->Draw();
  start->Draw();
}

view->UnZoom();
c->Update();

 if(name.Length()>0){
   TString filename("plots/");
   filename+=name;
   filename+=opt;
   filename+=".eps";
   filename.ReplaceAll(" ","");
   c->SaveAs(filename);
 }
}
