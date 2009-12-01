{
 
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();

  // Load this example libraries
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");

   gSystem->Load("libField");

   PndMultiField *fField= new PndMultiField();

  // PndTransMap *map= new PndTransMap("TransMap", "R");
  // PndDipoleMap *map1= new PndDipoleMap("DipoleMap", "R");
   PndSolenoidMap *map21= new PndSolenoidMap("SolenoidMap1", "R");
   PndSolenoidMap *map22= new PndSolenoidMap("SolenoidMap2", "R");
   PndSolenoidMap *map23= new PndSolenoidMap("SolenoidMap3", "R");
//   map1->Init();
//   map2->Init();
  // fField->AddField(map);
 //  fField->AddField(map1);
   fField->AddField(map21);
   fField->AddField(map22);
   fField->AddField(map23);
  
    fField->Init();
//  map2->WriteRootFile("SolenoidMap.root","SolenoidMap");
//  map1->WriteRootFile("DipoleMap.root","DipoleMap");
//   map->WriteRootFile("TransMap.root","TransMap");
  
  //   Int_t fnz=map23->GetNz();
  //   Int_t fZmin=map23->GetZmin();
     
     
     
     Int_t fnz1=map21->GetNz()-1;
     Int_t fnz2=map22->GetNz()-1;
     Int_t fnz3=map23->GetNz()-1;
     cout << "Nz1 =" << fnz1 << " Nz2 =  " << fnz2 << " Nz3 =  " << fnz3 << endl;
     Int_t fnz=fnz1/4+fnz2/2+fnz3/4;
     
     Int_t fZmin1=map21->GetZmin();
     Int_t fZmin2=map22->GetZmin();
     Int_t fZmin3=map23->GetZmin();
     
     
     Int_t fZmax1=map21->GetZmax();
     Int_t fZmax2=map22->GetZmax();
     Int_t fZmax3=map23->GetZmax();
     
     Int_t fdz1=map21->GetZstep();
     Int_t fdz2=map22->GetZstep();
     Int_t fdz3=map23->GetZstep();
     cout << "dz1 =" << fdz1 << " dz2 =  " << fdz2 << " dz3 =  " << fdz3 << endl;
     Double_t x=0;
     Double_t y=0;
     Double_t z=0;
     Double_t po[3], BB[3];
     Double_t Btot=0;
     
     TH1F *Bx=new TH1F("Bx(z)","Bx(z)",fnz,fZmin1,fZmax1);
     TH1F *By=new TH1F("By(z)","By(z)",fnz,fZmin1,fZmax2);
     TH1F *Bz=new TH1F("Bz(z)","Bz(z)",fnz,fZmin1,fZmax3);
     TH1F *Btotal=new TH1F("B(z)","B(z)",fnz,fZmin1,fZmax3);

     for (Int_t iz=0; iz<fnz1; iz++) 
     {
	  z = fZmin1 + Double_t(iz) * fdz1;
	      po[0]=x; po[1]=y; po[2]=z;
	      fField->GetFieldValue(po,BB); //return valuse in KG (G3)
	       Bx->SetBinContent(iz+1,BB[0]/ 10.);
	       By->SetBinContent(iz+1,BB[1]/ 10.);
	       Bz->SetBinContent(iz+1,BB[2]/ 10.);
               Btot=TMath::Sqrt(BB[0]*BB[0]+BB[1]*BB[1]+BB[2]*BB[2]);
	       Btotal->SetBinContent(iz+1,Btot/ 10.);
     }
     cout << """No = " << iz << endl;
    for (Int_t iz=0; iz<=fnz2; iz++) 
     {
	  z = fZmin2 + Double_t(iz) * fdz2;
	      po[0]=x; po[1]=y; po[2]=z;
	      fField->GetFieldValue(po,BB); //return valuse in KG (G3)
	       Bx->SetBinContent(iz+1,BB[0]/ 10.);
	       By->SetBinContent(iz+1,BB[1]/ 10.);
	       Bz->SetBinContent(iz+1,BB[2]/ 10.);
               Btot=TMath::Sqrt(BB[0]*BB[0]+BB[1]*BB[1]+BB[2]*BB[2]);
	       Btotal->SetBinContent(iz+1,Btot/ 10.);
     }
      cout << """No = " << iz << endl;
     for (Int_t iz=0; iz<=fnz3; iz++) 
      
     {
	  z = fZmin3 + Double_t(iz) * fdz3;
	      po[0]=x; po[1]=y; po[2]=z;
	      fField->GetFieldValue(po,BB); //return valuse in KG (G3)
	       Bx->SetBinContent(iz+1,BB[0]/ 10.);
	       By->SetBinContent(iz+1,BB[1]/ 10.);
	       Bz->SetBinContent(iz+1,BB[2]/ 10.);
               Btot=TMath::Sqrt(BB[0]*BB[0]+BB[1]*BB[1]+BB[2]*BB[2]);
	       Btotal->SetBinContent(iz+1,Btot/ 10.);
     }
     
       cout << """No = " << iz << endl;
     
     
     THStack *h1= new THStack("B(z)","Panda Field X=Y=0") ;
     
   TCanvas *c1 = new TCanvas("c1", "c1",4,31,900,600);
   c1->Range(-267,-0.285949,403,2.38354);
   c1->SetBorderSize(2);
   c1->SetFrameFillColor(0);
    

   Bx->SetLineColor(2);
   By->SetLineColor(3);
   Bz->SetLineColor(4);

   h1->Add(Bx);
   h1->Add(By);
   h1->Add(Bz);
   h1->Add(Btotal);

   h1->Draw("nostack");
   
   TPaveText *pt = new TPaveText(0.01,0.945,0.311609,0.995,"blNDC");
   pt->SetName("title");
   pt->SetBorderSize(2);
   pt->SetFillColor(19);
   TText *text = pt->AddText("Panda Field X=Y=0");
   pt->Draw();
   
   pt = new TPaveText(148.439,1.2000,250.534,1.94786,"br");
   pt->SetFillColor(19);
   pt->Draw();
   TLine *line = new TLine(160.,1.84535,193.685,1.84535);
   line->Draw();
   
   tex = new TLatex(200.,1.81766,"Bz");
   tex->SetLineWidth(2);
   line->SetLineColor(4);
   tex->Draw();

   line = new TLine(160.,1.6868,196.,1.6868);
   line->SetLineColor(2);
   line->Draw();

   tex = new TLatex(200.,1.64905,"Bx");
   tex->SetLineWidth(2);
   tex->Draw();

   line = new TLine(160.,1.51818,196.,1.51818);
   line->SetLineColor(3);
   line->Draw();

   tex = new TLatex(200.,1.4900,"By");
   tex->SetLineWidth(2);
   tex->Draw();


   line = new TLine(160.,1.31818,196.,1.31818);
   tex->SetLineWidth(2);
   line->Draw();

   tex = new TLatex(200.,1.3100,"Bmod");
   tex->SetLineWidth(2);
   tex->Draw();
   


   tex = new TLatex(-224.51,0.970725,"Tesla");
   tex->SetTextSize(0.054717);
   tex->SetTextAngle(90.0);
   tex->SetLineWidth(2);
   tex->Draw();

      tex = new TLatex(37.0985,-0.200324,"Z (cm)");
   tex->SetLineWidth(2);
   tex->Draw();
 
   TCanvas *c2 = new TCanvas("c2", "c2",4,31,1200,700);
   c2->Divide(1,3);
   c2->cd(1);
   Bx->Draw();
   c2->cd(2);
   By->Draw();
   c2->cd(3);
   Bz->Draw();

     Int_t fXmin=map23->GetXmin();
     Int_t fXmax=map23->GetXmax();
     Int_t fdx=map23->GetXstep();
     Int_t fnx=map23->GetNx();
     y=0;
     TH2F *B=new TH2F("B mod","B mod y=0 plane ",fnz+3,fZmin1,fZmax3,2*fnx+3,-fXmax,fXmax);

  	for (Int_t iz=0; iz<fnz1; iz++) 
	{
	  z = fZmin1 + Double_t(iz) * fdz3;
	      po[1]=y; po[2]=z;
              for (Int_t ix=0; ix<(2*fnx-1); ix++) {
		  x= -fXmax + Double_t(ix) * fdx;
                //  cout << "X = " << x << endl; 
		  po[0]=x;
	         fField->GetFieldValue(po,BB); //return valuse in KG (G3)
                 Btot=TMath::Sqrt(BB[0]*BB[0]+BB[1]*BB[1]+BB[2]*BB[2]);
	         B->SetBinContent(iz,ix,Btot/ 10.);
	      }
	}
 	for (Int_t iz=0; iz<fnz2; iz++) 
	{
	  z = fZmin2 + Double_t(iz) * fdz3;
	      po[1]=y; po[2]=z;
              for (Int_t ix=0; ix<(2*fnx-1); ix++) {
		  x= -fXmax + Double_t(ix) * fdx;
                //  cout << "X = " << x << endl; 
		  po[0]=x;
	         fField->GetFieldValue(po,BB); //return valuse in KG (G3)
                 Btot=TMath::Sqrt(BB[0]*BB[0]+BB[1]*BB[1]+BB[2]*BB[2]);
	         B->SetBinContent(iz,ix,Btot/ 10.);
	      }
	}
 	for (Int_t iz=0; iz<fnz3; iz++) 
	{
	  z = fZmin3 + Double_t(iz) * fdz3;
	      po[1]=y; po[2]=z;
              for (Int_t ix=0; ix<(2*fnx-1); ix++) {
		  x= -fXmax + Double_t(ix) * fdx;
                //  cout << "X = " << x << endl; 
		  po[0]=x;
	         fField->GetFieldValue(po,BB); //return valuse in KG (G3)
                 Btot=TMath::Sqrt(BB[0]*BB[0]+BB[1]*BB[1]+BB[2]*BB[2]);
	         B->SetBinContent(iz,ix,Btot/ 10.);
	      }
	}

	TCanvas *c3 = new TCanvas("c3", "c3",4,31,1200,700);
	B->Draw("cont1");
	
};












