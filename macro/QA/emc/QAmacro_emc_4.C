//
// Macro that checks the energy stored in hits with the energy sum of points TClonesArray for the EMC.
// The total energy per event are checked as well as the energy deposited per crystal.
// Run QAmacro_emc_1.C and QAmacro_emc_2.C prior to this macro.
//
// If a shashlyk module if fired in event, this event is skipped
//
// JGM, April 2010
//
int QAmacro_emc_4()
{
  
	////////////////////////////////////////////////////////////////////////////////
	// The following part of macro access RunTimeDataBase and initialize PndEmcMapper from it 
	////////////////////////////////////////////////////////////////////////////////
	FairRunAna *fRun= new FairRunAna();
	fRun->SetInputFile("sim_emc.root");
	fRun->SetOutputFile("dummy_out.root");
	
	FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
	FairParRootFileIo* parInput1 = new FairParRootFileIo();
	parInput1->open("simparams.root");
	
  	TString emcAsciiPar = gSystem->Getenv("VMCWORKDIR");
	emcAsciiPar += "/macro/params/";
	emcAsciiPar += "emc.par";
	
	FairParAsciiFileIo* parInput2 = new FairParAsciiFileIo();
	parInput2->open(emcAsciiPar.Data(),"in");
	
	rtdb->setFirstInput(parInput1);
	rtdb->setSecondInput(parInput2);
	
	PndEmcGeoPar *geoPar = (PndEmcGeoPar*) rtdb->getContainer("PndEmcGeoPar");
	fRun->Init();
	
	geoPar->InitEmcMapper();
	/////////////////////////////////////////////////////////////////////////////////
	
	TFile* f = new TFile("full_emc.root"); //file you want to analyse
	TTree *t=(TTree *) f->Get("cbmsim") ;
         
	TClonesArray* hit_array=new TClonesArray("PndEmcHit");
	t->SetBranchAddress("EmcHit",&hit_array);

	TFile* fsim = new TFile("sim_emc.root"); //file you want to analyse
	TTree *tsim=(TTree *) fsim->Get("cbmsim") ;
	
	TClonesArray* point_array=new TClonesArray("PndEmcPoint");
	tsim->SetBranchAddress("EmcPoint",&point_array);
		
	TH1F *h1= new TH1F("h1","Energy deposit, sum points",500,0,2);
	TH1F *h2= new TH1F("h2","Energy deposit, hits",500,0,2);

	Bool_t fTest=kTRUE;
	Double_t tolerance=1e-5; // tolerance in GeV

	Short_t module=0;

	for (Int_t j=0; j< t->GetEntriesFast(); j++)
	{
		t->GetEntry(j);
		tsim->GetEntry(j);

		// Sum over hits

		Double_t total_energy_from_hits=0;

		for (Int_t i=0; i<hit_array->GetEntriesFast(); i++)
		  {
		    PndEmcHit *hit=(PndEmcHit*)hit_array->At(i);
			
			// Skip shashlyk elements
			module = hit->GetModule();
			if (module==5) break;
			
		    total_energy_from_hits += hit->GetEnergy();

		    Int_t detID = hit->GetDetectorID();
		    Double_t energy_from_points=0;
		    for (Int_t k=0; k<point_array->GetEntriesFast(); k++)
		      {
			PndEmcPoint *point=(PndEmcPoint*)point_array->At(k);
			if (point->GetDetectorID()==detID) energy_from_points+=point->GetEnergyLoss();
		      }
		    if (fabs(hit->GetEnergy()-energy_from_points)>tolerance)
		      {
			cout << "<E> Energy of det=" << detID<< " does not match between points and digis: " << hit->GetEnergy() << "/" << energy_from_points << endl;
			fTest=kFALSE;
		      }

		  }
		  if (module==5) continue;

		h2->Fill(total_energy_from_hits);

		// Sum over the points

		Double_t total_energy_from_points=0;

		for (Int_t i=0; i<point_array->GetEntriesFast(); i++)
		{
			PndEmcPoint *point=(PndEmcPoint*)point_array->At(i);
			total_energy_from_points += point->GetEnergyLoss();
		}

		h1->Fill(total_energy_from_points);
		
		if (fabs(total_energy_from_points-total_energy_from_hits)>tolerance)
		  {
		    cout << "<E> Total energy values do not match: " << total_energy_from_points << "/" << total_energy_from_hits << endl;
		    fTest=kFALSE;
		  }
	}

if (fTest){
    cout << " Test passed" << endl;
    cout << " All ok " << endl;  
}else{
    cout << " Test Failed" << endl;
    cout << " Not Ok " << endl;         
}
  return 0;
}

