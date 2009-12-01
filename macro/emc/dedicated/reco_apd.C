{
// Macro loads a file after reconstruction and plots difference between initial direction of particle and angular position of cluster
  
gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
rootlogon();
basiclibs();
	
TFile* f = new TFile("hit_emc_apd.root"); //file you want to analyse
TTree *t=(TTree *) f->Get("cbmsim") ;

TClonesArray* hit_array=new TClonesArray("PndEmcApdHit");
t->SetBranchAddress("EmcApdHit",&hit_array);
	
TFile* fsim = new TFile("sim_emc_apd.root"); //file you want to analyse
TTree *tsim=(TTree *) fsim->Get("cbmsim") ;
TClonesArray* mctrack_array=new TClonesArray("PndMCTrack");
tsim->SetBranchAddress("MCTrack",&mctrack_array);

Float_t energy = 0;
Int_t ntrack = 0;


TH1F *hEnergy = new TH1F("hEnergy","Energy Loss",200,0,1); hEnergy->GetXaxis()->SetTitle("Energy Loss [MeV]");
TH1F *hMult = new TH1F("hMult","Particle Multiplicity",15,-0.5,14.5);; hMult->GetXaxis()->SetTitle("MCTrack Multiplicity");
TH1F *hMom = new TH1F("hMom","Particle  Momentum",150,0,1.5); hMom->GetXaxis()->SetTitle("Particle Momentum[GeV/c]");

// Loop over hit
for (Int_t j=0; j< t->GetEntriesFast(); j++)
{
  t->GetEntry(j);	
  tsim->GetEntry(j);
  
    for (Int_t i=0; i<hit_array->GetEntriesFast(); i++)
      {
	PndEmcApdHit *apd=(PndEmcApdHit*)hit_array->At(i);
	hit_energy=apd->GetEnergy();
	ntrack = apd->GetNPoints();
			
	hEnergy->Fill(hit_energy*1000.);
	hMult->Fill(ntrack);
	cout << "# Particles in APD: " << ntrack << endl;
	for (Int_t tt=0; tt<ntrack; tt++)
	  {
	    cout << apd->GetMCIndex(tt) << endl;
	    PndMCTrack* mctrack=(PndMCTrack *) mctrack_array->At(apd->GetMCIndex(tt));
	    cout << "Particle ID: " << mctrack->GetPdgCode()<< endl;
	    hMom->Fill(mctrack->GetMomentum().Mag());
	  } // end of MC loop
      } // end of hit loop
}



TCanvas* c1 = new TCanvas("c1", "Energy Loss in APDs", 100, 100, 800, 800); 	
hEnergy->Draw();

TCanvas* c2 = new TCanvas("c2", "Particles in APDs", 100, 100, 800, 800); 	
hMult->Draw();
	
TCanvas* c3 = new TCanvas("c3", "Particle Momentum in APDs", 100, 100, 800, 800); 	
hMom->Draw();
	


}

