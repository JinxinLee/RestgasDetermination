// root macro to analyze the simulation output from gem_material_sim.C to draw Radiation Length for GEM materials
 #include <map>
 #include <string>
 #include <iostream>
 #include <vector>

 #include <TStopwatch.h>
 #include <TFile.h>
 #include <TTree.h>
 #include <TGeoManager.h>
 #include <TClonesArray.h>
 #include <TH1D.h>
 #include <TROOT.h> // for global root variables
 #include <TH2D.h>
 #include <TVector3.h>
 #include <TPad.h>
 #include <TCanvas.h>
 #include <TStopwatch.h>
 #include <THStack.h>
 #include <TProfile.h>
 #include <TProfile2D.h>
 #include <TColor.h>

//#include <TROOT.h>
//#include <TSystem.h>

// //how to do this better?
//#include "/data.local1/Nazila/FairRoot/v1503/install/include/FairRadLenPoint.h"
//#include "../../pnddata/PndMCTrack.h"
//#include "../run/Tools.C"
//------------------------------------------------------------------------------

int gem_material_ana1(int nEvents = 1000000, bool verbose = false)
{
// int nEvents = 100;
//  verbose  = true;
  bool draw1    = true;
  bool draw2    = true;
  bool draw3    = true;
  bool draw4    = true;
 
// -----  Load libraries   ------------------------------------------------
//   gROOT->Macro("../Libs.C");
  gROOT->LoadMacro("../run/Tools.C");
  //LoadPandaStyle();

  TString sysFile = gSystem->Getenv("VMCWORKDIR");

 
  TString MCFile  =  "gem_mat_sim_geantino.root";
  //TString MCFile  =  "gem_mat_sim_withoutpipe.root";
  //TString MCFile  =  "gem_mat_sim_GemPipe.root";
  //TString MCFile  =  "gem_mat_sim_Gem.root";
  
  TString parFile =  "gem_mat_simparams_geantino.root";
  //TString parFile =  "gem_mat_simparams_withoutpipe.root";
  //TString parFile =  "gem_mat_simparams_GemPipe.root";
  //TString parFile =  "gem_mat_simparams_Gem.root";

  //  std::cout << "Output File: " << outFile.Data()<< std::endl;

  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------

  std::string inFile = "../../geometry/pndcave.geo";
  std::string inFile = "../../geometry/pipebeamtarget.geo";
  std::string inFile = "../../geometry/gem_3Stations_realistic_v1.root";
  TString allDigiFile = sysFile+"/macro/params/gem_3Stations_realistic_v1.root";

  TFile* f = new TFile(MCFile.Data()); // the sim file you want to analyse
  TTree* t =(TTree *) f->Get("cbmsim") ;
  TFile* par = TFile::Open(parFile.Data());

  cout << "file name = " << f->GetName() << ", tree = " << t->GetName() << endl;

  TClonesArray* mc_array=new TClonesArray("PndMCTrack");
  t->SetBranchAddress("MCTrack",&mc_array );//Branch names

  TClonesArray* rad_array=new TClonesArray("FairRadLenPoint");
  t->SetBranchAddress("RadLen",&rad_array);

  //  TGeoManager *geoMan = (TGeoManager*) gDirectory->Get("FAIRGeom");
  TGeoManager *geoMan = (TGeoManager*) gDirectory->Get("FairGeoParSet");
  //PndGeoHandling* fGeoH = new PndGeoHandling();

//---------------------------------------------------------------------------------
  // histos
  int res = 1000; int angres = 180;
  TH1D* hRadLenDistMat = new TH1D("radldm","Material thicknesses",100,0,15);
  TH1D* hRadLenDistEff = new TH1D("radlde","Effective Radiation length values per volume",100,0,0.2);
  TH2D* hisxy = new TH2D("hisxy","MC Points, xy view",res,-85.,85.,res,-85.,85.);
  TH2D* hisrz = new TH2D("hisrz","MC Points, rz view",res,-90.,210.,res,-85.,85.);

  TClonesArray* arrhisxy = new TClonesArray("TH2D");
  TClonesArray* arrhisrz = new TClonesArray("TH2D");
  TClonesArray* arrprothe = new TClonesArray("TProfile");
  TClonesArray* arrprophi = new TClonesArray("TProfile");
  TClonesArray* arrprothephi = new TClonesArray("TProfile2D");

  std::vector<std::string> namesList;

  //namesList.push_back("pipe");
  //namesList.push_back("pipeTS");
  //namesList.push_back("cave");//Reste, ausser "/cave"
  //namesList.push_back("Tcross");
  //namesList.push_back("Sensor");
  //namesList.push_back("Disk");
  //namesList.push_back("Gemriddle");
  namesList.push_back("Seg");
  //namesList.push_back("GEM");
  //namesList.push_back("Volume");
  //namesList.push_back("Ring");
  //namesList.push_back("riddle");
  //namesList.push_back("DISK");
  //namesList.push_back("GemHLayers");
  //namesList.push_back("copperbar");

  double radlList[100];

  //std::map<int,double> radlList;
  //radlList.insert(std::pair<int,double>(0,7.));
  int maxnames=namesList.size();
  cout << "before maxnames loop" << endl;

  for(int i=0;i<maxnames;i++){
     TString name  = "hisxy-";
     TString name2 = "hisrz-";
     TString name3 = "hthe-";
     TString name4 = "hphi-";
     TString name5 = "hthephi-";

     name += i;   name2 += i;   name3 += i;   name4 += i;    name5 += i;

     new ((*arrhisxy)[i]) TH2D(name.Data(),(namesList[i]).c_str(),2*res,-85.,85.,2*res,-85.,85.);
     new ((*arrhisrz)[i]) TH2D(name2.Data(),(namesList[i]).c_str(),2*res,-100.,225.,2*res,-85.,85.);
     new ((*arrprothe)[i]) TProfile(name3.Data(),(namesList[i]).c_str(),angres,0.,180.);//angres,0.,TMath::Pi());
     new ((*arrprophi)[i]) TProfile(name4.Data(),(namesList[i]).c_str(),angres,-180.,180.);//angres,-1.*TMath::Pi(),TMath::Pi());
     new ((*arrprothephi)[i]) TProfile2D(name5.Data(),(namesList[i]).c_str(),angres,0.,180.,angres,-180.,180.);
     //cout << "why?" << endl;
    radlList[i] = 0.;
  }

  cout << "after maxnames loop " << endl;

  new ((*arrhisxy)[maxnames]) TH2D("hisxy-rest","Gemrest",2*res,-85.,85.,2*res,-85.,85.);
  new ((*arrhisrz)[maxnames]) TH2D("hisrz-rest","Gemrest",2*res,100.,225.,2*res,-85.,85.);
  new ((*arrprothe)[maxnames]) TProfile("hthe-rest","Gemrest",angres,0.,180.);//angres,0.,TMath::Pi());
  new ((*arrprophi)[maxnames]) TProfile("hphi-rest","Gemrest",angres,-180.,180.);//angres,-1.*TMath::Pi(),TMath::Pi());
  new ((*arrprothephi)[maxnames]) TProfile2D("hthephi-rest","Gemrest",angres,0.,180.,angres,-180.,180.);
  radlList[maxnames]=0.;

  TProfile* thetaprofile = new TProfile("thetaprof","(a)Radiation length vs #theta;#theta[degree];X/X_{0}",angres,0.,180.);//angres,0.,TMath::Pi());
  TProfile* phiprofile = new TProfile("phiprof","(b)Radiation length vs #phi;#phi[degree];X/X_{0}",angres,-180.,180.);//angres,-1.*TMath::Pi(),TMath::Pi());

//  TH2D* histhephi = new TH2D("hThePhi","",100,0.2,TMath::Pi()-0.3,100,-1.*TMath::Pi(),TMath::Pi());
  TProfile2D* histhephi = new TProfile2D("hThePhi","",angres,0.0,180.0,angres,-180.,180.);//-1.*TMath::Pi(),TMath::Pi());
  histhephi->SetTitle("(c)Radiation length map vs #theta & #phi;#theta[degree];#phi[degree];X/X_{0}");
  
  TH1D* hisx = new TH1D("hX","X",100,-80.,80.);
  TH1D* hisy = new TH1D("hY","Y",100,-80.,80.);
  TH1D* hisz = new TH1D("hZ","Z",100,0.,210.);

  TString detname,volname;
  double radlen=0.,effradl=0.,effradlsum=0.,theta=0.,phi=0.;
  TVector3 in, out, dist, point;
  TVector3 vtx, mom;

  cout << "before event lop" << endl;
  for (Int_t event=0; event<nEvents && event<t->GetEntriesFast(); event++)
  {
    t->GetEntry(event);
    if(verbose) cout<<"Event No "<<event<<endl;
    else if (!(event%100)) cout <<"Event No "<<event<<endl;

    //cout << "before track loop" << endl;
    //cout << "there are " << mc_array->GetEntriesFast() << " tracks" << endl;

       for (Int_t trackno=0; trackno<mc_array->GetEntriesFast();trackno++){
      PndMCTrack* aTrack = (PndMCTrack*)mc_array->At(trackno);
      //if ( !aTrack ) cout << "adg" << endl;
      if (aTrack->GetMotherID()==-1)
	{
      mapMCIndex[trackno] = trackno;
	}
      else continue;
     
      mom = aTrack->GetMomentum();
      //      cout << " got mom = " << mom.X() <<  endl;
      
      theta = mom.Theta()*TMath::RadToDeg(); phi = mom.Phi()*TMath::RadToDeg();
//       if(theta < 0.05 || theta > (TMath::Pi()-0.05)) continue; // cut strange angles
//       vtx = aTrack->GetStartVertex();
      
      effradl=0.;      effradlsum=0.;
      for(int i=0; i<100; i++) radlList[i]=0.;
      
      //cout << "before radlen loop" << endl;
      
      for (Int_t k=0; k<rad_array->GetEntriesFast(); k++){
        FairRadLenPoint* radpoint = (FairRadLenPoint*)rad_array->At(k);
        if (radpoint->GetTrackID() != trackno) continue;

	//	cout << "got point " << k << endl;
	
        radlen = radpoint->GetRadLength();
        in = radpoint->GetPosition();
        out = radpoint->GetPositionOut();
        dist = in - out;
        //point.SetXYZ(0.5*(in.x()+out.x()),0.5*(in.y()+out.y()),0.5*(in.z()+out.z()));
	point.SetXYZ(0.5*(in.x()+in.x()),0.5*(in.y()+in.y()),0.5*(in.z()+in.z()));
	
	//	cout << "got dist = " << dist.X() << endl;
	
        if(in.Mag() < 0.02) continue; // cut target  // Mag()= magnitude=rho in spherical coordinates=sqrt(x*x+y*y+z*z)
	
        effradl = dist.Mag()/radlen;
	//cout << " effradl = " << effradl << endl;
	effradlsum += effradl;

	//cout << "ADDED " << effradl << " to the sum (" << effradlsum << ")" << endl;

	//cout << " effradlsum = " << effradlsum << endl;
	
	if ( !geoMan) 	cout << "have no manager" << endl;
	//cout << "got manager at " << point.x() << " " << point.y() << " " << point.z() << endl;
	
        TGeoNode* node = gGeoManager->FindNode(point.x(),point.y(),point.z());
        if( 0==node) 
        {
          std::cout<<"Warning: There is a node not defined properly!"<<std::endl;
          cout<<"\tEvent No "<<event<<" \t RadLenPoint No."<< k <<endl;
          continue;
        }
	
	//cout << "got node!" << endl;
        node->cd();
//         detname = geoMan->GetPath();
        volname = node->GetVolume()->GetName();
	//cout << "for point " << point.x() << "," << point.y() << "," << point.z() << " got vol "
	//<< volname.Data() << endl;

	//         if(verbose)cout<<detname.Data()<<endl;
	
	   if("cave" == volname && point.Mag() > 20. ) {
	       //  cout << " CONTINUE! in " << volname << " at " << point.Mag() << endl;
	      continue; // cut mainly the outer cave
	   	}
	
	
        if(verbose){
          cout<<"---> "<<volname.Data()<<endl;
        }

        hRadLenDistMat->Fill(dist.Mag());
        hRadLenDistEff->Fill(effradl);

        hisx->Fill(in.X(),effradl);
        hisy->Fill(in.Y(),effradl);
        hisz->Fill(in.Z(),effradl);

        hisxy->Fill(in.X(),in.Y());
        if(in.Y()>0.) hisrz->Fill(in.Z(),in.Perp());
        else hisrz->Fill(in.Z(),-1.*in.Perp());

	//cout << "NAMEIS " << volname.Data() << endl;

        int selected = maxnames;
        for(int i=0; i<namesList.size(); i++){
          if (volname.Contains( (namesList[i]).c_str())){
            selected=i;
            break;
	           }
	}
	
        radlList[selected]+=effradl;
	//cout << "ADDING " << effradl << " (radlList[" << selected << "] = " << radlList[selected] << ") for volume " << volname << " //// " << radlList[0]+radlList[1] << endl;
        ((TH2D*)arrhisxy->At(selected))->Fill(point.X(),point.Y(),effradl);
        if(point.Y()>0.) ((TH2D*)arrhisrz->At(selected))->Fill(point.Z(),point.Perp(),effradl); // Perp() get transverse component
        else ((TH2D*)(arrhisrz->At(selected)))->Fill(point.Z(),-1.*point.Perp(),effradl);
      }//radpoints
      
      thetaprofile->Fill(theta,effradlsum);
      phiprofile->Fill(phi,effradlsum);
      histhephi->Fill(theta,phi,effradlsum);

      // here add cout for effradlsum
      //cout << "effradlsum = " << "   " << effradlsum << endl;
      
      for(int i=0;i<=maxnames;i++){
        effradl=radlList[i];
	
	//here add cout for effradl
	//cout << "effradl(" << i << ") = " << "   " << effradl << endl;
	
        ((TProfile*)arrprothe->At(i))->Fill(theta,effradl);
        ((TProfile*)arrprophi->At(i))->Fill(phi,effradl);
	((TProfile2D*)arrprothephi->At(i))->Fill(theta,phi,effradl);
      }
     
    }//tracks
      
  }// end for event
  cout << "finished the loop" << endl;


  Int_t a=3,b=3;
int resol = 250;
if(draw1){
TCanvas* can1 = new TCanvas("can1","MCHit view in GEM",0,0,a*resol,b*resol);
can1->Divide(a,b);

can1->cd(1);
hisxy->SetStats(false);
hisxy->DrawCopy("colz");
 cout << "doing 2" << endl;

 can1->cd(2);
 hisrz->SetStats(false);
 hisrz->DrawCopy("colz");
  cout << "doing 3" << endl;

 can1->cd(3);

   cout << gPad->GetName() << endl;
   //cout << "Ja" << endl;
   //TPad* gPad = (TPad*) gPad;
   //cout << "Ja" << endl;
   if ( !gPad ) cout << "have no damn pad" << endl;
   //cout << "gotpad?" << endl;
   cout << gPad->GetName()<< endl;
  gPad->Divide(200,200);
  //cout << "Ja" << endl;
  gPad->cd(1);
  //cout << "Ja" << endl;
  hisx->SetFillColor(38);
  //cout << "Ja" << endl;
  hisx->DrawCopy();
  //cout << "Ja" << endl;
  gPad->cd(2);
  //cout << "Ja" << endl;
  hisy->SetFillColor(45);
  //cout << "Ja" << endl;
  hisy->DrawCopy();
  //cout << "Ja" << endl;
  gPad->cd(3);
  //cout << "Ja" << endl;
  hisz->SetFillColor(60);
 //cout << "Ja" << endl;
  hisz->DrawCopy();
  gPad->cd(4);

  cout << "doing 4" << endl;

   THStack* hs = new THStack("hs","test stacked histograms");
   //makes no sens - just test technics
   hs->Add(hisx);
   hs->Add(hisy);
   //hs->Add(hisz);
  hs->Draw();

  // THStack* hsz = new THStack("hsz","test stacked histograms");
  // //makes no sens - just test technics
  // hs->Add(hisz);
  // hs->Draw();


  // TLegend* legend = BuildLegend_THStack( hs, 0.8, 0.55, 0.98, 0.98);
  TLegend* legend = new TLegend(0.8, 0.55, 0.98, 0.98);
  legend->AddEntry(hisx,"","F");
  legend->AddEntry(hisy,"","F");
  //legend->AddEntry(hisz,"","F");
  legend->Draw();

can1->cd(4);
gPad->SetLogy();
hRadLenDistMat->SetFillColor(38);
hRadLenDistMat->DrawCopy();

can1->cd(5);
gPad->SetLogy();
hRadLenDistEff->SetFillColor(38);
hRadLenDistEff->DrawCopy();

can1->cd(6);
   cout << gPad->GetName() << endl;
   //cout << "Ja" << endl;
   //TPad* gPad = (TPad*) gPad;
   //cout << "Ja" << endl;
   if ( !gPad ) cout << "have no damn pad" << endl;
   //cout << "gotpad?" << endl;
   cout << gPad->GetName()<< endl;
  gPad->Divide(200,200);
  //cout << "Ja" << endl;
  gPad->cd(1);
  //cout << "Ja" << endl;
  hisx->SetFillColor(38);
  //cout << "Ja" << endl;
  hisx->DrawCopy();
  //cout << "Ja" << endl;
  gPad->cd(2);
  //cout << "Ja" << endl;
  hisy->SetFillColor(45);
  //cout << "Ja" << endl;
  hisy->DrawCopy();
  //cout << "Ja" << endl;
  gPad->cd(3);
  //cout << "Ja" << endl;
  hisz->SetFillColor(60);
 //cout << "Ja" << endl;
  hisz->DrawCopy();
  gPad->cd(4);

  cout << "doing 4" << endl;

   THStack* hs = new THStack("hs","test stacked histograms");
   //makes no sens - just test technics
   //hs->Add(hisx);
   //hs->Add(hisy);
  hs->Add(hisz);
  hs->Draw();

  // TLegend* legend = BuildLegend_THStack( hs, 0.8, 0.55, 0.98, 0.98);
  TLegend* legend = new TLegend(0.8, 0.55, 0.98, 0.98);
  //legend->AddEntry(hisx,"","F");
  //legend->AddEntry(hisy,"","F");
  legend->AddEntry(hisz,"","F");
  legend->Draw();

can1->cd(7);
gPad->SetLogy();
thetaprofile->SetFillColor(60);
thetaprofile->SetLineColor(60);
thetaprofile->Draw("hist");

can1->cd(8);
gPad->SetLogy();
phiprofile->SetFillColor(45);
phiprofile->SetLineColor(45);
phiprofile->Draw("hist");

 can1->cd(9);
 histhephi->SetStats(false);
 histhephi->DrawCopy("colz");
  
}


if(draw2){
resol = 250;
a=2,b=1;
TCanvas* can2 = new TCanvas("can2","MCHit view in GEM",50,50,a*2*resol,b*2*resol);
can2->Divide(a,b);

resol = 300;
//a=maxnames+2, b=1;
 a=2, b=1;
TCanvas* can4 = new TCanvas("can4","MCHit view in GEM",100,100,a*resol,b*resol);
can4->Divide(a,b);
//can4->Divide(3,b);
/*EColor colors[12] = 
  {kOrange,kAzure,kTeal ,kRed,kBlue,kGreen, kMagenta,kCyan,kYellow ,kPink,kViolet,kSpring };*/
//int colors[8] = {60,15,45,30,30,50,80,55};
//int colors[8] = {1,2,3,4,5,6,7,8};
int colors[5] = {60,45,15,30,80};
int coloff = -0;
 
THStack* thetastack = new THStack("thetastack","");
thetastack->SetTitle("(a)Radiation length vs #theta;#theta[degree];X/X_{0}    ");
THStack* phistack = new THStack("phistack","");
phistack->SetTitle("(b)Radiation length vs #phi;#phi[degree];X/X_{0}    ");
 
TProfile* aprof=0;
TH1D* ahist=0;
TLegend* legMat=new TLegend(0.6, 0.75, 0.8, 0.98);

for(int i=maxnames;i>=0;i--){
  aprof=(TProfile*)arrprothe->At(i);
  ahist=aprof->ProjectionX();
  ahist->SetFillColor(colors[i] + coloff);
  ahist->SetLineColor(colors[i] + coloff);
  legMat->AddEntry(ahist,"","F");
  can4->cd(i+1);
  gPad->SetLogy();
  ahist->DrawCopy("hist");
  thetastack->Add(ahist);  

  aprof=(TProfile*)arrprophi->At(i);
  ahist=aprof->ProjectionX();
  ahist->SetFillColor(colors[i] + coloff);
  ahist->SetLineColor(colors[i] + coloff);
  phistack->Add(ahist);
  
  } 
 can2->cd(1);
 thetastack->SetMaximum(2.0);
 //thetastack->SetMaximum(7.6);
 //thetastack->SetMaximum(0.6);
 thetastack->Draw("hist,nostack");
 legMat->Draw();

 can2->cd(2);
 //phistack->SetMaximum(1.0);
 phistack->SetMaximum(1.8);
 //phistack->SetMaximum(0.4);
 //phistack->SetMaximum(1.5);
 phistack->Draw("hist,nostack");
 legMat->Draw();
 
 }
//-------------------------------------------------------------------------------------
if(draw4){
 resol = 250;
 //a=maxnames+2, b=1;
 a=2,   b=1;
 TCanvas* can5 = new TCanvas("can5","RadMap",50,50,a*2*resol,b*2*resol);
 can5->Divide(a,b);
 //can5->Divide(3,b);
 TProfile2D* thephimap = new TProfile2D("thephimap","",angres,0.,180.,angres,-180.,180.);
 thephimap->SetTitle("Radiation length map vs #theta & #phi;#theta[degree];#phi[degree];X/X_{0}");

 TProfile2D* bprof=0;
 TLegend* legMat=new TLegend(0.6, 0.75, 0.8, 0.98);

 for(int i=maxnames;i>=0;i--){
   bprof=(TProfile2D*)arrprothephi->At(i);
   legMat->AddEntry(bprof,"","F");
   can5->cd(i+1);
   thephimap->SetStats(false);
   bprof->DrawCopy("colz");
   //legMat->Draw();
    } 
  
 }
 //------------------------------------------------------------------------------------------------

if(draw3){

  //a=maxnames+2,b=2;
  a=maxnames+1,b=2;
if(a>4) resol = int(1200/a);
TCanvas* can3 = new TCanvas("can3","MCHit view in GEM",150,150,a*resol,b*resol);
can3->Divide(a,b);
 
for(int i=0; i<a ;i++){
  can3->cd(i+1);
  ((TH2D*)arrhisxy->At(i))->DrawCopy("colz");
  can3->cd(i+a+1);
  ((TH2D*)arrhisrz->At(i))->DrawCopy("colz");
  gStyle->SetOptStat();
}
 }

can1->Print("outAnaGemSim1.ps");
  // -----   Finish   -------------------------------------------------------
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  //cout << "Output file is "    << outFile << endl;
  //cout << "Parameter file is " << parFile << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;
  // ------------------------------------------------------------------------

  return 0;
}
