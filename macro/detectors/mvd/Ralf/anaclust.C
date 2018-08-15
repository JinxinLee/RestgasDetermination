// root macro to analyze the clusterization output
#include <map>
#include <vector>
int anaclust()
{
  int  nEvents = 100;
  bool verbose = false;

  // -----  Load libraries   ------------------------------------------------
  gSystem->Load("../Helper_C.so");
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  gROOT->LoadMacro("../Tools.C");
  TString detFilter = "";// "","Trap","Rect"
  LoadPandaStyle();

  // -----   Timer   --------------------------------------------------------
//   TStopwatch timer;
//   timer.Start();
  // ------------------------------------------------------------------------


  TString name = "../data/mvddpm6GeV.root";
//   TString name = "../data/mvdStrip.root";
  PndFileNameCreator namecreator(name.Data());
  std::string inFile = namecreator.GetSimFileName(false);
  std::string digiFile = namecreator.GetDigiFileName(false);
  std::string recoFile = namecreator.GetRecoFileName(false);
  TString picture = namecreator.GetRecoFileName(false);
  picture.ReplaceAll(".root",".ps");

  TFile* f = new TFile(inFile.c_str()); // the sim file you want to analyse
  TTree* t=(TTree*)f->Get("pndsim");
  t->AddFriend("pndsim",digiFile.c_str()); // the digi file
  t->AddFriend("pndsim",recoFile.c_str()); // the reco file you want to analyse

  TClonesArray* mc_array=new TClonesArray("PndSdsMCPoint");
  t->SetBranchAddress("MVDPoint",&mc_array);//Branch names

  TClonesArray* digiPixel_array=new TClonesArray("PndSdsDigiPixel");
  t->SetBranchAddress("MVDPixelDigis",&digiPixel_array);//Branch names

  TClonesArray* digiStrip_array=new TClonesArray("PndSdsDigiStrip");
  t->SetBranchAddress("MVDStripDigis",&digiStrip_array);//Branch names

  TClonesArray* stripClust_array=new TClonesArray("PndSdsClusterStrip");
  t->SetBranchAddress("MVDStripClusterCand",&stripClust_array);//Branch names

  TClonesArray* pixelClust_array=new TClonesArray("PndSdsClusterPixel");
  t->SetBranchAddress("MVDPixelClusterCand",&pixelClust_array);//Branch names

  TClonesArray* stripHit_array=new TClonesArray("PndSdsHit");
  t->SetBranchAddress("MVDHitsStrip",&stripHit_array);//Branch names

  TClonesArray* pixelHit_array=new TClonesArray("PndSdsHit");
  t->SetBranchAddress("MVDHitsPixel",&pixelHit_array);//Branch names


  TGeoManager *geoMan = (TGeoManager*) gDirectory->Get("FAIRGeom");
  PndGeoHandling* fGeoH = new PndGeoHandling();

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

  TH2D* hisnomatchxy = new TH2D("nomatchxy","MVD reco Hit nomatch, xy view;x / cm;y / cm",nbins,-rim,rim,nbins,-rim,rim);
  TH2D* hisnomatchrz = new TH2D("nomatchrz","MVD reco Hit nomatch, rz view;z / cm;r/ cm",nbins,-rim,rim,nbins,-rim,rim);
  TH1D* hisnomatcheloss = new TH1D("denom","MVD blind Hit energy deposit;E / GeV;",nbins,0.,0.002);
  TH2D* hisLocalXYnom = new TH2D("LocalxyMCnom","Local nomatch MC Point XY;x_{L} / cm;y_{L} / cm",nbins,-5.,5.,nbins,-5.,5.);


  TH2D* hisDiffXY = new TH2D("diffxy","",nbins,-0.01,0.01,nbins,-0.04,0.04);
  hisDiffXY->SetTitle("(MC - RECO) Hit coordinates xy view;#Deltax / cm;#Deltay / cm");
  TH2D* hisDiffRZ = new TH2D("diffrz","",5*nbins,-0.025,0.025,nbins,-0.00,0.04);
  hisDiffRZ->SetTitle("(MC - RECO) Hit coordinates rz view;#Deltaz / cm;#Deltar / cm");
  TH2D* hisCZ = new TH2D("Cz","",nbins,-0.025,0.025,nbins,0.,1000000);
  hisCZ->SetTitle("Charge - zdeviation ;#Deltaz / cm;#Q / e-");

  Double_t range = 0.01;
  Double_t zed = 0.01;
  Int_t bins = 100;

  TH1D* hisDiffX = new TH1D("diffx","(MC - RECO) Hit coordinate x;#Deltax / cm;",bins,-range,range);
  TH1D* hisDiffY = new TH1D("diffy","(MC - RECO) Hit coordinate y;#Deltay / cm;",bins,-range,range);
  TH1D* hisDiffZ = new TH1D("diffz","(MC - RECO) Hit coordinate z;#Deltaz / cm;",bins,-range,range);
  TH1D* hisDiffM = new TH1D("diffm","(MC - RECO) residual;#R / cm;",bins,0.,5.);

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

  TH1I* hisDigiCounts = new TH1I("DigiCounts","Digis used n times in clusters;n;",10,0,10);
  TH1I* hisPointEff = new TH1I("pointeff","Reconstructed Hits per MC Point;n;",10,0,10);

  TVector3 vecsloc, vecmcloc, vecs, vecmc, vecdiff, mommc, sensorDim;
  TString detname;
  Double_t difftheta, diffphi;

  for (Int_t j=0; j<nEvents && j<t->GetEntriesFast(); j++)
  {
    t->GetEntry(j);
    if(verbose) cout<<"Event No "<<j<<endl;
    else if (!(j%20)) cout <<"Event No "<<j<<endl;
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
    if(stripClust_array){
      if(verbose) std::cout<<"stripClust_array ok with "<<stripClust_array->GetEntriesFast()<<" entries"<<std::endl;
    }
    else std::cout<<"*** stripClust_array broken - check your file! ."<<std::endl;
    if(stripHit_array){
      if(verbose) std::cout<<"stripHit_array ok with "<<stripHit_array->GetEntriesFast()<<" entries"<<std::endl;
    }
    else std::cout<<"*** stripHit_array broken - check your file! ."<<std::endl;

//     // -----  Strip Clusters  -----
//     // efficiency stuff
//     std::map<int,std::vector<int> > digirec;
//     for (int k=0;k<stripClust_array->GetEntriesFast();k++)
//     {
//       PndSdsClusterStrip* aclust = (PndSdsClusterStrip*)stripClust_array->At(k);
//       for(int m=0;m<aclust->GetClusterSize();m++)
//       {
//         digirec[aclust->GetDigiIndex(m)].push_back(k);
//       }
//     }
// cout<<digiStrip_array->GetEntriesFast()<<endl;
//     for (int l=0;l<digiStrip_array->GetEntriesFast();l++)
//     {
//       hisDigiCounts->Fill(digirec[l].size());
//     }

    std::map<int,std::vector<int> > matchmap;

    // -----  Strip HITS  -----
    for (Int_t i=0; i<stripHit_array->GetEntriesFast(); i++)
    {
      //hit info
      if(verbose) cout<<"Hit No "<<i;//<<endl;
      if(verbose) cout<< "  |  ";
      PndSdsHit *hit=(PndSdsHit*)stripHit_array->At(i);
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
      PndSdsClusterStrip *clust = (PndSdsClusterStrip*)stripClust_array->At(topclustid);
      if(verbose) cout<< topclustid<<"  "<<clust<<"  ";
      Int_t clsize = clust->GetClusterSize();
      if(verbose) cout<<clsize <<" ";
      int hclsize = hit->GetNDigiHits();
      if (hclsize < clsize) cout<<"Strange cluster sizes - this shall not happen!"<<endl;
      int bclsize =-1;
      if(botclustid>=0){
        if(verbose) cout<< " | "<<botclustid <<"   ";
        PndSdsClusterStrip *botcl = (PndSdsClusterStrip*)stripClust_array->At(botclustid);
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
      PndSdsDigiStrip *astripdigi = (PndSdsDigiStrip*)digiStrip_array->At(sid);
      if(0==astripdigi)cout<<"no strip digi found"<<endl;

      //point info
      int mcid = astripdigi->GetIndex();
      if(verbose) cout<< "#4#  |  ";
      if(mcid<0) continue;
      PndSdsMCPoint *point=(PndSdsMCPoint*)mc_array->At(mcid);
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
/*      if(fabs(vecdiff.X()) > 0.1)
      {  //something is wrong?
        std::cout<<"### warning ### large difference to MC info "<<endl;
        if(verbose) std::cout<< fGeoH->GetPath(hit->GetDetName())<<endl;
      }*/

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
      hisDiffM->Fill(vecdiff.Mag());

      if(verbose) cout<<endl;

      //efficiency stuff
      matchmap[mcid].push_back(i);
    }//end for i (strip hits in event)

    // ----- MC Points -----
    for (Int_t i=0; i<mc_array->GetEntriesFast(); i++)
    {
      if(verbose) cout<<"Point No "<<i<<endl;
      PndSdsMCPoint *point=(PndSdsMCPoint*)mc_array->At(i);
      vecmc=0.5*(point->GetPosition()+point->GetPositionOut());
      hisxymc->Fill(vecmc.x(),vecmc.y());
      hisrzmc->Fill(vecmc.z(),((vecmc.y()>0.)?1.:-1.)*vecmc.Perp());
      hisde->Fill(point->GetEnergyLoss());
      vecmcloc = fGeoH->MasterToLocalId(vecmc, point->GetDetName());
      hisLocalXYMC->Fill(vecmcloc.x(),vecmcloc.y());
      hisLocalZMC->Fill(vecmcloc.z());
      if(fGeoH->GetPath(point->GetDetName()).Contains("Strip"))
      {
        hisPointEff->Fill(matchmap[i].size());
        if(matchmap[i].size()==0)
        {
          hisnomatchxy->Fill(vecmc.x(),vecmc.y());
          hisnomatchrz->Fill(vecmc.z(),((vecmc.y()>0.)?1.:-1.)*vecmc.Perp());
          hisnomatcheloss->Fill(point->GetEnergyLoss());
          hisLocalXYnom->Fill(vecmcloc.x(),vecmcloc.y());
        }//check where the things are lost...
      }
    }


  }// end for j (events)


//-----
Int_t pix = 400;
Int_t a = 3, b = 2;

TCanvas* can1 = new TCanvas("MvdTestPlot","MCHit view in MVD",0,0,a*pix,b*pix);
can1->Divide(a,b);
TPad* mypad=0;
can1->cd(1);
mypad=gPad;
mypad->Divide(2,2);
mypad->cd(1);DrawNice2DHisto(hisxymc);
mypad->cd(2);DrawNice2DHisto(hisrzmc);
mypad->cd(3);DrawNice2DHisto(hisLocalXYMC);
// mypad->cd(4);gPad->SetLogy();hisLocalZMC->DrawCopy();
mypad->cd(4);gPad->SetLogy();hisde->DrawCopy();

can1->cd(2);
mypad=gPad;
mypad->Divide(2,2);
mypad->cd(1);DrawNice2DHisto(hisxy);
mypad->cd(2);DrawNice2DHisto(hisrz);
mypad->cd(3);DrawNice2DHisto(hisLocalXY);
mypad->cd(4);gPad->SetLogy();hisrecoeloss->DrawCopy();

can1->cd(3);
mypad=gPad;
mypad->Divide(2,2);
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

can1->cd(5);
mypad=gPad;
mypad->Divide(2,2);
mypad->cd(1);gPad->SetLogy();
hisDiffM->DrawCopy();
mypad->cd(2);
hisPointEff->SetFillColor(kBlue+5);
hisPointEff->DrawCopy();
mypad->cd(3);
mypad->cd(4);

can1->cd(6);
mypad=gPad;
mypad->Divide(2,2);
mypad->cd(1);DrawNice2DHisto(hisnomatchxy);
mypad->cd(2);DrawNice2DHisto(hisnomatchrz);
mypad->cd(3);DrawNice2DHisto(hisLocalXYnom);
mypad->cd(4);gPad->SetLogy();
hisnomatcheloss->DrawCopy();

can1->Print("/home/ralfk/Desktop/tempview.ps");


// Plots for talk
TCanvas* can5 = new TCanvas("MvdTalkPlot5","MVD clust",200,200,800,400);
can5->Divide(2,1);
can5->cd(1);
hisDiffTheta->DrawCopy();
can5->cd(2);
hisDiffPhi->DrawCopy();
can5->Print("res-clust-thetaphi.png");

TCanvas* can6 = new TCanvas("MvdTalkPlot6","MVD clust",250,250,400,400);
can6->cd();int off = -3;
hisClustSizeTop->SetLineColor(kOrange + off);
hisClustSizeBot->SetLineColor(kAzure + off);
hisClustSize->DrawCopy("");
hisClustSizeTop->DrawCopy("same");
hisClustSizeBot->DrawCopy("same");

can6->Print("res-clust-clsize.png");

TCanvas* can7 = new TCanvas("MvdTalkPlot7","MVD clust",200,200,800,400);
can7->Divide(2,1);
can7->cd(1);
hisPointEff->DrawCopy();
can7->cd(2);
Double_t zeros = hisPointEff->GetBinContent(hisPointEff->GetBin(1));
Double_t ones = hisPointEff->GetBinContent(hisPointEff->GetBin(2));
Double_t all = hisPointEff->Integral();
Double_t multiples = all - zeros - ones;
cout <<zeros<<" zeros, "<<ones<<" ones, "<<multiples<<" multiples"<<endl;
cout <<zeros/all<<"% "<<ones/all<<"% "<<multiples/all<<"%"<<endl;
can7->Print("res-clust-efiiciencies.png");




  // -----   Finish   -------------------------------------------------------
/*  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;*/
  // ------------------------------------------------------------------------

  return 0;
}
