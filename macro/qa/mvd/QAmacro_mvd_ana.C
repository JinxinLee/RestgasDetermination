// root macro to analyze the clusterization output
//#include "../../run/Tools.C"

int QAmacro_mvd_ana()
{
  cout << "QA Analysis module for the MVD - Hit resolution check." << endl;
  
  TString inFile = "mvdqasim.root";
  TString recoFile = "mvdqarec.root";
  TString parFile = "mvdqapar.root";
  
  Int_t nEvents = 100;
  Bool_t verbose = kTRUE;
  Bool_t isSuccessful = kFALSE;
  Bool_t test1=kTRUE, test2=kTRUE, test3=kTRUE;  
  
//  cout << "$VMCWORKDIR" << endl;
//  gROOT->LoadMacro("$VMCWORKDIR/macro/run/Tools.C");
//  LoadPandaStyle();

  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------

  // -----   Get Data from framework   --------------------------------------
  // load first the file with the GeoManager
  TString picture = recoFile;
  picture.ReplaceAll(".root",".png");

  TFile* f = new TFile(inFile.Data()); // the sim file you want to analyse
  TTree* t=(TTree*)f->Get("cbmsim");
  t->AddFriend("cbmsim",recoFile.Data()); // the reco file you want to analyse
  TFile* dbfile = new TFile(parFile.Data());

  PndGeoHandling* fGeoH = new PndGeoHandling(inFile,parFile);

  TGeoManager *geoMan;
  if (!gGeoManager) {      
    dbfile->Get("FairBaseParSet");      
    geoMan = gGeoManager;
    if(!geoMan) {      
      dbfile->Get("FairGeoParSet");      
      geoMan = gGeoManager;      
      if(!geoMan) {      
        std::cout<<"Could not find valid GeoManager. Abort now!"<<std::endl;      
        exit(1);      
      }      
    }      
  }      
  
  TClonesArray* mc_array=new TClonesArray("PndSdsMCPoint");
  t->SetBranchAddress("MVDPoint",&mc_array);//Branch names

  TClonesArray* digiPixel_array=new TClonesArray("PndSdsDigiPixel");
  t->SetBranchAddress("MVDPixelDigis",&digiPixel_array);//Branch names

  TClonesArray* digiStrip_array=new TClonesArray("PndSdsDigiStrip");
  t->SetBranchAddress("MVDStripDigis",&digiStrip_array);//Branch names

  TClonesArray* strhit_array=new TClonesArray("PndSdsHit");
  t->SetBranchAddress("MVDHitsStrip",&strhit_array);//Branch names

  TClonesArray* pixhit_array=new TClonesArray("PndSdsHit");
  t->SetBranchAddress("MVDHitsPixel",&pixhit_array);//Branch names

  if(!fGeoH){
    std::cout<<"No MvdGeoHandling existant. Abort now!"<<std::endl;
    exit(1);
  }
  fGeoH->FillSensorMap();
//  fGeoH->PrintSensorNames();

//  FairEventHeader* header = new FairEventHeader();
//  t->SetBranchAddress("EventHeader.", &header);
//  t->GetEntry(0);
//
//  Int_t runId = header->GetRunId();



  TH1D* hisDiff = new TH1D("diff","",100,-0.008,0.008);
  hisDiff->SetTitle("MVD Hit Resolution (double gaussian fit);#Deltax / cm;");

  double angmax = 0.02;
  TH1D* hisDiffTheta = new TH1D("DiffTheta","",100,-angmax,angmax);
  hisDiffTheta->SetTitle("#Theta resolution;#Delta#Theta/mrad;");

  TH1D* hisDiffPhi = new TH1D("DiffPhi","",100,-angmax,angmax);
  hisDiffPhi->SetTitle("#Phi resolution;#Delta#Phi/mrad;");

  TVector3 vecs, vecmc, vecdiff, mommc, sensorDim;
  Double_t tmpMaster[3],tmpLocal[3];
  TVector2 locals, localmc, localdiff;
  TGeoHMatrix* currentTransMat;
  TString detname;
  Double_t difftheta, diffphi;

  cout<<" -I- Start Loop"<<endl;
  for (Int_t j=0; j<nEvents && j<t->GetEntriesFast(); j++)
  {
    if(10==j) verbose=kFALSE;
    t->GetEntry(j);
    if(verbose) cout<<"Event No "<<j<<endl;
    else if (!(j%100)) cout <<"Event No "<<j<<endl;
    if(verbose) std::cout<<"Check sanity of the arrays:"<<std::endl;
    if(mc_array){
      if(verbose) std::cout<<"mc_array ok with "<<mc_array->GetEntriesFast()<<" entries"<<std::endl;
    }
    else {
      std::cout<<"*** mc_array broken - check your file! ."<<std::endl;
      test3=kFALSE;
    }
    if(digiPixel_array){
      if(verbose) std::cout<<"digiPixel_array ok with "<<digiStrip_array->GetEntriesFast()<<" entries"<<std::endl;
    }
    else {
      std::cout<<"*** digiPixel_array broken - check your file! ."<<std::endl;
      test3=kFALSE;
    }
    if(digiStrip_array){
      if(verbose) std::cout<<"digiStrip_array ok with "<<digiStrip_array->GetEntriesFast()<<" entries"<<std::endl;
    }
    else {
      std::cout<<"*** digiStrip_array broken - check your file! ."<<std::endl;
      test3=kFALSE;
    }
    if(strhit_array){
      if(verbose) std::cout<<"strhit_array ok with "<<strhit_array->GetEntriesFast()<<" entries"<<std::endl;
    }
    else {
      std::cout<<"*** strhit_array broken - check your file! ."<<std::endl;
      test3=kFALSE;
    }
    if(pixhit_array){
      if(verbose) std::cout<<"pixhit_array ok with "<<pixhit_array->GetEntriesFast()<<" entries"<<std::endl;
    }
    else {
      std::cout<<"*** pixhit_array broken - check your file! ."<<std::endl;
      test3=kFALSE;
    }


    // -----  PIXEL HITS  -----
    for (Int_t ii=0; ii<pixhit_array->GetEntriesFast(); ii++)
    {

      PndSdsHit *hit=(PndSdsHit*)pixhit_array->At(ii);
	    if(verbose) cout <<ii<< ".";
      detname = fGeoH->GetPath( hit->GetSensorID());
      geoMan->cd( detname.Data() );
      currentTransMat = geoMan->GetCurrentMatrix();
      vecs.SetXYZ(hit->GetX(), hit->GetY(), hit->GetZ());
      Int_t mcid = hit->GetRefIndex();
      if(verbose)cout<<mcid<<"  ";
      if(mcid<0 || mcid >= mc_array->GetEntriesFast()) continue;
      PndSdsMCPoint *point=(PndSdsMCPoint*)mc_array->At(mcid);
      Int_t mcpdg = -1;

      vecmc.SetXYZ( 0.5 * (point->GetX() + point->GetXOut()),
                    0.5 * (point->GetY() + point->GetYOut()),
                    0.5 * (point->GetZ() + point->GetZOut()));
      vecdiff = vecmc - vecs;
      difftheta = vecmc.Theta() - vecs.Theta();
      diffphi = vecmc.Phi() - vecs.Phi();
      //convert deg to mrad
      difftheta = difftheta*1000.*TMath::Pi()/180.;
      diffphi= diffphi*1000.*TMath::Pi()/180.;

      hisDiffTheta->Fill(difftheta);
      hisDiffPhi->Fill(diffphi);

      //--- Now move vectors to the local sensor system
      tmpMaster[0]=vecs.x();tmpMaster[1]=vecs.y();tmpMaster[2]=vecs.z();
      currentTransMat->MasterToLocal(tmpMaster,tmpLocal);
      vecs.SetXYZ(tmpLocal[0],tmpLocal[1],tmpLocal[2]);
      tmpMaster[0]=vecmc.x();tmpMaster[1]=vecmc.y();tmpMaster[2]=vecmc.z();
      currentTransMat->MasterToLocal(tmpMaster,tmpLocal);
      vecmc.SetXYZ(tmpLocal[0],tmpLocal[1],tmpLocal[2]);
      vecdiff = vecmc - vecs;

      hisDiff->Fill(vecdiff.X());
    }//end for ii (pixel hits in event)
	  if(verbose) cout <<endl;



    // -----  STRIP HITS  -----
    for (Int_t iii=0; iii<strhit_array->GetEntriesFast(); iii++)
    {
      PndSdsHit *hit=(PndSdsHit*)strhit_array->At(iii);
      detname = fGeoH->GetPath( hit->GetSensorID());
      geoMan->cd( detname.Data() );
      currentTransMat = geoMan->GetCurrentMatrix();

      vecs.SetXYZ(hit->GetX(), hit->GetY(), hit->GetZ());

      Int_t mcid = hit->GetRefIndex();
      if(mcid<0 || mcid >= mc_array->GetEntriesFast()) continue;
      PndSdsMCPoint *point=(PndSdsMCPoint*)mc_array->At(mcid);
      Int_t mcpdg = -1;

      vecmc.SetXYZ( 0.5 * (point->GetX() + point->GetXOut()),
                    0.5 * (point->GetY() + point->GetYOut()),
                    0.5 * (point->GetZ() + point->GetZOut()));
      vecdiff = vecmc - vecs;
      difftheta = vecmc.Theta() - vecs.Theta();
      diffphi = vecmc.Phi() - vecs.Phi();
      //convert deg to mrad
      difftheta = difftheta*1000.*TMath::Pi()/180.;
      diffphi= diffphi*1000.*TMath::Pi()/180.;

      hisDiffTheta->Fill(difftheta);
      hisDiffPhi->Fill(diffphi);

      //--- Now move vectors to the local sensor system
      tmpMaster[0]=vecs.x();tmpMaster[1]=vecs.y();tmpMaster[2]=vecs.z();
      currentTransMat->MasterToLocal(tmpMaster,tmpLocal);
      vecs.SetXYZ(tmpLocal[0],tmpLocal[1],tmpLocal[2]);
      tmpMaster[0]=vecmc.x();tmpMaster[1]=vecmc.y();tmpMaster[2]=vecmc.z();
      currentTransMat->MasterToLocal(tmpMaster,tmpLocal);
      vecmc.SetXYZ(tmpLocal[0],tmpLocal[1],tmpLocal[2]);
      vecdiff = vecmc - vecs;

      hisDiff->Fill(vecdiff.X());
    }//end for iii (strip hits in event)

  }// end for j (events)


  ////-----
  Int_t pix = 400;
  Int_t a = 2, b = 2;

  TCanvas* can1 = new TCanvas("MvdTestPlot","MCHit view in MVD",0,0,a*pix,b*pix);
  can1->Divide(a,b);

  TCanvas* can2 = new TCanvas("MvdResPlot","MVD point resolution");

  can2->cd();

  Double_t par[6]={0., 0., 0.0005, 0., 0., 0.002};
  //prefit peak
  TF1* g2    = new TF1("g2","gaus",-0.002,0.002);
  hisDiff->Fit(g2,"R");
  g2->GetParameters(&par[0]);
  //fit total
  TF1* total = new TF1("total","gaus(0)+gaus(3)",-0.008,0.008);
  total->SetParameters(par);
  total->SetLineColor(4);
  total->SetLineWidth(2);
  total->SetLineStyle(7);
  hisDiff->Fit(total,"R");
  total->GetParameters(&par[0]);

  hisDiff->SetMarkerStyle(3);
  hisDiff->SetMarkerSize(0.4);
  hisDiff->DrawCopy("pe");

  // change styling for text
  gStyle->SetOptTitle(kFALSE);
  gStyle->SetOptStat(kFALSE);

  TString str="";
  std::cout<<" --- Test resolution ---"<<std::endl;

  str = "Mean_{1} = ";
  str += (10000*par[1]); for (int i=0;i<6;i++) str.Chop();
  str += " #mum";
  std::cout<<str.Data()<<" is ";
  if( fabs(10000*par[1]) < 2*(10000*par[2]) ){
    std::cout<<"less";
    test1=test1 && kTRUE;
  } else {
    std::cout<<"MORE";
    test1=kFALSE;
  } std::cout<<" than 2 #sigma away from 0"<<std::endl;

  str = "Mean_{2} = ";
  str += (10000*par[4]); for (int i=0;i<6;i++) str.Chop();
  str += " #mum";
  std::cout<<str.Data()<<" is ";
  if( fabs(10000*par[4]) < 2*(10000*par[5]) ){
    std::cout<<"less";
    test1=test1 && kTRUE;
  } else {
    std::cout<<"MORE";
    test1=kFALSE;
  } std::cout<<" than 2 #sigma away from 0"<<std::endl;

  str = "#sigma_{1} = ";
  str += (10000*par[2]); for (int i=0;i<6;i++) str.Chop();
  str += " #mum";
//  DrawText( 0.2, 0.8, str.Data(),0.05,1);
  std::cout<< str.Data();
  if( fabs(10000*par[2]) < 10 ){
    std::cout<< " Passed a 10um window.";
    test2=test2 && kTRUE;
  } else {
    std::cout<<" Didn't pass a 10um window.";
    test2=kFALSE;
  } std::cout<<std::endl;

  str = "#sigma_{2} = ";
  str += (10000*par[5]); for (int i=0;i<6;i++) str.Chop();
  str += " #mum";
//  DrawText( 0.2, 0.7, str.Data(),0.05,1);
  std::cout<< str.Data();
  if( fabs(10000*par[5]) < 100 ){
    std::cout<< " Passed a 100um window.";
    test2=test2 && kTRUE;
  } else {
    std::cout<<" Didn't pass a 100um window.";
    test2=kFALSE;
  } std::cout<<std::endl;

  // reset styling for pictures
//  LoadPandaStyle();

  can2->Print(picture);

//  can2->Print(picture + "(");
//  can1->cd(1);
//  hisDiff->GetXaxis()->SetNdivisions(-05);
//  hisDiff->DrawCopy("");
//  can1->Update();mypad=(TPad*)gPad;BetterStatBox(mypad);
//  can1->cd(3);
//  hisDiffTheta->GetXaxis()->SetNdivisions(-05);
//  hisDiffTheta->DrawCopy("");
//  can1->Update();mypad=(TPad*)gPad;BetterStatBox(mypad);
//  can1->cd(4);
//  hisDiffPhi->GetXaxis()->SetNdivisions(-05);
//  hisDiffPhi->DrawCopy("");
//  can1->Update();mypad=(TPad*)gPad;BetterStatBox(mypad);
//  can1->Print(picture + ")");


  // -----   Finish   -------------------------------------------------------
  timer.Stop();
  Double_t rtime = timer.RealTime();
	Double_t ctime = timer.CpuTime();
	printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
  isSuccessful = test1 && test2 && test3;
  if(isSuccessful){
	  std::cout << " Test passed" << std::endl;
    std::cout << " All ok " << std::endl;
  } else {
    std::cout<<"Something is worong:"<<endl;
    std::cout<<"Test of Data Arrays: "<< ((test3) ? "ok" : "bad") <<std::endl;
    std::cout<<"Test of resolution mean: "<< ((test1) ? "ok" : "bad") <<std::endl;
    std::cout<<"Test of resolution sigma: "<< ((test2) ? "ok" : "bad") <<std::endl;
  }
  std::cout<<std::endl;
  return 0;
   
}
