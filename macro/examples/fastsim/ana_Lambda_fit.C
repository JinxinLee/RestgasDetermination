void propagate(TLorentzVector &l, TVector3 &p, float charge, TH2F* hpro=0)
{
  double x0=p.X()/100;
  double y0=p.Y()/100;
  double z0=p.Z()/100;

  double px0=l.Px();
  double py0=l.Py();
  double pz0=l.Pz();

  double B=2;

  double pt=sqrt(px0*px0+py0*py0);
  double lambda=pz0/pt;
  double s_t=z0/lambda;
  double a=-0.2998*B*charge/3;
  double rho=a/pt;

  //cout <<hpro<<endl;

  if (hpro)
  {
    int steps=100;
    double dz=z0/steps;
    for (int i=0;i<steps;i++)
    {
      if (i==90) i=99;
      double s_tt=(i+1)*dz/lambda;
      double cosrst=cos(rho*s_tt);
      double sinrst=sin(rho*s_tt);

      double px = px0*cosrst + py0*sinrst;
      double py = py0*cosrst - px0*sinrst;
      //double pz = pz0;

      double xt=x0 - px/a*sinrst + py/a*(1-cosrst);
      double yt=y0 - py/a*sinrst - px/a*(1-cosrst);
      //double z=z0 - lambda*s_tt;

       hpro->Fill(xt,yt);
    }
  }

  //cout <<"lam="<<atan(1/lambda)/3.1416*180<<" "<<rho<<" "<<s_t<<endl;

  double cosrs=cos(rho*s_t);
  double sinrs=sin(rho*s_t);

  double px = px0*cosrs + py0*sinrs;
  double py = py0*cosrs - px0*sinrs;
  double pz = pz0;

  double x=x0 - px/a*sinrs + py/a*(1-cosrs);
  double y=y0 - py/a*sinrs - px/a*(1-cosrs);
  double z=z0 - lambda*s_t;

  l.SetX(px);
  l.SetY(py);
  l.SetZ(pz);

  p.SetX(x*100);
  p.SetY(y*100);
  p.SetZ(z*100);
}

int ana_Lambda_fit(TString fname="tmp.root",int num=0)
{
    TStopwatch timer;
    timer.Start();

    gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
    basiclibs();

    // **** the library is needed in order to use the TCandidate and TCandList etc
    gSystem->Load("libRho");

    TCanvas *c1=new TCanvas("c1","c1",800,800);
    c1->Divide(2,2);

    // **** access the datastructure holding the particle lists
    //
    TFile* f = new TFile(fname.Data());
    TTree *t=f->Get("pndsim") ;

    // **** for every event, a TCLonesArray with the candidates is stored in pndsim
    //
    TClonesArray *fChrgCands=new TClonesArray("TCandidate");
    //TClonesArray *fNeutCands=new TClonesArray("TCandidate");
    //TClonesArray *fMcCands=new TClonesArray("TCandidate");

    t->SetBranchAddress("PndChargedCandidates",&fChrgCands);
    //t->SetBranchAddress("PndNeutralCandidates",&fNeutCands);
    //t->SetBranchAddress("PndMcTracks",&fMcCands);

    TCandidate *tc;

    // **** create and setup some histos for QA plots
    //
    TH1F *ppimass = new TH1F("ppimass","p pi cands",100,1.05,1.2);
    TH1F *ppi2mass = new TH1F("ppi2mass","p pi cands",100,1.05,1.2);
    ppi2mass->SetFillColor(2);
    ppi2mass->SetLineColor(2);
    ppi2mass->SetLineWidth(1);

    TH2F *hvtx=new TH2F("hvtx","vertex positions (x,y)",100,-10,10,100,-10,10);
    TH2F *hvtx2=new TH2F("hvtx2","vertex positions (x,z)",100,-10,10,100,-10,10);

    TH1F *hdist=new TH1F("hdist","vtx distance to (0,0,0)",100,0,15);

    ppimass->SetMinimum(0);

    if (num==0) num= t->GetEntriesFast();
    cout <<"\n####### Processing "<<num <<" events...\n"<<endl;

    // **** create all the particle lists we'll need for rebuilding the decay tree
    //
    TCandList mcCands;

    TCandList allCands,chargedCands, minusCands, plusCands, pplusCands, pminusCands, piplusCands, piminusCands;

    TCandList ppiCands;

    // **** create and configure the selectors/filters we'd like to use later
    //
    TPidChargedSelector *chargedSel = new TPidChargedSelector;
    TPidNeutralSelector *neutralSel = new TPidNeutralSelector;

    TPidPlusSelector    *plusSel    = new TPidPlusSelector;
    TPidMinusSelector   *minusSel   = new TPidMinusSelector;

    // **** mass selectors for the resonances/composites
    //
    TPidSimplePionSelector *piSel    = new TPidSimplePionSelector();
    piSel->SetCriterion("veryLoose");
    TPidSimpleProtonSelector *pSel    = new TPidSimpleProtonSelector();
    pSel->SetCriterion("veryLoose");

    int i1,i2;

    // **** loop over all _events_
    //
    for (Int_t j=0; j< num;j++)
    {
 	t->GetEntry(j);
	TFactory::Instance()->Reset();

	chargedCands.Cleanup();

 	t->GetEntry(j);
      if ((j%100)==0)
       {
         cout <<"evt "<<j<<endl;
         c1->cd(1);
	 if (ppi2mass->GetMaximum()<ppimass->GetMaximum()) ppi2mass->SetMaximum(ppimass->GetMaximum()*1.05);

         ppi2mass->Draw();
         ppimass->Draw("same");

	 c1->cd(2);
	 hdist->Draw();
          c1->cd(3);
          hvtx->Draw("");
          c1->cd(4);
          hvtx2->Draw("");
	  c1->Update();
        }

	// **** loop over all Candidates and add them to the list allCands
	//

	for (i1=0; i1<fChrgCands->GetEntriesFast(); i1++){
	    tc = (TCandidate *)fChrgCands->At(i1);

	    //TMatrixD cov=tc->Cov7();
	    //cov.Print();

	    TLorentzVector l=tc->P4();
	    TVector3 p=tc->Pos();

	    propagate(l,p,tc->GetCharge());
	    tc->SetP4(l);
	    tc->SetPos(p);

	    //TMatrixD cov=tc->Cov7();
	    //cov.Print();

	    chargedCands.Add(*tc);

	    //TMatrixD cov2=chargedCands[chargedCands.GetLength()-1].Cov7();
	    //cov2.Print();

	}

	plusCands.Select(chargedCands, plusSel);
	minusCands.Select(chargedCands, minusSel);

	pplusCands.Select(plusCands,pSel);
	pminusCands.Select(minusCands,pSel);

	piplusCands.Select(plusCands,piSel);
	piminusCands.Select(minusCands,piSel);
	cout <<endl<<"---------------------"<<endl;
	cout <<"chrg:"<<chargedCands.GetLength()<<" ";
	cout <<"plus:"<<plusCands.GetLength()<<" ";
	cout <<"minus:"<<minusCands.GetLength()<<" ";
	cout <<"p+:"<<pplusCands.GetLength()<<" ";
	cout <<"p-:"<<pminusCands.GetLength()<<" ";
	cout <<"pi+:"<<piplusCands.GetLength()<<" ";
	cout <<"pi-:"<<piminusCands.GetLength()<<endl;

	// **** select all the basic list
	//

	//cout <<"c:"<<chargedCands.GetLength()<<" n:"<<neutralCands.GetLength()<<endl;

	/*
	ppiCands.Combine(pplusCands,piminusCands);
	//ppiCands.Combine(pminusCands,piplusCands);

	TCandListIterator iterP(ppiCands);
	while (tc=iterP.Next()) ppimass->Fill(tc->M());
	*/
	int ii,jj;

	int npi=piminusCands.GetLength();
	int np=pplusCands.GetLength();


	for (ii=0;ii<npi;ii++)
        {
	  for (jj=0;jj<np;jj++)
    	  {
	    TCandidate pim=piminusCands[ii];
	    TCandidate pp=pplusCands[jj];

	    if( pp.Overlaps(pim)) continue;

	    //TMatrixD pcov=pp.Cov7();
            //TMatrixD picov=pim.Cov7();

	    TCandidate *compo=pim.Combine(pp);
	    ppimass->Fill(compo->Mass());
	    PndVtxFitter vtxf(*compo);
	    vtxf.Fit();
	    TCandidate *fitted=vtxf.FittedCand(*compo);
	    cout <<"unfitted="<<(*compo)<<endl;
	    if (fitted)
	    {
	      double d=fitted->Pos().Mag();
	      if (d==0) continue;
	      cout <<"fitted = "<<(*fitted)<<endl;
	      ppi2mass->Fill(fitted->Mass());
	      hvtx->Fill(fitted->Pos().X(),fitted->Pos().Y());
	      hvtx2->Fill(fitted->Pos().X(),fitted->Pos().Z());
	      hdist->Fill(fitted->Pos().Mag());
	    }
	   /*
	    cout <<"Proton:" <<pp<<endl;
	    pcov.Print();
	    cout <<"Pion:"<<pim<<endl;
	    picov.Print();
	    */
	    //delete compo;
	  }
	}

    }
    // **** plot all that stuff
    //
         c1->cd(1);
	 if (ppi2mass->GetMaximum()<ppimass->GetMaximum()) ppi2mass->SetMaximum(ppimass->GetMaximum()*1.05);

         ppi2mass->Draw();
         ppimass->Draw("same");

	 c1->cd(2);
	 hdist->Draw();
          c1->cd(3);
          hvtx->Draw("");
          c1->cd(4);
          hvtx2->Draw("");
	  c1->Update();

    timer.Stop();
    Double_t rtime = timer.RealTime();
    Double_t ctime = timer.CpuTime();

    printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
return 0;
}

/*void printTree(TCandidate *tc, int level=0)
{
  int i=0;
  int nd=tc->NDaughters();
  if (nd==0) return;
  cout <<tc->Uid()<<"("<<level<<") -> ";
  for (i=0;i<nd;i++) cout<<tc->Daughter(i)->Uid()<<"  ";
  cout <<endl;
  for (i=0;i<nd;i++) printTree(tc->Daughter(i),level+1);
  if (level==0) cout <<endl;
}
*/
