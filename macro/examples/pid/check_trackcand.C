int check_trackcand(Int_t nEntries = 0)
{
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  rootlogon();
  TString inRecoFile  = "reco_sttcombi.root";
  TString inDigiFile  = "digi_sttcombi.root";
  TString inSimFile = "points_sttcombi.root";

  TFile *inFile = TFile::Open(inSimFile,"READ");
  TFile *fiDigiFile = TFile::Open(inDigiFile,"READ");
  TFile *fiReco1File = TFile::Open(inRecoFile,"READ");

  TTree *tree=(TTree *) inFile->Get("pndsim") ;
  tree->AddFriend("pndsim",inRecoFile);
  tree->AddFriend("pndsim",inDigiFile);

  TClonesArray* trk_array=new TClonesArray("PndTrack");
  tree->SetBranchAddress("SttMvdTrack", &trk_array);

  TClonesArray* mc_array=new TClonesArray("PndMCTrack");
  tree->SetBranchAddress("MCTrack", &mc_array);

  TClonesArray *mvdpixarray = new TClonesArray("PndSdsHit");
  tree->SetBranchAddress("MVDHitsPixel",&mvdpixarray);
  TClonesArray *mvdstrarray = new TClonesArray("PndSdsHit");
  tree->SetBranchAddress("MVDHitsStrip",&mvdstrarray);
  TClonesArray *mvdmcarray = new TClonesArray("PndSdsMCPoint");
  tree->SetBranchAddress("MVDPoint",&mvdmcarray);
  TClonesArray *sttarray = new TClonesArray("PndSttHit");
  tree->SetBranchAddress("STTHit",&sttarray);
  TClonesArray *sttmcarray = new TClonesArray("PndSttPoint");
  tree->SetBranchAddress("STTPoint",&sttmcarray);
  TClonesArray *gemarray = new TClonesArray("PndGemHit");
  tree->SetBranchAddress("GEMHit",&gemarray);
  TClonesArray *gemmcarray = new TClonesArray("PndGemMCPoint");
  tree->SetBranchAddress("GEMPoint",&gemmcarray);


  TFile *out = TFile::Open("out_test.root","RECREATE");
  TNtuple *nt = new TNtuple("nt","nt","evt:trk:num:id:hit:x:y:z:mcx:mcy:mcz:rho:p:theta:phi:q");

  TList *bl = (TList*) fiDigiFile->Get("BranchList");
  FairRootManager *fairman = new FairRootManager();
  fairman->SetBranchNameList(bl);
  int mvdpixdetid =  fairman->GetBranchId("MVDHitsPixel");
  int mvdstrdetid =  fairman->GetBranchId("MVDHitsStrip");
  int sttdetid =  fairman->GetBranchId("STTHit");
  int gemdetid =  fairman->GetBranchId("GEMHit");
  cout << mvdpixdetid << endl;
  cout << mvdstrdetid << endl;
  cout << sttdetid << endl;
  cout << gemdetid  << endl;

  if (nEntries==0) nEntries =  tree->GetEntriesFast();
  for (Int_t j=0; j< nEntries; j++){

    tree->GetEntry(j);
    //if (cand_array->GetEntriesFast()==0) continue;
    //if ((j%100)==0)
    cout << "processing event " << j << "\n";

    for(int itrk = 0; itrk < trk_array->GetEntriesFast(); itrk++)
      {
	cout << "*** track " << itrk << endl;

	PndTrack *trk = (PndTrack*) trk_array->At(itrk);
	if(!trk) {
	  cout << "ERROR track " << itrk << " does not exist" << endl;
	  continue;
	}

	PndTrackCand *cand = trk->GetTrackCandPtr();
	if(!cand)
	  {
	    cout << "ERROR track " << itrk << " has no candidate association" << endl;
	    continue;
	  }
	FairTrackParP parFirst = trk->GetParamFirst();
	TVector3 mom = parFirst.GetMomentum();
	for (Int_t ihit = 0; ihit < cand->GetNHits(); ihit++) {
	  PndTrackCandHit candhit = cand->GetSortedHit(ihit);

	  Int_t hitId = candhit.GetHitId();
	  Int_t detId = candhit.GetDetId();
	  Double_t rho = candhit.GetRho();
	  Float_t x = 0, y= 0, z=0, mcx = 0, mcy= 0, mcz=0;
	  FairHit *hit = NULL;
	  FairMCPoint *point = NULL;
	  if(detId == FairRootManager::Instance()->GetBranchId("MVDHitsPixel"))
	    {
	      hit = (FairHit*) mvdpixarray->At(hitId);
	      if(!hit)
		{
		  cout << "ERROR mvd pix " << hitId << " does not exist" << endl;
		}
	      else
                {
                  if (hit->GetRefIndex()==-1) cout << "ERROR mvd: ref index -1" << endl;
    	          else
		    {
		      point = (PndSdsMCPoint*)(mvdmcarray->At(hit->GetRefIndex()));
		      mcx = point->GetX();  mcy = point->GetY();  mcz = point->GetZ();
                    }
		}
	    }
	  else if(detId == FairRootManager::Instance()->GetBranchId("MVDHitsStrip"))
	    {
	      hit = (FairHit*) mvdstrarray->At(hitId);
	      if(!hit) cout << "ERROR mvd str " << hitId << " does not exist" << endl;
	      else
                {
                  if (hit->GetRefIndex()==-1) cout << "ERROR mvd: ref index -1" << endl;
	          else
		    {
		      point = (PndSdsMCPoint*)(mvdmcarray->At(hit->GetRefIndex()));
		      mcx = point->GetX();  mcy = point->GetY();  mcz = point->GetZ();
                    }
		}
	    }
	  else if(detId == FairRootManager::Instance()->GetBranchId("STTHit"))
	    {
	      hit = (FairHit*) sttarray->At(hitId);
	      if(!hit) cout << "ERROR stt " << hitId << " does not exist" << endl;
	      else
		{
		  if (hit->GetRefIndex()==-1) cout << "ERROR stt: ref index -1" << endl;
		  else
		    {
		      point = (PndSttPoint*)(sttmcarray->At(hit->GetRefIndex()));
		      mcx = point->GetX();  mcy = point->GetY();  mcz = point->GetZ();
		    }
		}
	    }
	  else if(detId == FairRootManager::Instance()->GetBranchId("GEMHit"))
	    {
	      hit = (FairHit*) gemarray->At(hitId);
	      if(!hit) cout << "ERROR gem " << hitId << " does not exist" << endl;
	      else
		{
		  if (hit->GetRefIndex()==-1) cout << "ERROR gem: ref index -1" << endl;
		  else
		    {
		      point = (PndGemMCPoint*)(gemmcarray->At(hit->GetRefIndex()));
		      mcx = point->GetX();  mcy = point->GetY();  mcz = point->GetZ();
		    }
		}
	    }
	  else cout << "ERROR detId unknown " << detId << " for hit " << hitId << endl;
	  if (hit)
	    {
	      x = hit->GetX();  y = hit->GetY();  z = hit->GetZ();
	    }
	  //mcx = point->GetX();  mcy = point->GetY();  mcz = point->GetZ();
	  Float_t ntArray[] = {
	    j, itrk, cand->GetNHits(), detId, ihit,
	    x, y, z, mcx, mcy, mcz, rho,
	    mom.Mag(), mom.Theta()*TMath::RadToDeg(), mom.Phi()*TMath::RadToDeg(), parFirst.GetQ()
	  };

	  nt->Fill(ntArray);
	}
	cout << "Momentum @ first point" << endl;
	mom.Print();
	cout << "TrackCand:" << endl;
	cand->Print();
      }



  } // end of event loop


  out->cd();

  nt->Write();

  out->Save();

  return 0;
}
