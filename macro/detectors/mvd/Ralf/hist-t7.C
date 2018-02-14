{
  // -----  Load libraries   ------------------------------------------------
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libPndData");
  gSystem->Load("libField");
  gSystem->Load("libGen");
  gSystem->Load("libPassive");
  gSystem->Load("libMvd");
  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------

  gStyle->SetHistFillColor(9);
  gStyle->SetCanvasColor(0);
  gStyle->SetLabelSize(0.025,"X");
  gStyle->SetLabelSize(0.025,"Y");
  gStyle->SetTitleSize(0.03,"Y");
  gStyle->SetBarOffset(10);
  gStyle->SetTitleOffset(1,"X");
  gStyle->SetTitleOffset(1.55,"Y");
  gStyle->SetTitleSize(0.03,"X");
  gStyle->SetPalette(1,0);
  gStyle->SetOptFit(1111);
  //Creating canvas and diagramms
  TCanvas* can1 = new TCanvas("test","Energy in MVD",200,200,1100,800);
  TCanvas* can2 = new TCanvas("test2","Energy in MVD",200,200,1100,800);
  
  //TCanvas* can2 = new TCanvas("test1","Energy in MVD",200,200,1000,1000);

	TH2D * hist4 = new TH2D("EnergyLossVsP","dE(p) gemittelt  ueber alle Hits pro Track",200,0.02,3,200,0,0.0005);
	//hist4->SetFillColor(2);
	TH2D * hist5 = new TH2D("EnergyLossVsP1","dE(p)",200,0.02,3,200,0,0.0005);
	//hist5->SetFillColor(3);
	TH2D * hist6 = new TH2D("EnergyLossVsP2","dE(p)",200,0.02,3,200,0,0.0005);
	//hist6->SetFillColor(4);
	TH2D * hist7 = new TH2D("specEnergyLossVsP1","dE/dx(p)  gemittelt  ueber alle Hits pro Track",200,0.02,1,200,0,0.0005);
	//hist4->SetFillColor(2);
	TH2D * hist8 = new TH2D("specEnergyLossVsP2","dE/dx(p)",200,0.02,1,200,0,0.0005);
	//hist5->SetFillColor(3);
	TH2D * hist9 = new TH2D("specEnergyLossVsP3","dE/dx(p)",200,0.02,1,200,0,0.0005);
	//hist6->SetFillColor(4);
	TH2D * hist10 = new TH2D("specEnergyLossVsP4","dE/dx(p)",200,0.0,1,200,0,0.0005);

	//Bethe Bloch fuer Kaonen
	TF1 *bk = new TF1("bk","((x-0.05)*1000)/(sqrt(sq(439.71)+(sq((x-0.05)*1000))))",0,1);
	TF1 *bk2 = new TF1("bk1","((x-0.08)*1000)/(sqrt(sq(439.71)+(sq((x-0.08)*1000))))",0,1);
	TF1 *bk3 = new TF1("bk2","((x+0.0)*1000)/(sqrt(sq(439.71)+(sq((x+0.0)*1000))))",0,1);
	TF1 *dEBBk = new TF1("dEBBk","((11*sq(14)*1.5)/(sq(bk)*10000000))*(log((2*1000000*0.511*sq(bk))/(13.5*14))-log(1-sq(bk))-sq(bk))",0,1);
	dEBBk->SetFillColor(3);
	TF1 *dEBBku = new TF1("dEBBku",".0008+((11*sq(14)*1.5)/(sq(bk1)*10000000))*(log((2*1000000*0.511*sq(bk1))/(13.5*14))-log(1-sq(bk1))-sq(bk1))",0,1);
	dEBBk->SetFillColor(30);
	TF1 *dEBBkl = new TF1("dEBBkl","-.00125+((11*sq(14)*1.5)/(sq(bk2)*10000000))*(log((2*1000000*0.511*sq(bk2))/(13.5*14))-log(1-sq(bk2))-sq(bk2))",0,1);
	dEBBk->SetFillColor(30);

	//Bethe Bloch fuer Pionen
	TF1 *bpi = new TF1("bpi","(x*1000)/(sqrt(sq(139.57)+(sq(x*1000))))",0,1);
	TF1 *dEBBpi = new TF1("dEBBpi","((2.5*sq(14)*1.5)/(sq(bpi)*0.313*10000000))*(log((2*1000000*0.511*sq(bpi))/(13.5*14))-log(1-sq(bpi))-sq(bpi))",0,1);
  TF1 *bpi = new TF1("bpiu","((x-0.05)*1000)/(sqrt(sq(139.57)+(sq((x-0.05)*1000))))",0,1);
	TF1 *dEBBpiu = new TF1("dEBBpiu",".0012+((2.5*sq(14)*1.5)/(sq(bpiu)*0.313*10000000))*(log((2*1000000*0.511*sq(bpiu))/(13.5*14))-log(1-sq(bpiu))-sq(bpiu))",0.03,1);
  TF1 *bpi = new TF1("bpil","((x)*1000)/(sqrt(sq(139.57)+(sq((x)*1000))))",0,1);
	TF1 *dEBBpil = new TF1("dEBBpil","-.0005+((2.5*sq(14)*1.5)/(sq(bpil)*0.313*10000000))*(log((2*1000000*0.511*sq(bpil))/(13.5*14))-log(1-sq(bpil))-sq(bpil))",0,1);

	//Bethe Bloch fuer Protonen	
	TF1 *bp = new TF1("bp","((x-0.05)*1000)/(sqrt(sq(938.26)+(sq(0.05*1000))))",0,1);
	TF1 *dEBBp = new TF1("dEBBp","((23*sq(14)*1.5)/(sq(bp)*2.133*10000000))*(log((2*1000000*0.511*sq(bp))/(13.5*14))-log(1-sq(bp))-sq(bp))",0,1);
  TF1 *bp = new TF1("bpu","((x-0.1)*1000)/(sqrt(sq(938.26)+(sq((x-0.1)*1000))))",0,1);
	TF1 *dEBBpu = new TF1("dEBBpu",".0005+((23*sq(14)*1.5)/(sq(bpu)*2.133*10000000))*(log((2*1000000*0.511*sq(bpu))/(13.5*14))-log(1-sq(bpu))-sq(bpu))",0.014,1);
  TF1 *bp = new TF1("bpl","((x+0.0)*1000)/(sqrt(sq(938.26)+(sq((x+0.0)*1000))))",0,1);
	TF1 *dEBBpl = new TF1("dEBBpl","-.0010+((22*sq(14)*1.5)/(sq(bpl)*2.133*10000000))*(log((2*1000000*0.511*sq(bpl))/(13.5*14))-log(1-sq(bpl))-sq(bpl))",0,3);
	
	
	// Trick, um Maxima besser zu sehen: bei Null Impuls abschneiden
	//  TH2D * hist18 = new TH2D("specEnergyLoss","dE/dx(p)",200,0.05,3,200,0,0.01);

 	TH1D* hist1 = new TH1D("specEnergyLoss","dE",100,0,0.0004);
	hist1->SetFillColor(2);
 	TH1D* hist2 = new TH1D("specEnergyLoss","dE1",100,0,0.0004);
	hist2->SetFillColor(3);
 	TH1D* hist3 = new TH1D("specEnergyLoss","dE2",100,0,0.00040);
	hist3->SetFillColor(4);
	//TF1 *fa = new TF1("p","x*TMath::C()/sqrt(sq(x)*sq(TMath::C()))",0,10);
	//TF1 *fa = new TF1("p","x",0,10);
	//TF1 *fa = new TF1("v","0.76706[0]*x*sin([1]*x)",-3,3);
	//TF1 *fa = new TF1("fa","0.76706[0]*x*sin([1]*x)",-3,3);

 
  	//opening tree
//    TFile* inFile = new TFile("../data/testMC.root","READ");
  TFile* inFile = new TFile("/data_hilbert/PandaData/tbaldauf/MvdMC_PiKP2_50k.root","READ");
  TTree* tree = (TTree *)inFile->Get("cbmsim");
  TClonesArray* pointlist=new TClonesArray("PndSdsMCPoint");
  tree->SetBranchAddress("MVDPoint",&pointlist);
  TClonesArray* mc_array=new TClonesArray("PndMCTrack");
  tree->SetBranchAddress("MCTrack",&mc_array);
  
  
  	//variable 
  TVector3 vecFront,vecBack,vecPFront,vecPBack;
  Int_t nEvents = 10000; //1000;
  Int_t sEcur =0;
  Int_t t0,t1,t2,t3,cnt;
	Int_t a[25];
  Double_t dE0,dE1,dE2,dE3,pF0,pF1,pF2,pF3,dEX0,dEX1,dEX2,dEX3;
  Double_t dx;
	Double_t pnull=0.8;
 	Double_t pkt[2];
 	TString ltext="";
	Double_t pos,abst;
  
	//preparing canvas
  TPad *p1=new TPad("1","1",0,0,0.7,1);
	TPad *p2=new TPad("2","2",0.7,0,1,1);
	can1->cd();
	p1->Draw();
	p2->Draw();

  p1->cd();
  hist4->SetYTitle("dE/GeV");
  hist4->SetXTitle("p/(GeV/c)");
  hist7->SetYTitle("(dE/dx)/(GeV/cm)");
  hist7->SetXTitle("p/(GeV/c)");
	hist10->SetYTitle("(dE/dx)/(GeV/cm)");
  hist10->SetXTitle("p/(GeV/c)");
    	
  for(int w=0;w<25;w++)
  {
    a[w]=0;
  }
  
  for(int j=0;j<nEvents && j<tree->GetEntriesFast();j++)
	{
		t0=0;t1=0;t2=0;dE0=0;dE1=0;dE2=0;dEX0=0;dEX1=0;dEX2=0;pF0=0;pF1=0;pF2=0;
    		//cout<<"Event"<<j<<endl;
		tree->GetEntry(j);
		for(int i=0;i<pointlist->GetEntriesFast();i++)
		{
		PndSdsMCPoint* point=(PndSdsMCPoint*)pointlist->At(i);//getting point
		vecFront=(point->GetX(),point->GetY(),point->GetZ());
		vecBack=(point->GetXOut(),point->GetYOut(),point->GetZOut());
		vecPBack=(point->GetPxOut(),point->GetPyOut(),point->GetPzOut());
   	vecFront=(point->GetPxOut(),point->GetPyOut(),point->GetPzOut());
		PndMCTrack* mct=(PndMCTrack*) mc_array->At(point->GetTrackID());
		pdcid=mct->GetPdgCode();
   	dx=(vecBack-vecFront).Mag();
		cout<<"dx=  "<<dx<<endl;
		cout<<"P=  "<<vecPBack.Mag()<<endl;
		if(point->GetTrackID()==0)		
		{
			cout<<pdcid<<endl;
			t0++;
			dE0=dE0+(point->GetEnergyLoss());
			pF0=pF0+vecPFront.Mag();
			if(dx!=0)
			{
			  cout<<"0 "<<pdcid<<endl;
			  cout<<"Eloss "<<(point->GetEnergyLoss())<<endl;
			  hist7->Fill(vecPBack.Mag(),point->GetEnergyLoss()/dx);
			  dEX0=dEX0+((point->GetEnergyLoss())/dx);
			}
	
   	}
   		if(point->GetTrackID()==1)
  		{
	  		t1++;
		  	dE1=dE1+point->GetEnergyLoss();
			  pF1=pF1+vecPFront.Mag();
			  if(dx!=0)
  			{
	  		  cout<<"1  "<<pdcid<<endl;
			    cout<<"Eloss "<<(point->GetEnergyLoss())<<endl;
			    dEX1=dEX1+((point->GetEnergyLoss())/dx);
  			}
	  	}
		if(point->GetTrackID()==2)
		{
			cout<<pdcid<<endl;
			t2++;
			dE2=dE2+point->GetEnergyLoss();
			pF2=pF2+vecPFront.Mag();
			if(dx!=0)
			{
			dEX2=dEX2+((point->GetEnergyLoss())/dx);
			}
		}
		if(point->GetTrackID()>5)
			{
			cout<<pdcid<<endl;
			dE3=point->GetEnergyLoss();
			pF3=vecPFront.Mag();
			if(dx!=0)
				{
				dEX3=((point->GetEnergyLoss())/dx);
				}
			hist10->Fill(pF3,dEX3);
			if(pF3<pnull)
			{
				a[21]++;
				if((pF3>0.11)&&((dEX3)<(dEBBku->Eval(pF3,0,0)))&&((dEX3)>(dEBBkl->Eval(pF3,0,0)))) //K Schlauch
				{
					a[18]++;
					
				}
				if((pF3>0.2)&&((dEX3)<(dEBBpu->Eval(pF3,0,0)))&&((dEX3>(dEBBpl->Eval(pF3,0,0))))) //P Schlauch
				{
					a[19]++;
					hist10->Fill(pF3,dEX3);
					
				}
				if((pF3>0.03)&&((dEX3)<(dEBBpiu->Eval(pF3,0,0)))&&((dEX3)>(dEBBpil->Eval(pF3,0,0)))) //PI Schlauch
				{
					a[20]++;
					hist10->Fill(pF3,dEX3);
				}
			}
		}
		}
	



// //cout<<((1.0*j)/nEvents)<<endl;
// //cout<<t0<<endl;
// //cout<<dE0<<endl;
// 	if(t0>2) //Pionen
// 	{
// 		//hist1->Fill(dE0/t0);
// 		//hist4->Fill(pF0/t0,dE0/t0);
// 		//hist7->Fill(pF0/t0,dEX0/t0);
// 		if(pF0/t0<pnull)
// 		{
// 		a[15]++;
// 		if((pF0>0.11)&&((dEX0/t0)<(dEBBku->Eval(pF0/t0,0,0)))&&((dEX0/t0)>(dEBBkl->Eval(pF0/t0,0,0)))) //K Schlauch
// 		{
// 			if((pF0>0.03)&&((dEX0/t0)<(dEBBpiu->Eval(pF0/t0,0,0))))	//im Mischbereich
// 			{
// 			//a[6]++;
// 			}
// 			else	//
// 			{
// 			a[2]++;
// 			}
// 		}
// 		if((pF0>0.03)&&((dEX0/t0)<(dEBBpiu->Eval(pF0/t0,0,0)))&&((dEX0/t0)>(dEBBpil->Eval(pF0/t0,0,0)))) //PI Schlauch
// 		{
// 			
// 			if(((dEX0/t0)>(dEBBkl->Eval(pF0/t0,0,0))))	//im Mischbereich
// 			{
// 			a[1]++;
// 			}
// 			else
// 			{
// 			a[0]++;
// 			}
// 		}
// 		if(pF0>0.2&&((dEX0/t0)<(dEBBpu->Eval(pF0/t0,0,0)))&&((dEX0/t0)>(dEBBpl->Eval(pF0/t0,0,0)))) //P Schlauch
// 		{
// 			a[3]++;
// 		}
// 		}
// 	}
// 	if(t1>2)
// 	{
// 		//hist2->Fill(dE1/t1);
// 		//hist5->Fill(pF1/t1,dE1/t1);
// 		//hist8->Fill(pF1/t1,dEX1/t1);
// 		if(pF1/t1<pnull)
// 		{
// 		a[16]++;
// 		if((pF1>0.11)&&((dEX1/t1)<(dEBBku->Eval(pF1/t1,0,0)))&&((dEX1/t1)>(dEBBkl->Eval(pF1/t1,0,0)))) //K Schlauch
// 		{
// 			if((pF1>0.03)&&((dEX1/t1)<(dEBBpiu->Eval(pF1/t1,0,0))))
// 			{
// 			a[6]++;
// 			}
// 			else
// 			{
// 			a[7]++;
// 			}
// 		}
// 		if((pF1>0.03)&&((dEX1/t1)<(dEBBpiu->Eval(pF1/t1,0,0)))&&((dEX1/t1)>(dEBBpil->Eval(pF1/t1,0,0)))) //PI Schlauch
// 		{
// 			
// 			if(((dEX1/t1)>(dEBBkl->Eval(pF1/t1,0,0))))
// 			{
// 			//a[1]++;
// 			}
// 			else
// 			{
// 			a[5]++;
// 			}
// 		}
// 		if(pF1>0.2&&((dEX1/t1)<(dEBBpu->Eval(pF1/t1,0,0)))&&((dEX1/t1)>(dEBBpl->Eval(pF1/t1,0,0)))) //P Schlauch
// 		{
// 			a[8]++;
// 		}
// 		}
// 	}
// 	if(t2>2)
// 	{
// 		//hist3->Fill(dE2/t2);
// 		//hist6->Fill(pF2/t2,dE2/t2);
// 		//hist9->Fill(pF2/t2,dEX2/t2);
// 		if(pF2/t2<pnull)
// 		{
// 		a[17]++;
// 		if((pF2>0.11)&&((dEX2/t2)<(dEBBku->Eval(pF2/t2,0,0)))&&((dEX2/t2)>(dEBBkl->Eval(pF2/t2,0,0)))) //K Schlauch
// 		{
// 			a[12]++;
// 		}
// 		if(pF2>0.2&&((dEX2/t2)<(dEBBpu->Eval(pF2/t2,0,0)))&&((dEX2/t2)>(dEBBpl->Eval(pF2/t2,0,0)))) //P Schaluch
// 		{
// 			a[13]++;
// 		}
// 		if((pF2>0.03)&&((dEX2/t2)<(dEBBpiu->Eval(pF2/t2,0,0)))&&((dEX2/t2)>(dEBBpil->Eval(pF2/t2,0,0)))) //PI Schlauch
// 		{
// 			a[10]++;
// 		}
// 		}
// 	}
// 	if(t3!=0)
// 	{
// 		
// 	}
// }


//gPad->SetLogz();
hist7->Add(hist8);
hist7->Add(hist9);
hist7->Add(hist10);
hist7->Draw("col");
//dEBBk->Draw("same C");
//dEBBku->Draw("same C");
//dE0BBkl->Draw("same C");
//dEBBp->Draw("same C");
//dEBBpu->Draw("same C");
//dEBBpl->Draw("same C");
//dEBBpi->Draw("same C");
//dEBBpiu->Draw("same C");
//dEBBpil->Draw("same C");

can2->cd();
//gPad->SetLogz();
//hist7->Add(hist8);
//hist7->Add(hist9);
//hist7->Add(hist10);
hist7->Add(hist8);
hist7->Add(hist9);
//hist7->Add(hist10);
hist7->Draw("col");
//dEBBk->Draw("same C");
//dEBBku->Draw("same C");
//dEBBkl->Draw("same C");
//dEBBp->Draw("same C");
//dEBBpu->Draw("same C");
//dEBBpl->Draw("same C");
//dEBBpi->Draw("same C");
dEBBpiu->Draw("same C");
dEBBpil->Draw("same C");


/*
ltext+="";

ltext+="Gesamtzahl Protonen: ";
ltext+=a[0];
ltext+="";
ltext+="davon als Protonen bestimmt "
cout<<"Erfolgsrate "<<(a[1]*100.0)/a[0]<<" %"<<endl;
cout<<"Gesamtzahl Kaonen "<<a[4]<<endl;
cout<<"davon als Kaonen bestimmt "<<a[5]<<endl;
cout<<"Erfolgsrate "<<(a[5]*100.0)/a[4]<<" %"<<endl;
cout<<"Gesamtzahl Pionen "<<a[8]<<endl;
cout<<"davon als Pionen bestimmt "<<a[9]<<endl;
cout<<"Erfolgsrate "<<(a[9]*100.0)/a[8]<<" %"<<endl;
cout<<"Gesamtzahl andere Teilchen "<<a[6]+a[10]+a[2]<<endl;
cout<<"davon als Protonen bestimmt "<<a[2]<<endl;
cout<<"davon als Kaonen bestimmt "<<a[6]<<endl;
cout<<"davon als Pionen bestimmt "<<a[10]<<endl;
*/
/*
p2->cd();
TLatex *lat=new TLatex();
  lat->SetTextAlign(1);
   lat->SetTextSize(0.04);
TLatex *lat=new TLatex();
  lat->SetTextAlign(1);
   lat->SetTextSize(0.08);
ltext="Auswertung";
lat->DrawLatex(0.1,0.95,ltext);
pos=0.9;
abst=0.02;
lat->SetTextSize(0.045);
ltext="p_{max}=";
ltext+=pnull;
lat->DrawLatex(0.1,pos,ltext);
pos-=abst;
ltext="mit Untergrund, mit Track";
lat->DrawLatex(0.1,pos,ltext);
pos=0.85;
ltext="Gesamtzahl Protonen: ";
ltext+=a[17];
lat->DrawLatex(0.1,pos,ltext);
pos-=abst;
ltext="davon als Protonen bestimmt: ";
ltext+=a[13];
lat->DrawLatex(0.1,pos,ltext);
pos-=abst;
ltext="Erfolgsrate: ";
ltext+=(a[13]*100.0)/a[17];
lat->DrawLatex(0.1,pos,ltext);
pos-=abst;
ltext="Gesamtzahl Kaonen: ";
ltext+=a[16];
lat->DrawLatex(0.1,pos,ltext);
pos-=abst;
ltext="davon als Kaonen bestimmt: ";
ltext+=a[7]+a[6]/2;
lat->DrawLatex(0.1,pos,ltext);
pos-=abst;
ltext="Erfolgsrate: ";
ltext+=((a[7]+a[6]/2)*100.0)/a[16];
lat->DrawLatex(0.1,pos,ltext);
pos-=abst;
ltext="Gesamtzahl Pionen: ";
ltext+=a[15];
lat->DrawLatex(0.1,pos,ltext);
pos-=abst;
ltext="davon als Pionen bestimmt: ";
ltext+=a[0]+a[1]/2;
lat->DrawLatex(0.1,pos,ltext);
pos-=abst;
ltext="Erfolgsrate: ";
ltext+=((a[0]+a[1]/2)*100.0)/a[15];
lat->DrawLatex(0.1,pos,ltext);
pos-=abst;
ltext="Gesamtzahl sekundaerer Teilchen: ";
ltext+=a[21];
lat->DrawLatex(0.1,pos,ltext);
pos-=abst;
ltext="davon als Protonen bestimmt: ";
ltext+=a[19];
lat->DrawLatex(0.1,pos,ltext);
pos-=abst;
ltext="davon als Kaon bestimmt: ";
ltext+=a[18];
lat->DrawLatex(0.1,pos,ltext);
pos-=abst;
ltext="davon als Pion bestimmt: ";
ltext+=a[20];
lat->DrawLatex(0.1,pos,ltext);
pos-=abst;
ltext="Schnittmenge ";
ltext+=a[1]+a[6];
lat->DrawLatex(0.1,pos,ltext);
pos-=abst;
ltext="Falsche Protonen ";
ltext+=a[3]+a[8]+a[19];
lat->DrawLatex(0.1,pos,ltext);
pos-=abst;
ltext="Falsche Kaonen ";
ltext+=a[2]+a[12]+a[18];
lat->DrawLatex(0.1,pos,ltext);
pos-=abst;
ltext="Falsche Pionen ";
ltext+=a[5]+a[10]+a[20];
lat->DrawLatex(0.1,pos,ltext);
pos-=abst;
ltext="Falsch bestimmte Teilchen ";
ltext+=(a[3]+a[8]+a[19]+a[2]+a[12]+a[18]+a[5]+a[10]+a[20]);
lat->DrawLatex(0.1,pos,ltext);
pos-=abst;
ltext="Falsch als Proton bestimmte Teilchen ";
ltext+=(a[3]+a[8])*100.0/(a[15]+a[16]);
lat->DrawLatex(0.1,pos,ltext);
pos-=abst;
ltext="Falsch als Kaon bestimmte Teilchen ";
ltext+=(a[2]+a[12])*100.0/(a[15]+a[17]);
lat->DrawLatex(0.1,pos,ltext);
pos-=abst;
ltext="Falsch als Pion bestimmte Teilchen ";
ltext+=(a[5]+a[10])*100.0/(a[16]+a[17]);
lat->DrawLatex(0.1,pos,ltext);
pos-=abst;
ltext="Falsch bestimmte Teilchen ";
ltext+=(a[3]+a[8]+a[19]+a[2]+a[12]+a[18]+a[5]+a[10]+a[20])*100.0/(a[15]+a[16]+a[17]+a[21]);
lat->DrawLatex(0.1,pos,ltext);
pos-=abst;
ltext="Falsch als Proton bestimmte Teilchen ";
ltext+=(a[3]+a[8]+a[19])*100.0/(a[15]+a[16]+a[21]);
lat->DrawLatex(0.1,pos,ltext);
pos-=abst;
ltext="Falsch als Kaon bestimmte Teilchen ";
ltext+=(a[2]+a[12]+a[18])*100.0/(a[15]+a[17]+a[21]);
lat->DrawLatex(0.1,pos,ltext);
pos-=abst;
ltext="Falsch als Pion bestimmte Teilchen ";
ltext+=(a[5]+a[10]+a[20])*100.0/(a[16]+a[17]+a[21]);
lat->DrawLatex(0.1,pos,ltext);



can1->cd(2);
gPad->SetLogz();
hist4->Add(hist5);
hist4->Add(hist6);
hist4->Draw("col");


cout<<"Gesamtzahl Protonen "<<a[0]<<endl;
cout<<"davon als Protonen bestimmt "<<a[1]<<endl;
//cout<<"Erfolgsrate "<<(a[1]*100.0)/a[0]<<" %"<<endl;
cout<<"Gesamtzahl Kaonen "<<a[4]<<endl;
cout<<"davon als Kaonen bestimmt "<<a[5]<<endl;
//cout<<"Erfolgsrate "<<(a[5]*100.0)/a[4]<<" %"<<endl;
cout<<"Gesamtzahl Pionen "<<a[8]<<endl;
cout<<"davon als Pionen bestimmt "<<a[9]<<endl;
//cout<<"Erfolgsrate "<<(a[9]*100.0)/a[8]<<" %"<<endl;
cout<<"Gesamtzahl andere Teilchen "<<a[6]+a[10]+a[2]<<endl;
cout<<"davon als Protonen bestimmt "<<a[19]<<endl;
cout<<"davon als Kaonen bestimmt "<<a[]<<endl;
cout<<"davon als Pionen bestimmt "<<a[10]<<endl;
//cout<<"Fehlerrate "<<((a[7])*100.0)/a[6]<<" %"<<endl;
*/




  // -----   Finish   -------------------------------------------------------
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;
  // ------------------------------------------------------------------------
}
