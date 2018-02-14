int pid_check()
{
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  rootlogon();
  
  TString inPidFile  = "pid_sttcombi.root";
  TString inSimFile = "points_sttcombi.root";
  
  TFile *inFile = TFile::Open(inSimFile,"READ");
  
  TTree *tree=(TTree *) inFile->Get("cbmsim") ;
  tree->AddFriend("cbmsim",inPidFile);
    
  TClonesArray* cand_array=new TClonesArray("PndPidCandidate");
  tree->SetBranchAddress("PidChargedCand", &cand_array);

  TClonesArray* drc_array=new TClonesArray("PndPidProbability");
  tree->SetBranchAddress("PidAlgoDrc", &drc_array);
  TClonesArray* mvd_array=new TClonesArray("PndPidProbability");
  tree->SetBranchAddress("PidAlgoMvd", &mvd_array);
  
  TFile *out = TFile::Open("out.root","RECREATE");
  
  TNtuple *ntDrc = new TNtuple("ntDrc","ntDrc","mom:theta:q:drc:thetac:mvd:dedx:e:mu:pi:k:p:ec:muc:pic:kc:pc");
  TNtuple *ntMvd = new TNtuple("ntMvd","ntMvd","mom:theta:q:drc:thetac:mvd:dedx:e:mu:pi:k:p:ec:muc:pic:kc:pc");
  TNtuple *ntCom = new TNtuple("ntCom","ntCom","mom:theta:q:drc:thetac:mvd:dedx:e:mu:pi:k:p:ec:muc:pic:kc:pc");
  
  PndPidProbability *flux = new PndPidProbability(239+237,114+101,2282+2375,35+42,517+1052);
  
  for (Int_t j=0; j< tree->GetEntriesFast(); j++){
    tree->GetEntry(j);
    if (cand_array->GetEntriesFast()==0) continue;
    
    cout << "processing event " << j << "\n";
    
    for (Int_t ii=0; ii<cand_array->GetEntriesFast(); ii++) // Loop over pid candidates
      { 
	PndPidCandidate   *cand= (PndPidCandidate *)  cand_array->At(ii);
	if (cand->GetMomentum().Mag()>5.) continue;
	
	PndPidProbability *drc = (PndPidProbability *)drc_array->At(ii);  
	PndPidProbability *mvd = (PndPidProbability *)mvd_array->At(ii); 
	PndPidProbability *combo = (*drc) * (*mvd);
	if (drc->GetIndex()!=ii) cout << "DRC mismatch!!!" << endl;
	if (mvd->GetIndex()!=ii) cout << "MVD mismatch!!!" << endl;
	
	if (cand->GetDrcIndex()>-1)
	  {
	    Float_t ntuple_drc[] = {cand->GetMomentum().Mag(),cand->GetMomentum().Theta()*TMath::RadToDeg(),cand->GetCharge(),
				    cand->GetDrcIndex(),cand->GetDrcThetaC(),cand->GetMvdHits(),cand->GetMvdDEDX(),
				    drc->GetElectronPidProb(),
				    drc->GetMuonPidProb(),
				    drc->GetPionPidProb(),
				    drc->GetKaonPidProb(),
				    drc->GetProtonPidProb(),
				    drc->GetElectronPidProb(flux),
				    drc->GetMuonPidProb(flux),
				    drc->GetPionPidProb(flux),
				    drc->GetKaonPidProb(flux),
				    drc->GetProtonPidProb(flux)
	    };
	    ntDrc->Fill(ntuple_drc);
	  }
	
	if (cand->GetMvdHits()>0)
	  {
	    Float_t ntuple_mvd[] = {cand->GetMomentum().Mag(),cand->GetMomentum().Theta()*TMath::RadToDeg(),cand->GetCharge(),
				    cand->GetDrcIndex(),cand->GetDrcThetaC(),cand->GetMvdHits(),cand->GetMvdDEDX(),
				    mvd->GetElectronPidProb(),
				    mvd->GetMuonPidProb(),
				    mvd->GetPionPidProb(),
				    mvd->GetKaonPidProb(),
				    mvd->GetProtonPidProb(),
				    mvd->GetElectronPidProb(flux),
				    mvd->GetMuonPidProb(flux),
				    mvd->GetPionPidProb(flux),
				    mvd->GetKaonPidProb(flux),
				    mvd->GetProtonPidProb(flux)
	    };
	    ntMvd->Fill(ntuple_mvd);
	  }
	
	if (cand->GetMvdHits()>0&&cand->GetDrcIndex()>-1)
	  {
	    Float_t ntuple_combo[] = {cand->GetMomentum().Mag(),cand->GetMomentum().Theta()*TMath::RadToDeg(),cand->GetCharge(),
				      cand->GetDrcIndex(),cand->GetDrcThetaC(),cand->GetMvdHits(),cand->GetMvdDEDX(),
				      combo->GetElectronPidProb(),
				      combo->GetMuonPidProb(),
				      combo->GetPionPidProb(),
				      combo->GetKaonPidProb(),
				      combo->GetProtonPidProb(),
				      combo->GetElectronPidProb(flux),
				      combo->GetMuonPidProb(flux),
				      combo->GetPionPidProb(flux),
				      combo->GetKaonPidProb(flux),
				      combo->GetProtonPidProb(flux)
	    };
	    
	    ntCom->Fill(ntuple_combo);
	  }
	
      } // end of cand loop

  } // end of event loop
  
  
  out->cd();
  
  ntDrc->Write();
  ntMvd->Write();
  ntCom->Write();
  out->Save();
  
  
  return 0;
}	
