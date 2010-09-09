//void run_ana_tpccombi(TString fname="output.evt.fast.root",int nevts=0)
void run_ana_tpccombi(TString fname="pid_tpccombi.root",int nevts=0)
{
  	TStopwatch timer;
  	timer.Start();
	
	gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
	
	TFile *out = TFile::Open("invmass_histo.root","RECREATE");

	//TCanvas *c1=new TCanvas("c1","c1",600,600);
	
	// the PndEventReader takes care about file/event handling
	PndEventReader evr(fname);
	
	// to attach more files: evr.Add(fname2);evr.Add(fname3); ...

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
         /// For fast sim, replace by if(true){   
                if ((p1[j].GetMicroCandidate().GetEmcRawEnergy()/p1[j].GetMicroCandidate().GetMomentum().Mag())>0.0) {
		    p1[j].SetMass(TRho::Instance()->GetPDG()->GetParticle(11)->Mass());
		} else {
		    p1.Remove(p1[j]);
		    j--;
		}
	    }
	    for (j=0;j<p2.GetLength();++j) { 
         /// For fast sim, replace by if(true){   
                if ((p2[j].GetMicroCandidate().GetEmcRawEnergy()/p2[j].GetMicroCandidate().GetMomentum().Mag())>0.0) {
		    p2[j].SetMass(TRho::Instance()->GetPDG()->GetParticle(11)->Mass());
		} else {
		    p2.Remove(p2[j]);
		    j--;
		}

	    }

	    jpsi.Combine(p1,p2);

	    // cout << jpsi.GetLength() << " J/Psi candidates found" << "\n";
	    for (l=0;l<jpsi.GetLength();++l) { 
		invmass->Fill(jpsi[l].M());
	    }

	}
	
	//c1->cd(); 

        invmass->Draw();

	out->cd();
	invmass->Write();
	out->Save();
	
	timer.Stop();
	Double_t rtime = timer.RealTime();
	Double_t ctime = timer.CpuTime();	
	printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);

}
