#include <iomanip>
void runRadlenCalc(TGeoManager * gGeoMan, Int_t tpc)
{
  TStopwatch timer;
  timer.Start();

  //  Bool_t dogif=kTRUE; 
  Bool_t dogif=kFALSE; 

  //gROOT->LoadMacro("macro/tpc/nicehist.C");

  Int_t phistart, phiend, thestart, theend;
  Double_t phistep, thestep;
  Int_t piotr;
  Int_t phibin, thebin, lastbin;
  phistart=0;
  phiend=360;
  phistep=.25;
  thestart=0.;
  theend=180;
  thestep=.25;
  piotr=10;

  phibin=Int_t((phiend-phistart)/phistep);
  thebin=Int_t((theend-thestart)/(thestep));

  cout<<phibin<<" "<<thebin<<endl;

  Double_t labs;
  Double_t ldir[3];
  Int_t phicount;
  Double_t rphimean;

  Double_t trmin, trmax, tzmin, tzmax;
  Int_t tpc;
  TString filename;

  if(tpc==0){
    trmin = -22.0;
    trmax = 22.0;

    tzmin=-70.0;
    tzmax=20.0;
    filename="outfiles/radlenhists_proto.root";

  }else if(tpc==1){
    trmin = -44.0;
    trmax = 44.0;

    tzmin=-60.0;
    tzmax=120.0;
    filename="outfiles/radlenhists_TPC.root";
  }    

// {{{ create histos

  TH3F* hradmap = new TH3F("Radlen Map","Radlen Map",400,trmin,trmax,400,trmin,trmax,400,tzmin,tzmax);
  //  hradmap->SetXtitle("
  
  TH1F* hradvsphi = new TH1F("Radlen phimean vs theta","Radlen pimean vs theta",thebin,thestart,theend);
  hradvsphi->SetXTitle("Theta");
  hradvsphi->SetYTitle("X_{0} (%)");
  TH1F* hradvstheta = new TH1F("Radlen vs theta","Radlen vs theta",thebin,thestart,theend);
  hradvstheta->SetXTitle("Theta");
  hradvstheta->SetYTitle("X/X_{0}(%)");
  TH2F* hradphivstheta = new TH2F("Radlen","Radlen",phibin,phistart,phiend,thebin,thestart,theend);
  hradphivstheta->SetXTitle("Phi");
  hradphivstheta->SetYTitle("Theta");
  TH2F* hxzproj = new TH2F("XZ-Projection","XZ-Projection",(1.5*thebin),tzmin,tzmax,(phibin+10)/2,0.,trmax);
  hxzproj->SetXTitle("Z Position (cm)");
  hxzproj->SetYTitle("Radius (cm)");
  TH2F* hthetabin = new TH2F("Radiation Length","Radlen phi Thetabin",phibin,trmin,trmax,phibin,trmin,trmax);
  hthetabin->SetXTitle("X Position (cm)");
  hthetabin->SetYTitle("Y Position (cm)");

// }}}

  TCanvas* c4 = new TCanvas("Radiation length  vs Theta","Radiation length vs theta");
  c4->cd();
  c4->Divide(1,2);
  c4->GetPad(2)->Divide(2,1);
 
  c4->GetPad(1)->cd();
  hradvsphi->Draw();
  c4->GetPad(2)->GetPad(1)->cd();
  hthetabin->Draw("colz");
  c4->GetPad(2)->GetPad(2)->cd();
  hxzproj->Draw("colz");

  

  hradvsphi->SetStats(kFALSE);
  hthetabin->SetStats(kFALSE);
  hxzproj->SetStats(kFALSE);

  const Int_t histcount=5;
  TH1* histos[histcount];
  histos[0]=hradmap;
  histos[1]=hradvsphi;
  histos[2]=hradvstheta;
  histos[3]=hradphivstheta;
  histos[4]=hxzproj;

  Double_t px,py,pz,dx,dy,dz;
  Double_t theta,phi;
  theta=90;
  phi=90;
  px=py=pz=0;
  //  px=0.05;
  
    Bool_t debug = kFALSE;
  //    Bool_t debug = kTRUE;

  TGeoNode* node;
  TGeoNode* lastnode;
  TGeoNode* lastlastnode;
  Double_t dist=2000.;
  Double_t X=0.;
  Double_t step=100;
  Double_t snext;
  Double_t thick=0;  
  Double_t tx[2], ty[2], tz[3];
  Double_t actpoint[5];
  Double_t radlen;
  Double_t pradlen;
  Double_t gradlen;

  Double_t xpos,ypos,zpos,radi;
  Double_t lastx, lasty, lastz, lastr;
  lastr=trmax;
  lastx=trmax;
  lasty=0.;
  lastz=tzmax;

  gGeoMan->CheckGeometry();

  if(debug)cout<<"Top Volume: "<<gGeoMan->GetTopVolume()->GetName()<<endl;
  gGeoMan->SetVisLevel(2);
  TCanvas* c1 = new TCanvas("3d","3d");
  gGeoMan->GetTopVolume()->Draw();
  //  gGeoMan->GetTopVolume()->Raytrace();
 
  for(theta=thestart;theta<theend;theta+=thestep)
    {
    rphimean=0;
    phicount=0;
    for(phi=phistart;phi<phiend;phi+=phistep)
      {
	gradlen=0;
	dx = TMath::Sin(TMath::DegToRad()*theta)*TMath::Cos(TMath::DegToRad()*phi);
	dy = TMath::Sin(TMath::DegToRad()*theta)*TMath::Sin(TMath::DegToRad()*phi);
	dz = TMath::Cos(TMath::DegToRad()*theta);
	
	if(debug)cout<<"angles: "<<theta<<" "<<phi<<endl;
	if(debug)cout<<"directions"<<dx<<" "<<dy<<" "<<dz<<endl;

	gGeoMan->InitTrack(px,py,pz,dx,dy,dz);
 
	node=gGeoMan->GetCurrentNode();
	for(Int_t i=0;i<5;i++) actpoint[i]=gGeoMan->GetCurrentPoint()[i];
	if(debug)cout<<"actpoint: "<<actpoint[0]<<" "<<actpoint[1]<<" "<<actpoint[2]<<endl;

	tx[0]=actpoint[0];
	ty[0]=actpoint[1];
	tz[0]=actpoint[2];
	if(debug)cout<<"Now in: "<<node->GetVolume()->GetName()<<endl;
	snext=gGeoMan->GetStep();
	//	cout<<"stepsize "<<snext<<endl;    
	
	X=0;
	while(X<dist){
	  node=gGeoMan->FindNextBoundaryAndStep();   
	  if(debug)if(node)cout<<"Now in node: "<<node->GetVolume()->GetName()<<endl;
	  
	  snext=gGeoMan->GetStep();
	  
	    if(lastnode){
	      thick+=snext;
	      radlen=lastnode->GetVolume()->GetMaterial()->GetRadLen();
	      if(debug)cout<<"Radlen of material: "<<radlen<<endl;
	      pradlen=snext/radlen;
	      gradlen+=pradlen;
	      if(debug)cout<<"thickness of node "<<lastnode->GetVolume()->GetName()<<" : "<<snext<<endl;
	      if(debug)cout<<"radlen pf node: "<<pradlen<<" ges radlen: "<<gradlen<<endl;
	      if(debug)cout<<"total thickness: "<<thick<<endl;
	    }
	  
	  X+=snext;
	  if(debug)cout<<"stepped by: "<<snext<<" X="<<X<<endl;
	  if(debug)cout<<endl;
	  lastlastnode=lastnode;
	  lastnode=node;	 
	}
	
	for(Int_t i=0;i<5;i++) actpoint[i]=gGeoMan->GetCurrentPoint()[i];
	if(debug)cout<<"actpoint: "<<actpoint[0]<<" "<<actpoint[1]<<" "<<actpoint[2]<<endl;
	tx[1]=actpoint[0];
	ty[1]=actpoint[1];
	tz[1]=actpoint[2];   
	//	cout<<tz[1]<<endl;
	gradlen=gradlen*100;
	
	for(Int_t i=0;i<2;i++)
	  if(debug)cout<<tx[i]<<" "<<ty[i]<<" "<<tz[i]<<endl;

// {{{ calc length of polyline for 3d radlen

	labs=sqrt(tx[1]*tx[1]+ty[1]*ty[1]+tz[1]*tz[1]);
	//	cout<<"absolute: "<<labs<<endl;
	ldir[0]=tx[1]/labs;
	ldir[1]=ty[1]/labs;
	ldir[2]=tz[1]/labs;
	//	cout<<"dir: "<<ldir[0]<<" "<<ldir[1]<<" "<<ldir[2]<<endl;
	tx[0]=tx[1];
	ty[0]=ty[1];
	tz[0]=tz[1];
	tx[1]=ldir[0]*gradlen+tx[0];
	ty[1]=ldir[1]*gradlen+ty[0];
	tz[1]=ldir[2]*gradlen+tz[0];
	//	cout<<"start: "<<tx[0]<<" "<<ty[0]<<" "<<tz[0]<<endl;
	//	cout<<"end: "<<tx[1]<<" "<<ty[1]<<" "<<tz[1]<<endl;
	TPolyLine3D* track = new TPolyLine3D(2,tx,ty,tz);
	track->SetLineColor(kBlack);
	if( (Int_t)(theta/thestep)%(Int_t)(piotr/thestep) == 0 )
	  if( (Int_t)(phi/phistep)%(Int_t)(piotr/phistep) == 0){
	    c1->cd();
	    if(labs>0)track->Draw("same");
	    //c1->Update();
	  }

// }}}

	//	cout<<"Total radlen: "<<gradlen<<endl;
	hradmap->Fill(tx[0],ty[0],tz[0],gradlen);

	
	if(gradlen != 0 ){
	 rphimean+=gradlen;
	 phicount++;
	}
		
	xpos=tx[0];
	ypos=ty[0];
	zpos=tz[0];

	hradphivstheta->Fill(phi,theta,gradlen);	

	if(lastbin != hthetabin->FindBin(xpos,ypos))
	  hthetabin->Fill(xpos,ypos,gradlen);
	lastbin=hthetabin->FindBin(xpos,ypos);
	

	if(phi==180){
	  radi=sqrt(xpos*xpos+ypos*ypos);
	}
      }

    //    cout<<gradlen<<endl;
    
    if(rphimean !=0 ) rphimean=rphimean/phicount;
   
    hradvstheta->Fill(theta,gradlen);
    hradvsphi->Fill(theta,rphimean);
    
   
    //    cout<<setprecision(20)<<"last act z: "<<lastz<<" "<<zpos<<" delta: "<<TMath::Abs(lastz-zpos)<<endl;
    //    cout<<"last r act r: "<<lastr<<" "<<radi<<" delta: "<<TMath::Abs(lastr-radi)<<endl;


    for(Int_t i=0;i<10;i++){
      if(TMath::Abs(lastz-zpos)<1e-10){
	//	cout<<"zfill: "<<zpos+i*(abs(tzmax-tzmin)/(thebin+20))<<endl;
	hxzproj->Fill(zpos+i*(hxzproj->GetXaxis()->GetBinWidth(1)),radi,rphimean);
      }
      if(TMath::Abs(lastr-radi)<1e-10){
	//	cout<<"rfill: "<<radi+i*(trmax/((phibin+10)/2))<<endl;
	hxzproj->Fill(zpos,radi+i*(hxzproj->GetYaxis()->GetBinWidth(1)),rphimean);
      }

    
    }
    
    // cout<<endl;
    //    hxzproj->Fill(zpos,radi,rphimean);
    lastr=radi;
    lastx=xpos;
    lasty=ypos;
    lastz=zpos;
    //cout<<zpos<<" "<<radi<<endl;
    
    if(dogif)
      if(gradlen>0){
	c4->GetPad(1)->cd();
	hradvsphi->Draw();
	c4->GetPad(2)->GetPad(1)->cd();
	//	hthetabin->GetZaxis()->SetRangeUser();
	hthetabin->Draw("colz");
	c4->GetPad(2)->GetPad(2)->cd();
	//	hxzproj->GetZaxis()->SetRangeUser();
	hxzproj->Draw("colz");
	c4->Update();
	c4->Print("radvstheta.gif+");
	c4->Print(Form("animated/proto/radvstheta_%f.jpg",theta));
	hthetabin->Reset("M");
      }
 
    if( (Int_t)(theta/thestep)%(Int_t)(piotr/thestep) == 0 ){
      cout<<theta<<endl;
    }
  }

  //  gStyle->SetPalette(1); 
  //  gROOT->SetStyle("Plain");

  //TCanvas* c2 = new TCanvas("3D RadMap","3D RadMap");
  //c2->cd();
  //gStyle->SetCanvasPreferGL(1);
  //  nicehist(hradmap);
  //  hradmap->Draw("iso");

  //TCanvas* c3 = new TCanvas("Rad vs Theta","Rad vs theta");
  //c3->cd();
  //hradvsphi->Draw();

  //TCanvas* c4 = new TCanvas("Rad vs Theta","Rad vs Theta");
  //c4->cd();
  //  nicehist(hradvstheta);
  //hradvstheta->Draw();

  //TCanvas* c5 = new TCanvas("Rad Phi vs Theta","Rad Phi vs Theta");
  //c5->cd();
  //hradphivstheta->Draw("colz");

  //TCanvas* c6 = new TCanvas("XZ-Proj","XZ-Proj");
  //c6->cd();
  //hxzproj->Draw("colz");

  TFile* outfile = new TFile(filename,"RECREATE");
  for(Int_t i=0; i<histcount;i++)
    histos[i]->Write();
  c1->Write();
  outfile->Close();

  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout<<"Realtime: "<<rtime<<" CPUtime: "<<ctime<<endl;
}
