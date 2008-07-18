// root macro to analyze the clusterization output
{
  int  nEvents = 1000;
  bool verbose = false;

  // -----  Load libraries   ------------------------------------------------
  gROOT->Macro("../Libs.C");
  gROOT->LoadMacro("../Tools.C");
  TString detFilter = "";// "","Trap","Rect"
  LoadPandaStyle();

  // -----   Timer   --------------------------------------------------------
//   TStopwatch timer;
//   timer.Start();
  // ------------------------------------------------------------------------


//   TString name = "../data/mvddpm6GeV.root";
  TString name = "../data/mvdStrip.root";
  PndMvdFileNameCreator namecreator(name.Data());
  std::string inFile = namecreator.GetSimFileName(false);
  std::string digiFile = namecreator.GetDigiFileName(false);
  std::string recoFile = namecreator.GetRecoFileName(false);
  TString picture = namecreator.GetRecoFileName(false);
  picture.ReplaceAll(".root",".ps");

  TFile* f = new TFile(inFile.c_str()); // the sim file you want to analyse
  TTree* t=(TTree*)f->Get("cbmsim");
  t->AddFriend("cbmsim",digiFile.c_str()); // the digi file
  t->AddFriend("cbmsim",recoFile.c_str()); // the reco file you want to analyse

  TClonesArray* mc_array=new TClonesArray("PndMvdMCPoint");
  t->SetBranchAddress("MVDPoint",&mc_array);//Branch names

  TClonesArray* digiPixel_array=new TClonesArray("PndMvdDigiPixel");
  t->SetBranchAddress("MVDPixelDigis",&digiPixel_array);//Branch names

  TClonesArray* digiStrip_array=new TClonesArray("PndMvdDigiStrip");
  t->SetBranchAddress("MVDStripDigis",&digiStrip_array);//Branch names

  TClonesArray* clust_array=new TClonesArray("PndMvdCluster");
  t->SetBranchAddress("MVDStripClusterCand",&clust_array);//Branch names
//   t->SetBranchAddress("MVDClusterCand",&clust_array);//Branch names

  TClonesArray* hit_array=new TClonesArray("PndMvdHit");
  t->SetBranchAddress("MVDHitsStrip",&hit_array);//Branch names
//   t->SetBranchAddress("MVDClusterHit",&hit_array);//Branch names



  TGeoManager *geoMan = (TGeoManager*) gDirectory->Get("CBMGeom");
  PndMvdGeoHandling* fGeoH = new PndMvdGeoHandling(geoMan);

  Int_t nbins = 120; //200
  Double_t rim = 22.5;
  TH2D* hisxymc = new TH2D("xymc","MVD MC Points, xy view;x / cm;y / cm",nbins,-rim,rim,nbins,-rim,rim);
  TH2D* hisrzmc = new TH2D("rzmc","MVD MC Points, rz view;z / cm;r/ cm",nbins,-rim,rim,nbins,-rim,rim);
  TH1D* hisde = new TH1D("de","MVD MC Points, Energyloss;E / GeV;",nbins,0.,0.002);
  TH2D* hisLocalXYMC = new TH2D("LocalxyMC","Local MC Point XY;x_{L} / cm;y_{L} / cm",nbins,-5.,5.,nbins,-5.,5.);
  TH1D* hisLocalZMC  = new TH1D("LocalzMC","Local MC Point Z;Z_{L} / cm;",nbins,-5.,5.);//-0.02,0.02);

  TH2D* hisxy = new TH2D("xy","MVD reco Hit, xy view;x / cm;y / cm",nbins,-rim,rim,nbins,-rim,rim);
  TH2D* hisrz = new TH2D("rz","MVD reco Hit, rz view;z / cm;r/ cm",nbins,-rim,rim,nbins,-rim,rim);
  TH2D* hisLocalXY = new TH2D("Localxy","MVD Local reco Hit XY;x_{L} / cm;y_{L} / cm",nbins,-5.,5.,nbins,-5.,5.);
  TH1D* hisrecoeloss = new TH1D("derec","MVD reco Hit energy deposit;E / GeV;",nbins,0.,0.002);


  TH2D* hisDiffXY = new TH2D("diffxy","",nbins,-0.01,0.01,nbins,-0.04,0.04);
  hisDiffXY->SetTitle("(MC - RECO) Hit coordinates xy view;#Deltax / cm;#Deltay / cm");
  TH2D* hisDiffRZ = new TH2D("diffrz","",nbins,-0.02,0.02,nbins,-0.00,0.04);
  hisDiffRZ->SetTitle("(MC - RECO) Hit coordinates rz view;#Deltaz / cm;#Deltar / cm");

  Double_t range = 0.01;
  Double_t zed = 0.01;
  Int_t bins = 100;

  TH1D* hisDiffX = new TH1D("diffx","(MC - RECO) Hit coordinate x;#Deltax / cm;",bins,-range,range);
  TH1D* hisDiffY = new TH1D("diffy","(MC - RECO) Hit coordinate y;#Deltay / cm;",bins,-range,range);
  TH1D* hisDiffZ = new TH1D("diffz","(MC - RECO) Hit coordinate z;#Deltaz / cm;",bins,-range,range);
  TH1D* hisDiffDiskX = new TH1D("diffDiskx","",bins,-range,range);
  TH1D* hisDiffDiskY = new TH1D("diffDisky","",bins,-range,range);
  TH1D* hisDiffDiskZ = new TH1D("diffDiskz","",bins,-range,range);
  TH1D* hisDiffSideX = new TH1D("diffSidex","",bins,-range,range);
  TH1D* hisDiffSideY = new TH1D("diffSidey","",bins,-range,range);
  TH1D* hisDiffSideZ = new TH1D("diffSidez","",bins,-range,range);
  TH1D* hisDiffOneX  = new TH1D("diffOnex" ,"",bins,-range,range);
  TH1D* hisDiffOneY  = new TH1D("diffOney" ,"",bins,-range,range);
  TH1D* hisDiffOneZ  = new TH1D("diffOnez" ,"",bins,-zed,zed);
  TH1D* hisDiffMoreX = new TH1D("diffMorex","",bins,-range,range);
  TH1D* hisDiffMoreY = new TH1D("diffMorey","",bins,-range,range);
  TH1D* hisDiffMoreZ = new TH1D("diffMorez","",bins,-zed,zed);

  double angmax = 0.02;
  TH1D* hisDiffTheta = new TH1D("DiffTheta","#Theta resolution;#Delta#Theta/mrad;",100,-angmax,angmax);
  TH1D* hisDiffThetaDisk = new TH1D("DiffThetaDisk","",100,-angmax,angmax);
  TH1D* hisDiffThetaSide = new TH1D("DiffThetaSide","",100,-angmax,angmax);
  TH1D* hisDiffThetaOne  = new TH1D("DiffThetaOne" ,"",100,-angmax,angmax);
  TH1D* hisDiffThetaMore = new TH1D("DiffThetaMore","",100,-angmax,angmax);

  TH1D* hisDiffPhi = new TH1D("DiffPhi","#Phi resolution;#Delta#Phi/mrad;",100,-angmax,angmax);
  TH1D* hisDiffPhiDisk = new TH1D("DiffPhiDisk","",100,-angmax,angmax);
  TH1D* hisDiffPhiSide = new TH1D("DiffPhiSide","",100,-angmax,angmax);
  TH1D* hisDiffPhiOne  = new TH1D("DiffPhiOne" ,"",100,-angmax,angmax);
  TH1D* hisDiffPhiMore = new TH1D("DiffPhiMore","",100,-angmax,angmax);

  TH1I* hisClustSize = new TH1I("ClustSize","Cluster size",10,0,10);
  hisClustSize->SetTitle("Cluster size;n_{cl};");
  TH1I* hisClustSizeDisk = new TH1I("ClustSizeDisk","Cluster size",10,0,10);
  TH1I* hisClustSizeSide = new TH1I("ClustSizeSize","Cluster size",10,0,10);
  TH1I* hisClustSizeTop = new TH1I("ClustSizetop","Cluster size",10,0,10);
  TH1I* hisClustSizeBot = new TH1I("ClustSizebot","Cluster size",10,0,10);

  TVector3 vecsloc, vecmcloc, vecs, vecmc, vecdiff, mommc, sensorDim;
  TString detname;
  Double_t difftheta, diffphi;

  for (Int_t j=0; j<nEvents && j<t->GetEntriesFast(); j++)
  {
    t->GetEntry(j);
    if(verbose) cout<<"Event No "<<j<<endl;
    else if (!(j%100)) cout <<"Event No "<<j<<endl;
    if(verbose) std::cout<<"Check sanity of the arrays:"<<std::endl;
    if(mc_array){
      if(verbose) std::cout<<"mc_array ok with "<<mc_array->GetEntriesFast()<<" entries"<<std::endl;
    }
    else std::cout<<"*** mc_array broken - check your file! ."<<std::endl;
    if(digiPixel_array){
      if(verbose) std::cout<<"digiPixel_array ok with "<<digiStrip_array->GetEntriesFast()<<" entries"<<std::endl;
    }
    else std::cout<<"*** digiPixel_array broken - check your file! ."<<std::endl;
    if(digiStrip_array){
      if(verbose) std::cout<<"digiStrip_array ok with "<<digiStrip_array->GetEntriesFast()<<" entries"<<std::endl;
    }
    else std::cout<<"*** digiStrip_array broken - check your file! ."<<std::endl;
    if(clust_array){
      if(verbose) std::cout<<"clust_array ok with "<<clust_array->GetEntriesFast()<<" entries"<<std::endl;
    }
    else std::cout<<"*** clust_array broken - check your file! ."<<std::endl;
    if(hit_array){
      if(verbose) std::cout<<"hit_array ok with "<<hit_array->GetEntriesFast()<<" entries"<<std::endl;
    }
    else std::cout<<"*** hit_array broken - check your file! ."<<std::endl;

    // ----- MC Points -----
    for (Int_t i=0; i<mc_array->GetEntriesFast(); i++)
    {
      if(verbose) cout<<"Point No "<<i<<endl;
      PndMvdMCPoint *point=(PndMvdMCPoint*)mc_array->At(i);
      vecmc=0.5*(point->GetPosition()+point->GetPositionOut());
      hisxymc->Fill(vecmc.x(),vecmc.y());
      hisrzmc->Fill(vecmc.z(),((vecmc.y()>0.)?1.:-1.)*vecmc.Perp());
      hisde->Fill(point->GetEnergyLoss());
      vecmcloc = fGeoH->MasterToLocalId(vecmc, point->GetDetName());
      hisLocalXYMC->Fill(vecmcloc.x(),vecmcloc.y());
      hisLocalZMC->Fill(vecmcloc.z());
    }

    // -----  HITS  -----
    for (Int_t i=0; i<hit_array->GetEntriesFast(); i++)
    {
      //hit info
      if(verbose) cout<<"Hit No "<<i;//<<endl;
      if(verbose) cout<< "  |  ";
      PndMvdHit *hit=(PndMvdHit*)hit_array->At(i);
      if( detFilter!="" && 
          fGeoH->GetPath(hit->GetDetName()).Contains(detFilter))
         continue;
//       if(fGeoH->GetPath(hit->GetDetName()).Contains("Rect")) continue;
      vecs.SetXYZ(hit->GetX(), hit->GetY(), hit->GetZ());
      hisxy->Fill(vecs.x(),vecs.y());
      hisrz->Fill(vecs.z(),((vecs.y()>0.)?1.:-1.)*vecs.Perp());
      hisrecoeloss->Fill(hit->GetEloss());
      vecsloc = fGeoH->MasterToLocalId(vecs, hit->GetDetName());
      hisLocalXY->Fill(vecsloc.x(),vecsloc.y());

      //cluster info
      int topclustid = hit->GetRefIndex();
      int botclustid = hit->GetBotIndex();
      if(verbose) cout<<"top/bot cluster index "<< topclustid<<"/"<<botclustid<<" ";
      if(verbose) cout<< "  |  ";
      PndMvdCluster *clust = (PndMvdCluster*)clust_array->At(topclustid);
      if(verbose) cout<< topclustid<<"  "<<clust<<"  ";
      Int_t clsize = clust->GetClusterSize();
      if(verbose) cout<<clsize <<" ";
      int hclsize = hit->GetNDigiHits();
      if (hclsize < clsize) cout<<"Strange cluster sizes - this shall not happen!"<<endl;
      int bclsize =-1;
      if(botclustid>=0){
        if(verbose) cout<< " | "<<botclustid <<"   ";
        PndMvdCluster *botcl = (PndMvdCluster*)clust_array->At(botclustid);
        bclsize = botcl->GetClusterSize();
        if(verbose) cout<< botcl <<"  "<<bclsize<<"  |  ";
      }
      hisClustSize->Fill(clsize);
      if(bclsize>0) hisClustSize->Fill(bclsize);
      hisClustSizeTop->Fill(clsize);
      if(bclsize>0) hisClustSizeBot->Fill(bclsize);

      //digis info (getting point ID)
      Int_t sid = clust->GetDigiIndex(0);
      if(verbose) cout<< "sid="<<sid<<"  |  ";
      if(sid>digiStrip_array->GetEntriesFast()) cout<<"Exceeding digi array size"<<endl;
      PndMvdDigiStrip *astripdigi = (PndMvdDigiStrip*)digiStrip_array->At(sid);
      if(0==astripdigi)cout<<"no strip digi found"<<endl;

      //point info
      int mcid = astripdigi->GetIndex();
      if(verbose) cout<< "#4#  |  ";
      PndMvdMCPoint *point=(PndMvdMCPoint*)mc_array->At(mcid);
      if(point->GetDetName()!=hit->GetDetName()) 
      {
        cout<<"-error- Point and hit detector names don't match!"<<endl;
        cout<<"point (id)  : "<<point->GetDetName()<<endl;
        cout<<"hit   (id)  : "<<  hit->GetDetName()<<endl;
        cout<<"point (name): "<<fGeoH->GetPath(point->GetDetName()).Data()<<endl;
        cout<<"hit   (name): "<<fGeoH->GetPath(  hit->GetDetName()).Data()<<endl;
        continue;
      }
      vecmc=0.5*(point->GetPosition()+point->GetPositionOut());
      vecmcloc = fGeoH->MasterToLocalId(vecmc, point->GetDetName());
      vecdiff = vecmcloc - vecsloc;
//       vecdiff = vecmc - vecs;
      if(vecdiff.X() > 0.1)
      {  //something is wrong?
        std::cout<<"### warning ### large difference to MC info "<<endl;
        if(verbose) std::cout<< fGeoH->GetPath(hit->GetDetName())<<endl;
      }
      difftheta = vecmc.Theta() - vecs.Theta();
      diffphi = vecmc.Phi() - vecs.Phi();
      //convert deg to mrad
      difftheta = difftheta*1000.*TMath::Pi()/180.;
      diffphi= diffphi*1000.*TMath::Pi()/180.;

      hisxy->Fill(vecs.x(),vecs.y());
      if(vecs.y() > 0.) hisrz->Fill(vecs.z(),vecs.Perp());
      else hisrz->Fill(vecs.z(),-1.*vecs.Perp());

      hisDiffTheta->Fill(difftheta);
      hisDiffPhi->Fill(diffphi);

      hisDiffXY->Fill(vecdiff.x(),vecdiff.y());
      hisDiffRZ->Fill(vecdiff.z(),vecdiff.Perp());
      hisDiffX->Fill(vecdiff.x());
      hisDiffY->Fill(vecdiff.y());
      hisDiffZ->Fill(vecdiff.z());


//       if(clsize==1){
//         hisDiffOneX->Fill(vecdiff.x());
//         hisDiffOneY->Fill(vecdiff.y());
//         hisDiffOneZ->Fill(vecdiff.z());
//         hisDiffThetaOne->Fill(difftheta);
//         hisDiffPhiOne->Fill(diffphi);
//       }else{
//         hisDiffMoreX->Fill(vecdiff.x());
//         hisDiffMoreY->Fill(vecdiff.y());
//         hisDiffMoreZ->Fill(vecdiff.z());
//         hisDiffThetaMore->Fill(difftheta);
//         hisDiffPhiMore->Fill(diffphi);
//       }
//       if(detname.Contains("Disk")){
//         hisDiffDiskX->Fill(vecdiff.x());
//         hisDiffDiskY->Fill(vecdiff.y());
//         hisDiffDiskZ->Fill(vecdiff.z());
//         hisDiffThetaDisk->Fill(difftheta);
//         hisDiffPhiDisk->Fill(diffphi);
//         hisClustSizeDisk->Fill(clsize);
//         if(bclsize>0) hisClustSizeDisk->Fill(bclsize);
//       }else
//       if(detname.Contains("Side")){
//         hisDiffSideX->Fill(vecdiff.x());
//         hisDiffSideY->Fill(vecdiff.y());
//         hisDiffSideZ->Fill(vecdiff.z());
//         hisDiffThetaSide->Fill(difftheta);
//         hisDiffPhiSide->Fill(diffphi);
//         hisClustSizeSide->Fill(clsize);
//         if(bclsize>0) hisClustSizeSide->Fill(bclsize);
//       }
      if(verbose) cout<<endl;
    }//end for i (hits in event)

  }// end for j (events)


//-----
Int_t pix = 400;
Int_t a = 2, b = 2;

TCanvas* can1 = new TCanvas("MvdTestPlot","MCHit view in MVD",0,0,a*pix,b*pix);
can1->Divide(a,b);

can1->cd(1);
mypad=gPad;
mypad.Divide(2,2);
mypad->cd(1);DrawNice2DHisto(hisxymc);
mypad->cd(2);DrawNice2DHisto(hisrzmc);
mypad->cd(3);DrawNice2DHisto(hisLocalXYMC);
// mypad->cd(4);gPad->SetLogy();hisLocalZMC->DrawCopy();
mypad->cd(4);gPad->SetLogy();hisde->DrawCopy();

can1->cd(2);
mypad=gPad;
mypad.Divide(2,2);
mypad->cd(1);DrawNice2DHisto(hisxy);
mypad->cd(2);DrawNice2DHisto(hisrz);
mypad->cd(3);DrawNice2DHisto(hisLocalXY);
mypad->cd(4);gPad->SetLogy();hisrecoeloss->DrawCopy();

can1->cd(3);
mypad=gPad;
mypad.Divide(2,2);
mypad->cd(1);DrawNice2DHisto(hisDiffXY);
mypad->cd(2);DrawNice2DHisto(hisDiffRZ);
mypad->cd(3);hisDiffTheta->DrawCopy();
mypad->cd(4);hisDiffPhi->DrawCopy();

can1->cd(4);
hisClustSize->Draw();
hisClustSizeBot->SetLineColor(kRed);
hisClustSizeTop->SetLineColor(kBlue);
hisClustSizeBot->Draw("sames");
hisClustSizeTop->Draw("sames");
can1->Update();mypad=(TPad*)gPad;BetterStatBox(mypad);
/*
can1->cd(5);
hisDiffDiskX->SetLineColor(kRed);
hisDiffSideX->SetLineColor(kBlue);
hisDiffX->GetXaxis()->SetNdivisions(-05);
hisDiffX->DrawCopy("");
hisDiffDiskX->DrawCopy("sames");
hisDiffSideX->DrawCopy("sames");
can1->Update();mypad=(TPad*)gPad;BetterStatBox(mypad);

can1->cd(6);
hisDiffDiskY->SetLineColor(kRed);
hisDiffSideY->SetLineColor(kBlue);
hisDiffY->GetXaxis()->SetNdivisions(-05);
hisDiffY->DrawCopy("");
hisDiffDiskY->DrawCopy("sames");
hisDiffSideY->DrawCopy("sames");
can1->Update();mypad=(TPad*)gPad;BetterStatBox(mypad);

can1->cd(7);
hisDiffSideZ->SetLineColor(kBlue);
hisDiffDiskZ->SetLineColor(kRed);
hisDiffZ->GetXaxis()->SetNdivisions(-05);
hisDiffZ->DrawCopy("");
hisDiffDiskZ->DrawCopy("sames");
hisDiffSideZ->DrawCopy("sames");
can1->Update();mypad=(TPad*)gPad;BetterStatBox(mypad);

can1->cd(8);
hisDiffThetaDisk->SetLineColor(kRed);
hisDiffThetaSide->SetLineColor(kBlue);
hisDiffTheta->GetXaxis()->SetNdivisions(-05);
hisDiffTheta->DrawCopy("");
hisDiffThetaDisk->DrawCopy("sames");
hisDiffThetaSide->DrawCopy("sames");
can1->Update();mypad=(TPad*)gPad;BetterStatBox(mypad);



can1->cd(9);
hisDiffLocalDiskX->SetLineColor(kRed);
hisDiffLocalSideX->SetLineColor(kBlue);
hisDiffLocalX->GetXaxis()->SetNdivisions(-05);
hisDiffLocalX->DrawCopy("");
hisDiffLocalDiskX->DrawCopy("sames");
hisDiffLocalSideX->DrawCopy("sames");
can1->Update();mypad=(TPad*)gPad;BetterStatBox(mypad);

can1->cd(10);
hisDiffLocalDiskY->SetLineColor(kRed);
hisDiffLocalSideY->SetLineColor(kBlue);
hisDiffLocalY->GetXaxis()->SetNdivisions(-05);
hisDiffLocalY->DrawCopy("");
hisDiffLocalDiskY->DrawCopy("sames");
hisDiffLocalSideY->DrawCopy("sames");
can1->Update();mypad=(TPad*)gPad;BetterStatBox(mypad);

can1->cd(11);
hisDiffLocalDiskZ->SetLineColor(kRed);
hisDiffLocalSideZ->SetLineColor(kBlue);
hisDiffLocalZ->GetXaxis()->SetNdivisions(-05);
hisDiffLocalZ->DrawCopy("");
hisDiffLocalDiskZ->DrawCopy("sames");
hisDiffLocalSideZ->DrawCopy("sames");
can1->Update();mypad=(TPad*)gPad;BetterStatBox(mypad);

can1->cd(12);
hisDiffPhiDisk->SetLineColor(kRed);
hisDiffPhiSide->SetLineColor(kBlue);
hisDiffPhi->GetXaxis()->SetNdivisions(-05);
hisDiffPhi->DrawCopy("");
hisDiffPhiDisk->DrawCopy("sames");
hisDiffPhiSide->DrawCopy("sames");
can1->Update();mypad=(TPad*)gPad;BetterStatBox(mypad);
*/


/*
a = 3, b = 3;
TCanvas* can2 = new TCanvas("MvdCluPlot","MVD clust",50,50,a*pix,b*pix);
can2->Divide(a,b);

can2->cd(1);
hisDiffOneX->SetLineColor(kRed);
hisDiffMoreX->SetLineColor(kBlue);
hisDiffX->GetXaxis()->SetNdivisions(-05);
hisDiffX->DrawCopy("");
hisDiffOneX->DrawCopy("sames");
hisDiffMoreX->DrawCopy("sames");
can2->Update();mypad=(TPad*)gPad;BetterStatBox(mypad);

can2->cd(2);
hisDiffOneY->SetLineColor(kRed);
hisDiffMoreY->SetLineColor(kBlue);
hisDiffY->GetXaxis()->SetNdivisions(-05);
hisDiffY->DrawCopy("");
hisDiffOneY->DrawCopy("sames");
hisDiffMoreY->DrawCopy("sames");
can2->Update();mypad=(TPad*)gPad;BetterStatBox(mypad);

can2->cd(3);
hisDiffMoreZ->SetLineColor(kBlue);
hisDiffOneZ->SetLineColor(kRed);
hisDiffZ->GetXaxis()->SetNdivisions(-05);
hisDiffZ->DrawCopy("");
hisDiffOneZ->DrawCopy("sames");
hisDiffMoreZ->DrawCopy("sames");
can2->Update();mypad=(TPad*)gPad;BetterStatBox(mypad);

can2->cd(4);
hisDiffLocalOneX->SetLineColor(kRed);
hisDiffLocalMoreX->SetLineColor(kBlue);
hisDiffLocalX->GetXaxis()->SetNdivisions(-05);
hisDiffLocalX->DrawCopy("");
hisDiffLocalOneX->DrawCopy("sames");
hisDiffLocalMoreX->DrawCopy("sames");
can2->Update();mypad=(TPad*)gPad;BetterStatBox(mypad);

can2->cd(5);
hisDiffLocalOneY->SetLineColor(kRed);
hisDiffLocalMoreY->SetLineColor(kBlue);
hisDiffLocalY->GetXaxis()->SetNdivisions(-05);
hisDiffLocalY->DrawCopy("");
hisDiffLocalOneY->DrawCopy("sames");
hisDiffLocalMoreY->DrawCopy("sames");
can2->Update();mypad=(TPad*)gPad;BetterStatBox(mypad);

can2->cd(6);
hisDiffLocalOneZ->SetLineColor(kRed);
hisDiffLocalMoreZ->SetLineColor(kBlue);
hisDiffLocalZ->GetXaxis()->SetNdivisions(-05);
hisDiffLocalZ->DrawCopy("");
hisDiffLocalOneZ->DrawCopy("sames");
hisDiffLocalMoreZ->DrawCopy("sames");
can2->Update();mypad=(TPad*)gPad;BetterStatBox(mypad);

can2->cd(7);
hisDiffThetaOne->SetLineColor(kRed);
hisDiffThetaMore->SetLineColor(kBlue);
hisDiffTheta->GetXaxis()->SetNdivisions(-05);
hisDiffTheta->DrawCopy("");
hisDiffThetaOne->DrawCopy("sames");
hisDiffThetaMore->DrawCopy("sames");
can2->Update();mypad=(TPad*)gPad;BetterStatBox(mypad);

can2->cd(8);
hisDiffPhiOne->SetLineColor(kRed);
hisDiffPhiMore->SetLineColor(kBlue);
hisDiffPhi->GetXaxis()->SetNdivisions(-05);
hisDiffPhi->DrawCopy("");
hisDiffPhiOne->DrawCopy("sames");
hisDiffPhiMore->DrawCopy("sames");
can2->Update();mypad=(TPad*)gPad;BetterStatBox(mypad);

can1->Print(picture+"(");
can2->Print(picture+")");
*/

/*
// NICE PLOT FOR TALK

a = 2, b = 1;
int off = -3;
gStyle->SetOptStat(0);
TLegend* legende = new TLegend(0.75, 0.75, 0.98, 0.98);

TCanvas* can3 = new TCanvas("MvdTalkPlot1","MVD clust",100,100,a*400,b*400);
can3->Divide(a,b);

can3->cd(1);
hisDiffDiskX->SetLineColor(kOrange + off);
hisDiffSideX->SetLineColor(kAzure + off);
hisDiffX->GetXaxis()->SetNdivisions(-05);
hisDiffX->DrawCopy("");
hisDiffDiskX->DrawCopy("same");
hisDiffSideX->DrawCopy("same");

legende->AddEntry(hisDiffX," All","L");
legende->AddEntry(hisDiffDiskX," Disks","L");
legende->AddEntry(hisDiffSideX," Barrel","L");
legende->Draw();

can3->cd(2);
hisDiffDiskY->SetLineColor(kOrange + off);
hisDiffSideY->SetLineColor(kAzure + off);
hisDiffY->GetXaxis()->SetNdivisions(-05);
hisDiffY->DrawCopy("");
hisDiffDiskY->DrawCopy("same");
hisDiffSideY->DrawCopy("same");
legende->Draw();

can3->Print("res-clust-xy.png");

TCanvas* can4 = new TCanvas("MvdTalkPlot2","MVD clust",150,150,a*400,b*400);
can4->Divide(a,b);
can4->cd(1);
hisDiffLocalDiskX->SetLineColor(kOrange + off);
hisDiffLocalSideX->SetLineColor(kAzure + off);
hisDiffLocalX->GetXaxis()->SetNdivisions(-05);
hisDiffLocalX->DrawCopy("");
hisDiffLocalDiskX->DrawCopy("same");
hisDiffLocalSideX->DrawCopy("same");
legende->Draw();

can4->cd(2);
hisDiffLocalDiskY->SetLineColor(kOrange + off);
hisDiffLocalSideY->SetLineColor(kAzure + off);
hisDiffLocalY->GetXaxis()->SetNdivisions(-05);
hisDiffLocalY->DrawCopy("");
hisDiffLocalDiskY->DrawCopy("same");
hisDiffLocalSideY->DrawCopy("same");
legende->Draw();

can4->Print("res-clust-xy-local.png");

TCanvas* can5 = new TCanvas("MvdTalkPlot3","MVD clust",200,200,a*400,b*400);
can5->Divide(a,b);
can5->cd(1);
hisDiffThetaDisk->SetLineColor(kOrange + off);
hisDiffThetaSide->SetLineColor(kAzure + off);
hisDiffTheta->GetXaxis()->SetNdivisions(-05);
hisDiffTheta->DrawCopy("");
hisDiffThetaDisk->DrawCopy("same");
hisDiffThetaSide->DrawCopy("same");
legende->Draw();

can5->cd(2);
hisDiffPhiDisk->SetLineColor(kOrange + off);
hisDiffPhiSide->SetLineColor(kAzure + off);
hisDiffPhi->GetXaxis()->SetNdivisions(-05);
hisDiffPhi->DrawCopy("");
hisDiffPhiDisk->DrawCopy("same");
hisDiffPhiSide->DrawCopy("same");
legende->Draw();

can5->Print("res-clust-thetaphi.png");

TCanvas* can6 = new TCanvas("MvdTalkPlot4","MVD clust",250,250,400,400);
can6->cd();
hisClustSizeDisk->SetLineColor(kOrange + off);
hisClustSizeSide->SetLineColor(kAzure + off);
hisClustSize->DrawCopy("");
hisClustSizeDisk->DrawCopy("same");
hisClustSizeSide->DrawCopy("same");
legende->Draw();

can6->Print("res-clust-clsize.png");
*/
  // -----   Finish   -------------------------------------------------------
/*  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;*/
  // ------------------------------------------------------------------------

}
