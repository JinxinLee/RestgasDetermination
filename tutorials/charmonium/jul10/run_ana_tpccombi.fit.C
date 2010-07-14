void run_ana_tpccombi(TString fname="output.evt.fast.root",int nevts=1000000)
//void run_ana_tpccombi(TString fname="pid_tpccombi.root",int nevts=0)
{
  	TStopwatch timer;
  	timer.Start();
	
	gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
	
	TFile *out = TFile::Open("invmass_histo.root","RECREATE");

	//TCanvas *c1=new TCanvas("c1","c1",600,600);
	
	// the PndEventReader takes care about file/event handling
	PndEventReader evr(fname);
	
	// to attach more files: evr.Add(fname2);evr.Add(fname3); ...
	// TH1F *invmass=new TH1F("invmass","invariant mass",400,0.0,5.0);
	TH1F *invmass=new TH1F("invmass","invariant mass",100,2.5,3.5);
	
	// the candidates lists we need
	TCandList p1;
	TCandList p2;
	TCandList jpsi;

	TLorentzVector ini(0,0,6.23164,7.24015);
		
	if (nevts==0) nevts=evr.GetEntries();
	// cout << "nevts " << nevts << "\n";
	int i=0,j=0, k=0, l=0;
	
	// *************
	// this is the loop through the events ... as simple as this...
	// ****************
	while (evr.GetEvent() && i++<nevts)
	{

	    if (!((i+1)%1000)) cout << i << "\n";

	    evr.FillList(p1,"All");
	    evr.FillList(p2,"All");
//	    evr.FillList(p1,"Charged");
//	    evr.FillList(p2,"Charged");

	    // cout << p1.GetLength() << " e+ candidates found" << "\n";
	    // cout << p2.GetLength() << " e- candidates found" << "\n";

	    for (j=0;j<p1.GetLength();++j) { 
                p1[j].PrintOn();
		//cout << "E/p for PID " << p1[j].GetMicroCandidate().GetEmcRawEnergy() << " " << 
		//    p1[j].GetMicroCandidate().GetMomentum().Mag() << "\n";
	    }

	    for (j=0;j<p1.GetLength();++j) { 
                if ((p1[j].GetMicroCandidate().GetEmcRawEnergy()/p1[j].GetMicroCandidate().GetMomentum().Mag())>0.0) {
		    p1[j].SetMass(TRho::Instance()->GetPDG()->GetParticle(11)->Mass());
		} else {
		    p1.Remove(p1[j]);
		    j--;
		}
	    }
	    for (j=0;j<p2.GetLength();++j) { 
                if ((p2[j].GetMicroCandidate().GetEmcRawEnergy()/p2[j].GetMicroCandidate().GetMomentum().Mag())>0.0) {
		    p2[j].SetMass(TRho::Instance()->GetPDG()->GetParticle(11)->Mass());
		} else {
		    p2.Remove(p2[j]);
		    j--;
		}

	    }

	    //p1[j].GetMicroCandidate().GetEmcRawEnergy()/p1[j].GetMicroCandidate().GetMomentum().Mag()
	    //p2[j].GetMicroCandidate().GetEmcRawEnergy()/p2[j].GetMicroCandidate().GetMomentum().Mag()

	    jpsi.Combine(p1,p2);

	    // cout << jpsi.GetLength() << " J/Psi candidates found" << "\n";

	    for (l=0;l<jpsi.GetLength();++l) { 
		invmass->Fill(jpsi[l].M());

		//Pnd4CFitter fitter(jpsi[l],ini);
		//fitter.FitConserveMasses();
		//if (fitter.GlobalChi2()>200) continue;
		//TCandidate *ppfit=fitter.FittedCand(jpsi[l]);
		//TCandidate *epfit=fitter.FittedCand(*(jpsi[l].Daughter(0)));
		//TCandidate *emfit=fitter.FittedCand(*(jpsi[l].Daughter(1)));
		//TLorentzVector sum=epfit->P4()+emfit->P4();

		// kfitter
		//PndMassFitter fitter(jpsi[l],3.10);				
		//TCandidate *ppfit=fitter.FittedCand(jpsi[l]);
                //cout << "after fitter " << ppfit.M() << "\n";
		//TCandidate *epfit=fitter.FittedCand(*(jpsi[l].Daughter(0)->Daughter(0)));
		//TCandidate *emfit=fitter.FittedCand(*(jpsi[l].Daughter(0)->Daughter(1)));
		//TLorentzVector sum=epfit->P4()+emfit->P4();
		//jpsi2mass->Fill(sum.Mag());
				
		//PndVtxFitter fitter(jpsi[j]);
		//fitter.SetBeamConstraint(0);
		//fitter.Fit();
		//TCandidate *jpsifit=fitter.FittedCand(jpsi[j]);
		//TVector3 jpsipos=jpsifit->Pos();
		// plot mass and vtx x,y projection after fit
		//jpsi2mass->Fill(jpsifit->M());
		//vtxpos->Fill(jpsipos.X(),jpsipos.Y());
		//dist->Fill(jpsipos.Mag()*3.096/(jpsi[j].P()*3e10));

	    }

	}
	
	... and plot it
	c1->cd(); invmass->Draw();

	out->cd();
	invmass->Write();
	out->Save();
	
	// now take the time...
	timer.Stop();
	Double_t rtime = timer.RealTime();
	Double_t ctime = timer.CpuTime();	
	printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);

}
