void run_ana_eta_c(int nevts=0)
{
	bool use4cfit=1;
	TString OutFile;
	if (use4cfit==1)
		OutFile="etac_histo_4c.root";
	else
		OutFile="etac_histo_vtx.root";
	
	gStyle->SetOptFit(1011);

  	TStopwatch timer;
  	timer.Start();
	
	gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");

	TString inPidFile  = "evt_pid_stt.root"; 	
	TString inSimFile = ".evt_points_stt.root";                                                                                                                                        
//	TString inPidFile  = "./data_stt_160611/evt_pid_stt.root"; 	
//	TString inSimFile = "./data_stt_160611/evt_points_stt.root";                                                                                                                                        
	TFile *inFile = TFile::Open(inPidFile,"READ");                                                                                             
	TTree *tree=(TTree *) inFile->Get("cbmsim") ;                                                                                           
	tree->AddFriend("cbmsim",inSimFile);                                                                                                     

	TClonesArray* mc_array=new TClonesArray("PndMCTrack");
	tree->SetBranchAddress("MCTrack",&mc_array);

	TClonesArray* cand_array=new TClonesArray("PndPidCandidate");
	tree->SetBranchAddress("PidChargedCand",&cand_array);
	
	TFile *out = TFile::Open(OutFile,"RECREATE");

	// the PndEventReader takes care about file/event handling
	PndEventReader evr(inPidFile);

	TH1F *h_etac_nocut=new TH1F("h_etac_nocut","m(eta_c), (no cuts);E, GeV",100,2.5,3.5);
	TH1F *h_etac_pid=new TH1F("h_etac_pid","m(eta_c), (MC PID);E, GeV",100,2.5,3.5);
	TH1F *h_etac_vtx=new TH1F("h_etac_vtx","m(eta_c), Vertex fit",100,2.5,3.5);
	TH1F *h_etac_4c=new TH1F("h_etac_4c","m(eta_c), 4C-fit",100,2.5,3.5);
	TH1F *h_etac_phimass=new TH1F("h_etac_phimass","m(eta_c), (cut on #phi mass);E, GeV",100,2.5,3.5);
	
	TH1F *h_mphi_nocuts=new TH1F("h_mphi_nocuts","#phi: m(K+ K-) (no cuts)",100,1.020-0.15,1.020+0.5);
	TH1F *h_mphi_pid=new TH1F("h_mphi_pid","#phi: m(K+ K-) (MC PID)",100,1.020-0.15,1.020+0.5);
	TH1F *h_mphi_vtx=new TH1F("h_mphi_vtx","#phi: m(K+ K-) (Vertex fit)",100,1.020-0.15,1.020+0.5);
	TH1F *h_mphi_4c=new TH1F("h_mphi_4c","#phi: m(K+ K-) (4C-fit)",100,1.020-0.15,1.020+0.5);
	TH1F *h_mphi_final=new TH1F("h_mphi_final","#phi: m(K+ K-)",200,1.020-0.15,1.020+0.15);
	
	TH1F *nc=new TH1F("nc","n charged",20,0,20);
	
	TH1F *h_chi2_4c=new TH1F("h_chi2_4c","#chi^{2} 4C-fit;#chi^{2}/N_{df}",100,0,100);
	TH1F *h_chi2_vtx=new TH1F("h_chi2_vtx","#chi^{2} vertex;#chi^{2}/N_{df}",100,0,100);
	TH2F *hvpos = new TH2F("hvpos","(x,y) projection of fitted decay vertex",100,-5,5,100,-5,5);
	TH1F *hvzpos = new TH1F("hvzpos","z position of fitted decay vertex",100,-10,10);

	
	//TPidMassSelector *phiMassSel=new TPidMassSelector("phi",1.02,0.03);
	TPidPlusSelector *kplusSel=new TPidPlusSelector("kplus");
	TPidMinusSelector *kminusSel=new TPidMinusSelector("kminus");
	
	// the candidates lists we need
	TCandList p1, p2, p3, p4, phi1, phi2, etac, etac_nocut;
	
	int n_reco=0;
	// Number of events in file and number of reconstructed eta_c to store in root file
	TVectorD n_events(1);
	TVectorD n_etac(1);

	TLorentzVector ini(0,0,3.6772,4.7333);
		
	if (nevts==0) nevts=evr.GetEntries();
	n_events[0]=nevts;
	// cout << "nevts " << nevts << "\n";
	int i=0,j=0, k=0, l=0;
	
	// *************
	// this is the loop through the events ... as simple as this...
	// ****************
	while (evr.GetEvent() && i++<nevts)
	{

	    if (!((i+1)%100)) cout<<"evt " << i << "\n";
	    
	    evr.FillList(p1,"Charged");
	    evr.FillList(p2,"Charged");
		evr.FillList(p3,"Charged");
		evr.FillList(p4,"Charged");
	    
		p1.Select(kplusSel);
		p2.Select(kminusSel);
		p3.Select(kplusSel);
		p4.Select(kminusSel);
		
		int nchrg=p1.GetLength();
		nc->Fill(nchrg);

	    for (j=0;j<p1.GetLength();++j) { 
			p1[j].SetMass(TRho::Instance()->GetPDG()->GetParticle(321)->Mass());
	    }
	    for (j=0;j<p2.GetLength();++j) { 
			p2[j].SetMass(TRho::Instance()->GetPDG()->GetParticle(321)->Mass());
	    }
	    for (j=0;j<p3.GetLength();++j) { 
			p3[j].SetMass(TRho::Instance()->GetPDG()->GetParticle(321)->Mass());
	    }
	    for (j=0;j<p4.GetLength();++j) { 
			p4[j].SetMass(TRho::Instance()->GetPDG()->GetParticle(321)->Mass());
	    }


	    phi1.Combine(p1,p2);
		phi2.Combine(p3,p4);
		for (j=0;j<phi1.GetLength();++j) h_mphi_nocuts->Fill(phi1[j].M()); 
		etac_nocut.Combine(phi1,phi2);

		for (l=0;l<etac_nocut.GetLength();++l) { 
			h_etac_nocut->Fill(etac_nocut[l].M());
		}
		
		tree->GetEntry(i);

		// MC PID
		// Leave only kaons in particle lists
		for (l=0;l<p1.GetLength();++l) { 
			if (p1[l].GetMicroCandidate().GetMcIndex()>-1){
				PndMCTrack *mcTrack = (PndMCTrack*)mc_array->At(p1[l].GetMicroCandidate().GetMcIndex());
				if (mcTrack!=0)
				{
					if (mcTrack->GetPdgCode()!=321)
					p1.Remove(p1[l]);
				}
				else 
				{
					std::cout<<"Kaon list 1, element "<<l<<" has no assosiated mcTRack"<<std::endl;
				}
			}
		}
		for (l=0;l<p2.GetLength();++l) { 
			if (p2[l].GetMicroCandidate().GetMcIndex()>-1){
				PndMCTrack *mcTrack = (PndMCTrack*)mc_array->At(p2[l].GetMicroCandidate().GetMcIndex());
				if (mcTrack!=0)
				{
					if (mcTrack->GetPdgCode()!=-321)
					p2.Remove(p2[l]);
				}
				else 
				{
					std::cout<<"Kaon list 2, element "<<l<<" has no assosiated mcTRack"<<std::endl;
				}
			}
		}
		for (l=0;l<p3.GetLength();++l) { 
			if (p3[l].GetMicroCandidate().GetMcIndex()>-1){
				PndMCTrack *mcTrack = (PndMCTrack*)mc_array->At(p3[l].GetMicroCandidate().GetMcIndex());
				if (mcTrack!=0)
				{
					if (mcTrack->GetPdgCode()!=321)
					p3.Remove(p3[l]);
				}
				else 
				{
					std::cout<<"Kaon list 3, element "<<l<<" has no assosiated mcTRack"<<std::endl;
				}
			}
		}
		for (l=0;l<p4.GetLength();++l) { 
			if (p4[l].GetMicroCandidate().GetMcIndex()>-1){
				PndMCTrack *mcTrack = (PndMCTrack*)mc_array->At(p4[l].GetMicroCandidate().GetMcIndex());
				if (mcTrack!=0)
				{
					if (mcTrack->GetPdgCode()!=-321)
					p4.Remove(p4[l]);
				}
				else 
				{
					std::cout<<"Kaon list 4, element "<<l<<" has no assosiated mcTRack"<<std::endl;
				}
			}
		}
	    phi1.Combine(p1,p2);
		phi2.Combine(p3,p4);
		for (j=0;j<phi1.GetLength();++j) h_mphi_pid->Fill(phi1[j].M()); 
		etac.Combine(phi1,phi2);
		for (l=0;l<etac.GetLength();++l) { 
			h_etac_pid->Fill(etac[l].M());
		}


		if (use4cfit)
		{
			////////////// 4C-fit ///////////////
			int best_i=0;
			double best_chi2=1000;
			for (l=0;l<etac.GetLength();++l) { 
				Pnd4CFitter fitter(etac[l],ini);
				fitter.FitConserveMasses();
				double chi2=fitter.GetChi2();
				if(chi2<best_chi2)
				{
					best_chi2=chi2;
					best_i=l;
				}
				h_chi2_4c->Fill(chi2/9); // Ndf=3N-3=9
			}
			
			if((best_chi2<90)&&(etac.GetLength()!=0))
			{
				h_etac_4c->Fill(etac[best_i].M());
				TCandidate *phi1best=(etac[best_i].Daughter(0));
				TCandidate *phi2best=(etac[best_i].Daughter(1));
				double m_phi1=phi1best->M();
				double m_phi2=phi2best->M();
				h_mphi_4c->Fill(m_phi1);
				h_mphi_4c->Fill(m_phi2);
				h_mphi_final->Fill(m_phi1);
				h_mphi_final->Fill(m_phi2);
				//std::cout<<"m_phi1="<<m_phi1<<" m_phi2="<<m_phi2<<std::endl;
				if (((m_phi1>1.02-0.03)&&(m_phi1<1.02+0.03))&&((m_phi2>1.02-0.03)&&(m_phi2<1.02+0.03)))
				{
					h_etac_phimass->Fill(etac[best_i].M());
					if ((etac[best_i].M()>2.9)&&(etac[best_i].M()<3.06))
						n_reco++;
				}
			}
			
		}
		else
		{
			////////////// Vertex fit /////////////
			int best_i=0;
			double best_chi2=100;
			TCandidate *etacfit_best=0;
			TCandidate *phi1fit_best;
			TCandidate *phi2fit_best;
			
			for (j=0;j<etac.GetLength();++j) 
			{
				PndKinVtxFitter vtxfitter(etac[j]);        // instantiate a vertex fitter
				vtxfitter.Fit();                          // do the vertex fit
		  
				TCandidate *etacfit=vtxfitter.FittedCand(etac[j]);  // request the fitted EtaC candidate
				TVector3 etacVtx=etacfit->Pos();                    // and the decay vertex position
				double chi2_vtx=vtxfitter.GlobalChi2();
				h_chi2_vtx->Fill(chi2_vtx/5); // Number degree of freedom 2N-3=5
				// plot mass and vtx x,y projection after fit
				hvpos->Fill(etacVtx.X(),etacVtx.Y());   
				hvzpos->Fill(etacVtx.Z());   
				if(chi2_vtx<best_chi2)
				{
					best_chi2=chi2;
					best_i=l;
					etacfit_best=etacfit;
					phi1fit_best=vtxfitter.FittedCand(*(etacfit_best->Daughter(0)));
					phi2fit_best=vtxfitter.FittedCand(*(etacfit_best->Daughter(1)));
				}
				
				if((best_chi2<30)&&(etac.GetLength()!=0))
				{
					h_etac_vtx->Fill(etacfit_best->M());
					double m_phi1=phi1fit_best->M();
					double m_phi2=phi2fit_best->M();
					h_mphi_vtx->Fill(m_phi1);
					h_mphi_vtx->Fill(m_phi2);
					if (((m_phi1>1.02-0.03)&&(m_phi1<1.02+0.03))&&((m_phi2>1.02-0.03)&&(m_phi2<1.02+0.03)))
					{
						h_etac_phimass->Fill(etacfit_best->M());
						if ((etacfit_best->M()>2.9)&&(etacfit_best->M()<3.06))
							n_reco++;
					}
				}
			}
		}


	}
	
	std::cout<<"Number of reconstructed eta_c = "<<n_reco<<std::endl;
	n_etac[0]=n_reco;
	
		
	out->cd();
	n_etac.Write("n_etac");
	n_events.Write("n_events");
	h_etac_nocut->Write();
	h_etac_pid->Write();
	h_etac_phimass->Write();
	h_etac_vtx->Write();
	h_etac_4c->Write();

	h_mphi_nocuts->Write();
	h_mphi_pid->Write();
	h_mphi_vtx->Write();
	h_mphi_4c->Write();
	h_mphi_final->Write();

	nc->Write();

	h_chi2_4c->Write();
	h_chi2_vtx->Write();
	hvzpos->Write();
	hvpos->Write();

	out->Save();

	timer.Stop();
	Double_t rtime = timer.RealTime();
	Double_t ctime = timer.CpuTime();	
	printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);

}
