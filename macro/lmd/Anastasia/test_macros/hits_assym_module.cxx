/*
 *
 *  Created on: Sep 20, 2014
 *  by A.Karavdina
 */
// to compile and run this macro do:
//# to compile it add in "# install #" part of ${PANDAROOT}/macro/lmd/Anastasia/test_macros/CMakeLists.txt lines:
//# add_executable(hits_assym hits_assym.cxx)
//# target_link_libraries(hits_assym -lrt ${ROOT_LIBRARIES} LmdTool MathMore)
//#
//# to run it (and see options): 
//# ${PANDAROOT}/build/bin/./hits_assym --help


#include<iostream>
#include<sstream>
#include<string>

#include<TString.h>
#include<TChain.h>
#include<TClonesArray.h>
#include<TROOT.h>
#include<TVector3.h>
#include<TH1.h>
#include<TH2.h>
#include<TApplication.h>
#include<TCanvas.h>
#include<LmdQA/PndLmdDigiQ.h>
#include<PndLmdDim.h>
#include<PndSdsMCPoint.h>
#include "TStyle.h"
#include <TLegend.h>
#include "TGraphErrors.h"
#include "TMultiGraph.h"
using namespace std;

double last_percent(0);

void DrawProgressBar(int len, double percent) {
	if ((int)(last_percent*100) == (int)(percent*100)) return;
	//cout << " drawing " << endl;
	cout << "\x1B[2K"; // Erase the entire current line.
	cout << "\x1B[0E"; // Move to the beginning of the current line.
	string progress;
	for (int i = 0; i < len; ++i) {
	if (i < static_cast<int>(len * percent)) {
	progress += "=";
	} else {
	progress += " ";
	}
	}
	cout << "[" << progress << "] " << (static_cast<int>(100 * percent)) << "%";
	flush(cout); // Required.
	last_percent = percent;
}

int main(int nargs, char** args) {

  // const int scalefac = 1;// TEST
	// load first all dictionaries for ROOT in order
	// to read the files correctly
	gROOT->Macro("$VMCWORKDIR/macro/lmd/Anastasia/test_macros/rootlogon.C");
	// TStyle* m_gStyle = new TStyle("style_name","get rid from headears");
	// m_gStyle->SetOptStat(0);
	// m_gStyle->SetLabelSize(0.045,"xyz");
	// m_gStyle->SetTitleYSize(0.04);
	// m_gStyle->SetTitleXSize(0.04);
	// gROOT->SetStyle("style_name");
	TApplication myapp("myapp",0,0);
	TString storePath = ".";
	TString Pbeam = "0";
	// set the storepath to somewhere else, if provided
	if (nargs == 3) {
		storePath = args[1];
		Pbeam = args[2];
	} else {
		if (nargs == 1)
			cout << " Hint: you may change the standard path from "
					<< storePath.Data()
					<< " to any other folder by providing the path as an argument! "
					<< endl;
		if (nargs > 3)
			cout << " Error: too many arguments! " << endl;
	}

	TString outname = storePath + "/HitsAssymPIXELS_x_y_";
	outname+=Pbeam;
	outname+="GeV";
	TString outname_pdf_o = outname + ".pdf(";
	TString outname_pdf_c = outname + ".pdf)";
	TString outname_root = outname + ".root";

	TString startEvent = "0";
	// Input file (MC events)
	TString MCFile = storePath + "/Lumi_MC_";
	MCFile += startEvent;
	MCFile += ".root";
	TChain tMC("cbmsim");
	tMC.Add(MCFile);
	// Input file (sorted digi)
	TString DigiFile = storePath+"/Lumi_DigisQA_";
	DigiFile += startEvent;
	DigiFile += ".root";
	TChain tDigi("cbmsim");
	tDigi.Add(DigiFile);


	std::cout << "MCFile  : " << MCFile.Data() << std::endl;
	std::cout << "DigiFile  : " << DigiFile.Data() << std::endl;

	//--- MC info -----------------------------------------------------------------
	TClonesArray* true_tracks = new TClonesArray("PndMCTrack");
	tMC.SetBranchAddress("MCTrack", &true_tracks); //True Tracks to compare

	TClonesArray* true_points = new TClonesArray("PndSdsMCPoint");
	tMC.SetBranchAddress("LMDPoint", &true_points); //True Points to compare
	//----------------------------------------------------------------------------------
	int nEvents = tMC.GetEntries();

	//--- DigiQ info -----------------------------------------------------------------
	TClonesArray* digiq_points = new TClonesArray("PndLmdDigiQ");
	tDigi.SetBranchAddress("LMDPixelDigisQ", &digiq_points); //Digi hits
	//--------------------------------------------------------------------------------
	const double scalefac = 1;
	// initialize the lmd dimension class
	PndLmdDim& lmddim = PndLmdDim::Get_instance();
	//	TString mtx_corr =  storePath+"/matrices_corrected.txt";
	TString mtx_corr =  "/home/karavdina/soft/pandaroot/geometry/trafo_matrices_lmd.dat";
	TString mtx_perfect = "/home/karavdina/soft/pandaroot/geometry/trafo_matrices_lmd.dat";
	lmddim.Read_transformation_matrices(mtx_perfect.Data(), false);
	lmddim.Read_transformation_matrices(mtx_corr.Data(), true);

	TCanvas canvas_map_x_y("canvas_map_x_y", "map x y", 800, 800);
	canvas_map_x_y.Divide(2,2);
	int maxSum=180;
	int maxEl=180;
	int maxInEl=50;
	if(Pbeam=="15"){
	  maxSum=25;
	  maxEl=25;
	  maxInEl=25;
	}
	// TH2Poly* el_map_x_y_plane[4][2];
	// TH2Poly* inel_map_x_y_plane[4][2];
	TH2Poly* sum_map_x_y_plane[4][2];
	//	TH2Poly* sum_map_x_y_module[40][2];
	TH2Poly* sum_map_x_y_module; 	TH2Poly* sum_map_x_y_module_0;
	TGraphErrors *meansXY[4][2];
	TGraphErrors *meansX_Z[20];
	TGraphErrors *meansY_Z[20];


	//	TLegend *leg = new TLegend(0.65,0.7,0.99,0.99);
	TLegend *leg = new TLegend(0.4,0.40,0.6,0.6);
	leg->SetFillColor(0);
	leg->SetTextFont(42);
	leg->SetTextSize(0.04);
	TMultiGraph *mg_meansXY = new TMultiGraph();
	TMultiGraph *mg_meansX_Z = new TMultiGraph();
	TMultiGraph *mg_meansY_Z = new TMultiGraph();
	for(int imd=0; imd<20; imd++){
	  meansX_Z[imd] = new TGraphErrors();
	  meansX_Z[imd]->SetTitle("");
	  meansX_Z[imd] ->SetMarkerStyle(20+imd);
	  meansX_Z[imd] ->SetMarkerSize(1.5);
	  meansX_Z[imd] ->SetMarkerColor(1+imd);

	  meansY_Z[imd] = new TGraphErrors();
	  meansY_Z[imd]->SetTitle("");
	  meansY_Z[imd] ->SetMarkerStyle(20+imd);
	  meansY_Z[imd] ->SetMarkerSize(1.5);
	  meansY_Z[imd] ->SetMarkerColor(1+imd);
	}
	for(int ipl=0; ipl<4; ipl++){
	  for(int is=0; is<2; is++){
	  //	  for(int is=0; is<2; is++){
	    TString name = "pl ";
	    name +=ipl;
	    name += " sd ";
	    name +=is;
	    TString name_xy = name;
	    name_xy += " (Mean XY)";
	    TString hname = "hxy_pl";
	    hname += ipl;
	    hname += "_sd_";
	    hname +=is;
	    //	    meansXY[ipl][is] =new TH2D(hname,"; x, cm; y, cm",1e3,-11,11,1e3,-11,11);
	    meansXY[ipl][is] = new TGraphErrors();
	    meansXY[ipl][is]->SetTitle("");
	    //	    meansXY[ipl][is] ->SetTitle(name_xy);
	    meansXY[ipl][is] ->SetMarkerStyle(21+is);
	    meansXY[ipl][is] ->SetMarkerSize(1.5);
	    meansXY[ipl][is] ->SetMarkerColor(1+ipl);
	    if(is==0) leg->AddEntry(meansXY[ipl][is],name,"p");	  
	    TString name_sum = name + " (el.+inel. signal)";
	    sum_map_x_y_plane[ipl][is] = lmddim.Get_histogram_Plane(ipl,is);
	    sum_map_x_y_plane[ipl][is]->SetTitle(name_sum);
	    sum_map_x_y_plane[ipl][is]->SetXTitle("X [cm]");
	    sum_map_x_y_plane[ipl][is]->SetYTitle("Y [cm]");
	    sum_map_x_y_plane[ipl][is]->SetZTitle("events");
	    for(int ih=0;ih<2;ih++){
	    for(int imd=0;imd<5;imd++){
	      int curmd = 10*ipl+ih*5+imd;
	      cout<<"curmd ="<<curmd<<endl;
	      //	      sum_map_x_y_module[curmd][is] = lmddim.Get_histogram_Plane(ipl,is,true,true,true);
	      //   sum_map_x_y_module = lmddim.Get_histogram_Moduleside(ih, ipl, imd, is,true,true,true); //pixels
	      //	      if(curmd==0 && is==0) sum_map_x_y_module_0 = lmddim.Get_histogram_Moduleside(ih, ipl, imd, is,true,true,true); //pixels
	      sum_map_x_y_module = lmddim.Get_histogram_Moduleside(ih, ipl, imd, is,true,true,false);//sensors
	      if(curmd==0 && is==0) sum_map_x_y_module_0 = lmddim.Get_histogram_Moduleside(ih, ipl, imd, is,true,true,false); //sensors
	      TString mdname = "plane ";
	      mdname +=ipl;
	      mdname += " module ";
	      mdname +=curmd;
	      mdname +=" side ";
	      mdname +=is;
	      // sum_map_x_y_module->SetTitle(mdname);
	      // sum_map_x_y_module->SetXTitle("X [cm]");
	      // sum_map_x_y_module[curmd][is]->SetYTitle("Y [cm]");
	      // sum_map_x_y_module[curmd][is]->SetZTitle("events");
	      //read events and get avarage coordinates ---------------------
	      for (Int_t iEvent = 0; iEvent < nEvents; iEvent++) { 
	      //	      for (Int_t iEvent = 0; iEvent<2e6; iEvent++) {
		DrawProgressBar(50, (iEvent+1)/((double)nEvents));
		tMC.GetEntry(iEvent);
		tDigi.GetEntry(iEvent);
		const int nDigi = digiq_points->GetEntriesFast();
		for (Int_t i=0; i<nDigi; i++){
		  PndLmdDigiQ* DigiPoint = (PndLmdDigiQ*)(digiq_points->At(i)); // read digi hit
		  bool sigFl = DigiPoint->GetFlSig();
		  int sensorID = DigiPoint->GetSensorID(); 
		  int column = DigiPoint->GetPixelColumn();
		  int row = DigiPoint->GetPixelRow();
		  int ihalf, iplane, imodule, iside, idie, isensor;
		  lmddim.Get_sensor_by_id(sensorID, ihalf, iplane, imodule, iside, idie, isensor);
		  TVector3 hitCoor = lmddim.Decode_hit(sensorID,column,row,true);
		  TVector3 hitCoorlmd = lmddim.Transform_global_to_lmd_local(hitCoor);
		  //fill only if this is the module
		  if(iplane==ipl && iside==is && ihalf==ih && imd==imodule){
		  sum_map_x_y_plane[iplane][iside]->Fill(hitCoorlmd.X(),hitCoorlmd.Y(),scalefac);
		  //	  int modid = 5*ihalf+10*iplane+imodule;
		  //	  cout<<"modid "<<modid<<endl;
		  sum_map_x_y_module->Fill(hitCoorlmd.X(),hitCoorlmd.Y(),scalefac);
		  if(curmd==0 && is==0) sum_map_x_y_module_0->Fill(hitCoorlmd.X(),hitCoorlmd.Y(),scalefac);
		  }
		}
	      }// loop over all events

	      double Xmean = sum_map_x_y_module->GetMean(1);
	      double Ymean = sum_map_x_y_module->GetMean(2);
	      double Xrms = sum_map_x_y_module->GetMeanError(1);
	      double Yrms = sum_map_x_y_module->GetMeanError(2);
	      // double Xrms = sum_map_x_y_module->GetRMS(1);
	      // double Yrms = sum_map_x_y_module->GetRMS(2);
	      cout<<"X_mod = "<<Xmean<<" +/- "<<Xrms<<endl;
	      int imdhf = ih*5+imd;
	      meansXY[ipl][is]->SetPoint(imdhf,Xmean,Ymean);
	      meansXY[ipl][is]->SetPointError(imdhf,Xrms,Yrms);
	      delete sum_map_x_y_module;
	      double zPl = 0;
	      if(ipl==1) zPl =20;
	      if(ipl==2) zPl =30;
	      if(ipl==3) zPl =40;
	      int curimd = imdhf*2+is;
	      if(is==0) curimd +=1;
	      else curimd -=1;

	      cout<<" fill hists for curimd = "<<curimd<<endl;
	      meansX_Z[curimd]->SetPoint(ipl,zPl,Xmean);
	      meansY_Z[curimd]->SetPoint(ipl,zPl,Ymean);
	      meansX_Z[curimd]->SetPointError(ipl,0,Xrms);
	      meansY_Z[curimd]->SetPointError(ipl,0,Yrms);
	      //[END] read events and get avarage coordinates ------------
	    }//module imd
	    }//half ih
	  }//side is
	}//plane ipl
	

	TCanvas canvas_map("canvas_map_x_y", "map x y", 900, 900);
	sum_map_x_y_module_0->GetXaxis()->SetLimits(-10,10);
	sum_map_x_y_module_0->GetYaxis()->SetLimits(-10,10);
	sum_map_x_y_module_0->Draw("COLZ");
	canvas_map.Print(outname_pdf_o.Data());
	canvas_map.Clear();
	for(int ipl=0; ipl<4; ipl++){
	  for(int is=0; is<2; is++){
	    sum_map_x_y_plane[ipl][is]->Draw("COLZ");
	    lmddim.Draw_Sensors(ipl,false,true,is);
	    canvas_map.Print(outname_pdf_o.Data());
	    canvas_map.Clear();
	  }
	}
	for(int ipl=0; ipl<4; ipl++){
	  //	  for(int is=0; is<1; is++){//only one side!
	  for(int is=0; is<2; is++){
	    mg_meansXY->Add(meansXY[ipl][is]);
	  }
	}
	mg_meansXY->Draw("AP");
	mg_meansXY->GetXaxis()->SetTitle("x, cm");
	mg_meansXY->GetXaxis()->SetLimits(-10,10);
	mg_meansXY->GetYaxis()->SetTitle("y, cm");
	//	mg_meansXY->GetYaxis()->SetLimits(-10,10);
	mg_meansXY->SetMinimum(-10);
	mg_meansXY->SetMaximum(10);
	leg->Draw();
	canvas_map.Print(outname_pdf_o.Data());
	canvas_map.Clear();

	TGraphErrors *modX0 = new TGraphErrors(20);
	modX0->SetMarkerColor(2);
	modX0->SetMarkerStyle(21);
	modX0->SetMarkerSize(1.5);
	TGraphErrors *modY0 = new TGraphErrors(20);
	modY0->SetMarkerColor(2);
	modY0->SetMarkerStyle(21);
	modY0->SetMarkerSize(1.5);
	TGraphErrors *modSLX = new TGraphErrors(20);
	modSLX->SetMarkerColor(2);
	modSLX->SetMarkerStyle(21);
	modSLX->SetMarkerSize(1.5);
	TGraphErrors *modSLY = new TGraphErrors(20);
	modSLY->SetMarkerColor(2);
	modSLY->SetMarkerStyle(21);
	modSLY->SetMarkerSize(1.5);
	TF1 *fa = new TF1("fa","([0]*x+[1])",0,40); 
	TF1 *fa2 = new TF1("fa2","([0]*x+[1])",0,40); 
	for(int imd=0; imd<20; imd++){
	//	for(int imd=0; imd<10; imd++){//TESTS
	  meansX_Z[imd]->Fit(fa);
	  double x0 = fa->GetParameter(1);
	  double errx0 = fa->GetParError(1);
	  double slx = fa->GetParameter(0);
	  double errslx = fa->GetParError(0);
	  modX0->SetPoint(imd,imd,x0);
	  modX0->SetPointError(imd,0,errx0);
	  modSLX->SetPoint(imd,imd,slx);
	  modSLX->SetPointError(imd,0,errslx);
	  meansY_Z[imd]->Fit(fa2);
	  double y0 = fa2->GetParameter(1);
	  double sly = fa2->GetParameter(0);
	  double erry0 = fa2->GetParError(1);
	  double errsly = fa2->GetParError(0);
	  modY0->SetPoint(imd,imd,y0);
	  modY0->SetPointError(imd,0,erry0);
	  modSLY->SetPoint(imd,imd,sly);
	  modSLY->SetPointError(imd,0,errsly);
	  mg_meansX_Z->Add(meansX_Z[imd]);
	  mg_meansY_Z->Add(meansY_Z[imd]);
	}
	mg_meansX_Z->Draw("AP");
	mg_meansX_Z->GetXaxis()->SetTitle("z, cm");
	mg_meansX_Z->GetYaxis()->SetTitle("x_{mean}, cm");
	mg_meansX_Z->SetMinimum(-10);
	mg_meansX_Z->SetMaximum(10);
	canvas_map.Print(outname_pdf_o.Data());
	canvas_map.Clear();
	mg_meansY_Z->Draw("AP");
	mg_meansY_Z->GetXaxis()->SetTitle("z, cm");
	mg_meansY_Z->GetYaxis()->SetTitle("y_{mean}, cm");
	mg_meansY_Z->SetMinimum(-10);
	mg_meansY_Z->SetMaximum(10);
	canvas_map.Print(outname_pdf_o.Data());
	canvas_map.Clear();

	modX0->SetTitle(" ");
	modY0->SetTitle(" ");
	modSLX->SetTitle(" ");
	modSLY->SetTitle(" ");
	modX0->SetMinimum(-10);
	modX0->SetMaximum(10);
	modX0->GetXaxis()->SetTitle("sector");
	modX0->GetXaxis()->SetLimits(-1,20);
	modX0->GetYaxis()->SetTitle("X0, cm");
	modX0->Draw("AP");
	canvas_map.Print(outname_pdf_o.Data());
	canvas_map.Clear();
	modSLX->SetMinimum(-0.05);
	modSLX->SetMaximum(0.05);
	modSLX->GetXaxis()->SetTitle("sector");
	modSLX->GetXaxis()->SetLimits(-1,20);
	modSLX->GetYaxis()->SetTitle("slope X");
	modSLX->Draw("AP");
	canvas_map.Print(outname_pdf_o.Data());
	canvas_map.Clear();
	modY0->SetMinimum(-10);
	modY0->SetMaximum(10);
	modY0->GetXaxis()->SetLimits(-1,20);
	modY0->GetXaxis()->SetTitle("sector");
	modY0->GetYaxis()->SetTitle("Y0, cm");
	modY0->Draw("AP");
	canvas_map.Print(outname_pdf_o.Data());
	canvas_map.Clear();
	modSLY->SetMinimum(-0.05);
	modSLY->SetMaximum(0.05);
	modSLY->GetXaxis()->SetLimits(-1,20);
	modSLY->GetXaxis()->SetTitle("sector");
	modSLY->GetYaxis()->SetTitle("slope Y");
	modSLY->Draw("AP");
	canvas_map.Print(outname_pdf_o.Data());
	canvas_map.Clear();

	canvas_map.Print(outname_pdf_c.Data());

	TFile *f = new TFile(outname_root,"RECREATE");
	for(int ipl=0; ipl<4; ipl++){
	  for(int is=0; is<2; is++){
	    TString name_can = "canvas_map_x_y_sum_pl";
	    name_can +=ipl;
	    name_can +="_side_";
	    name_can +=is;
	    TCanvas canvas_1(name_can, "map x y (sum)", 900, 900);
	    sum_map_x_y_plane[ipl][is]->Draw("COLZ");
	    lmddim.Draw_Sensors(ipl,false,true,is);
	    canvas_1.Write();
	    name_can = "canvas_map_x_y_el_pl";
	    name_can +=ipl;
	    name_can +="_side_";
	    name_can +=is;
	    // TCanvas canvas_2(name_can, "map x y (el)", 900, 900);
	    // el_map_x_y_plane[ipl][is]->Draw("COLZ");
	    // lmddim.Draw_Sensors(ipl,false,true,is);
	    // canvas_2.Write();
	    // name_can = "canvas_map_x_y_inel_pl";
	    // name_can +=ipl;
	    // name_can +="_side_";
	    // name_can +=is;
	    // TCanvas canvas_3(name_can, "map x y (inel)", 900, 900);
	    // inel_map_x_y_plane[ipl][is]->Draw("COLZ");
	    // lmddim.Draw_Sensors(ipl,false,true,is);
	    // canvas_3.Write();
	  }
	}
	modX0->SetName("modX0");
	modY0->SetName("modY0");
	modSLX->SetName("modSLX");
	modSLY->SetName("modSLY");
	modX0->Write();
	modY0->Write();
	modSLX->Write();
	modSLY->Write();
	f->Write();
	f->Close();
	cout<<"------- SUMMARY -------"<<endl;
	//	cout<<"Pbeam = "<<Pbeam<<"GeV tot. cross-section "<<sigTot<<" mb"<<endl;
	cout<<"Pbeam = "<<Pbeam<<" GeV"<<endl;
	// cout<<"Overall count rate: "<<TOT_count*scalefac*1e-3<<" MHz"<<endl;
	// cout<<"Elastic count rate: "<<el_count*scalefac*1e-3<<" MHz"<<endl;
	// cout<<"Inelastic count rate: "<<inel_count*scalefac*1e-3<<" MHz"<<endl;
}
