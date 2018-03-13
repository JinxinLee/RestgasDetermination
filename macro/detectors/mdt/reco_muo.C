{
  TFile* f = new TFile("tracks_combi.root"); //file you want to analyse
  TFile* f2 = new TFile("points_combi.root");
  TTree *t=(TTree *) f->Get("pndsim") ;
  t->AddFriend("pndsim","points_combi.root");

  TClonesArray* trk_array=new TClonesArray("PndMdtTrk");
  t->SetBranchAddress("MdtTrk",&trk_array);

  TClonesArray* hit_array=new TClonesArray("PndMdtHit");
  t->SetBranchAddress("MdtHit",&hit_array);

  TClonesArray* point_array=new TClonesArray("PndMdtPoint");
  t->SetBranchAddress("MdtPoint",&point_array);

  TClonesArray* mc_array=new TClonesArray("PndMCTrack");
  t->SetBranchAddress("MCTrack",&mc_array);
  PndEmcMapper *emcMap=PndEmcMapper::Instance(1);

  TFile* out = TFile::Open("reco_combi.root","RECREATE");

  TNtuple *nHit = new TNtuple("nHit","nHit","evt:x:y:z:phi:theta:lay:mod:sec:xt:yt");
  TNtuple *nHit0 = new TNtuple("nHit0","nHit0","evt:trk:x:y:z:phi:theta:lay:mod:sec:pdg:mot:xt:yt:phit:px:py:pz:y0:y1:y2:y3:y4:y5:y6:y7:y8:y9");
  TNtuple *ext = new TNtuple("ext","ext","evt:trk:xt:yt:zt:lay:mod:sec:x:y:z:dist:angle");
  PndMdtRecoPar *par = new PndMdtRecoPar();

  for (Int_t j=0; j< t->GetEntriesFast(); j++)
    {
      t->GetEntry(j);

      for (Int_t hh=0; hh<hit_array->GetEntriesFast(); hh++)
	{
	  PndMdtHit *mdtHit=(PndMdtHit*)hit_array->At(hh);
	  TVector3 pos(0.,0.,0.);
	  mdtHit->Position(pos);
	  Int_t layer = int((pos.Phi()*TMath::RadToDeg()+180+22.5)/45);
	  if (layer==8) layer = 0;

	  Float_t xt, yt;

	  xt = cos(-layer*45*TMath::DegToRad())*pos.X() - sin(-layer*45*TMath::DegToRad())*pos.Y();
	  yt =  sin(-layer*45*TMath::DegToRad())*pos.X() +  cos(-layer*45*TMath::DegToRad())*pos.Y();


	  nHit->Fill(j,pos.X(), pos.Y(), pos.Z(), pos.Phi(), pos.Theta(), mdtHit->GetLayerID(), mdtHit.GetModule(), mdtHit.GetSector(), xt, yt);
	} // end of hit loop

      for (Int_t tt=0; tt<trk_array->GetEntriesFast(); tt++)
	{
	  PndMdtTrk *trkHit=(PndMdtTrk*)trk_array->At(tt);
	  /*
	    if ( ( (trkHit->GetModule()==1) && (trkHit->GetHitBit()!=1023) ) ||
	    ( (trkHit->GetModule()==2) && (trkHit->GetHitBit()!=127) ) ||
	    (trkHit->GetModule()==10) ) continue;*/
	  if ( (trkHit->GetModule()!=1) || (trkHit->GetHitBit()!=1023) ) continue;

	  //for (Int_t pp=0;pp<10;pp++) cout << trkHit->GetModule() << "  " << pp << "  " << trkHit->GetHitNumber(pp)<< endl;
	  PndMdtHit *hit0 = (PndMdtHit*)hit_array->At(trkHit->GetHitNumber(0));
	  PndMdtPoint *point0 = (PndMdtPoint*)point_array->At(hit0->GetRefIndex());
	  PndMCTrack *mc0 = (PndMCTrack*)mc_array->At(((PndMdtPoint*)point_array->At(hit0->GetRefIndex()))->GetTrackID());

	  TVector3 pos0(0.,0.,0.);
	  hit0->Position(pos0);
	  TVector3 mom(point0->GetPx(),point0->GetPy(),point0->GetPz());
	  // mom = point0->GetMomentum();

	  Float_t xt0, yt0;
	  xt0 = (cos(-hit0->GetSector()*45*TMath::DegToRad())*pos0.X() - sin(-hit0->GetSector()*45*TMath::DegToRad())*pos0.Y());
	  yt0 =  sin(-hit0->GetSector()*45*TMath::DegToRad())*pos0.X() +  cos(-hit0->GetSector()*45*TMath::DegToRad())*pos0.Y();
	  mom->SetPhi(mom.Phi()-hit0->GetSector()*45.*TMath::DegToRad());
	  if (mom.X()==0)
	    {
	      cout << "px == 0 errore"<< endl;
	      continue;
	    }
	  Float_t xlayer0 = -par->GetLayerPos(hit0->GetModule()-1,0);
	  Float_t yproj[10], zproj[10];
	  for (Int_t yy=0; yy<10; yy++)
	    {
	      Float_t xlayer = -par->GetLayerPos(hit0->GetModule()-1,yy);
	      yproj[yy] = yt0 + (xlayer - xt0) * mom.Y() / mom.X();
	      zproj[yy] = pos0.Z() + (xlayer - xt0) * mom.Z() / mom.X();

	      PndMdtHit *testhit = (PndMdtHit*)hit_array->At(trkHit->GetHitNumber(yy));
	      TVector3 hpos(0.,0.,0.);
	      testhit->Position(hpos);
	      Float_t xt, yt;
	      xt = (cos(-hit0->GetSector()*45*TMath::DegToRad())*hpos.X() - sin(-hit0->GetSector()*45*TMath::DegToRad())*hpos.Y());
	      yt =  sin(-hit0->GetSector()*45*TMath::DegToRad())*hpos.X() +  cos(-hit0->GetSector()*45*TMath::DegToRad())*hpos.Y();
	      Float_t dist = sqrt(((yproj[yy]-yt0)*(yproj[yy]-yt0))+((zproj[yy]-hpos.Z())*(zproj[yy]-hpos.Z())));
	      TVector3 vext(xlayer-xlayer0,yproj[yy]-yt0,zproj[yy]-pos0.Z());
	      TVector3 vhit(xlayer-xlayer0,yt-yt0,hpos.Z()-pos0.Z());

	      ext->Fill(j, tt,xlayer, yproj[yy], zproj[yy], testhit->GetLayerID(), hit0.GetModule(), hit0.GetSector(), xt, yt, hpos.Z(),dist,vext.Angle(vhit) );
	    }

	  Float_t nhit0_nt[] = {
	    j,tt,pos0.X(), pos0.Y(), pos0.Z(), pos0.Phi(), pos0.Theta(),
	    hit0->GetLayerID(), hit0.GetModule(), hit0.GetSector(),
	    mc0->GetPdgCode(), mc0->GetMotherID(), xt0, yt0,mom.Phi()*TMath::RadToDeg(),
	    mom.X(), mom.Y(), mom.Z(),
	    yproj[0],yproj[1],yproj[2],yproj[3],yproj[4],yproj[5],yproj[6],yproj[7],yproj[8],yproj[9],


	  };

	  nHit0->Fill(nhit0_nt);
	} // end of MdtTrk loop

    } // end of event loop


  nHit->Write();
  nHit0->Write();
  ext->Write();
  nHit->Write();
  out->Save();
  //out->Close();


}

