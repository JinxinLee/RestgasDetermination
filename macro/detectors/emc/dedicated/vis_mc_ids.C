

vis_mc_ids()
{
  gROOT->LoadMacro("$VMCWORKDIR/macro/mvd/Tools.C");
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  gROOT->Macro("~/rootlogon.C");

//  TTree *sim1, *sim2;
//  TFile *_file0 = TFile::Open("full_emc_test_gamma.root");
//  sim1=cbmsim;
//  sim1->SetName("sim1");
  TChain *abc=new TChain("cbmsim");
  abc->Add("emc_complete.root");
//  abc->Add("sim_emc_test_gamma.root");
//  abc->AddFriend("sim1");

    TClonesArray* mctrack_array=new TClonesArray("PndMCTrack");
  abc->SetBranchAddress("MCTrack",&mctrack_array);

  TClonesArray* hit_array=new TClonesArray("PndEmcHit");		
  abc->SetBranchAddress("EmcHit",&hit_array);

//   TClonesArray* digi_array=new TClonesArray("PndEmcDigi");		
//   abc->SetBranchAddress("EmcDigi",&digi_array);

  TClonesArray* cluster_array=new TClonesArray("PndEmcCluster");		
  abc->SetBranchAddress("EmcCluster",&cluster_array);

  TCanvas* c1 = new TCanvas("c1", "c1", 100, 100, 800, 800);
  TLatex *lt;
  lt=new TLatex();
  lt->SetTextFont(32);
  lt->SetTextAlign(22);

  Int_t ncounts = abc->GetEntries();
  for(int k = 0; k < ncounts; k++){
    abc->GetEntry(k);
    c1->Clear();

    cout << "Hits: "<<endl;
    lt->SetTextColor(1);
    lt->SetTextSize(0.015);
    Int_t nhit = hit_array->GetEntries();
    for (Int_t i=0; i<nhit; i++)
    {
	    PndEmcHit *hit;
	    hit=(PndEmcHit*)hit_array->At(i);
//         cout <<"Hit "<<i<<" energy: "<<hit->GetEnergy()<<endl;
    	Double_t x,y;
    	x=hit->GetX();
    	y=hit->GetY();
    	TLine *l;
    	Double_t fak;
    	fak=4.1e-3+1.5e-3*hit->GetEnergy();
	    l=new TLine(x*4e-3+.5,y*4e-3+.5,x*fak+.5,y*fak+.5);
	    l->SetLineWidth(5);
	    l->SetLineColor(6);
	    l->Draw();

	    TString mctxt;
	    mctxt="";
	    if( hit->GetMcList().size()==0) cout << "Zero List??"<<endl;
		for(Int_t j=0; j<hit->GetMcList().size(); j++){
			Int_t id;
			id=hit->GetMcList()[j];
			propagate_mclist(id, mctrack_array,x,y);
			if(j!=0) mctxt+=",";
			add_mccode(mctxt,((PndMCTrack *)mctrack_array->At(id))->GetPdgCode());
//                        cout <<"MCIndex: "<<id<<": "<<((PndMCTrack *)mctrack_array->At(id))->Get4Momentum().E()<<endl;
 		}

	    lt->DrawLatex(x*(fak+.8e-3)+.5,y*(fak+.8e-3)+.5,mctxt.Data());
    }

    cout << "Clusters: "<<endl;
    lt->SetTextColor(2);
    lt->SetTextSize(0.03);
    Int_t nclu = cluster_array->GetEntries();
    for (Int_t i=0; i<nclu; i++)
    {
    	PndEmcCluster *clu;
    	clu=(PndEmcCluster*)cluster_array->At(i);
	cout <<"Cluster "<<i<<" energy: "<<clu->energy()<< " , "<< clu->GetEnergyCorrected()<<endl;
    	Double_t x,y;
    	x=clu->x();
    	y=clu->y();
    	TLine *l;
    	Double_t fak;
    	fak=4.1e-3+1.5e-3*clu->energy();
	    l=new TLine(x*4e-3+.5,y*4e-3+.5,x*fak+.5,y*fak+.5);
	    l->SetLineWidth(9);
	    l->Draw();

	    TString mctxt;
	    mctxt="";
	    if( clu->GetMcList().size()==0) cout << "Zero List??"<<endl;
		for(Int_t j=0; j<clu->GetMcList().size(); j++){
			Int_t id;
			id=clu->GetMcList()[j];
			propagate_mclist(id, mctrack_array,x,y);
			if(j!=0) mctxt+=",";
			add_mccode(mctxt,((PndMCTrack *)mctrack_array->At(id))->GetPdgCode());
			cout <<"MCIndex: "<<id<<": "<<((PndMCTrack *)mctrack_array->At(id))->Get4Momentum().E()<<endl;
		}
	    lt->DrawLatex(x*(fak+.8e-3)+.5,y*(fak+.8e-3)+.5,mctxt.Data());
    }


    TString bild="bild.ps[";
    c1->Print(bild);
   }
}

void add_mccode(TString &mctxt,Int_t id)
{
	switch(id)
	{
	case 22: mctxt+="#gamma" ; break;
	case 11: mctxt+="e^{-}" ; break;
	case -11: mctxt+="e^{+}" ; break;
	case 13: mctxt+="#mu^{-}" ; break;
	case -13: mctxt+="#mu^{+}" ; break;
	case 111: mctxt+="#pi^{0}" ; break;
	case 211: mctxt+="#pi^{+}" ; break;
	case -211: mctxt+="#pi^{-}" ; break;
	case 2112: mctxt+="n" ; break;
	case 2212: mctxt+="p" ; break;
	case 310: mctxt+="K^{0}_{S}" ; break;
	case 130: mctxt+="K^{0}_{L}" ; break;
	case 321: mctxt+="K^{+}" ; break;
	case -321: mctxt+="K^{-}" ; break;
	default: mctxt+="?" ; break;
	}
}


void propagate_mclist(Int_t id,TClonesArray * mctrack_array,Double_t x,Double_t y)
{
	PndMCTrack *mct;
	PndMCTrack *mo;
	TLine *l;
	TVector3 v;
	Double_t x1,y1,x2,y2;

	if(id<0) return;
	mct=(PndMCTrack *)mctrack_array->At(id);
	v=mct->GetStartVertex();
	x1=v.x();
	y1=v.y();

    l=new TLine(x1*4e-3+.5,y1*4e-3+.5,x*4e-3+.5,y*4e-3+.5);
	l->SetLineWidth(1);
	l->SetLineColor(3);
    l->Draw();

	Bool_t loop=false;
	while(mct){
		int mid;
		mid=mct->GetMotherID();
		if(mid<0) break;
		mo=((PndMCTrack *))mctrack_array->At(mid);

		v=mct->GetStartVertex();
		x1=v.x();
		y1=v.y();
		v=mo->GetStartVertex();
		x2=v.x();
		y2=v.y();

	    l=new TLine(x1*4e-3+.5,y1*4e-3+.5,x2*4e-3+.5,y2*4e-3+.5);
	    if(loop){
	    	l->SetLineWidth(1);
	    	l->SetLineColor(7);
	    }else{
	    	l->SetLineWidth(3);
	    	l->SetLineColor(2);
	    }
	    l->Draw();

	    mct=mo;
	    loop=true;
	}
}
