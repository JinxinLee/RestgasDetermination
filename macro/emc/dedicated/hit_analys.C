{
  // Macro to compare angular position of the CbmEmcHit and CbmEmcPoint
  
       gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
       gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
       rootlogon();
       basiclibs();
	
        TFile* f = new TFile("hit_emc.root"); // the sim file you want to analyse
        TTree *t=(TTree *) f->Get("cbmsim") ;
   
	TClonesArray* hit_array=new TClonesArray("PndEmcHit");
		
	t->SetBranchAddress("EmcHit",&hit_array);
	
	double hit_theta, point_theta;
	
	TH1F *h1= new TH1F("h1","Theta",100,-0.1,0.1);
	
	for (Int_t j=0; j< t->GetEntriesFast(); j++)
	{
		t->GetEntry(j);
		for (Int_t i=0; i<hit_array->GetEntriesFast(); i++)
		{
			PndEmcHit *hit=(PndEmcHit*)hit_array->At(i);
			TVector3 hit_pos(hit->GetX(),hit->GetY(),hit->GetZ());
			hit_theta=hit_pos.Theta();
			
			std::vector<PndEmcPoint*> pointList=hit->GetPointList();
			if (pointList.size()!=0)
			{
				TVector3 point_pos(pointList[0]->GetX(),pointList[0]->GetY(),pointList[0]->GetZ());
				point_theta=point_pos.Theta();
				h1->Fill(hit_theta-point_theta);
			}
		}
	}
	
	TCanvas* c1 = new TCanvas("c1", "", 100, 100, 800, 800); 
	h1->Draw();
	c1->Update();

}

